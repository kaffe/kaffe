/*
 * thread.c
 * Thread support.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "kaffe/jni_md.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "errors.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "lookup.h"
#include "thread.h"
#include "jthread.h"
#include "locks.h"
#include "ksem.h"
#include "exception.h"
#include "support.h"
#include "external.h"
#include "gc.h"
#include "jni.h"
#include "md.h"
#include "jvmpi_kaffe.h"

/* If not otherwise specified, assume at least 1MB for main thread */
#ifndef MAINSTACKSIZE
#define MAINSTACKSIZE (1024*1024)
#endif
		
extern struct JNINativeInterface Kaffe_JNINativeInterface;

static size_t threadStackSize;	/* native stack size */

/* referenced by native/Runtime.c */
jbool runFinalizerOnExit;	/* should we run finalizers? */

jbool deadlockDetection = 1;	/* abort if we detect deadlock */

Hjava_lang_Class* ThreadClass;
Hjava_lang_Class* VMThreadClass;
Hjava_lang_Class* ThreadGroupClass;
Hjava_lang_ThreadGroup* standardGroup;

static void firstStartThread(void*);
static void runfinalizer(void);

static iStaticLock thread_start_lock = KAFFE_STATIC_LOCK_INITIALIZER;

/*
 * How do I get memory?
 */
static
void *
thread_malloc(size_t s)
{
	return gc_malloc(s, KGC_ALLOC_THREADCTX);
}

static
void
thread_free(void *p)
{
	gc_free(p);
}

static
void *
thread_realloc(void *p, size_t s)
{
	return gc_realloc(p, s, KGC_ALLOC_THREADCTX);
}

static void
linkNativeAndJavaThread(jthread_t thread, Hjava_lang_VMThread *jlThread)
{
	threadData *thread_data = jthread_get_data(thread);

	thread_data->jlThread = jlThread;
	unhand (jlThread)->jthreadID = (struct Hkaffe_util_Ptr *)thread;

	thread_data->jniEnv = &Kaffe_JNINativeInterface;

	thread_data->needOnStack = STACK_HIGH; 
}

/*
 * Destroys current thread's heavy lock and resets jniEnv. Called from
 * the threading implementation before a thread is destroyed or reused.
 */
void
KaffeVM_unlinkNativeAndJavaThread()
{
	jthread_t thread = jthread_current();
	threadData *thread_data = jthread_get_data(thread);

	thread_data->jniEnv = 0;

	ksemDestroy (&thread_data->sem);
}

/*
 * Initialise threads.
 */
void
initThreads(void)
{
	errorInfo info;

	DBG(INIT, dprintf("initThreads()\n"); )

	/* Get a handle on the thread and thread group classes */
	ThreadClass = lookupClass(THREADCLASS, NULL, &info);
	assert(ThreadClass != 0);
	VMThreadClass = lookupClass(VMTHREADCLASS, NULL, &info);
	assert(VMThreadClass != 0);
	ThreadGroupClass = lookupClass(THREADGROUPCLASS, NULL, &info);
	assert(ThreadGroupClass != 0);

	/* Create base group */
	standardGroup = (struct Hjava_lang_ThreadGroup*)
	  execute_java_constructor(NULL, NULL,
				   ThreadGroupClass, "()V");

	assert(standardGroup != 0);

	/* Allocate a thread to be the main thread */
	attachFakedThreadInstance("main", false);

	DBG(INIT, dprintf("initThreads() done\n"); )
}


static jthread_t
createThread(Hjava_lang_VMThread* vmtid, void (*func)(void *), size_t stacksize,
	     struct _errorInfo *einfo)
{
	jthread_t nativeThread;
	Hjava_lang_Thread* tid = unhand(vmtid)->thread;

	nativeThread = 
	  jthread_create(((unsigned char)unhand(tid)->priority),
			 func,
			 unhand(tid)->daemon,
			 vmtid,
			 stacksize);

	if (nativeThread == NULL) {
		postOutOfMemory(einfo);
		return 0;
	}

	return nativeThread;
}

/*
 * Start a new thread running.
 */
void
startThread(Hjava_lang_VMThread* tid)
{
	jthread_t nativeTid;
	struct _errorInfo info;
	int iLockRoot;

	/* Hold the start lock while the thread is created.
	 * This lock prevents the new thread from running until we're
	 * finished in create.
	 * See also firstStartThread.
	 */
	lockStaticMutex(&thread_start_lock);

	nativeTid = createThread(tid, &firstStartThread,
				 threadStackSize, &info);

	linkNativeAndJavaThread (nativeTid, tid);
	
	unlockStaticMutex(&thread_start_lock);

	if (nativeTid == NULL) {
		throwError(&info);
	}
}

/*
 * Interrupt a thread
 */
void
interruptThread(Hjava_lang_VMThread* tid)
{
	if ((jthread_t)unhand(tid)->jthreadID) {
		jthread_interrupt((jthread_t)unhand(tid)->jthreadID);
	}
}

/*
 * Stop a thread from running and terminate it.
 */
void
stopThread(Hjava_lang_VMThread* tid, Hjava_lang_Object* obj)
{
	if (getCurrentThread() == unhand(tid)->thread) {
		throwException((Hjava_lang_Throwable*)obj);
	}
	else {
		/*
		 * Note that we deviate from the spec here in that the target 
		 * thread won't throw the exception `obj', but it will 
		 * construct a new ThreadDeath exception when it dies.
		 */
		if ((jthread_t)unhand(tid)->jthreadID)
			jthread_stop((jthread_t)unhand(tid)->jthreadID);
	}
}

/*
 * Create the initial thread with a given name.
 *  We should only ever call this once.
 */
void
attachFakedThreadInstance(const char* nm, int isDaemon)
{
	Hjava_lang_Thread* tid;

	DBG(VMTHREAD, dprintf("attachFakedThreadInstance(%s)\n", nm); )

	/* Allocate a thread to be the main thread */
	tid = (Hjava_lang_Thread*)newObject(ThreadClass);
	assert(tid != 0);

	unhand(tid)->name = stringC2Java(nm);
	assert(unhand(tid)->name != NULL);
	unhand(tid)->priority = java_lang_Thread_NORM_PRIORITY;
	unhand(tid)->daemon = isDaemon;
	unhand(tid)->group = standardGroup;
	unhand(tid)->runnable = NULL;
	unhand(tid)->vmThread = (Hjava_lang_VMThread *)
	  execute_java_constructor(NULL, NULL,
				   VMThreadClass, "(Ljava/lang/Thread;)V",
				   tid);

	/* set Java thread associated with main thread */
	linkNativeAndJavaThread (jthread_current(), unhand(tid)->vmThread);

        /*
	 * set context class loader of primordial thread to app classloader
	 * must not be done earlier, since getCurrentThread() won't work
         * before the jthread_createfirst and the jthreadID assignment
	 */
        unhand(tid)->contextClassLoader = (struct Hjava_lang_ClassLoader *)
	  do_execute_java_class_method ("kaffe/lang/AppClassLoader",
					NULL,
					"getSingleton",
					"()Ljava/lang/ClassLoader;").l;

	/* Attach thread to threadGroup */
	do_execute_java_method(unhand(tid)->group, "addThread", "(Ljava/lang/Thread;)V", 0, 0, tid);

	DBG(VMTHREAD, dprintf("attachFakedThreadInstance(%s)=%p done\n", nm, tid); )
}

/*
 * helper function to start gc thread
 */
static
void
startSpecialThread(void *arg UNUSED)
{
	void (*func)(void *);
	void *argument;
	int iLockRoot;

	ksemInit(&THREAD_DATA()->sem);

	lockStaticMutex(&thread_start_lock);
	signalStaticCond(&thread_start_lock);
	unlockStaticMutex(&thread_start_lock);

	func = (void *)THREAD_DATA()->exceptPtr;
	THREAD_DATA()->exceptPtr = NULL;

	argument = (void *)THREAD_DATA()->exceptObj;
	THREAD_DATA()->exceptObj = NULL;

	func(argument);
}

/*
 * Start a daemon thread, such as a gc or finalizer thread.
 * We give these threads a java incarnation for consistency.
 * It might not be strictly necessary for the gc thread.
 *
 * This guy can fail because of allocation failure, or because
 * createThread failed.
 * XXX
 */
Hjava_lang_Thread*
createDaemon(void* func, const char* nm, void *arg, int prio,
	     size_t stacksize, struct _errorInfo *einfo)
{
  Hjava_lang_Thread* tid;
  Hjava_lang_VMThread *vmtid;
  jthread_t nativeTid;
  int iLockRoot;
  Hjava_lang_String* name;

DBG(VMTHREAD,	dprintf("createDaemon %s\n", nm);	)
  
  /* Keep daemon threads as root objects */
  vmtid = (Hjava_lang_Thread*)newObject(VMThreadClass);
  assert(vmtid != 0);
  
  name = stringC2Java(nm);
  if (!name) {
    postOutOfMemory(einfo);
    return 0;
  }
  tid = (Hjava_lang_Thread *)
    execute_java_constructor(NULL, NULL,
			     ThreadClass, "(Ljava/lang/VMThread;Ljava/lang/String;IZ)V",
			     vmtid, name, prio, true);
  unhand(vmtid)->thread = tid;
  unhand(vmtid)->running = true;

  unhand(tid)->contextClassLoader = (struct Hjava_lang_ClassLoader *)
    do_execute_java_class_method ("java/lang/ClassLoader",
				  NULL,
				  "getSystemClassLoader",
				  "()Ljava/lang/ClassLoader;").l;
  
  lockStaticMutex(&thread_start_lock);
  
  nativeTid = createThread(vmtid, startSpecialThread, stacksize, einfo);
  
  linkNativeAndJavaThread (nativeTid, vmtid);
  
  jthread_get_data(nativeTid)->exceptPtr = func;
  jthread_get_data(nativeTid)->exceptObj = arg;

  waitStaticCond(&thread_start_lock, (jlong)0);
  
  unlockStaticMutex(&thread_start_lock);
  
  return (tid);
}

/*
 * All threads start here.
 */
static
void
firstStartThread(void* arg UNUSED)
{
	Hjava_lang_VMThread* tid;
	jthread_t cur;
	JNIEnv *env;
	jmethodID runmethod;
	int iLockRoot;

	cur = jthread_current();

	ksemInit(&jthread_get_data(cur)->sem);
 
	lockStaticMutex(&thread_start_lock);
	unlockStaticMutex(&thread_start_lock);

	tid = (Hjava_lang_VMThread *)(jthread_get_data(cur)->jlThread);
	env = &jthread_get_data(cur)->jniEnv;

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_THREAD_START) )
	{
		JVMPI_Event ev;

		jvmpiFillThreadStart(&ev, tid);
		jvmpiPostEvent(&ev);
		KFREE(ev.u.thread_start.parent_name);
		KFREE(ev.u.thread_start.group_name);
		KFREE(ev.u.thread_start.thread_name);
	}
#endif

DBG(VMTHREAD,	
	dprintf("firstStartThread %p\n", tid);		
    )

	/*
	 * We use JNI here to make sure the stack is unrolled when we get
	 * into the uncaughtException handler.  Otherwise, we wouldn't be
	 * able to handle StackOverflowError.
	 */

	/* Find the run()V method and call it */
	runmethod = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, tid),
					"run", "()V");
	if (runmethod != 0)
		(*env)->CallVoidMethod(env, tid, runmethod);

	/*
	 * We may ignore all exceptions here as it is already handled by VMThread.run().
	 */
	(*env)->ExceptionClear(env);
	exitThread();
}

/*
 * Yield process to another thread of equal priority.
 */
void
yieldThread(void)
{
	jthread_yield();
}

/*
 * Change thread priority.
 *
 * We assume the call comes from VMThread and so that the field priority has
 * already been updated. In the other case, there will be a small inconsistency.
 */
void
setPriorityThread(Hjava_lang_VMThread* tid, int prio)
{
	/* no native thread yet */
	if (unhand(tid)->jthreadID == 0)
	        return;

	jthread_setpriority((jthread_t)unhand(tid)->jthreadID, prio);
}

/*
 * Terminate a thread.  This function never returns.
 */
void
exitThread(void)
{
DBG(VMTHREAD,	
	dprintf("exitThread %p\n", getCurrentThread());		
    )

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_THREAD_END) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_THREAD_END;
		jvmpiPostEvent(&ev);
	}
#endif

	/*
	 * This may never return. If it does return, control is returned to
	 * the threading implementation where firstStartThread() is called.
	 *
	 * The threading implementation is responsible for calling
	 * KaffeVM_unlinkNativeAndJavaThread() before killing or
	 * reusing the thread.
	 */
	jthread_exit();
}

/*
 * Get the current JavaVM thread.
 */
Hjava_lang_VMThread*
getCurrentVMThread(void)
{
	return (Hjava_lang_VMThread *) THREAD_DATA()->jlThread;
}

/*
 * Get the current Java thread.
 */
Hjava_lang_Thread*
getCurrentThread(void)
{
	Hjava_lang_VMThread *vmtid;

	vmtid = getCurrentVMThread();
	assert(vmtid != NULL);
	
	return unhand(vmtid)->thread;
}

/*
 * Finalize a thread.
 *  This is to free the native thread context.
 */
void
finalizeThread(Hjava_lang_VMThread* tid)
{
	jthread_t jtid = (jthread_t)unhand(tid)->jthreadID;

	if (jtid != NULL) {
		jthread_destroy(jtid);
	}
}

/* 
 * extract name of a thread as a C string.
 * Returns static buffer.
 */
char *
nameThread(Hjava_lang_VMThread *tid)
{
	static char buf[80];

	stringJava2CBuf(unhand(unhand(tid)->thread)->name, buf, sizeof(buf));

	return buf;
}

static void 
broadcastDeath(void *jlThread)
{
        Hjava_lang_VMThread *vmtid = (Hjava_lang_VMThread *)jlThread;
	Hjava_lang_Thread *tid = unhand(vmtid)->thread;
	int iLockRoot;

        /* Notify on the object just in case anyone is waiting */
        lockMutex(&tid->base);
        broadcastCond(&tid->base);
        unlockMutex(&tid->base);
}

static void NONRETURNING
throwDeath(void)
{
	throwException(ThreadDeath);
}

static
void
runfinalizer(void)
{
	DBG (VMTHREAD, dprintf ("shutting down %p\n", THREAD_DATA()->jlThread); )

	if (THREAD_DATA()->jlThread == 0) {
		/* if the thread executing the shutdown hooks doesn't have a
		 * java.lang.Thread instance or it was gc'ed in the mean time,
		 * create a new one.
		 */
		attachFakedThreadInstance("main", false);
	}

	/* Do java-land cleanup */
	do_execute_java_method(SystemClass, "exitJavaCleanup",
			       "()V", NULL, true);
	if (runFinalizerOnExit) {
		invokeFinalizer();
	}

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JVM_SHUT_DOWN) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_JVM_SHUT_DOWN;
		jvmpiPostEvent(&ev);
	}
#endif
}

/*
 * print info about a java thread.
 */
static void
dumpJavaThread(jthread_t thread, UNUSED void *p)
{
	Hjava_lang_VMThread *tid = (Hjava_lang_VMThread *)jthread_get_data(thread)->jlThread;
	dprintf("`%s' ", nameThread(tid));
	jthread_dumpthreadinfo(thread);
	dprintf("\n");
}

static void
dumpThreads(void)
{
	dprintf("Dumping live threads:\n");
	jthread_walkLiveThreads(dumpJavaThread, NULL);
}

/*
 * Return the name of a java thread, given its native thread pointer.
 */
char*
nameNativeThread(void* native_data)
{
	return nameThread((Hjava_lang_VMThread*)
		jthread_get_data((jthread_t)native_data)->jlThread);
}

/*
 * Invoked when threading system detects a deadlock.
 */
static void
onDeadlock(void)
{
	if (!deadlockDetection) {
		return;
	}
#if defined(JTHREAD_RESTORE_FD)
        jthreadRestoreFD(2);
#endif
	dumpLocks();
	dumpThreads();
	dprintf("Deadlock: all threads blocked on internal events\n");
	fflush(stderr);
	ABORT();
}

void
initNativeThreads(int nativestacksize)
{
	threadData *thread_data;
	rlim_t stackSize;

	DBG(INIT, dprintf("initNativeThreads(0x%x)\n", nativestacksize); )

	/* Even though the underlying operating or threading system could
	 * probably extend the main thread's stack, we must impose this 
	 * artificial boundary, because otherwise we wouldn't be able to 
	 * catch stack overflow exceptions thrown by the main thread.
	 */
	threadStackSize = nativestacksize;
	jthread_init(
		DBGEXPR(JTHREADNOPREEMPT, false, true),
		java_lang_Thread_MAX_PRIORITY+1,
		java_lang_Thread_MIN_PRIORITY,
		thread_malloc,
		thread_free,
		thread_realloc,
		broadcastDeath,
		throwDeath,
		onDeadlock);

	jthread_atexit(runfinalizer);

	/*
	 * Doing jthread_createfirst as early as possible has several advantages:
	 *
	 * 	- we can rely on a working jthread_current() and jthread_get_data()
	 *	  everywhere else in the vm, no need to specialcase anything
	 *
	 *	- catching exceptions during the initialisation is easier now
	 *
	 * Since everything is stored in the threadData struct now, we can simply
	 * attach a faked java.lang.Thread instance later on.
	 */
#if defined(KAFFEMD_STACKSIZE)
	stackSize = mdGetStackSize();

	if (stackSize == KAFFEMD_STACK_ERROR)
	  {
	    fprintf(stderr, "WARNING: Impossible to retrieve the real stack size\n");
	    fprintf(stderr, "WARNING: You may experience deadlocks\n");
	  }
	else if (stackSize == KAFFEMD_STACK_INFINITE || stackSize >= threadStackSize)
	  {
	    mdSetStackSize(threadStackSize);
	    /* Last chance. We check whether the size has really been updated. */
	    stackSize = mdGetStackSize();
	  }
	else
	  {
	    fprintf(stderr, 
	            "NOTE: It is impossible to set the main thread stack\n"
		    "NOTE: size because the system stack size is too low\n");
	  }
#else
	stackSize = MAINSTACKSIZE;
#endif
	DBG(INIT, dprintf("Detected stackSize %lu\n", stackSize); )
	jthread_createfirst(stackSize, (unsigned char)java_lang_Thread_NORM_PRIORITY, 0);

	/*
	 * initialize some things that are absolutely necessary:
	 *    - the semaphore
	 *    - the jniEnv
         */
	thread_data = THREAD_DATA(); 

	ksemInit(&thread_data->sem);

	thread_data->jniEnv = &Kaffe_JNINativeInterface;

	DBG(INIT, dprintf("initNativeThreads(0x%x) done\n", nativestacksize); )
}
