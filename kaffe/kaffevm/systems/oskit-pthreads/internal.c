/*
 * Copyright (c) 1998 The University of Utah. All rights reserved.
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

/*
 * This file links the ThreadInterface as defined by Tim to the revised
 * version of the internal thread interface ("jthreads").
 *
 * This file implements the functions in ThreadInterface by invoking
 * functions declared in jthread.h
 * It is responsible for unhanding, as well as invoking functions in other
 * parts of the VM.
 *
 * Note that jthreads - while heavily based on Tim's code - can be used 
 * independently of the Kaffe JVM.  That is, they don't have any compile or
 * link time dependencies on it.  This file knows about all the JVM related 
 * types, whereas jthread does not.
 */

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "jtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "lookup.h"
#include "thread.h"
#include "locks.h"
#include "exception.h"
#include "support.h"
#include "external.h"
#include "errors.h"
#include "gc.h"
#include "md.h"
#include "lerrno.h"
#include "jsyscall.h"

#include "jthread.h"

/* If not otherwise specified, assume at least 1MB for main thread */
#ifndef MAINSTACKSIZE
#define MAINSTACKSIZE (1024*1024)
#endif

/* 
 * store the native thread for the main thread here 
 * between init and createFirst 
 */
static struct Hkaffe_util_Ptr*	mainthread;
static int threadStackSize;	/* native stack size */
/* referenced by native/Runtime.c */
jbool runFinalizerOnExit;	/* should we run finalizers? */

static void 
broadcastDeath(void *jlThread)
{
        Hjava_lang_Thread *tid = jlThread;

        /* Notify on the object just in case anyone is waiting */
        lockMutex(&tid->base);
        broadcastCond(&tid->base);
        unlockMutex(&tid->base);
}

static void 
throwDeath(void)
{
	throwException(ThreadDeath);
}

static void *
thread_malloc(size_t s)
{
	return gc_malloc(s, GC_ALLOC_THREADCTX);
}

static void
thread_free(void *p)
{
	gc_free(p);
}

static
void
runfinalizer(void)
{
	if (runFinalizerOnExit)
		invokeFinalizer();
}

/* 
 * For debugging purposes, the jthread system might ask us to assign
 * a name to a given thread.  This can be single-threaded.
 */
static
char *
nameThread(void *tid)
{
	static char buf[80];
	int i = 0;
	HArrayOfChar* name = unhand((Hjava_lang_Thread*)tid)->name;
	while (i < sizeof buf - 1 && i < ARRAY_SIZE(name)) {
		buf[i] = ((short*)ARRAY_DATA(name))[i];
		i++;
	}
	buf[i] = 0;
	return buf;
}

static
void
Tinit(int nativestacksize)
{
	/* Even though the underlying operating or threading system could
	 * probably extend the main thread's stack, we must impose this 
	 * artificial boundary, because otherwise we wouldn't be able to 
	 * catch stack overflow exceptions thrown by the main thread.
	 */
	threadStackSize = nativestacksize;
	mainthread = (struct Hkaffe_util_Ptr*)jthread_init(
		DBGEXPR(JTHREADNOPREEMPT, false, true),
		java_lang_Thread_MAX_PRIORITY+1,
		java_lang_Thread_MIN_PRIORITY,
		java_lang_Thread_NORM_PRIORITY,
		MAINSTACKSIZE,
		thread_malloc,
		thread_free,
		broadcastDeath,
		throwDeath,
		nameThread);	
	assert(mainthread);
	gc_add_ref(mainthread);
}

static
void
TcreateFirst(Hjava_lang_Thread* tid)
{
	jthread_atexit(runfinalizer);
	/* set Java thread associated with main thread */
	SET_COOKIE(tid);
	GC_WRITE(tid, mainthread);
	unhand(tid)->PrivateInfo = mainthread;
	unhand(tid)->stackOverflowError = 
		(Hjava_lang_Throwable*)StackOverflowError;
	unhand(tid)->needOnStack = STACK_HIGH;
}

static
void
Tcreate(Hjava_lang_Thread* tid, void* func)
{
	struct Hkaffe_util_Ptr* nativethread;

	nativethread = (struct Hkaffe_util_Ptr*)jthread_create(
		unhand(tid)->priority,
		func,
		unhand(tid)->daemon,
		tid,
		threadStackSize);
	GC_WRITE(tid, nativethread);
	unhand(tid)->PrivateInfo = nativethread;
	/* preallocate a stack overflow error for this thread in case it 
	   runs out */
	unhand(tid)->stackOverflowError = 
		(Hjava_lang_Throwable*)StackOverflowError;
	unhand(tid)->needOnStack = STACK_HIGH;
}

static  
void    
Tprio(Hjava_lang_Thread* tid, jint prio)
{       
	/* no native thread yet */
	if (unhand(tid)->PrivateInfo == 0)
		return;

	jthread_setpriority((jthread_t)unhand(tid)->PrivateInfo, prio);
}               

static          
void            
Tstop(Hjava_lang_Thread* tid)
{       
	jthread_stop((jthread_t)unhand(tid)->PrivateInfo);
}       

static          
void            
Tinterrupt(Hjava_lang_Thread* tid)
{       
#ifdef  notyet
	jthread_interrupt((jthread_t)unhand(tid)->PrivateInfo);
#else
	unimp("Cannot interrupt threads yet");
#endif
}       

static  
bool    
Talive(Hjava_lang_Thread* tid)
{
	return jthread_alive((jthread_t)unhand(tid)->PrivateInfo);
}
        
static
jint    
Tframes(Hjava_lang_Thread* tid)
{
	return jthread_frames((jthread_t)unhand(tid)->PrivateInfo);
}       
 
static
void    
Tfinalize(Hjava_lang_Thread* tid)
{
	if (unhand(tid)->PrivateInfo != 0) {
		jthread_t jtid = (jthread_t)unhand(tid)->PrivateInfo;
		unhand(tid)->PrivateInfo = 0;
		jthread_destroy(jtid);
	}
}       

static          
Hjava_lang_Thread*      
TcurrentJava(void)      
{               
	return GET_COOKIE();
}

static          
void
TwalkThreads(void)
{               
	/* this is from gc-incremental.c */
	extern void walkMemory(void*);
	jthread_walkLiveThreads(walkMemory);
}

/*      
 * Walk the thread's internal context.
 */
static
void
TwalkThread(Hjava_lang_Thread* tid)
{
	void *from;
	unsigned len;
	jthread_t jtid = (jthread_t)unhand(tid)->PrivateInfo;

        if (jtid == 0)
                return;
 
        markObject(unhand(tid)->exceptObj);

	/* ask threading system what the interesting stack range is */
	jthread_extract_stack(jtid, &from, &len);

	/* and walk it */
	walkConservative(from, len);
}

/* called when unrolling exceptions */
static  
void*   
TnextFrame(void* fm)
{
#if defined(TRANSLATOR)
        exceptionFrame* nfm;

        nfm = (exceptionFrame*)(((exceptionFrame*)fm)->retbp);
	/* Note: this should obsolete the FRAMEOKAY macro */
        if (nfm && jthread_on_current_stack((void *)nfm->retbp)) {
                return (nfm);
        }
        else {
                return (0);
        }
#else
        vmException* nfm; 
        nfm = ((vmException*)fm)->prev;
        if (nfm != 0 && nfm->meth != (Method*)1) {
                return (nfm);
        }
        else {
                return (0);
        }
#endif 
}

/*
 * implementation of the locking subsystem based on jlocks
 *
 * Note that we keep track of lk->holder, and its type is void*.
 */
static
void  
Linit(iLock* lk)
{
	static bool first = true;
	static jmutex first_mutex;
	static jcondvar first_condvar;

	/* The first lock init is for the memory manager - so we can't
	 * use it yet.  Allocate from static space.
	 */
	if (first == true) {
		first = false;
		lk->mux = &first_mutex;
		lk->cv = &first_condvar;
	}
	else {
		lk->mux = thread_malloc(sizeof(jmutex));
		lk->cv = thread_malloc(sizeof(jcondvar));
	}
	jmutex_initialise(lk->mux);
	jcondvar_initialise(lk->cv);
}               
                
static  
void    
Llock(iLock* lk)
{
	jmutex_lock(lk->mux);
	lk->holder = (void *)jthread_current();
}

static  
void    
Lunlock(iLock* lk)
{       
	lk->holder = 0;
	jmutex_unlock(lk->mux);
}

static
void
Lwait(iLock* lk, jlong timeout)
{
	int count;
	/*
	 * We must reacquire the Java lock before we're ready to die
	 */
	jthread_disable_stop();
	count = lk->count;
	lk->count = 0;
	jcondvar_wait(lk->cv, lk->mux, timeout);
	lk->holder = (void *)jthread_current();
	lk->count = count;

	/* now it's safe to start dying */ 
	jthread_enable_stop();
}
 
static  
void    
Lsignal(iLock* lk)
{
	jcondvar_signal(lk->cv, lk->mux);
}       
        
static          
void
Lbroadcast(iLock* lk)
{
	jcondvar_broadcast(lk->cv, lk->mux);
}

void    
Tspinon(void* arg)
{       
	intsDisable();
}       
        
void            
Tspinoff(void* arg)
{               
	intsRestore();
}               

void    
Tspinoffall(void* arg)
{       
	intsRestoreAll();
}       
        
/*
 * check whether we have at least `left' bytes left on the stack
 */
static
Hjava_lang_Throwable*
TcheckStack(int left)
{
	int rc = jthread_stackcheck(left);

DBG(VMTHREAD,
	if (rc == false) {
		dprintf("%s doesn't have %d bytes left\n", 
			nameThread(GET_COOKIE()), left);
	}
   )
	return (Hjava_lang_Throwable*)
		(rc == true ? 0 : unhand(TcurrentJava())->stackOverflowError);
}

/*
 * Define the thread interface.
 */
ThreadInterface Kaffe_ThreadInterface = {

        Tinit,
        TcreateFirst,
        Tcreate,
        jthread_sleep,
        jthread_yield,
        Tprio, 
        Tstop, 
        Tinterrupt, 
        jthread_exit,
        Talive,
        Tframes,
        Tfinalize,
        TcurrentJava,
	(void *(*)(void)) jthread_current,
        TwalkThreads,
        TwalkThread,
        TnextFrame,
	TcheckStack,

};

/*
 * Define the lock interface.
 */
LockInterface Kaffe_LockInterface = {

        Linit,
        Llock,
        Lunlock,
        Lwait,
        Lsignal,
        Lbroadcast,

        Tspinon,
        Tspinoff,

};
