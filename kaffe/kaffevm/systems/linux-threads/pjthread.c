/*
 * Copyright (c) 1998 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */
/*
 * This file implements jthreads on top of Linux-pthreads.
 */

#include "debug.h"
#include "jthread.h"
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

#include <semaphore.h>
/*
 * This idea stolen from gc/linux_threads.c in Ferguson & Boehm's pthreads
 * port.
 */
static sem_t GC_suspend_ack_sem;
#define SIG_STOP 	SIGPWR
#define SIG_RESUME	SIGXCPU

/* thread status */
#define THREAD_NEWBORN                	0
#define THREAD_RUNNING                  1
#define THREAD_DYING                    2
#define THREAD_DEAD                     3
#define THREAD_STOPPED                  4
#define THREAD_CONTINUE                 5

/*
 * Variables.
 * These should be kept static to ensure encapsulation.
 */
static int talive; 		/* number of threads alive */
static int tdaemon;		/* number of daemons alive */
static void (*runOnExit)(void);	/* function to run when all non-daemon die */

static struct jthread* liveThreads;	/* list of all live threads */

/* static lock to protect liveThreads etc. */
static pthread_mutex_t threadLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

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

/* All spin locks are one */
pthread_mutex_t spin = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

/*
 * Native linux threads.
 *
 * This "spinlock" implementation will grind the machine to a halt...
 */
void
/* ARGSUSED */
jthread_spinon(void *arg)
{
	sigset_t nsig;

	sigemptyset(&nsig);
	sigaddset(&nsig, SIG_STOP);
	sigprocmask(SIG_BLOCK, &nsig, 0);
	pthread_mutex_lock(&spin);
}

void
/* ARGSUSED */
jthread_spinoff(void *arg)
{
	sigset_t nsig;

	pthread_mutex_unlock(&spin);
	sigemptyset(&nsig);
	sigaddset(&nsig, SIG_STOP);
	sigprocmask(SIG_UNBLOCK, &nsig, 0);
}

int
jthread_on_current_stack(void *bp)
{
#if defined(STACKGROWSUP)
	not yet
#else
	/* only need to check to one side !? */
	return (bp < GET_JTHREAD()->end);	
#endif
}

int
jthread_stackcheck(int need)
{
	int x;
	struct jthread *jtid = GET_JTHREAD();

	/* Linux grows automatically to 2MB 
	 * We say 1 MB is enough for this guy.
	 */
	return (jtid->end - (void*)&x < 1024*1024);
}

int
jthread_extract_stack(jthread_t jtid, void **from, unsigned *len)
{
	/* This information is not valid unless the thread is stopped */
	if (jtid->status != THREAD_STOPPED) {
		return (0);
	} else {
#if defined(STACKGROWSUP)
	not yet
#else
DBG(JTHREAD,
		dprintf("thread %d has a stack from %p to %p\n",
			jtid->native_thread, jtid->sp, jtid->end);
    )
		*from = jtid->sp;
		*len = jtid->end - jtid->sp;
#endif
		return (1);
	}
}

void
resumeHere(int s)
{
	struct jthread *jtid = GET_JTHREAD();

DBG(JTHREAD,
	dprintf("%d received resume signal (%d->%d)\n", jtid->native_thread,
		jtid->saved_status, jtid->status);
    )
	jtid->status = jtid->saved_status;
}

/*
 * Note: in linuxthreads, none of the pthread_ functions are async-signal-safe.
 * That is, we can't use any of them in here or we would deadlock.
 *
 * This also means that if onstop simply jumps out of the signal handler
 * to deliver the ThreadDeathException we're screwed too.  Great.
 */
void
stopHere(int s)
{
	int x;
	struct jthread *jtid = GET_JTHREAD();

DBG(JTHREADDETAIL,
	dprintf("thread %d (jthread@%p), pid %d caught signal\n",
		jtid->native_thread, jtid, getpid());
    )

	/* DIE */
	if (jtid->status == THREAD_DYING) {
DBG(JTHREAD,
		dprintf("calling onstop\n");
    )
		onstop();
	} else {
		sigset_t mask;

		jtid->sp = &x;
		jtid->saved_status = jtid->status;
DBG(JTHREAD,
		if (jtid->saved_status == THREAD_STOPPED) {
			dprintf("%d already stopped!?\n", jtid->native_thread);
		}
    )
		jtid->status = THREAD_STOPPED;
		sem_post(&GC_suspend_ack_sem);

	DBG(JTHREAD,
		dprintf("%d stopped\n", jtid->native_thread);
	    )
		sigfillset(&mask);
		sigdelset(&mask, SIG_RESUME);
		while (jtid->status == THREAD_STOPPED) {
			sigsuspend(&mask);
		}

	DBG(JTHREAD,
		dprintf("%d resumed\n", jtid->native_thread);
	    )
	}
}

/*
 * Stop all threads.  pthread-compatible multiprocessor version
 */
void
jthread_suspendall(void)
{
	int n = 0, rc;
	struct jthread *tid, *current = GET_JTHREAD();

	/* taking and holding this lock prevents other threads from
	 * starting while the world stands still.
	 * The lock is unlocked in jthread_resumeall().
	 */
	pthread_mutex_lock(&threadLock);
DBG(JTHREAD,
	dprintf("STARTING TO STOP THE WORLD\n");
    )
        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
		if (tid == current) {
DBG(JTHREADDETAIL,
			dprintf("skipping myself %d\n", tid->native_thread);
    )
			continue;
		}
		if (!(tid->status == THREAD_RUNNING ||
		      tid->status == THREAD_DYING)) 
		{
DBG(JTHREADDETAIL,
			dprintf("skipping thread %d (s=%d)\n", 
				tid->native_thread, tid->status);
    )
			continue;
		}
DBG(JTHREAD,
		dprintf("attempting to stop %d\n", tid->native_thread);
    )
		n++;
		rc = pthread_kill(tid->native_thread, SIG_STOP);
		switch (rc) {
		case ESRCH:
DBG(JTHREAD,
			dprintf("didn't find %d\n", tid->native_thread);
    )
			n--;
			break;
		case 0:
			break;
		default:
			fprintf(stderr, "pthread_kill failed %d\n", rc);
			exit(-1);
		}
	}
DBG(JTHREAD,
	dprintf("waiting for %d threads till stop completes...\n", n);
    )
	while (n-- > 0) {
DBG(JTHREADDETAIL,
		dprintf("%d left\n", n+1);
    )
		sem_wait(&GC_suspend_ack_sem);
	}
DBG(JTHREAD,
	dprintf("ALL THREADS STOPPED\n");
    )
}

void
jthread_unsuspendall(void)
{
	struct jthread *tid;

        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
		if (tid->status != THREAD_STOPPED) {
			continue;
		}
DBG(JTHREAD,
		dprintf("unsuspending %d\n", tid->native_thread);
    )
		pthread_kill(tid->native_thread, SIG_RESUME);
	}
DBG(JTHREAD,
	dprintf("ALL THREADS RESUMED\n");
    )
	pthread_mutex_unlock(&threadLock);
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
	pthread_join(tid->native_thread, &status);
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
	int maxpr, int minpr, int mainthreadpr, 
	size_t mainThreadStackSize,
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

        /*
         * XXX: ignore mapping of min/max priority for now and assume
         * pthread priorities include java priorities.
	 *
	 * This does not hold true for Linux pthreads, where we must
	 * scale priorities and renice. 
         */
	/* XXX: use 
	   pthread_setschedparam here --- Linux doesn't have setprio 
           pthread_setprio(pmain, mainthreadpr);
         */

        pthread_key_create(&jthread_key, 0 /* destructor */);
        pthread_key_create(&cookie_key, 0 /* destructor */);

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
#if defined(STACKGROWSUP)
	jtid->sp = jtid->base = &pmain;
#else
	jtid->sp = jtid->end = &pmain;
#endif
	act.sa_handler = resumeHere;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIG_RESUME, &act, 0);

	act.sa_handler = stopHere;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIG_STOP, &act, 0);

	if (0 != sem_init(&GC_suspend_ack_sem, 0, 0)) {
		fprintf(stderr, "couldn't init sem\n");
		abort();
	}
#if defined(DEBUG)
	signal(SIGHUP, dumpLiveThreads);
#endif
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
	/* XXX */
}

/*
 * reallow cancellation and stop if cancellation pending
 */
void 
jthread_enable_stop(void)
{
	/* XXX */
}

/*
 * interrupt a thread
 */
void
jthread_interrupt(jthread_t tid)
{
    fprintf(stderr, "jthread_interrupt is not yet implemented\n");
    /* ignore */
}

/*
 * start function for each thread.  
 * This function sets jthread-specific data and calls the actual work function.
 */
void*
start_me_up(void *arg)
{
	jthread_t tid = (jthread_t)arg;

DBG(JTHREAD, 
	dprintf("starting thread %p\n", tid); 
    )
	pthread_mutex_lock(&threadLock);
	SET_JTHREAD(tid);
	SET_COOKIE(tid->jlThread);
#if defined(STACKGROWSUP)
	tid->sp = tid->base = &tid;
#else
	tid->sp = tid->end = &tid;
#endif
        tid->status = THREAD_RUNNING;
	pthread_mutex_unlock(&threadLock);

DBG(JTHREAD, 
	dprintf("calling thread %p, %d\n", tid, tid->native_thread); )
	tid->func(tid->jlThread);
DBG(JTHREAD, 
	dprintf("thread %d returned, calling jthread_exit\n", 
		tid->native_thread); 
    )

	/* drop onstop handler if that thread is exiting by itself */
	assert (tid->status != THREAD_DYING);

	mark_thread_dead();
	/* by returning, we exit this thread */
	return (0);
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

	pthread_attr_init(&attr);

	/* use setschedparam here */
	/* Linux doesn't have setstacksize */
	/*
	    pthread_attr_setstacksize(&attr, threadStackSize);
	    pthread_attr_setprio(&attr, pri);
	*/

	/* 
	 * Note that we create the thread in a joinable state, which is the
	 * default.  Our finalizer will join the threads, allowing the
	 * pthread system to free its resources.
	 */

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
	/* XXX */
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
	/* XXX use setschedparam */
}

/*
 * Stop a thread in its tracks.
 */
void
jthread_stop(jthread_t jtid)
{
	/* can I cancel myself safely??? */
	/* NB: jthread_stop should never be invoked on the current thread */
	jtid->status = THREAD_DYING;
	pthread_kill(jtid->native_thread, SIGUNUSED);
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
	if (talive == tdaemon) {
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
		}

		/* Am I suppose to close things down nicely ?? */
		EXIT(0);
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
	assert (currentJThread->status != THREAD_DEAD);
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
	jthread_t currentJThread = GET_JTHREAD();

DBG(JTHREAD,
	dprintf("jthread_exit called by %d\n", currentJThread->native_thread);
    )

	mark_thread_dead();
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
	fprintf(stderr, "jthread %p native %ld status %s\n", 
		tid, tid->native_thread,
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
jcondvar_initialise(jcondvar *cv)
{
	if (0 != pthread_cond_init(cv, (const pthread_condattr_t *)0)) {
		assert(!!!"Could not initialise condvar");   /* XXX */
	}
}

void
jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout)
{
	struct timespec abstime;
	struct timeval now;

	if (timeout == (jlong)0) {
		pthread_cond_wait(cv, lock);
		return;
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
}

void
jcondvar_signal(jcondvar *cv, jmutex *lock)
{
	pthread_cond_signal(cv);
}

void
jcondvar_broadcast(jcondvar *cv, jmutex *lock)
{
	pthread_cond_broadcast(cv);
}
