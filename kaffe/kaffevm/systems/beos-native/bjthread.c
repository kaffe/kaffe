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
#define THREAD_STOPPED			4
#define THREAD_CONTINUE			5

/*
 * We use this as a stop signal instead of SIGSTOP, because an apparent
 * bug in R4 prevents a custom SIGSTOP handler from ever being called.
 * Kaffe already uses SIGUSR1 elsewhere, I think.
 */
#define STOP_SIGNAL			SIGUSR2

/*
 * Variables.
 * These should be kept static to ensure encapsulation.
 */
static int talive; 		/* number of threads alive */
static int tdaemon;		/* number of daemons alive */
static void (*runOnExit)(void);	/* function to run when all non-daemon die */

static struct jthread* liveThreads;	/* list of all live threads */

/* static lock to protect liveThreads etc. */
static sem_id threadLock;

static void remove_thread(jthread_t tid);
static void mark_thread_dead(void);
void dumpLiveThreads(int);

static void deathcallback(int sig);	/* runs when STOPped and stop is OK */
static void undeathcallback(int sig);	/* runs when stop is not OK */

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
 * This is gross: ordinarily we'd include KernelExport.h to get the
 * types for cpu_status and prototypes for the interrupt and spinlock
 * stuff, but Kaffe defines dprintf and kprintf, both of which are
 * prototyped in KernelExport.h.
 */
#if !defined(_KERNEL_EXPORT_H)
/* Where the heck are these defined?
typedef ulong cpu_status;
extern _IMPEXP_KERNEL cpu_status disable_interrupts();
extern _IMPEXP_KERNEL void       restore_interrupts(cpu_status);
*/

typedef vlong spinlock;
extern _IMPEXP_KERNEL void       acquire_spinlock(spinlock*);
extern _IMPEXP_KERNEL void       release_spinlock(spinlock*);
#endif

/*
 * BeOS' interrupt management routines pass a cpu_status between them.
 * Will allocating it statically here work for multiprocessors?
 */
/* static cpu_status my_cpu_status; */

/* this is where the cookies are kept */
per_thread_info_t*	per_thread_info;
static area_id		pti_area;

/*
 * Function declarations.
 */


/*============================================================================
 *
 * Functions related to interrupt handling
 *
 */

/*
 * Woo-hoo!  BeOS has spinlocks and interrupt enable/disable functions.
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
/*
	cpu_status cstat;

	cstat = disable_interrupts();
	acquire_spinlock((spinlock*)arg);
	restore_interrupts(cstat);
*/
}

/*
 * release spinlock
 */
void 
jthread_spinoff(void *arg)
{
/*
	cpu_status cstat;

	cstat = disable_interrupts();
	release_spinlock((spinlock*)arg);
	restore_interrupts(cstat);
*/
}

/*
 * determine the interesting stack range for a conservative gc
 *
 * XXX BeOS doesn't provide a way to get the SP for a specific thread
 *     other than the one currently running :(
 */
int
jthread_extract_stack(jthread_t jtid, void **from, unsigned *len)
{
	thread_info tinfo;

	if (B_OK != get_thread_info(jtid->native_thread, &tinfo)) {
		/* Error */
		return (0);
	}

	*from = tinfo.stack_base;
	*len = tinfo.stack_end - tinfo.stack_base;

	if (*len < 0 || *len > (256*1024)) {
	    printf("(%lx) get_thread_info(%lx) reported obscene numbers: "
		  "base = 0x%p, end = 0x%p, sp = 0x%p\n", 
		  find_thread(NULL),
		  jtid->native_thread,
		  tinfo.stack_base, tinfo.stack_end, tinfo.stack_base);
	    exit(-1);
	}

DBG(JTHREADDETAIL,
	dprintf("extract_stack(%) base=%p size=%d sp=%p; from=%p len=%d\n", 
		jtid->native_thread,
		tinfo.stack_base, tinfo.stack_end-tinfo.stack_base, tinfo.stack_base, *from, *len);
    )

	return (1);
}

/*
 * determine whether an address lies on your current stack frame
 */
int
jthread_on_current_stack(void *bp)
{
	void* curr_sp;
	thread_info tinfo;
	int rc;

	if (B_OK != get_thread_info(find_thread(NULL), &tinfo)) {
		return (0);
	}

	curr_sp = (void*)get_stack_frame();
	rc = (bp >= curr_sp && bp < tinfo.stack_end);

DBG(JTHREADDETAIL,
	dprintf("on current stack(%x) base=%p size=%d bp=%p %s\n",
		find_thread(NULL),
		tinfo.stack_base, tinfo.stack_end-tinfo.stack_base, bp, (rc ? "yes" : "no"));
    )
	return rc;
}       

/*
 * See if there is enough room on the stack.
 */
int
jthread_stackcheck(int need)
{
	void* curr_sp;
	thread_info tinfo;
	int room;

	if (B_OK != get_thread_info(find_thread(NULL), &tinfo)) {
		/* Error */
		return (0);
	}

	curr_sp = (void*)get_stack_frame();
	room = curr_sp - tinfo.stack_base;

DBG(JTHREADDETAIL,
	dprintf("stackcheck(%x) need=%d base=%p size=%d sp=%p room=%d\n",
		find_thread(NULL),
		need, tinfo.stack_base, tinfo.stack_end-tinfo.stack_base, curr_sp, room);
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
	status_t status;
	void (*old_stop_handler)(int);

	assert(tid);
DBG(JTHREAD, 
	dprintf("destroying tid %x\n", tid->native_thread);	
    )
	old_stop_handler = signal(STOP_SIGNAL, undeathcallback);
	wait_for_thread(tid->native_thread, &status);
	signal(STOP_SIGNAL, old_stop_handler);
	deallocator(tid);
}

/*
 * find a native BeOS thread's cookie
 * (Maps BeOS threads to java.lang.Threads)
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

/*
 * Initialize the threading system.
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
	thread_id	pmain;
	jthread_t	jtid;
	void*		pti_addr;

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

	pmain = find_thread(NULL);
	rename_thread(pmain, "Kaffe main thread");

        /*
         * XXX: ignore mapping of min/max priority for now and assume
         * BeOS priorities include java priorities.
         */
        set_thread_priority(pmain, mainthreadpr);

	threadLock = create_sem(1, "Kaffe threadLock");

        jtid = allocator(sizeof (*jtid));
        SET_JTHREAD(jtid);

	jtid->native_thread = pmain;
	/*
	 * In case of unnatural death (induced by receiving a STOP_SIGNAL),
	 * we install the appropriate signal handler.
	 */
	signal(STOP_SIGNAL, deathcallback);
	jtid->stop_pending = FALSE;

        jtid->nextlive = liveThreads;
        liveThreads = jtid;
	jtid->status = THREAD_RUNNING;
        talive++;
DBG(JTHREAD,
	dprintf("main thread has id %x\n", jtid->native_thread);
    )
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
	signal(STOP_SIGNAL, undeathcallback);
}

/*
 * reallow cancellation and stop if cancellation pending
 */
void 
jthread_enable_stop(void)
{
	jthread_t currentJThread = GET_JTHREAD();

	signal(STOP_SIGNAL, deathcallback);
	if (currentJThread->stop_pending) {
		send_signal(currentJThread->native_thread, STOP_SIGNAL);
	}
}

/*
 * interrupt a thread
 */
void
jthread_interrupt(jthread_t tid)
{
DBG(JTHREAD,
        dprintf("interrupting thread %p, %x\n", tid, tid->native_thread); )
	resume_thread(tid->native_thread);
}

/*
 * cleanup handler for a given thread.  This handler is called when that
 * thread is killed (i.e., when it receives a STOP_SIGNAL).
 */
static void
deathcallback(int sig)
{
	jthread_t tid = GET_JTHREAD();
	tid->status = THREAD_DYING;
	onstop();
	mark_thread_dead();
	/* by returning, we proceed with the STOP and exit that thread */
}

/*
 * this handler catches STOP_SIGNAL when stops are disabled
 */
static void
undeathcallback(int sig)
{
	jthread_t tid = GET_JTHREAD();
	tid->stop_pending = TRUE;
}

/*
 * start function for each thread.  
 * This function install the cleanup handler, sets jthread-specific 
 * data and calls the actual work function.
 */
int32
start_me_up(void *arg)
{
	jthread_t tid = (jthread_t)arg;

DBG(JTHREAD, 
	dprintf("starting thread %p\n", tid); 
    )
	acquire_sem(threadLock);
	signal(STOP_SIGNAL, deathcallback);
	tid->stop_pending = FALSE;
	SET_JTHREAD(tid);
	SET_COOKIE(tid->jlThread);
        tid->status = THREAD_RUNNING;
	release_sem(threadLock);

DBG(JTHREAD, 
	dprintf("calling thread %p, %x\n", tid, tid->native_thread); )
	tid->func(tid->jlThread);
DBG(JTHREAD, 
	dprintf("thread %x returned, calling jthread_exit\n", 
		tid->native_thread); 
    )

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

        tid = allocator(sizeof (*tid));
        assert(tid != 0);      /* XXX */

	acquire_sem(threadLock);
        tid->jlThread = jlThread;
        tid->func = func;

        tid->nextlive = liveThreads;
        liveThreads = tid;
        tid->status = THREAD_NEWBORN;

	ntid = spawn_thread(start_me_up, "", pri, tid);
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
DBG(JTHREAD,
	dprintf("created thread %x, daemon=%d\n", ntid, daemon); )

		resume_thread(ntid);
        	return (tid);
	}
	else {
DBG(JTHREAD,
	dprintf("can't create thread: per-thread info table too small\n"); )

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
	/* snooze_until takes an arg in usecs */
DBG(JTHREAD,
        dprintf("thread %x: sleeping for %g second(s)...",
                find_thread(NULL), (double)time/1000.0);
    )
	snooze_until(system_time() + (time*1000), B_SYSTEM_TIMEBASE);
DBG(JTHREAD,
        dprintf("OK, I'm awake!\n");
    )
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
	set_thread_priority(jtid->native_thread, prio);
}

/*
 * Stop a thread in its tracks.
 */
void
jthread_stop(jthread_t jtid)
{
	/* can I cancel myself safely??? */
	/* NB: jthread_stop should never be invoked on the current thread */
	send_signal(jtid->native_thread, STOP_SIGNAL);
}

static void
remove_thread(jthread_t tid)
{
	jthread_t* ntid;
	int found = 0;

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
			send_signal(tid->native_thread, STOP_SIGNAL);
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
	dprintf("jthread_exit called by %x\n", currentJThread->native_thread);
    )

	mark_thread_dead();
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
	*lock = create_sem(1, "Kaffe sem");
	assert(*lock >= B_NO_ERROR);
}

void
jmutex_lock(jmutex *lock)
{
	status_t rc;

	rc = acquire_sem(*lock);
	assert(B_NO_ERROR == rc);
}

void
jmutex_unlock(jmutex *lock)
{
	status_t rc;

	rc = release_sem(*lock);
	assert(B_NO_ERROR == rc);
}

void
jcondvar_initialise(jcondvar *cv)
{
	cv->mutex = create_sem(1, "Kaffe cv - mutex");
	cv->cond = create_sem(0, "Kaffe cv - cond");
	cv->num_sleeping = 0;
	assert(cv->mutex >= B_NO_ERROR && cv->cond >= B_NO_ERROR);
}

void
jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout)
{
	acquire_sem(cv->mutex);
	++cv->num_sleeping;
	jmutex_unlock(lock);
	release_sem(cv->mutex);
	acquire_sem(cv->cond);
	jmutex_lock(lock);
}

void
jcondvar_signal(jcondvar *cv, jmutex *lock)
{
	acquire_sem(cv->mutex);
	if (cv->num_sleeping > 0) {
		release_sem(cv->cond);
		--cv->num_sleeping;
	}
	release_sem(cv->mutex);
}

void
jcondvar_broadcast(jcondvar *cv, jmutex *lock)
{
	acquire_sem(cv->mutex);
	while (cv->num_sleeping > 0) {
		release_sem(cv->cond);
		--cv->num_sleeping;
	}
	release_sem(cv->mutex);
}

