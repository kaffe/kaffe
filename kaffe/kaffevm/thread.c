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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif

#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "jni_md.h"
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
jboolean runFinalizerOnExit;	/* should we run finalizers? */

static jboolean deadlockDetection = 1;	/* abort if we detect deadlock */

/**
 * Turn deadlock detection on or off.
 *
 * @param detect 0 to turn detection off, !0 to turn it on.
 */
void 
KaffeVM_setDeadlockDetection(jboolean detect) 
{
  deadlockDetection = detect;
}

Hjava_lang_Class* ThreadClass;
Hjava_lang_Class* VMThreadClass;
Hjava_lang_Class* ThreadGroupClass;

static void firstStartThread(void*);
static void runfinalizer(void);

static void
linkNativeAndJavaThread(jthread_t thread, Hjava_lang_VMThread *jlThread)
{
	threadData *thread_data = KTHREAD(get_data)(thread);

	thread_data->jlThread = jlThread;
	unhand (jlThread)->vmdata = (struct Hkaffe_util_Ptr *)thread;

	thread_data->jnireferences = NULL;
	thread_data->jniEnv = &Kaffe_JNINativeInterface;

	thread_data->needOnStack = STACK_HIGH; 
}

/*
 * Destroys current thread's heavy lock and resets jniEnv. Called from
 * the threading implementation before a thread is destroyed or reused.
 */
void
KaffeVM_unlinkNativeAndJavaThread(void)
{
	jthread_t thread = KTHREAD(current)();
	threadData *thread_data = KTHREAD(get_data)(thread);

	thread_data->jniEnv = NULL;

	KSEM(destroy) (&thread_data->sem);
}

/*
 * Initialise threads.
 */
void
initThreads(void)
{
	errorInfo info;

	DBG(INIT, dprintf("initThreads()\n"); );

	/* Get a handle on the thread and thread group classes */
	ThreadClass = lookupClass(THREADCLASS, NULL, &info);
	assert(ThreadClass != 0);
	VMThreadClass = lookupClass(VMTHREADCLASS, NULL, &info);
	assert(VMThreadClass != 0);
	ThreadGroupClass = lookupClass(THREADGROUPCLASS, NULL, &info);
	assert(ThreadGroupClass != 0);

	/* Allocate a thread to be the main thread */
	KaffeVM_attachFakedThreadInstance("main", false);

	DBG(INIT, dprintf("initThreads() done\n"); );
}


static jthread_t
createThread(Hjava_lang_VMThread* vmtid, void (*func)(void *), void *arg,
	     size_t stacksize,
	     struct _errorInfo *einfo)
{
	jthread_t nativeThread;
	Hjava_lang_Thread* tid = unhand(vmtid)->thread;

	nativeThread = 
	  KTHREAD(create)(((unsigned char)unhand(tid)->priority),
			 func,
			 unhand(tid)->daemon,
			 arg,
			 stacksize);

	if (nativeThread == NULL) {
		postOutOfMemory(einfo);
		return NULL;
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

DBG(VMTHREAD, dprintf ("%p starting thread %p (vmthread %p)\n\n", KTHREAD(current)(), unhand(tid)->thread, tid); );

	/* Hold the start lock while the thread is created.
	 * This lock prevents the new thread from running until we're
	 * finished in create.
	 * See also firstStartThread.
	 */
	nativeTid = createThread(tid, &firstStartThread,
				 KTHREAD(current)(),
				 threadStackSize, &info);
	if (nativeTid == NULL) {
		throwError(&info);
	}
	KSEM(get)(&THREAD_DATA()->sem, (jlong)0);

	linkNativeAndJavaThread (nativeTid, tid);

	KSEM(put)(&KTHREAD(get_data)(nativeTid)->sem);
}

/*
 * Interrupt a thread
 */
void
interruptThread(Hjava_lang_VMThread* tid)
{
DBG(VMTHREAD, dprintf ("%p (%p) interrupting %p (%p)\n", KTHREAD(current)(),
                       THREAD_DATA()->jlThread, unhand(tid)->vmdata, tid); );
	assert(unhand(tid)->vmdata != NULL);

	KTHREAD(interrupt)((jthread_t)unhand(tid)->vmdata);
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
		if ((jthread_t)unhand(tid)->vmdata)
			KTHREAD(stop)((jthread_t)unhand(tid)->vmdata);
	}
}

/**
 * Detach this thread from the VM.
 * This must not be called only when the thread has left the VM
 * completely (e.g. the native code must not return in the interpreter/JIT
 * after that).
 */
void KaffeVM_detachCurrentThread()
{
  Hjava_lang_Thread* tid = getCurrentThread();

  DBG(VMTHREAD, dprintf("detachThreadInstance(%p, %s)\n", tid, nameThread(unhand(tid)->vmThread)); );

  KaffeVM_unlinkNativeAndJavaThread();
}

/**
 * Attach an external thread to the VM.
 * We should only ever call this once for each external thread.
 * (Note: All assertions on that must have been checked before)
 */
void
KaffeVM_attachFakedThreadInstance(const char* nm, int isDaemon)
{
	Hjava_lang_Thread* tid;
	jvalue retval;
	int i;

	DBG(VMTHREAD, dprintf("attachFakedThreadInstance(%s)\n", nm); );

	/* Allocate a thread to be the main thread */
	tid = (Hjava_lang_Thread*)newObject(ThreadClass);
	assert(tid != 0);

	unhand(tid)->name = stringC2Java(nm);
	assert(unhand(tid)->name != NULL);
	unhand(tid)->priority = java_lang_Thread_NORM_PRIORITY;
	unhand(tid)->daemon = isDaemon;

	/* use root group for this thread:
	 * loop over all static fields of java.lang.ThreadGroup ...
	 */
	for (i=0; i<CLASS_NSFIELDS(ThreadGroupClass); i++)
	{
		Field* f = &CLASS_SFIELDS(ThreadGroupClass)[i];

		/* ... and if it's a field called root, take it */
		if (!strcmp (f->name->data, "root"))
		{
			unhand(tid)->group = * (jref *)FIELD_ADDRESS(f);
		}
	}
	/* finally complain if we did not find the field */
	assert(unhand(tid)->group != NULL);

	unhand(tid)->runnable = NULL;
	unhand(tid)->vmThread = (Hjava_lang_VMThread *)
	  execute_java_constructor(NULL, NULL,
				   VMThreadClass, "(Ljava/lang/Thread;)V",
				   tid);

	/* set Java thread associated with main thread */
	linkNativeAndJavaThread (KTHREAD(current)(), unhand(tid)->vmThread);

        /*
	 * set context class loader of primordial thread to app classloader
	 * must not be done earlier, since getCurrentThread() won't work
         * before the KTHREAD(createfirst) and the vmdata assignment.
	 *
	 * If we're creating the Thread instance of the main thread, this
	 * will trigger the initialization process of the java part of the
	 * runtime. One pitfall during initialization is that java.lang.System
	 * has to be initialized before kaffe.lang.AppClassLoader. Therefore,
	 * we cannot call AppClassLoader.getSingleton() here, since that would
	 * cause System and AppClassLoader to be initialized in the wrong order.
	 */
	do_execute_java_class_method (&retval, "java/lang/ClassLoader",
				      NULL,
				      "getSystemClassLoader",
				      "()Ljava/lang/ClassLoader;");
        unhand(tid)->contextClassLoader = (struct Hjava_lang_ClassLoader *) retval.l;

	/* Attach thread to threadGroup */
	do_execute_java_method(NULL, unhand(tid)->group, "addThread", "(Ljava/lang/Thread;)V", NULL, 0, tid);

	DBG(VMTHREAD, dprintf("attachFakedThreadInstance(%s)=%p done\n", nm, tid); );
}

/*
 * helper function to start gc thread
 */
static
void
startSpecialThread(void *arg)
{
	void (*func)(void *);
	void **pointer_args = (void **)arg;
	void *argument;
	jthread_t calling_thread;
	threadData *thread_data = THREAD_DATA();

	KSEM(init)(&thread_data->sem);

	/* We save the value before the lock so we are sure
	 * pointer_args is still a valid pointer on the stack.
	 */
	func = (void(*)(void*))pointer_args[0];
	argument = pointer_args[1];
	calling_thread = (jthread_t) pointer_args[2];

	/* Thread started and arguments retrieved. */
	KSEM(put)(&KTHREAD(get_data)(calling_thread)->sem);
	/* We have now to wait the parent to synchronize the data
	 * and link the thread to the Java VM.
	 */
	KSEM(get)(&thread_data->sem, (jlong)0);

	thread_data->exceptObj = NULL;

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
  Hjava_lang_String* name;
  void *specialArgument[3];
  jvalue retval;

DBG(VMTHREAD,	dprintf("createDaemon %s\n", nm);	);
  
  /* Keep daemon threads as root objects */
  vmtid = (Hjava_lang_VMThread*)newObject(VMThreadClass);
  assert(vmtid != NULL);
  
  name = stringC2Java(nm);
  if (!name) {
    postOutOfMemory(einfo);
    return NULL;
  }
  tid = (Hjava_lang_Thread *)
    execute_java_constructor(NULL, NULL,
			     ThreadClass, "(Ljava/lang/VMThread;Ljava/lang/String;IZ)V",
			     vmtid, name, prio, true);
  unhand(vmtid)->thread = tid;
  unhand(vmtid)->running = true;

    do_execute_java_class_method (&retval, "java/lang/ClassLoader",
				  NULL,
				  "getSystemClassLoader",
				  "()Ljava/lang/ClassLoader;");
  unhand(tid)->contextClassLoader = (struct Hjava_lang_ClassLoader *) retval.l;
  
  specialArgument[0] = func;
  specialArgument[1] = arg;
  specialArgument[2] = KTHREAD(current)();
  
  nativeTid = 
    KTHREAD(create)((unhand(tid)->priority),
		   startSpecialThread,
		   true,
		   specialArgument,
		   stacksize);
  
  if (nativeTid == NULL) {
    postOutOfMemory(einfo);
    return NULL;
  }

  KTHREAD(get_data)(nativeTid)->exceptPtr = NULL;
  KTHREAD(get_data)(nativeTid)->exceptObj = NULL;

  KSEM(get)(&THREAD_DATA()->sem, (jlong)0);
  
  linkNativeAndJavaThread (nativeTid, vmtid);

  KSEM(put)(&KTHREAD(get_data)(nativeTid)->sem);
  
  return (tid);
}

/*
 * All threads start here.
 */
static
void
firstStartThread(void* arg)
{
	Hjava_lang_VMThread* tid;
	jthread_t cur;
	JNIEnv *env;
	jmethodID runmethod;
	jthread_t calling_thread = (jthread_t) arg;
	threadData *thread_data;

	cur = KTHREAD(current)();
	thread_data = KTHREAD(get_data)(cur);

	KSEM(init)(&thread_data->sem);

	/* We acknowledge the parent thread that this thread has been started. */
	KSEM(put)(&KTHREAD(get_data)(calling_thread)->sem);
	/* Now we must wait the parent to link the thread to the Java VM. */
	KSEM(get)(&thread_data->sem, (jlong)0);
 
	tid = (Hjava_lang_VMThread *)(thread_data->jlThread);
	env = &thread_data->jniEnv;

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_THREAD_START) )
	{
		JVMPI_Event ev;

		jvmpiFillThreadStart(&ev, tid);
		jvmpiPostEvent(&ev);
		jvmpiCleanupThreadStart(&ev);
	}
#endif

DBG(VMTHREAD,	
	dprintf("%p (%p) firstStartThread\n", cur, tid);		
    );

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
	KTHREAD(yield)();
}

/*
 * Change thread priority.
 *
 * We assume the call comes from VMThread and so that the field priority has
 * already been updated. In the other case, there will be a small inconsistency.
 */
void
setPriorityThread(Hjava_lang_VMThread* tid, jint prio)
{
	/* no native thread yet */
	if (unhand(tid)->vmdata == 0)
	        return;

	KTHREAD(setpriority)((jthread_t)unhand(tid)->vmdata, prio);
}

/*
 * Terminate a thread.  This function never returns.
 */
void
exitThread(void)
{
DBG(VMTHREAD,	
	dprintf("%p (%p) exitThread\n", KTHREAD(current)(), THREAD_DATA()->jlThread);
    );

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
	KTHREAD(exit)();
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
	jthread_t jtid = (jthread_t)unhand(tid)->vmdata;

	if (jtid != NULL) {
		KTHREAD(destroy)(jtid);
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

	if (tid != NULL && unhand(tid)->thread != NULL && unhand(unhand(tid)->thread)->name != NULL)
	  stringJava2CBuf(unhand(unhand(tid)->thread)->name, buf, sizeof(buf));
	else
	  strcpy(buf, "<null name>");

	return buf;
}

static void 
broadcastDeath(void *jlThread)
{
        Hjava_lang_VMThread *vmtid = (Hjava_lang_VMThread *)jlThread;
	Hjava_lang_Thread *tid = unhand(vmtid)->thread;

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

static
void
runfinalizer(void)
{
	DBG (VMTHREAD, dprintf ("shutting down %p\n", THREAD_DATA()->jlThread); );

	if (THREAD_DATA()->jlThread == 0) {
		/* if the thread executing the shutdown hooks doesn't have a
		 * java.lang.Thread instance or it was gc'ed in the mean time,
		 * create a new one.
		 */
		KaffeVM_attachFakedThreadInstance("main", false);
	}

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
DBG(VMTHREAD,
	Hjava_lang_VMThread *tid = (Hjava_lang_VMThread *)KTHREAD(get_data)(thread)->jlThread;
	dprintf("`%s' ", nameThread(tid));
	KTHREAD(dumpthreadinfo)(thread);
	dprintf("\n");
);
}

static void
dumpThreads(void)
{
DBG(VMTHREAD,
	dprintf("Dumping live threads:\n");
	KTHREAD(walkLiveThreads_r)(dumpJavaThread, NULL);
);
}

/*
 * Return the name of a java thread, given its native thread pointer.
 */
char*
nameNativeThread(void* native_data)
{
	return nameThread((Hjava_lang_VMThread*)
		KTHREAD(get_data)((jthread_t)native_data)->jlThread);
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
DBG(VMTHREAD,
	dprintf("Deadlock: all threads blocked on internal events\n");
);
	fflush(stderr);
	KAFFEVM_ABORT();
}

void
initNativeThreads(int nativestacksize)
{
	threadData *thread_data;
	rlim_t stackSize;

	DBG(INIT, dprintf("initNativeThreads(0x%x)\n", nativestacksize); );

	/* Even though the underlying operating or threading system could
	 * probably extend the main thread's stack, we must impose this 
	 * artificial boundary, because otherwise we wouldn't be able to 
	 * catch stack overflow exceptions thrown by the main thread.
	 */
	threadStackSize = nativestacksize;
	KTHREAD(init)(
		DBGEXPR(JTHREADNOPREEMPT, false, true),
		java_lang_Thread_MAX_PRIORITY+1,
		java_lang_Thread_MIN_PRIORITY,
		main_collector,
		broadcastDeath,
		throwDeath,
		onDeadlock);

	KTHREAD(atexit)(runfinalizer);

	/*
	 * Doing KTHREAD(createfirst) as early as possible has several advantages:
	 *
	 * 	- we can rely on a working KTHREAD(current)() and KTHREAD(get_data)()
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
	DBG(INIT, dprintf("Detected stackSize %zu\n", (size_t)stackSize); );
	KTHREAD(createfirst)((size_t)stackSize, (unsigned char)java_lang_Thread_NORM_PRIORITY, NULL);

	/*
	 * initialize some things that are absolutely necessary:
	 *    - the semaphore
	 *    - the jniEnv
         */
	thread_data = THREAD_DATA(); 

	KSEM(init)(&thread_data->sem);

	thread_data->jnireferences = NULL;
	thread_data->jniEnv = &Kaffe_JNINativeInterface;

	DBG(INIT, dprintf("initNativeThreads(0x%x) done\n", nativestacksize); );
}
