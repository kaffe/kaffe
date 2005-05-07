/*
 * Copyright (c) 1998-2000 The University of Utah.
 * All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */
/*
 * This file implements jthreads on top of OSKit pthreads.
 */

#include "debug.h"
#include "jthread.h"
#include <sys/wait.h>
#include <sys/time.h>
#include <oskit/dev/dev.h>
#include <signal.h>

#ifdef CPU_INHERIT
#include <jcpuinherit.h>
#endif

/* thread status */
#define THREAD_NEWBORN                	0
#define THREAD_RUNNING                  1
#define THREAD_DYING                    2
#define THREAD_DEAD                     3
#define THREAD_STOPPED                  4 /* XXX only used in status dump! */
#define THREAD_CONTINUE                 5 /* XXX only used in status dump! */

#define THREAD_FLAG_DONTSTOP			1

/*
 * Variables.
 * These should be kept static to ensure encapsulation.
 */
static int talive; 		/* number of threads alive */
static int tdaemon;		/* number of daemons alive */
static void (*runOnExit)(void);	/* function to run when all non-daemon die */

static struct jthread* liveThreads;	/* list of all live threads */

/* static lock to protect liveThreads etc. */
static pthread_mutex_t threadLock;

static void remove_thread(jthread_t tid);
static void mark_thread_dead(void);
void dumpLiveThreads(int);

/*
 * the following variables are set by jthread_init, and show how the
 * threading system is parametrized.
 */
static void *(*allocator)(size_t); 	/* malloc */
static void (*deallocator)(void*);	/* free */
static void (*destructor1)(void*);	/* call when a thread exits */
static void (*onstop)(void);		/* call when a thread is stopped */
static int  max_priority;		/* maximum supported priority */
static int  min_priority;		/* minimum supported priority */

pthread_key_t	cookie_key;	/* key to map pthread -> Hjava_lang_Thread */
pthread_key_t	jthread_key;	/* key to map pthread -> jthread */

/*
 * Function declarations.
 */

/*============================================================================
 *
 * Functions related to interrupt handling
 *
 */

/*
 * Handle the stop signal to effect a Thread.stop().  This handler is
 * called when that thread is killed.
 */
static void
catch_death(void)
{
	jthread_t tid = GET_JTHREAD();

	if (!(tid->flags & THREAD_FLAG_DONTSTOP)) {
		onstop();
		jthread_exit();
	} else {
		/* try {} catch(Error e) may save a dying thread.
		 * We only mark a thread as dying between catching the
		 * signal and calling the onstop handler
		 */
		tid->status = THREAD_DYING;
	}

}

static void 
catch_int(void)
{
    /* do nothing, we will get a spurious wakeup out of cond_wait or
       sleep, and the java code will magically throw
       InterruptedException */
}

#if !defined(SMP)
/*
 * On the uniprocessor OSKit, stopping all threads as well as acquiring
 * a spinlock means to simply disable interrupts.  Simple enough.
 */
/*
 * disable interrupts
 */
void 
jthread_suspendall(void)
{
	osenv_intr_disable();
}

/*
 * restore interrupts
 */
void
jthread_unsuspendall(void)
{
	osenv_intr_enable();
}

void 
/* ARGSUSED */
jthread_spinon(void *arg)
{
	osenv_intr_disable();
}

void 
/* ARGSUSED */
jthread_spinoff(void *arg)
{
	osenv_intr_enable();
}

#else /* SMP */

/*
 * On the SMP OSKit, spinlocks will be spinlocks and Leigh will invent
 * a way to stop all processors
 */
#error Not yet.
#endif

/*
 * These functions use the OSKit specific "pthread_getstate" extension
 * to obtain the current stack location & stack pointer.
 */
/*
 * determine the interesting stack range for a conservative gc
 */
int
jthread_extract_stack(jthread_t jtid, void **from, unsigned *len)
{
	struct pthread_state ps;

	/*
	 * Newborn/dead threads don't have a useful stack, and may not
	 * have a native_thread.
	 */
	if ((jtid->status == THREAD_NEWBORN)
	    || (jtid->status == THREAD_DEAD))
		return 0;

	if (oskit_pthread_getstate(jtid->native_thread, &ps)) {
		 panic("jthread_extract_stack: oskit_pthread_getstate failed for jtid(%p)\n",
		       jtid);
	}
	
#if defined(STACK_GROWS_UP)
#error FIXME
#else
	*from = (void *)ps.stackptr;
	*len = ps.stackbase + ps.stacksize - ps.stackptr;
	if (*len < 0 || *len > (256*1024)) {
	    panic("(%d) oskit_pthread_getstate(%d) reported obscene numbers: "
		  "base = 0x%x, sp = 0x%x\n", 
		  pthread_self(),
		  jtid->native_thread,
		  ps.stackbase, ps.stackptr);
	    exit(-1);
	}
#endif
DBG(JTHREAD,
	dprintf("extract_stack(%) base=%p size=%d sp=%p; from=%p len=%d\n", 
		jtid->native_thread,
		ps.stackbase, ps.stacksize, ps.stackptr, *from, *len);
    )
	return (1);
}

/*
 * determine whether an address lies on your current stack frame
 */
int
jthread_on_current_stack(void *bp)
{
	struct pthread_state ps;
	int rc;

	rc = oskit_pthread_getstate(pthread_self(), &ps);
	if (rc != 0)
	{
		panic("jthread_on_current_stack: oskit_pthread_getstate(pid %d) failed (rc=%#x)",
		      pthread_self(), rc);
	}

        rc = (uint32)bp >= ps.stackbase && 
	     (uint32)bp < ps.stackbase + ps.stacksize;
DBG(JTHREAD,
	dprintf("on current stack(%d) base=%p size=%d bp=%p %s\n",
		pthread_self(),
		ps.stackbase, ps.stacksize, bp, (rc ? "yes" : "no"));
    )
	return rc;
}       


/*
 * See if there is enough room on the stack.
 */
int
jthread_stackcheck(int need)
{
	struct pthread_state ps;
	int room;
	pthread_t tid = pthread_self();

	if (oskit_pthread_getstate(tid, &ps))
		panic("jthread_stackcheck: oskit_pthread_getstate(%d)",
		      (int)tid);
	
#if defined(STACK_GROWS_UP)
#	error FIXME
#else
	room = (ps.stackptr - ps.stackbase);
#endif
	
DBG(JTHREAD,
	dprintf("stackcheck(%d) need=%d base=%p size=%d sp=%p room=%d\n",
		(int)pthread_self(),
		need, ps.stackbase, ps.stacksize, ps.stackptr, room);
    )
	return (room >= need);
}

/*============================================================================
 *
 * Functions dealing with thread contexts and the garbage collection interface
 *
 */

/*
 * free a thread context
 */
void    
jthread_destroy(jthread_t tid)
{
	void *status;

	assert(tid);
DBG(JTHREAD, 
	dprintf("destroying tid %d\n", tid->native_thread);	
    )
#ifdef newer_than_990722
	/* We can't use join here because of a bug in main thread
	 * initialization.  See the comment in jthread_exit.  This
	 * doesn't need to be synchronous anyway?
	 */
	jthread_disable_stop();
	pthread_join(tid->native_thread, &status);
	jthread_enable_stop();
#endif
	deallocator(tid);
}

/*
 * find a native pthread's cookie
 * (Maps pthreads to java.lang.Threads)
 */
void* 
jthread_getcookie(void* ntid)
{
	struct jthread* tid;

        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
		if ((void*)tid->native_thread == ntid) {
			return (tid->jlThread);
		}
	}
	return (0);
}

/*
 * iterate over all live threads
 */
void
jthread_walkLiveThreads(void (*func)(void *jlThread))
{
        jthread_t tid;

	pthread_mutex_lock(&threadLock);
        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
                func(tid->jlThread);
        }
	pthread_mutex_unlock(&threadLock);
}

/* 
 * XXX this is supposed to count the number of stack frames 
 */
int
jthread_frames(jthread_t thrd)
{
        return 0;
}

/*============================================================================
 *
 * Functions for initialization and thread creation
 *
 */

/*
 * Initialize the threading system.
 *
 * XXX: pthread_init() has already been called.
 */
jthread_t 
jthread_init(int pre,
	int maxpr, int minpr,
	void *(*_allocator)(size_t), 
	void (*_deallocator)(void*),
	void (*_destructor1)(void*),
	void (*_onstop)(void),
	void (*_ondeadlock)(void))		/* ignored for now */
{
	pthread_t	pmain;
	jthread_t	jtid;
	struct sigaction act;

	max_priority = maxpr;
	min_priority = minpr;
	allocator = _allocator;
	deallocator = _deallocator;
	onstop = _onstop;
	destructor1 = _destructor1;

	pmain = pthread_self();

	/* establish SIG_STOP handler */
	act.sa_handler = (void *)catch_death;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIG_STOP);
	sigaction(SIG_STOP, &act, 0);

	act.sa_handler = (void *)catch_int;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIG_INT);
	sigaction(SIG_INT, &act, 0);
	
        /*
         * XXX: ignore mapping of min/max priority for now and assume
         * pthread priorities include java priorities.
         */

        pthread_key_create(&jthread_key, 0 /* destructor */);
        pthread_key_create(&cookie_key, 0 /* destructor */);
	pthread_mutex_init(&threadLock, (const pthread_mutexattr_t *)0);

        jtid = allocator(sizeof (*jtid));

        SET_JTHREAD(jtid);

	jtid->native_thread = pmain;

        jtid->nextlive = liveThreads;
        liveThreads = jtid;
	jtid->status = THREAD_RUNNING;
        talive++;
DBG(JTHREAD,
	dprintf("main thread has id %d\n", jtid->native_thread);
    )

	return (jtid);
}

/*
 * Create an OSKit jthread context to go with the initial thread in the system.
 * (The initial thread is created in jthread_init, above.
 */
jthread_t
jthread_createfirst(size_t mainThreadStackSize, unsigned char prio, void* jlThread)
{
        jthread_t jtid; 

	jtid = GET_JTHREAD();
	assert(jtid != NULL);
	assert(jtid->native_thread != NULL);
	assert(jtid->status == THREAD_RUNNING);

	jtid->jlThread = jlThread;
	
	/* Main thread should not yet have a jlThread associated with it. */
	assert(pthread_getspecific(cookie_key) == NULL);
	pthread_setspecific(cookie_key, jlThread);

	/* XXX what to do with mainThreadStackSize?? */

	jthread_setpriority(jtid, prio);

	return (jtid);
}

/*
 * set a function to be run when all non-daemon threads have exited
 */
void
jthread_atexit(void (*f)(void))
{
	runOnExit = f;
}

/*
 * disallow cancellation
 */
void 
jthread_disable_stop(void)
{
	jthread_t tid = GET_JTHREAD();

	/* XXX is called exactly once before initial thread context is created. */
	if (tid != NULL)
		tid->flags |= THREAD_FLAG_DONTSTOP;
}

/*
 * reallow cancellation and stop if cancellation pending
 */
void 
jthread_enable_stop(void)
{
	jthread_t tid = GET_JTHREAD();
	
	/* XXX is called exactly once before initial thread context is created. */
	if (tid != NULL)
	{
		tid->flags &= ~THREAD_FLAG_DONTSTOP;
		if (tid->status == THREAD_DYING) {
			tid->status = THREAD_RUNNING;
			onstop();
			jthread_exit();
		}
	}
}

/*
 * interrupt a thread
 */
void
jthread_interrupt(jthread_t tid)
{
	if (tid->native_thread != pthread_self())
		pthread_kill(tid->native_thread, SIG_INT);
}

/*
 * start function for each thread.  
 * This function install the cleanup handler, sets jthread-specific 
 * data and calls the actual work function.
 */
void *
start_me_up(void *arg)
{
	jthread_t tid = (jthread_t)arg;

DBG(JTHREAD, 
	dprintf("starting thread %p\n", tid); 
    )
	pthread_mutex_lock(&threadLock);
	SET_JTHREAD(tid);
	SET_COOKIE(tid->jlThread);
        tid->status = THREAD_RUNNING;
	pthread_mutex_unlock(&threadLock);

DBG(JTHREAD, 
	dprintf("calling thread %p, %d\n", tid, tid->native_thread); )
	tid->func(tid->jlThread);

	assert(!"firstStartThread has returned");
#if 0
DBG(JTHREAD, 
	dprintf("thread %d returned, calling jthread_exit\n", 
		tid->native_thread); 
    )

	/* drop onstop handler if that thread is exiting by itself */
	assert (tid->status != THREAD_DYING);

	mark_thread_dead();
	/* by returning, we exit this thread */
#endif
	return 0;
}

/*
 * create a new jthread
 */
jthread_t
jthread_create(unsigned int pri, void (*func)(void *), int daemon,
	       void *jlThread, size_t threadStackSize)
{
	int err;
	pthread_t new;
	jthread_t tid;
	pthread_attr_t attr;
#ifdef CPU_INHERIT
	extern struct JNINativeInterface Kaffe_JNINativeInterface;
	JNIEnv env = &Kaffe_JNINativeInterface;
	pthread_t scheduler = cpui_thread_scheduler(&env, jlThread);
#endif

	pthread_attr_init(&attr);
	/* XXX use setschedparam here */
	pthread_attr_setstacksize(&attr, threadStackSize);
	oskit_pthread_attr_setprio(&attr, pri);

	/* 
	 * Note that we create the thread in a joinable state, which is the
	 * default.  Our finalizer will join the threads, allowing the
	 * pthread system to free its resources.
	 */

#ifdef  CPU_INHERIT
	pthread_attr_setscheduler(&attr, (pthread_t) scheduler);
	pthread_attr_setopaque(&attr, pri);
#endif

        tid = allocator(sizeof (*tid));
        assert(tid != 0);      /* XXX */

	pthread_mutex_lock(&threadLock);
        tid->jlThread = jlThread;
        tid->func = func;

        tid->nextlive = liveThreads;
        liveThreads = tid;
        tid->status = THREAD_NEWBORN;

	err = pthread_create(&new, &attr, start_me_up, tid);
	tid->native_thread = new;

        talive++;       
        if ((tid->daemon = daemon) != 0) {
                tdaemon++;
        }
	pthread_mutex_unlock(&threadLock);
DBG(JTHREAD,
	dprintf("created thread %d, daemon=%d\n", new, daemon); )
#ifdef CPU_INHERIT
DBG(JTHREAD,
        dprintf("scheduler=%d, pri=%d\n", scheduler, pri); )
#endif
        return (tid);
}

/*============================================================================
 *
 * Functions that are part of the user interface
 *
 */

/*      
 * sleep for time milliseconds
 */     
void
jthread_sleep(jlong time)
{
	/* pthread_sleep is an oskit extension */
	oskit_pthread_sleep((oskit_s64_t)time);
}

/* 
 * Check whether a thread is alive.
 *
 * Note that threads executing their cleanup function are not (jthread-) alive.
 * (they're set to THREAD_DEAD)
 */
int
jthread_alive(jthread_t tid)
{
	return tid && (tid->status == THREAD_NEWBORN || 
		       tid->status == THREAD_RUNNING);
}

/*
 * Change thread priority.
 */
void
jthread_setpriority(jthread_t jtid, int prio)
{
	/* XXX use setschedparam here */
	oskit_pthread_setprio(jtid->native_thread, prio);
}

/*
 * Stop a thread in its tracks.
 */
void
jthread_stop(jthread_t jtid)
{
	/* can I cancel myself safely??? */
	/* NB: jthread_stop should never be invoked on the current thread */
	pthread_kill(jtid->native_thread, SIG_STOP);
}

static void
remove_thread(jthread_t tid)
{
	jthread_t* ntid;
	int found = 0;
	pthread_mutex_lock(&threadLock);

	talive--;
	if (tid->daemon) {
		tdaemon--;
	}

	/* Remove thread from live list so it can be garbage collected */
	for (ntid = &liveThreads; *ntid != 0; ntid = &(*ntid)->nextlive) 
	{
		if (tid == (*ntid)) {
			found = 1;
			(*ntid) = tid->nextlive;
			break;
		}
	}

	assert(found);
	pthread_mutex_unlock(&threadLock);

	/* If we only have daemons left, then we should exit. */
	if (talive == tdaemon && !tid->daemon) {
DBG(JTHREAD,
		dprintf("all done, closing shop\n");
    )
		if (runOnExit != 0) {
		    runOnExit();
		}

		/* does that really make sense??? */
		for (tid = liveThreads; tid != 0; tid = tid->nextlive) {
			if (destructor1) {
				(*destructor1)(tid->jlThread);
			}
			jthread_stop(tid);
		}

		/* Am I suppose to close things down nicely ?? */
		KAFFEVM_EXIT(0);
	} else {
		if (destructor1) {
			(*destructor1)(tid->jlThread);
		}
	}
}

/*
 * mark the current thread as dead and remove it from the lists.
 */
static void
mark_thread_dead(void)
{
	jthread_t currentJThread = GET_JTHREAD();
	assert(currentJThread);
	assert(currentJThread->status != THREAD_DEAD);
	currentJThread->status = THREAD_DEAD;

	remove_thread(currentJThread);
}

/*
 * Have a thread exit.
 * Each thread exits only once.
 */
void
jthread_exit(void)
{
DBG(JTHREAD,
	dprintf("jthread_exit called by %d\n", GET_JTHREAD()->native_thread);
    )

	mark_thread_dead();

	/* XXX disconnect the native thread object */
	GET_JTHREAD()->native_thread = -1;

	pthread_detach(pthread_self());
	pthread_exit(0);

	while (1)
		assert(!"This better not return.");
}

/*
 * Print info about a given jthread to stderr
 */
void 
jthread_dumpthreadinfo(jthread_t tid)
{
	dprintf("jthread %p native %ld status %s\n", 
		tid, (long) tid->native_thread,
		tid->status == THREAD_NEWBORN ? "NEWBORN" :
		tid->status == THREAD_RUNNING ? "RUNNING" :
		tid->status == THREAD_DYING   ? "DYING"   :
		tid->status == THREAD_DEAD    ? "DEAD"    :
		tid->status == THREAD_CONTINUE? "CONTINUE":
		tid->status == THREAD_STOPPED ? "STOPPED" : "???");
}

/*
 * dump info on all live threads
 */
void
/* ARGSUSED */
dumpLiveThreads(int s)
{
        jthread_t tid;
        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
		jthread_dumpthreadinfo(tid);
	}
}

/*
 * have main thread wait for all threads to finish
 */
void jthread_exit_when_done(void)
{
        while (talive > 1)
		jthread_yield();
	jthread_exit();
}

/*============================================================================
 * 
 * locking subsystem
 *
 */
void 
jmutex_initialise(jmutex *lock)
{
	if (0 != pthread_mutex_init(lock, (const pthread_mutexattr_t *)0)) {
		assert(!!!"Could not initialise mutex");   /* XXX */
	}
}

void
jmutex_lock(jmutex *lock)
{
	pthread_mutex_lock(lock);
}

void
jmutex_unlock(jmutex *lock)
{
	pthread_mutex_unlock(lock);
}

void
jmutex_destroy(jmutex *lock)
{
	pthread_mutex_destroy(lock);
}

void
jcondvar_initialise(jcondvar *cv)
{
	if (0 != pthread_cond_init(cv, (const pthread_condattr_t *)0)) {
		assert(!!!"Could not initialise condvar");   /* XXX */
	}
}

jbool
jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout)
{
	struct oskit_timespec abstime;
	struct timeval now;

	if (timeout == (jlong)0) {
		pthread_cond_wait(cv, lock);
		return true;
	}

	/* Need to convert timeout to an abstime. Very dumb! */
	gettimeofday(&now, 0);
	TIMEVAL_TO_TIMESPEC(&now, &abstime);

	abstime.tv_sec  += timeout / 1000;
	abstime.tv_nsec += (timeout % 1000) * 1000000;
	if (abstime.tv_nsec > 1000000000) {
		abstime.tv_sec  += 1;
		abstime.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(cv, lock, &abstime);
	
	return true;  /* XXX what should I be returning?? */
}

void
jcondvar_signal(jcondvar *cv, jmutex *lock)
{
	pthread_cond_signal(cv);
}

void
jcondvar_destroy(jcondvar *cv)
{
	pthread_cond_destroy(cv);
}
