/*
 * jthread.c
 * Java thread package - derived from thread-internal.c
 *
 * Internal threading system support
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Godmar Back <gback@cs.utah.edu> and 
 *            Tim Wilkinson <tim@transvirtual.com>
 */

#include "jthread.h"

/* Flags used for threading I/O calls */
#define TH_READ                         0
#define TH_WRITE                        1
#define TH_ACCEPT                       TH_READ
#define TH_CONNECT                      TH_WRITE

/* thread status */
#define THREAD_SUSPENDED                0
#define THREAD_RUNNING                  1
#define THREAD_DEAD                     2

/* thread flags */
#define THREAD_FLAGS_GENERAL            0
#define THREAD_FLAGS_NOSTACKALLOC       1
#define THREAD_FLAGS_KILLED             2
#define THREAD_FLAGS_ALARM              4
#define THREAD_FLAGS_USERSUSPEND        8
#define THREAD_FLAGS_DONTSTOP        	16
#define THREAD_FLAGS_DYING        	32
#define THREAD_FLAGS_BLOCKEDEXTERNAL	64

/*
 * If debug option DETECTDEADLOCK is given, detect deadlocks.  
 * A deadlock is defined as a situation where no thread is runnable and 
 * no threads is blocked on a timer, IO, or other external events.
 */

#define BLOCKED_ON_EXTERNAL(t)						\
	if (DBGEXPR(DETECTDEADLOCK, true, false)) {			\
	    tblocked_on_external++; 					\
	    t->flags |= THREAD_FLAGS_BLOCKEDEXTERNAL;			\
	}

#define CLEAR_BLOCKED_ON_EXTERNAL(t) 					\
	if (DBGEXPR(DETECTDEADLOCK, true, false)) {			\
		if (t->flags & THREAD_FLAGS_BLOCKEDEXTERNAL) { 		\
			tblocked_on_external--; 			\
			t->flags &= ~THREAD_FLAGS_BLOCKEDEXTERNAL;	\
		}							\
	}

/* number of threads blocked on external events */
static int tblocked_on_external;

/*
 * Variables.
 * These should be kept static to ensure encapsulation.
 */
static int preemptive = true;	/* enable preemptive scheduling */
static int talive; 		/* number of threads alive */
static int tdaemon;		/* number of daemons alive */
static void (*runOnExit)(void);	/* function to run when all non-daemon die */

static jthread**threadQhead;	/* double-linked run queue */ 
static jthread**threadQtail;
static jthread* liveThreads;	/* list of all live threads */
static jthread* alarmList;	/* list of all threads on alarm queue */
static jthread* waitForList;	/* list of all threads waiting for a child */

static int maxFd = -1;		/* highest known fd */
static fd_set readsPending;	/* fds we want to read from */
static fd_set writesPending;	/* fds we want to write to */
static jthread* readQ[FD_SETSIZE];	/* threads blocked on read */
static jthread* writeQ[FD_SETSIZE];	/* threads blocked on write */
static jmutex threadLock;	/* static lock to protect liveThreads etc. */

/* from old version, it is not clear that this is still important 
 * see comments near use
 */
static int alarmBlocked;

static int sigPending;		/* flags that says whether a intr is pending */
static int pendingSig[NSIG];	/* array that says which intrs are pending */

static int blockInts;		/* counter that says whether irqs are blocked */
static int needReschedule;	/* is a change in the current thread required */

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

jthread* currentJThread;

/*
 * Function declarations.
 * Again, keep these static to ensure encapsulation.
 */
static void handleInterrupt(int sig);
static void interrupt(int sig);
static void childDeath(void);
static void handleIO(int);
static void blockOnFile(int fd, int op);
static void killThread(jthread *jtid);
static void resumeThread(jthread* jtid);
static void reschedule(void);
static void restore_fds(void);
static void restore_fds_and_exit(void);
static void die(void);
static int jthreadedFileDescriptor(int fd);

/*
 * macros to set and extract stack pointer from jmp_buf
 * make sure SP_OFFSET has the correct value for your architecture!
 */
#define GET_SP(E)       (((void**)(E))[SP_OFFSET])
#define SET_SP(E, V)    ((void**)(E))[SP_OFFSET] = (V)
#define GET_FP(E)       (((void**)(E))[FP_OFFSET])
#define SET_FP(E, V)    ((void**)(E))[FP_OFFSET] = (V)

/* amount of stack space to be duplicated at stack creation time */
#if !defined(STACK_COPY)
#define STACK_COPY      (32*4)
#endif

#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif

/* Select an alarm system */
#if defined(HAVE_SETITIMER) && defined(ITIMER_REAL)
#define	MALARM(_mt)							\
	{								\
		struct itimerval tm;					\
		tm.it_interval.tv_sec = 0;				\
		tm.it_interval.tv_usec = 0;				\
		tm.it_value.tv_sec = (_mt) / 1000;			\
		tm.it_value.tv_usec = ((_mt) % 1000) * 1000;		\
		setitimer(ITIMER_REAL, &tm, 0);				\
	}
#elif defined(HAVE_ALARM)
#define	MALARM(_mt)	alarm((int)(((_mt) + 999) / 1000))
#endif

/*============================================================================
 *
 * Functions related to run and alarm queue manipulation
 */
/*
 * yield to another thread
 */
static inline void
internalYield(void)
{
        int priority = currentJThread->priority; 
   
        if (threadQhead[priority] &&
		threadQhead[priority] != threadQtail[priority])
        {
                /* Get the first thread and move it to the end */
		jthread *firstThread = threadQhead[priority];
                threadQhead[priority] = firstThread->nextQ;  
                threadQtail[priority]->nextQ = firstThread;
                threadQtail[priority] = firstThread;
                firstThread->nextQ = 0;
                needReschedule = true;
        }
}

static void
addToAlarmQ(jthread* jtid, jlong timeout)
{
	jthread** tidp;

	assert(intsDisabled());

	jtid->flags |= THREAD_FLAGS_ALARM;

	/* Get absolute time */
	jtid->time = timeout + currentTime();

	/* Find place in alarm list and insert it */
	for (tidp = &alarmList; (*tidp) != 0; tidp = &(*tidp)->nextalarm) {
		if ((*tidp)->time > jtid->time) {
			break;
		}
	}
	jtid->nextalarm = *tidp;
	*tidp = jtid;

	/* If I'm head of alarm list, restart alarm */
	if (tidp == &alarmList) {
		MALARM(timeout);
	}
}

static void
removeFromAlarmQ(jthread* jtid)
{
	jthread** tidp;

	assert(intsDisabled());

	jtid->flags &= ~THREAD_FLAGS_ALARM;

	/* Find thread in alarm list and remove it */
	for (tidp = &alarmList; (*tidp) != 0; tidp = &(*tidp)->nextalarm) {
		if ((*tidp) == jtid) {
			(*tidp) = jtid->nextalarm;
			jtid->nextalarm = 0;
			break;
		}
	}
}


/*
 * Resume a thread running.
 * This routine has to be called only from locations which ensure
 * run / block queue consistency. There is no check for illegal resume
 * conditions (like explicitly resuming an IO blocked thread). 
 */
static void
resumeThread(jthread* jtid)
{
	jthread** ntid;

DBG(JTHREAD,	dprintf("resumeThread %x\n", jtid);	)
	intsDisable();

	if (jtid->status != THREAD_RUNNING) {

		CLEAR_BLOCKED_ON_EXTERNAL(jtid);

		/* Remove from alarmQ if necessary */
		if ((jtid->flags & THREAD_FLAGS_ALARM) != 0) {
			removeFromAlarmQ(jtid);
		}
		/* Remove from lockQ if necessary */
		if (jtid->blockqueue != 0) {
			for (ntid = jtid->blockqueue; 
				*ntid != 0; 
				ntid = &(*ntid)->nextQ) 
			{
				if (*ntid == jtid) {
					*ntid = jtid->nextQ;
					break;
				}
			}
			jtid->blockqueue = 0;
		}

		jtid->status = THREAD_RUNNING;

		/* Place thread on the end of its queue */
		if (threadQhead[jtid->priority] == 0) {
			threadQhead[jtid->priority] = jtid;
			threadQtail[jtid->priority] = jtid;
			if (jtid->priority > currentJThread->priority) {
				needReschedule = true;
			}
		}
		else {
			threadQtail[jtid->priority]->nextQ = jtid;
			threadQtail[jtid->priority] = jtid;
		}
		jtid->nextQ = 0;
	} else {
DBG(JTHREAD,		dprintf("Re-resuming 0x%x\n", jtid); )
	}
	intsRestore();
}

/*
 * Suspend a thread on a queue.
 */
void
suspendOnQThread(jthread* jtid, jthread** queue, jlong timeout)
{
	jthread** ntid;
	jthread* last;

DBG(JTHREAD,	dprintf("suspendOnQThread %p %p (%qd) bI %d\n", 
	jtid, queue, timeout, blockInts); )

	assert(intsDisabled());

	if (jtid->status != THREAD_SUSPENDED) {
		jtid->status = THREAD_SUSPENDED;

		last = 0;
		for (ntid = &threadQhead[jtid->priority]; 
			*ntid != 0; 
			ntid = &(*ntid)->nextQ) 
		{
			if (*ntid == jtid) {
				/* Remove thread from runq */
				*ntid = jtid->nextQ;
				if (*ntid == 0) {
					threadQtail[jtid->priority] = last;
				}

				/* Insert onto head of lock wait Q */
				if (queue != 0) {
					jtid->nextQ = *queue;
					*queue = jtid;
					jtid->blockqueue = queue;
				}

				/* If I have a timeout, insert into alarmq */
				if (timeout > NOTIMEOUT) {
					addToAlarmQ(jtid, timeout);
				}

				/* If I was running, reschedule */
				if (jtid == currentJThread) {
					reschedule();
				}
				break;
			}
			last = *ntid;
		}
	} else {
	DBG(JTHREAD,	
		dprintf("Re-suspending 0x%x on %x\n", jtid, *queue); )
	}
}

/*
 * Kill thread.
 */
static void 
killThread(jthread *tid)
{
	jthread	**ntid;
	jthread* last;

	intsDisable();

	/* allow thread to perform any action before being killed -
	 * such as notifying on the object 
	 */
	if (destructor1)
		(*destructor1)(tid->jlThread);

DBG(JTHREAD,	
	dprintf("killThread %x kills %x\n", currentJThread, tid); )

	if (tid->status != THREAD_DEAD) {

		/* Get thread off runq (if it needs it) */
		if (tid->status == THREAD_RUNNING) {
			int pri = tid->priority;
			last = 0;

			for (ntid = &threadQhead[pri]; 
				*ntid != 0; 
				ntid = &(*ntid)->nextQ) 
			{
				if (*ntid == tid) {
					*ntid = tid->nextQ;
					if (*ntid == 0)
						threadQtail[pri] = last;	
					break;
				}
				last = *ntid;
			}
		}

		/* Run something else */
		if (currentJThread == tid) {
			needReschedule = true;
			blockInts = 1;
		}

		/* Dead Jim - let the GC pick up the remains */
		tid->status = THREAD_DEAD;
	}

	intsRestore();
}


/*============================================================================
 *
 * Functions related to interrupt handling
 *
 */

/*
 * check whether interrupts are disabled
 */
int
intsDisabled(void)
{       
        return blockInts > 0;           
}       

/*
 * disable interrupts
 *
 * Instead of blocking signals, we increment a counter.
 * If a signal comes in while the counter is non-zero, we set a pending flag
 * and mark the signal as pending.
 *
 * intsDisable may be invoked recursively. (is that really a good idea? - gb)
 */
void 
intsDisable(void)
{
        blockInts++;
}

static void
processSignals(void)
{
	int i;
	for (i = 1; i < NSIG; i++) {
		if (pendingSig[i])
			handleInterrupt(i);
		pendingSig[i] = 0;
	}
	sigPending = 0;
}

/*
 * restore interrupts
 *
 * If interrupts are about to be reenabled, execute the handlers for all
 * signals that are pending.
 */
void
intsRestore(void)
{ 
        /* DEBUG */
        assert(blockInts >= 1);

        if (blockInts == 1) {   
                if (sigPending)
			processSignals();
 
		/* reschedule if necessary */
                if (needReschedule == true)
                        reschedule(); 
        }
        blockInts--;
}

/*
 * reenable interrupts, non-recursive version.
 */
void
intsRestoreAll(void)
{
	blockInts = 1;
	intsRestore();
}

/*
 * Handle a signal/interrupt.
 *
 * This is the handler given to catchSignal.
 */
static void
interrupt(int sig)
{
        static int withchild = false;

        /* This bizzare bit of code handles the SYSV machines which re-throw 
	 * SIGCHLD when you reset the handler.  It also works for those that 
	 * don't.
         * Perhaps there's a way to detect this in the configuration process?
         */
        if (sig == SIGCHLD) {
		if (withchild == true) {
			return;
		}
		withchild = true;
	}
	/* Re-enable signal - necessary for SysV */
        catchSignal(sig, interrupt);
	withchild = false;

	/*
	 * If ints are blocked, this might indicate an inconsistent state of
	 * one of the thread queues (either alarmList or threadQhead/tail).
	 * We better don't touch one of them in this case and come back later.
	 */
	if (blockInts > 0) {
		pendingSig[sig] = 1;
		sigPending = 1;
		return;
	}
	intsDisable();

	pendingSig[sig] = 0;
	handleInterrupt(sig);

	/*
	 * The next bit is rather tricky.  If we don't reschedule then things
	 * are fine, we exit this handler and everything continues correctly.
	 * On the otherhand, if we do reschedule, we will schedule the new
	 * thread with alarms blocked which is wrong.  However, we cannot
	 * unblock them here incase we have just set an alarm which goes
	 * off before the reschedule takes place (and we enter this routine
	 * recusively which isn't good).  So, we set a flag indicating alarms
	 * are blocked, and allow the rescheduler to unblock the alarm signal
	 * after the context switch has been made.  At this point it's safe.
	 */
	alarmBlocked = true;
	intsRestore();
	alarmBlocked = false;
}

/*
 * handle a SIGVTALRM alarm.
 *
 * If preemption is disabled, we have the current thread so that it is
 * scheduled in a round-robin fashion with its peers who have the same
 * priority.
 */
static void 
handleVtAlarm(void)
{
	static int c;

	if (preemptive)
		internalYield();

	/*
	 * This is kind of ugly: some fds won't send us SIGIO.
	 * Example: the pseudo-tty driver in FreeBSD won't send a signal
	 * if we blocked on a write because the output buffer was full, and
	 * the output buffer became empty again.
	 *
	 * So we check periodically, every 0.2 seconds virtual time.
	 */
	if (++c % 20 == 0)
		handleIO(false);
}

/*
 * handle a SIGALRM alarm.
 */
static void 
alarmException(void)
{
	jthread* jtid;
	jlong time;

	/* Wake all the threads which need waking */
	time = currentTime();
	while (alarmList != 0 && alarmList->time <= time) {
		/* Restart thread - this will tidy up the alarm and blocked
		 * queues.
		 */
		jtid = alarmList;
		alarmList = alarmList->nextalarm;
		resumeThread(jtid);
	}

	/* Restart alarm */
	if (alarmList != 0) {
		MALARM(alarmList->time - time);
	}
}

/*
 * handle an interrupt.
 * 
 * this function is either invoked from within a signal handler, or as the
 * result of intsRestore.
 */
static void 
handleInterrupt(int sig)
{
	switch(sig) {
	case SIGALRM:
		alarmException();
		break;

	case SIGVTALRM:
		handleVtAlarm();
		break;

	case SIGCHLD:
		childDeath();
		break;

	case SIGIO:
		handleIO(false);
		break;

	default:
		printf("unknown signal %d\n", sig);
		exit(-1);
	}
}

/*============================================================================
 *
 * Functions dealing with thread contexts and the garbage collection interface
 *
 */

/*
 * Allocate a new thread context and stack.
 */
static jthread*
newThreadCtx(int stackSize)
{
	jthread *ct;

	ct = allocator(sizeof(jthread) + stackSize);
	ct->stackBase = (ct + 1);
	ct->stackEnd = ct->stackBase + stackSize;
	ct->restorePoint = ct->stackEnd;
	ct->status = THREAD_SUSPENDED;

DBG(JTHREAD,
	dprintf("allocating new thread, stack base %p-%p\n", 
	    ct->stackBase, ct->stackEnd); )

	return (ct);
}

/*
 * free a thread context and its stack
 */
void    
jthread_destroy(jthread *jtid)
{
	deallocator(jtid);
}

/*
 * iterate over all live threads
 */
void
jthread_walkLiveThreads(void (*func)(void *jlThread))
{
        jthread* tid;

        for (tid = liveThreads; tid != NULL; tid = tid->nextlive) {
                func(tid->jlThread);
        }
}

/*
 * determine the interesting stack range for a conservative gc
 */
void
jthread_extract_stack(jthread *jtid, void **from, unsigned *len)
{
    assert(jtid);
#if defined(STACK_GROWS_UP)
    *from = jtid->stackBase;
    *len = jtid->restorePoint - jtid->stackBase;
#else   
    *from = jtid->restorePoint;
    *len = jtid->stackEnd - jtid->restorePoint;
#endif
}

/*
 * determine whether an address lies on your current stack frame
 */
int
jthread_on_current_stack(void *bp)
{
        return bp >= currentJThread->stackBase && bp < currentJThread->stackEnd;
}       

/* 
 * XXX this is supposed to count the number of stack frames 
 */
int
jthread_frames(jthread *thrd)
{
        return 0;
}

/*============================================================================
 *
 * Functions for initialization and thread creation
 *
 */

#if defined(HAVE_SETITIMER)
/*
 * set virtual timer for 10ms round-robin time-slicing
 */
static void
activate_time_slicing(void)
{
	struct itimerval tm;
	tm.it_interval.tv_sec = tm.it_value.tv_sec = 0;
	tm.it_interval.tv_usec = tm.it_value.tv_usec = 10000;/* 10 ms */
	setitimer(ITIMER_VIRTUAL, &tm, 0);
}

/*
 * deactivate virtual timer for 10ms round-robin time-slicing
 */
static void
deactivate_time_slicing(void)
{
	struct itimerval tm;
	tm.it_interval.tv_sec = tm.it_value.tv_sec = 0;
	tm.it_interval.tv_usec = tm.it_value.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL, &tm, 0);
}
#else
static void activate_time_slicing(void) { }
static void deactivate_time_slicing(void) { }
#endif

/*
 * Initialize the threading system. 
 */
jthread * 
jthread_init(int pre,
	int maxpr, int minpr, int mainthreadpr, 
	size_t mainThreadStackSize,
	void *(*_allocator)(size_t), 
	void (*_deallocator)(void*),
	void (*_destructor1)(void*),
	void (*_onstop)(void))
{
        jthread *jtid; 
	int i;

	/* 
	 * If debugging is not enabled, set stdin, stdout, and stderr in 
	 * async mode.
	 *
	 * If debugging is enabled and ASYNCSTDIO is not given, do not
	 * put them in async mode.
	 *
	 * If debugging is enabled and ASYNCSTDIO is given, put them in
	 * async mode (as if debug weren't enabled.)
	 *
	 * This is useful because large amounts of fprintfs might be
	 * not be seen if the underlying terminal is put in asynchronous
	 * mode.  So by default, when debugging, we want stdio be synchronous.
	 * To override this, give the ASYNCSTDIO flag.
	 */
	if (DBGEXPR(ANY, DBGEXPR(ASYNCSTDIO, true, false), true))
		for (i = 0; i < 3; i++)
			if (i != jthreadedFileDescriptor(i))
				return 0;

	/*
	 * On some systems, it is essential that we put the fds back
	 * in their non-blocking state
	 */
	atexit(restore_fds);
	catchSignal(SIGINT, restore_fds_and_exit);
	catchSignal(SIGTERM, restore_fds_and_exit);
	/* XXX this is f***ed.  On BSD, we get a SIGHUP if we try to put
	 * a process that has a pseudo-tty in async mode in the background
	 * So we'll just ignore it and keep running.  Note that this will
	 * detach us from the session too.
	 */
	catchSignal(SIGHUP, SIG_IGN);

	preemptive = pre;
	max_priority = maxpr;
	min_priority = minpr;
	allocator = _allocator;
	deallocator = _deallocator;
	onstop = _onstop;
	destructor1 = _destructor1;
	threadQhead = allocator((maxpr + 1) * sizeof (jthread *));
	threadQtail = allocator((maxpr + 1) * sizeof (jthread *));

	catchSignal(SIGVTALRM, interrupt);
	catchSignal(SIGALRM, interrupt);
	catchSignal(SIGIO, interrupt);

#if defined(SIGCHLD)
        catchSignal(SIGCHLD, interrupt);
#endif  

	jtid = newThreadCtx(0);
	if (!jtid)
		return 0;

        jtid->priority = mainthreadpr;
        jtid->status = THREAD_SUSPENDED;
        jtid->flags = THREAD_FLAGS_NOSTACKALLOC;
        jtid->func = (void (*)(void*))jthread_init;
        jtid->nextlive = liveThreads;
        jtid->time = 0;
        liveThreads = jtid;

	/*
	 * Note: the stackBase and stackEnd values are used for two purposes:
	 * - to report to the gc what area to scan (extract_stack)
	 * - to help in determining whether the next frame in the link chain
	 *   of frames is valid.  This is done by checking its range.
	 */
#if defined(STACK_GROWS_UP)
	jtid->stackBase = ((void *)&jtid);
	jtid->stackEnd = jtid->stackBase + mainThreadStackSize;
        jtid->restorePoint = jtid->stackEnd;
#else
        jtid->stackEnd = ((void *)&jtid);
        jtid->stackBase = jtid->stackEnd - mainThreadStackSize;
        jtid->restorePoint = jtid->stackBase;
#endif
        talive++;
        currentJThread = jtid;
        resumeThread(jtid);
        return jtid;
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
	intsDisable();
	currentJThread->flags |= THREAD_FLAGS_DONTSTOP;
	intsRestore();
}

/*
 * reallow cancellation and stop if cancellation pending
 */
void 
jthread_enable_stop(void)
{
	intsDisable();
	currentJThread->flags &= ~THREAD_FLAGS_DONTSTOP;
	if ((currentJThread->flags & THREAD_FLAGS_KILLED) != 0 && 
		blockInts == 1) 
	{
		die();
	}
	intsRestore();
}

/*
 * interrupt a thread
 */
void
jthread_interrupt(jthread *jtid)
{
	resumeThread(jtid);	/* XXX */
}

static void 
die(void)
{
	currentJThread->flags &= ~THREAD_FLAGS_KILLED;
	currentJThread->flags |= THREAD_FLAGS_DYING;
	assert(blockInts == 1);
	blockInts = 0;
	/* this is used to throw a ThreadDeath exception */
	onstop();
	assert(!"Rescheduling dead thread");
}

static void
start_this_sucker_on_a_new_frame(void)
{
	/* I might be dying already */
	if ((currentJThread->flags & THREAD_FLAGS_KILLED) != 0)
		die();

	/* all threads start with interrupts turned off */
	intsRestore();
	currentJThread->func(currentJThread->jlThread);
	jthread_exit(); 
}


/*
 * create a new jthread
 */
jthread *
jthread_create(unsigned char pri, void (*func)(void *), int daemon,
        void *jlThread, size_t threadStackSize)
{
	jthread *jtid; 
	void	*oldstack, *newstack;

	jmutex_lock(&threadLock);
        jtid = newThreadCtx(threadStackSize);
        assert(jtid != 0);      /* XXX */
        jtid->priority = pri;
        jtid->jlThread = jlThread;
        jtid->status = THREAD_SUSPENDED;
        jtid->flags = THREAD_FLAGS_GENERAL;

        jtid->nextlive = liveThreads;
        liveThreads = jtid;

        talive++;       
	/* HACK: thread 1 is the main thread,
	 * thread 2 and 3 are the garbage collector/finalizer.
	 * Let's assume for now the finalizer does not have to be
	 * preempted.
	 */
	if (talive > 3) {
		activate_time_slicing();
	}

        if ((jtid->daemon = daemon) != 0) {
                tdaemon++;
        }
DBG(JTHREAD,
	dprintf("creating thread %p, daemon=%d\n", jtid, daemon); )
	jmutex_unlock(&threadLock);

        assert(func != 0); 
        jtid->func = func;

	/* 
	 * set the first jmp point 
	 *
	 * Note that when we return from setjmp in the context of
	 * a new thread, we must no longer access any local variables
	 * in this function.  The reason is that we didn't munge
	 * the base pointer that is used to access these variables.
	 *
	 * To be safe, we immediately call a new function.
	 */
        if (setjmp(jtid->env)) {
		/* new thread */
		start_this_sucker_on_a_new_frame();
		assert(!"Never!");
		/* NOT REACHED */
	} 

#if defined(SAVE_FP)
	SAVE_FP(jtid->fpstate);
#endif
	/* set up context for new thread */
	oldstack = GET_SP(jtid->env);

#if defined(STACK_GROWS_UP)
	newstack = jtid->stackBase+STACK_COPY;
	memcpy(newstack-STACK_COPY, oldstack-STACK_COPY, STACK_COPY);
#else /* !STACK_GROWS_UP */
	newstack = jtid->stackEnd-STACK_COPY;
	memcpy(newstack, oldstack, STACK_COPY);
#endif /* !STACK_GROWS_UP */

	SET_SP(jtid->env, newstack);

#if defined(FP_OFFSET)
	/* needed for: IRIX */
	SET_FP(jtid->env, newstack + ((void *)GET_FP(jtid->env) - oldstack));
#endif

        resumeThread(jtid);
        return jtid;
}

/*============================================================================
 *
 * Functions that are part of the user interface
 *
 */

/*
 * yield to a thread of equal priority
 */
void
jthread_yield(void)
{
        intsDisable();
        internalYield();
        intsRestore();
}

/*      
 * sleep for time milliseconds
 */     
void
jthread_sleep(jlong time)
{
        if (time == 0) {
                return; 
        }
        intsDisable();
	BLOCKED_ON_EXTERNAL(currentJThread);
        suspendOnQThread(currentJThread, 0, time);
        intsRestore();
}

/* 
 * Check whether a thread is alive.
 *
 * Note that threads executing their onstop function are not alive.
 */
int
jthread_alive(jthread *jtid)
{
        int status = true;
        intsDisable();
        if (jtid == 0 || 
		(jtid->flags & (THREAD_FLAGS_KILLED | THREAD_FLAGS_DYING)) != 0 
		|| jtid->status == THREAD_DEAD)
                status = false;
        intsRestore();
        return status;
}

/*
 * Change thread priority.
 */
void
jthread_setpriority(jthread* jtid, int prio)
{
	jthread** ntid;
	jthread* last;

	if (jtid->status == THREAD_SUSPENDED) {
		jtid->priority = (unsigned char)prio;
		return;
	}

	intsDisable();

	/* Remove from current thread list */
	last = 0;
	for (ntid = &threadQhead[jtid->priority]; 
		*ntid != 0; 
		ntid = &(*ntid)->nextQ) 
	{
		if (*ntid == jtid) {
			*ntid = jtid->nextQ;
			if (*ntid == 0) {
				threadQtail[jtid->priority] = last;
			}
			break;
		}
		last = *ntid;
	}

	/* Insert onto a new one */
	jtid->priority = (unsigned char)prio;
	if (threadQhead[prio] == 0) {
		threadQhead[prio] = jtid;
		threadQtail[prio] = jtid;
	}
	else {
		threadQtail[prio]->nextQ = jtid;
		threadQtail[prio] = jtid;
	}
	jtid->nextQ = 0;

	/* If I was rescheduled, or something of greater priority was,
	 * insist on a reschedule.
	 */
	if (jtid == currentJThread || prio > currentJThread->priority) {
		needReschedule = true;
	}

	intsRestore();
}

/*
 * Stop a thread in its tracks.
 */
void
jthread_stop(jthread *jtid)
{
        jtid->flags |= THREAD_FLAGS_KILLED;
	intsDisable();

	/* if it's us, die */
	if (jtid == jthread_current() && 
		(jtid->flags & THREAD_FLAGS_DONTSTOP) != 0 && blockInts == 1)
		die();

        resumeThread(jtid);
	intsRestore();
}

/*
 * Have a thread exit.
 */
void
jthread_exit(void)
{
	jthread** ntid;
	jthread* tid;

DBG(JTHREAD,
	dprintf("jthread_exit %x\n", currentJThread);		)

	jmutex_lock(&threadLock);

	talive--;
	if (currentJThread->daemon) {
		tdaemon--;
	}

	/* Remove thread from live list so it can be garbaged */
	for (ntid = &liveThreads; *ntid != 0; ntid = &(*ntid)->nextlive) 
	{
		if (currentJThread == (*ntid)) {
			(*ntid) = currentJThread->nextlive;
			break;
		}
	}

	jmutex_unlock(&threadLock);

	/* If we only have daemons left, then we should exit. */
	if (talive == tdaemon) {
DBG(JTHREAD,
	dprintf("all done, closing shop\n");	)
		if (runOnExit != 0) {
		    runOnExit();
		}

		for (tid = liveThreads; tid != 0; tid = tid->nextlive) {
		    killThread(tid);
		}
		EXIT(0);
	}
	for (;;) {
		killThread(currentJThread);
		jthread_sleep(1000);
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

/*
 * Reschedule the thread.
 * Called whenever a change in the running thread is required.
 */
void
reschedule(void)
{
	int i;
	jthread* lastThread;
	int b;
	sigset_t nsig;

	/* A reschedule in a non-blocked context is half way to hell */
	assert(intsDisabled());

	b = blockInts;

	for (;;) {
	        for (i = max_priority; i >= min_priority; i--) {
			if (threadQhead[i] == 0) 
			    continue;

			if (threadQhead[i] != currentJThread) {
				lastThread = currentJThread;
				currentJThread = threadQhead[i];

DBG(JTHREADDETAIL,
dprintf("switch from %p to %p\n", lastThread, currentJThread); )

				/* save and restore floating point state */
#if defined(SAVE_FP)
				SAVE_FP(lastThread->fpstate);
#endif
#if defined(CONTEXT_SWITCH)
				CONTEXT_SWITCH(lastThread, currentJThread);
#else
				if (setjmp(lastThread->env) == 0) {
				    lastThread->restorePoint = 
					GET_SP(lastThread->env);
				    longjmp(currentJThread->env, 1);
				}
#endif
#if defined(LOAD_FP)
				LOAD_FP(currentJThread->fpstate);
#endif
				/* Alarm signal may be blocked - if so
				 * unblock it. - XXX
				 */
				if (alarmBlocked == true) {
					alarmBlocked = false;
					sigemptyset(&nsig);
					sigaddset(&nsig, SIGALRM);
					sigaddset(&nsig, SIGVTALRM);
					sigaddset(&nsig, SIGIO);
					sigprocmask(SIG_UNBLOCK, 
						&nsig, 0);
				}

				/* Restore ints */
				blockInts = b;

				assert(currentJThread == lastThread);

				/* I might be dying */
				if ((currentJThread->flags & 
					THREAD_FLAGS_KILLED) != 0 && 
				    (currentJThread->flags & 
					THREAD_FLAGS_DONTSTOP) == 0 &&
				    blockInts == 1) 
					die();
			}
			/* Now kill the schedule */
			needReschedule = false;
			return;
		}

		if (DBGEXPR(DETECTDEADLOCK, true, false) &&
			tblocked_on_external == 0)
			assert(!!!"Deadlock: "
			   " all threads blocked on internal events\n");
		handleIO(true);
	}
}

/*============================================================================
 * 
 * I/O interrupt related functions
 *
 */

/*
 * Process incoming SIGIO
 * return 1 if select was interrupted
 */
static
void
handleIO(int sleep)
{
	int r;
	fd_set rd;
	fd_set wr;
	jthread* tid;
	jthread* ntid;
	struct timeval zero = { 0, 0 };
	int i, b = 0;

	assert(intsDisabled());

DBG(JTHREADDETAIL,
	dprintf("handleIO(sleep=%d)\n", sleep);		)

	FD_COPY(&readsPending, &rd);
	FD_COPY(&writesPending, &wr);

	/*
	 * figure out which fds are ready
	 */
retry:
	if (sleep) {
		b = blockInts;
		blockInts = 0;
	}
	r = select(maxFd+1, &rd, &wr, 0, sleep ? 0 : &zero);
	if (sleep) {
		blockInts = b;
		if (sigPending)
			processSignals();
	}
	if ((r < 0 && errno == EINTR) && !sleep) 
		goto retry;

	if (r <= 0)
		return;

DBG(JTHREADDETAIL,
	dprintf("Select returns %d\n", r);			)

	for (i = 0; r > 0 && i <= maxFd; i++) {
		if (readQ[i] != 0 && FD_ISSET(i, &rd)) {
			needReschedule = true;
			for (tid = readQ[i]; tid != 0; tid = ntid) {
				ntid = tid->nextQ;
				tid->blockqueue = 0;
				resumeThread(tid);
			}
			readQ[i] = 0;
			r--;
		}
		if (writeQ[i] != 0 && FD_ISSET(i, &wr)) {
			needReschedule = true;
			for (tid = writeQ[i]; tid != 0; tid = ntid) {
				ntid = tid->nextQ;
				tid->blockqueue = 0;
				resumeThread(tid);
			}
			writeQ[i] = 0;
			r--;
		}
	}
	return;
}

/*
 * A file I/O operation could not be completed. Sleep until we are woken up
 * by the SIGIO handler.
 *
 * Interrupts are disabled on entry and exit.
 * fd is assumed to be valid.
 */
static void
blockOnFile(int fd, int op)
{
DBG(JTHREAD,
	dprintf("blockOnFile(%d,%s)\n", fd, op == TH_READ ? "r":"w"); )

	assert(intsDisabled());
	BLOCKED_ON_EXTERNAL(currentJThread);

	if (fd > maxFd) {
		maxFd = fd;
	}
	if (op == TH_READ) {
		FD_SET(fd, &readsPending);
		suspendOnQThread(currentJThread, &readQ[fd], NOTIMEOUT);
		FD_CLR(fd, &readsPending);
	}
	else {
		FD_SET(fd, &writesPending);
		suspendOnQThread(currentJThread, &writeQ[fd], NOTIMEOUT);
		FD_CLR(fd, &writesPending);
	}
}

/*============================================================================
 * 
 * locking subsystem
 *
 */

void 
jmutex_initialise(jmutex *lock)
{
	lock->holder = lock->waiting = NULL;
}

void
jmutex_lock(jmutex *lock)
{
	intsDisable();
	while (lock->holder != NULL)
		suspendOnQThread(jthread_current(), &lock->waiting, NOTIMEOUT);

	lock->holder = jthread_current();			
	intsRestore();
}

void
jmutex_unlock(jmutex *lock)
{
	intsDisable();
	lock->holder = NULL;
	if (lock->waiting != 0) {
		jthread* tid;
		tid = lock->waiting;
		lock->waiting = tid->nextQ;
		assert(tid->status != THREAD_RUNNING);
		tid->blockqueue = 0;
		resumeThread(tid);
	}
	intsRestore();
}

void
jcondvar_initialise(jcondvar *cv)
{
	cv = NULL;
}

void
jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout)
{
	jthread *current = jthread_current();
	intsDisable();

	/* give up mutex */
	lock->holder = NULL;			
	if (lock->waiting != NULL) {
		jthread* tid;
		tid = lock->waiting;
		lock->waiting = tid->nextQ;
		assert(tid->status != THREAD_RUNNING);
		tid->blockqueue = 0;
		resumeThread(tid);
	}
	/* wait to be signaled */
	suspendOnQThread(current, cv, timeout);
	/* reacquire mutex */
	while (lock->holder != NULL) {
		suspendOnQThread(current, &lock->waiting, NOTIMEOUT);
	}
	lock->holder = current;
	intsRestore();
}

void
jcondvar_signal(jcondvar *cv, jmutex *lock)
{
	intsDisable();
	if (*cv != NULL) {
		jthread* tid;
		/* take off condvar queue */
		tid = *cv;
		*cv = tid->nextQ;
		/* put on lock queue */
		tid->nextQ = lock->waiting;
		lock->waiting = tid;
	}
	intsRestore();
}

void
jcondvar_broadcast(jcondvar *cv, jmutex *lock)
{
	intsDisable();
	if (*cv != NULL) {
		/* splice the lists `*cv' and `lock->waiting' */

		jthread** tidp;
		/* advance to last element in cv list */
		for (tidp = cv; *tidp != 0; tidp = &(*tidp)->nextQ)
			;
		(*tidp) = lock->waiting;
		lock->waiting = *cv;
		*cv = NULL;
	}
	intsRestore();
}

/*============================================================================
 * 
 * I/O routines that are exported to the user
 *
 */

/*
 * Create a threaded file descriptor.
 *
 * We try various fcntl and ioctl to put the file descriptor in non-blocking
 * mode and to enable asynchronous notifications.
 */
static int
jthreadedFileDescriptor(int fd)
{
	int r, on = 1;
	int pid = getpid();

	if (fd == -1)
		return (fd);

	/* Make non-blocking */
	if ((r = fcntl(fd, F_GETFL, 0)) < 0) {
		perror("F_GETFL");
		return (r);
	}

#if defined(F_SETFD)
	/* set close-on-exec flag for this file descriptor */
	if ((r = fcntl(fd, F_SETFD, 1)) < 0) {
		perror("F_SETFD");
		return (r);
	}
#endif

	/*
	 * Apparently, this can fail, for instance when we stdout is 
	 * redirected to /dev/null. (On FreeBSD)
	 */
	fcntl(fd, F_SETFL, r | O_NONBLOCK 
#if defined(O_ASYNC)
		| O_ASYNC
#elif defined(FASYNC)
		| FASYNC
#endif
		);

#if defined(FIOSSAIOSTAT) && !(defined(hpux) && defined(FIOASYNC))
	r = ioctl(fd, FIOSSAIOSTAT, &on);
	if (r < 0 && errno != ENOTTY) {
		/* Defines ENOTTY to be an acceptable error */
		perror("FIOSSAIOSTAT");
 		return (r);
	}
#elif defined(FIOASYNC)
	/* 
	 * This ioctl fails for so many systems on so many occasions.
	 * Reasons include ENXIO, ENOTTY, EINVAL(?)
	 */
	r = ioctl(fd, FIOASYNC, &on);
	if (r < 0) {
		DBG(JTHREAD, perror("FIOASYNC"); )
        }
#endif

#if !(defined(O_ASYNC) || defined(FIOASYNC) ||  \
      defined(FASYNC) || defined(FIOSSAIOSTAT))
#error	Could not put socket in async mode
#endif


	/* Allow socket to signal this process when new data is available */
#if defined(FIOSSAIOOWN) && !(defined(hpux) && defined(F_SETOWN))
	r = ioctl(fd, FIOSSAIOOWN, &pid);
        if (r == -1 && errno != ENOTTY) {
		perror("Error doing FIOSSAIOWN");
	}
	
#elif defined(F_SETOWN)
	/* On some systems, this will flag an error if fd is not a socket */
	r = fcntl(fd, F_SETOWN, pid);
	if (r < 0) {
		DBG(JTHREAD, perror("F_SETOWN"); )
	}
#endif
	return (fd);
}

/*
 * In SVR4 systems (notably AIX and HPUX 9.x), putting a file descriptor 
 * in non-blocking mode affects the actual terminal file.  
 * Thus, the shell we see the fd in
 * non-blocking mode when we exit and log the user off.
 *
 * Under Solaris, this happens if you use FIONBIO to get into non-blocking 
 * mode.  (as opposed to O_NONBLOCK)
 */
static void
restore_fds(void)
{
	int i;
	/* clear non-blocking flag for file descriptor stdin, stdout, stderr */
	for (i = 0; i < 3; i++)
	    fcntl(i, F_SETFL, fcntl(i, F_GETFL, 0) & ~O_NONBLOCK);
}

static void
restore_fds_and_exit()
{
	restore_fds();
	/* technically, we should restore the original handler and rethrow
	 * the signal.
	 */
	EXIT(-1);		/* XXX */
}

/*
 * Threaded socket create.
 */
int
jthreadedSocket(int af, int type, int proto)
{
	int fd;

	fd = socket(af, type, proto);
	return (jthreadedFileDescriptor(fd));
}

/*
 * Threaded file open.
 */
int
jthreadedOpen(const char* path, int flags, int mode)
{
	int fd;

	/* Cygnus WinNT requires this */
	fd = open(path, flags|O_BINARY, mode);
	return (jthreadedFileDescriptor(fd));
}

/*
 * Threaded socket connect.
 */
int
jthreadedConnect(int fd, struct sockaddr* addr, size_t len)
{
	int r;

	intsDisable();	
	for (;;) {
		r = connect(fd, addr, len);
		if (r < 0 && !(errno == EINPROGRESS || errno == EINTR)) 
		      break;

		blockOnFile(fd, TH_CONNECT);
	}
	/* annul EISCONN error */
	if (r < 0 && errno == EISCONN)
		r = 0;
	intsRestore();
	return (r);
}

/*
 * Threaded socket accept.
 */
int
jthreadedAccept(int fd, struct sockaddr* addr, size_t* len)
{
	int r;

	intsDisable();
	for (;;) {
		r = accept(fd, addr, len);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR))
			break;
		blockOnFile(fd, TH_ACCEPT);
	}
	intsRestore();
	return (r >= 0 ? jthreadedFileDescriptor(r) : r);
}

/*
 * Threaded read
 */
ssize_t
jthreadedRead(int fd, void* buf, size_t len)
{
	ssize_t r;

	intsDisable();
	for (;;) {
		r = read(fd, buf, len);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR))
			break;

		blockOnFile(fd, TH_READ);
	}
	intsRestore();
	return r;
}

/*
 * Threaded write
 */
ssize_t
jthreadedWrite(int fd, const void* buf, size_t len)
{
	ssize_t r = 1;
	const void* ptr;

	ptr = buf;

	intsDisable();
	while (len > 0 && r > 0) {
		r = (ssize_t)write(fd, ptr, len);
		if (r >= 0) {
			ptr += r;
			len -= r;
			r = ptr - buf;
			continue;
		}
		if (!(errno == EWOULDBLOCK || errno == EINTR))
			break;

		blockOnFile(fd, TH_WRITE);
		r = 1;
	}
	intsRestore();
	return r; 
}

/*
 * Threaded recvfrom 
 */
ssize_t 
jthreadedRecvfrom(int fd, void* buf, size_t len, int flags, 
	struct sockaddr* from, int* fromlen)
{
	ssize_t r;
 
	intsDisable();
	for (;;) {
		r = recvfrom(fd, buf, len, flags, from, fromlen);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR))
			break;
		blockOnFile(fd, TH_READ);
	}
	intsRestore();
	return r;
}

/*============================================================================
 * 
 * Routines dealing with Process::waitFor. 
 *
 */

/*
 * Child process has died.
 */
static
void
childDeath(void)
{
	if (waitForList) {
		resumeThread(waitForList);
	}
}

/* 
 * Wait for a child process.
 */
int
jthreadedWaitpid(int wpid, int* status, int options)
{
#if defined(HAVE_WAITPID)
	int npid;

DBG(JTHREAD,
	dprintf("waitpid %d current=%p\n", wpid, currentJThread); )

	intsDisable();
	for (;;) {
		npid = waitpid(wpid, status, options|WNOHANG);
		if (npid > 0) {
			break;
		}
		BLOCKED_ON_EXTERNAL(currentJThread);
		suspendOnQThread(currentJThread, &waitForList, NOTIMEOUT);
	}
	intsRestore();
	return (npid);
#else
	return -1;
#endif
}

/* helper function for forkexec, close fd[0..n-1] */
static void
close_fds(int fd[], int n)
{
	int i = 0;
	while (i < n)
		close(fd[i++]);
}

int 
jthreadedForkExec(char **argv, char **arge, int ioes[4])
{
/* these defines are indices in ioes */
#define IN_IN		0
#define IN_OUT		1
#define OUT_IN		2
#define OUT_OUT		3
#define ERR_IN		4
#define ERR_OUT		5
#define SYNC_IN		6
#define SYNC_OUT	7

	int fds[8];
	int nfd;		/* number of fds in `fds' that are valid */
	sigset_t nsig;
	char b[1];
	int pid, i, err;

	/* 
	 * we need execve() and fork() for this to work.  Don't bother if
	 * we don't have them.
	 */
#if !defined(HAVE_EXECVE) && !defined(HAVE_EXECVP)
	unimp("neither execve() nor execvp() not provided");
#endif
#if !defined(HAVE_FORK)
	unimp("fork() not provided");
#endif

DBG(JTHREAD,	
	{
		char **d = argv;
		dprintf("argv = [`%s ", *d++); 
		while (*d)
			dprintf(", `%s'", *d++);
		dprintf("]\n");
	}
    )
	/* Create the pipes to communicate with the child */
	/* Make sure fds get closed if we can't create all pipes */
	for (nfd = 0; nfd < 8; nfd += 2) {
		err = pipe(fds + nfd);
		if (err == -1) {
			close_fds(fds, nfd);
			return -1;
		}
	}

	/* 
	 * We must avoid that the child dies because of SIGVTALRM or
	 * other signals.  We disable interrupts before forking and then
	 * reenable signals in the child after we cleaned up.
	 */
	sigfillset(&nsig);
	sigprocmask(SIG_BLOCK, &nsig, 0);

	pid = fork();

	switch (pid) {
	case 0:
		/* Child */
		/* turn timers off */
		deactivate_time_slicing();
		MALARM(0);

		/* set all signals back to their default state */
		for (i = 0; i < NSIG; i++) {
			catchSignal(i, SIG_DFL);
		}

		/* now reenable interrupts */
		sigprocmask(SIG_UNBLOCK, &nsig, 0);

		/* set stdin, stdout, and stderr up from the pipes */
		dup2(fds[IN_IN], 0);
		dup2(fds[OUT_OUT], 1);
		dup2(fds[ERR_OUT], 2);

		/* What is sync about anyhow?  Well my current guess is that
		 * the parent writes a single byte to it when it's ready to
		 * proceed.  So here I wait until I get it before doing
		 * anything.
		 */
		/* note that this is a blocking read */
		read(fds[SYNC_IN], b, sizeof(b));

		/* now close all pipe fds */
		close_fds(fds, 8);

		/*
		 * If no environment was given and we have execvp, we use it.
		 * If an environment was given, we use execve.
		 * This is roughly was the linux jdk seems to do.
		 */

		/* execute program */
#if defined(HAVE_EXECVP)
		if (arge == NULL)
			execvp(argv[0], argv);
		else
#endif
			execve(argv[0], argv, arge);
		break;

	case -1:
		/* Error */
		/* Close all pipe fds */
		close_fds(fds, 8);
		return (-1);

	default:
		/* Parent */
		/* close the fds we won't need */
		close(fds[IN_IN]);
		close(fds[OUT_OUT]);
		close(fds[ERR_OUT]);
		close(fds[SYNC_IN]);

		/* copy and fix up the fds we do need */
		ioes[0] = jthreadedFileDescriptor(fds[IN_OUT]);
		ioes[1] = jthreadedFileDescriptor(fds[OUT_IN]);
		ioes[2] = jthreadedFileDescriptor(fds[ERR_IN]);
		ioes[3] = jthreadedFileDescriptor(fds[SYNC_OUT]);

		sigprocmask(SIG_UNBLOCK, &nsig, 0);
		return (pid);
	}

	exit(-1);
	/* NEVER REACHED */	
}
