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
 * This file implements jthreads on top of BeOS native threads and
 * was derived from oskit-pthreads/pjthread.c.
 *
 * Please address BeOS-related questions to alanlb@vt.edu.
 */

#include "debug.h"
#include "jthread.h"

#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

/* thread status */
#define THREAD_NEWBORN                	0
#define THREAD_RUNNING                  1
#define THREAD_DYING                    2
#define THREAD_DEAD                     3

/*
 * We use this as a stop signal instead of SIGSTOP, because an apparent
 * bug in R4 prevents a custom SIGSTOP handler from ever being called.
 */
#define STOP_SIGNAL			SIGUSR2

/*
 * Variables.
 * These should be kept static to ensure encapsulation.
 */
static int talive; 			/* number of threads alive */
static int tdaemon;			/* number of daemons alive */
static void (*runOnExit)(void);		/* run when all non-daemons die */

static struct jthread* liveThreads;	/* list of all live threads */
static sem_id threadLock;		/* static lock to protect liveThreads */

static int map_Java_priority(int jprio);
static void remove_thread(jthread_t tid);
static void mark_thread_dead(void);
void dumpLiveThreads(int);

static void deathcallback(int sig);	/* runs when STOPped and stop is OK */

/*
 * the following variables are set by jthread_init, and show how the
 * threading system is parameterized.
 */
static void *(*allocator)(size_t); 	/* malloc */
static void (*deallocator)(void*);	/* free */
static void (*destructor1)(void*);	/* call when a thread exits */
static void (*onstop)(void);		/* call when a thread is stopped */
static int  max_priority;		/* maximum supported priority */
static int  min_priority;		/* minimum supported priority */

/*
 * This is used to track the id of the main thread, so that we can
 * prevent it from exiting while there are daemon threads still
 * running.  otherwise, the environ ptr would become invalid, and
 * any remaining non-daemon thread that called getenv() either directly
 * or indirectly via, say, gethostbyname(), would fail with a
 * NullPointerException.
 *
 * This scenario revealed itself with the UDPTest program in the
 * regression suite.
 */
static thread_id		the_main_thread;

/*
 * This is where the cookies are kept
 */
per_thread_info_t*		per_thread_info;
static area_id			pti_area;

/*
 * This is used to prevent multiple STOP_SIGNALs from being sent to the
 * daemon threads during final shutdown.
 */
static int32			isShuttingDown = 0;

/*
 * Helpers
 */
#define THREAD_NAME(jtid)	nameThread(jtid->jlThread)


/*============================================================================
 *
 * Functions related to interrupt handling
 *
 */

/*
 * Suspend all threads
 */
void 
jthread_suspendall(void)
{
	/* Unimplemented */
}

/*
 * Unsuspend all threads
 */
void
jthread_unsuspendall(void)
{
	/* Unimplemented */
}

/*
 * acquire spinlock
 */
void 
jthread_spinon(void *arg)
{
	int32* lock = arg;
	int32  prev;

	if (NULL != lock) {
		do {
			prev = atomic_or(lock, 1);
		} while (1 == prev);
	}
}

/*
 * release spinlock
 */
void 
jthread_spinoff(void *arg)
{
	int32* lock = arg;

	if (NULL != lock) {
		atomic_and(lock, 0);
	}
}

/*============================================================================
 *
 * Functions related to the stack
 *
 */

/*
 * determine the interesting stack range for a conservative gc
 */
int
jthread_extract_stack(jthread_t jtid, void **from, unsigned *len)
{
	if (NULL == jtid) {
		return(0);
	}
	*from = jtid->stack_bottom;
	*len = jtid->stack_top - jtid->stack_bottom;
	return(1);
}

/*
 * determine whether an address lies on your current stack frame
 */
int
jthread_on_current_stack(void *bp)
{
	int dummy = 0;
	jthread_t currentJThread = GET_JTHREAD();
	return(bp >= (void*)&dummy && bp < currentJThread->stack_top);
}       

/*
 * See if there is enough room on the stack.
 */
int
jthread_stackcheck(int need)
{
	int dummy = 0;
	if (0 == need) {
		return(1);
	}
	else {
		jthread_t currentJThread = GET_JTHREAD();
		return(((void*)&dummy - currentJThread->stack_bottom) >= need);
	}
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
	status_t status;

	assert(tid);
	DBG(JTHREAD, dprintf("destroying %s\n", THREAD_NAME(tid));)

	atomic_and(&tid->stop_allowed, 0);
	wait_for_thread(tid->native_thread, &status);
	atomic_or(&tid->stop_allowed, 1);
	deallocator(tid);
}

/*
 * find a native BeOS thread's cookie
 * (Maps BeOS threads to java.lang.Threads)
 */
/*
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
*/

/*
 * iterate over all live threads
 */
void
jthread_walkLiveThreads(void (*func)(void *jlThread))
{
        jthread_t tid;

	acquire_sem(threadLock);
        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
                func(tid->jlThread);
        }
	release_sem(threadLock);
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

static int
map_Java_priority(int prio)
{
	double range = max_priority - min_priority;
	double diff = prio - min_priority;

	if (0 == range) {
		return(B_NORMAL_PRIORITY);
	}
	else {
		return(B_LOW_PRIORITY +
			(double)(B_URGENT_DISPLAY_PRIORITY-B_LOW_PRIORITY) *
				(diff / range));
	}
}

/*
 * Initialize the threading system.
 */
jthread_t 
jthread_init(
	int preemptive,
	int maxpr,
	int minpr,
	void *(*_allocator)(size_t), 
	void (*_deallocator)(void*),
	void (*_destructor1)(void*),
	void (*_onstop)(void),
	void (*_ondeadlock)(void))		/* ignored for now */
{
	thread_id	pmain;
	jthread_t	jtid;
	void*		pti_addr;
	thread_info	tinfo;

	max_priority = maxpr;
	min_priority = minpr;
	allocator = _allocator;
	deallocator = _deallocator;
	onstop = _onstop;
	destructor1 = _destructor1;

	/*
	 * Prepare the area used for the cookies, etc.
	 */
	pti_area = create_area("Kaffe cookies", &pti_addr,
			B_ANY_ADDRESS, PTI_AREA_SIZE,
			B_LAZY_LOCK, B_READ_AREA|B_WRITE_AREA);
	assert(pti_area >= B_OK);
	memset(pti_addr, PTI_AREA_SIZE, 0);
	per_thread_info = (per_thread_info_t*)pti_addr;

	threadLock = create_sem(1, "Kaffe threadLock");

	pmain = find_thread(NULL);
	rename_thread(pmain, "Kaffe main thread");

	/*
	 * Record the id of the main thread, for use by jthread_exit
	 */
	the_main_thread = pmain;

        jtid = allocator(sizeof (*jtid));
        SET_JTHREAD(jtid);

	jtid->native_thread = pmain;

	/*
	 * Determine the top and bottom addrs of this thread's stack
	 */
	get_thread_info(pmain, &tinfo);
	jtid->stack_top = tinfo.stack_end;
	jtid->stack_bottom = tinfo.stack_base;

	/*
	 * In case of unnatural death (induced by receiving a STOP_SIGNAL),
	 * we install the appropriate signal handler.
	 */
	signal(STOP_SIGNAL, deathcallback);
	jtid->stop_allowed = 1;
	jtid->stop_pending = 0;

        jtid->nextlive = liveThreads;
        liveThreads = jtid;
	jtid->status = THREAD_RUNNING;
        talive++;

	DBG(JTHREAD, dprintf("main thread has id %x\n", jtid->native_thread);)
	return (jtid);
}

/*
 * Create the first thread - actually bind the first thread to the java
 * context.
 */
jthread_t
jthread_createfirst(size_t mainThreadStackSize, 
		    unsigned char prio, 
		    void* jlThread)
{
        jthread_t jtid; 

	jtid = GET_JTHREAD();
	assert(jtid != NULL);
	assert(jtid->native_thread != 0);
	assert(jtid->status == THREAD_RUNNING);

	jtid->jlThread = jlThread;
	SET_COOKIE(jtid->jlThread);

	signal(STOP_SIGNAL, deathcallback);
	jtid->stop_allowed = 1;
	jtid->stop_pending = 0;

	jthread_setpriority(jtid, prio);
	rename_thread(jtid->native_thread, "Kaffe main thread");

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
	jthread_t currentJThread = GET_JTHREAD();

	if (NULL != currentJThread) {
		atomic_and(&currentJThread->stop_allowed, 0);
	}
}

/*
 * reallow cancellation and stop if cancellation pending
 */
void 
jthread_enable_stop(void)
{
	jthread_t currentJThread = GET_JTHREAD();

	if (NULL != currentJThread) {
		atomic_or(&currentJThread->stop_allowed, 1);
		if (currentJThread->stop_pending) {
			currentJThread->status = THREAD_DYING;
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
	thread_info tinfo;

	DBG(JTHREAD, dprintf("interrupting thread %p, %s\n",
		tid, THREAD_NAME(tid));)

	if (THREAD_DYING != tid->status && THREAD_DEAD != tid->status) {
		get_thread_info(tid->native_thread, &tinfo);

		if (B_THREAD_SUSPENDED != tinfo.state &&
		    B_THREAD_RUNNING != tinfo.state) {
			/*
			 * This thread is asleep, waiting on I/O or
			 * blocked on a semaphore, so resume_thread won't
			 * work without a preceding suspend_thread, and
			 * since these amount to signalling the targeted
			 * thread, we need to snooze a bit, as described
			 * in the Be Book section on the "snooze" function.
			 */
			suspend_thread(tid->native_thread);
			snooze(1000);   /* microseconds */
		}

		/*
		 * At this point, the targeted thread is either already
		 * running, or is in a resumable state.
		 */
		resume_thread(tid->native_thread);
	}
}

/*
 * cleanup handler for a given thread.  This handler is called when that
 * thread is killed (i.e., when it receives a STOP_SIGNAL).
 */
static void
deathcallback(int sig)
{
	jthread_t currentJThread = GET_JTHREAD();

	atomic_or(&currentJThread->stop_pending, 1);
	if (0 == atomic_or(&currentJThread->stop_allowed, 0)) {
		DBG(JTHREAD, dprintf("%s rejects the STOP request",
			THREAD_NAME(currentJThread));)
		return;
	}

	currentJThread->status = THREAD_DYING;
	onstop();
	mark_thread_dead();
	/* by returning, we proceed with the STOP and exit that thread */
}

/*
 * start function for each thread.  
 * This function install the cleanup handler, sets jthread-specific 
 * data and calls the actual work function.
 */
int32
start_me_up(void *arg)
{
	jthread_t   tid = (jthread_t)arg;
	thread_info tinfo;

	extern void initExceptions(void);

	DBG(JTHREAD, dprintf("start_me_up: setting up for %s\n",
		THREAD_NAME(tid));)

	acquire_sem(threadLock);

	/* GROSS HACK ALERT -- On R4, child threads don't inherit the
	 * spawning thread's signal handlers!
	 */

	initExceptions();   /* from ../../exceptions.c */

	/* END OF GROSS HACK
	 */

	signal(STOP_SIGNAL, deathcallback);
	tid->stop_allowed = 1;
	tid->stop_pending = 0;

	/*
	 * Determine the top and bottom addrs of this thread's stack
	 */
	get_thread_info(tid->native_thread, &tinfo);
	tid->stack_top = tinfo.stack_end;
	tid->stack_bottom = tinfo.stack_base;

	SET_JTHREAD(tid);
	SET_COOKIE(tid->jlThread);
        tid->status = THREAD_RUNNING;
	release_sem(threadLock);

	DBG(JTHREAD, dprintf("start_me_up: calling t-func for %s\n",
		THREAD_NAME(tid));)
	tid->func(tid->jlThread);
	DBG(JTHREAD, dprintf("start_me_up: thread %s returned\n", 
		THREAD_NAME(tid));)

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
	thread_id ntid;
	jthread_t tid;

	/* 
	 * Note that we create the thread in a joinable state, which is the
	 * default.  Our finalizer will join the threads, allowing the
	 * thread system to free its resources.
	 */

        tid = allocator(sizeof(*tid));
        assert(tid != 0);

	acquire_sem(threadLock);
        tid->jlThread = jlThread;
        tid->func = func;

        tid->nextlive = liveThreads;
        liveThreads = tid;
        tid->status = THREAD_NEWBORN;

	ntid = spawn_thread(start_me_up, nameThread(jlThread),
				map_Java_priority(pri), tid);
	tid->native_thread = ntid;

        talive++;       
        if ((tid->daemon = daemon) != 0) {
                tdaemon++;
        }
	release_sem(threadLock);

	/* Check if we can safely save the per-thread info for
	 * this thread.  Yes, I know the per-thread stuff is lame,
	 * but let's get this working first, shall we?
	 */
	if (NULL == per_thread_info[ntid % MAX_THREADS].jtid) {
		DBG(JTHREAD, dprintf("created thread %s, daemon=%d\n",
			nameThread(jlThread), daemon);)

		resume_thread(ntid);
        	return (tid);
	}
	else {
		DBG(JTHREAD, dprintf("can't create thread: "
			"per-thread info table too small\n");)

		kill_thread(ntid);   /* stillborn */
		deallocator(tid);
		return NULL;
	}
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
	DBG(JTHREAD, dprintf("thread %s: sleeping for %g second(s)...",
                THREAD_NAME(GET_JTHREAD()), (double)time/1000.0);)

	/* snooze_until takes an arg in usecs */
	snooze_until(system_time() + (time*1000L), B_SYSTEM_TIMEBASE);

	DBG(JTHREAD, dprintf("OK, I'm awake!\n");)
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
	set_thread_priority(jtid->native_thread, map_Java_priority(prio));
}

/*
 * Stop a thread in its tracks.
 */
void
jthread_stop(jthread_t jtid)
{
	/* can I cancel myself safely??? */
	/* NB: jthread_stop should never be invoked on the current thread */

	DBG(JTHREAD, dprintf("stopping %s...\n", THREAD_NAME(jtid));)
	send_signal(jtid->native_thread, STOP_SIGNAL);
}

static void
remove_thread(jthread_t tid)
{
	jthread_t* ntid;
	int found = 0;

	DBG(JTHREAD, dprintf("Removing entry for thread %s\n",
		THREAD_NAME(tid));)

	acquire_sem(threadLock);

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

	release_sem(threadLock);

	/* If we only have daemons left, then we should exit. */
	if (talive == tdaemon) {

		/* Make sure this section doesn't get executed
		 * more than once.
		 */
		if (isShuttingDown) {
			return;
		}
		atomic_or(&isShuttingDown, 1);

		DBG(JTHREAD, dprintf("%s: all done, closing shop\n",
			THREAD_NAME(tid));)

		if (runOnExit != 0) {
		    runOnExit();
		}

		/* does that really make sense??? */
		for (tid = liveThreads; tid != 0; tid = tid->nextlive) {
			if (destructor1) {
				(*destructor1)(tid->jlThread);
			}
			kill_thread(tid->native_thread);
			DBG(JTHREAD, dprintf("killed %s\n", THREAD_NAME(tid));)
		}

		/* Shut down this thread */
		DBG(JTHREAD, dprintf("%s: Goodbye!\n",
			THREAD_NAME(GET_JTHREAD()));)
		exit_thread(0);

	} else {
		if (destructor1) {
			(*destructor1)(tid->jlThread);
		}
		/* This thread will now return to mark_thread_dead, and
		 * then call exit_thread() from that function's caller.
		 */
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

	DBG(JTHREAD, dprintf("jthread_exit called by %s\n",
		THREAD_NAME(currentJThread));)

	mark_thread_dead();

	/*
	 * If this is the main thread, wait for all other threads to finish.
	 * At this point, the main jthread is no longer part of the
	 * live list.
	 */
	if (the_main_thread == currentJThread->native_thread) {
		while (talive > tdaemon) {
	        jthread_t tid = NULL;
			int32 rc;

			acquire_sem(threadLock);
	        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
				if (tid->daemon) break;
			}
			release_sem(threadLock);
			if (NULL != tid) {
				wait_for_thread(tid->native_thread, &rc);
			}
		}
	}

	/*
	 * OK, now it's safe to exit
	 */
	DBG(JTHREAD, dprintf("%s: at the point of no return in jthread_exit\n",
		THREAD_NAME(currentJThread));)

	exit_thread(0);
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
		tid, tid->native_thread,
		tid->status == THREAD_NEWBORN ? "NEWBORN" :
		tid->status == THREAD_RUNNING ? "RUNNING" :
		tid->status == THREAD_DYING   ? "DYING"   :
		tid->status == THREAD_DEAD    ? "DEAD"    : "???");
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
