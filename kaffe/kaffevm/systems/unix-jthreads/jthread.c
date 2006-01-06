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

#include "config.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif /* defined(HAVE_SYS_WAIT_H) */

#include "jthread.h"
#include "jsignal.h"
#include "xprofiler.h"
#include "jqueue.h"
/* For NOTIMEOUT */
#include "jsyscall.h"
/* For jlong */
#include "jni_md.h"
/* For Hjava_lang_VMThread */
#include "thread.h"
#include "gc.h"

/* Flags used for threading I/O calls */
#define TH_READ                         0
#define TH_WRITE                        1
#define TH_ACCEPT                       TH_READ
#define TH_CONNECT                      TH_WRITE

/*
 * If option DETECTDEADLOCK is given, detect deadlocks.  
 * A deadlock is defined as a situation where no thread is runnable and 
 * no threads is blocked on a timer, IO, or other external events.
 *
 * Undeffing this will save a few cycles, but kaffe will just hang if
 * there is a deadlock.
 */
#define DETECTDEADLOCK

#if defined(DETECTDEADLOCK)
#define BLOCKED_ON_EXTERNAL(t)						\
	do {								\
	    tblocked_on_external++; 					\
	    t->flags |= THREAD_FLAGS_BLOCKEDEXTERNAL;			\
	} while (0)

#define CLEAR_BLOCKED_ON_EXTERNAL(t) 					\
	do {								\
		if (t->flags & THREAD_FLAGS_BLOCKEDEXTERNAL) { 		\
			tblocked_on_external--; 			\
			t->flags &= ~THREAD_FLAGS_BLOCKEDEXTERNAL;	\
		}							\
	} while (0)

/* number of threads blocked on external events */
static int tblocked_on_external;

#else /* !DETECTDEADLOCK */

#define BLOCKED_ON_EXTERNAL(t)
#define CLEAR_BLOCKED_ON_EXTERNAL(t)

#endif

/*
 * Variables.
 * These should be kept static to ensure encapsulation.
 */
static int preemptive = true;	/* enable preemptive scheduling */
static int talive; 		/* number of threads alive */
static int tdaemon;		/* number of daemons alive */
static void (*runOnExit)(void);	/* function to run when all non-daemon die */

#define JTHREADQ(q) ((jthread *)(q)->element)
static KaffePool *queuePool;    /* pool of single-linked node */
static KaffeNodeQueue**threadQhead;	/* double-linked run queue */ 
static KaffeNodeQueue**threadQtail;
static KaffeNodeQueue* liveThreads;	/* list of all live threads */
static KaffeNodeQueue* alarmList;	/* list of all threads on alarm queue */
static KaffeNodeQueue* waitForList;	/* list of all threads waiting for a child */

static int maxFd = -1;		/* highest known fd */
static fd_set readsPending;	/* fds we want to read from */
static fd_set writesPending;	/* fds we want to write to */
static KaffeNodeQueue* readQ[FD_SETSIZE];	/* threads blocked on read */
static KaffeNodeQueue* writeQ[FD_SETSIZE];	/* threads blocked on write */
static jboolean blockingFD[FD_SETSIZE];            /* file descriptor which should 
						   really block */
static jmutex threadLock;	/* static lock to protect liveThreads etc. */
static jmutex GClock;

static int sigPending;		/* flags that says whether a intr is pending */
static int pendingSig[NSIG];	/* array that says which intrs are pending */
static int sigPipe[2];		/* a pipe to ensure we don't lose our wakeup */
static int bytesInPipe;		/* total number of bytes written to pipe */
static int wouldlosewakeup;	/* a flag that says whether we're past the
				   point where we check for pending signals 
				   before sleeping in select() */

static int blockInts;		/* counter that says whether irqs are blocked */
static int needReschedule;	/* is a change in the current thread required */

/** This is the garbage collector to use to allocate thread data. */
static Collector *threadCollector;

/*
 * the following variables are set by jthread_init, and show how the
 * threading system is parametrized.
 */
static void (*destructor1)(void*);	/* call when a thread exits */
static void (*onstop)(void);		/* call when a thread is stopped */
static void (*ondeadlock)(void);	/* call when we detect deadlock */
static int  max_priority;		/* maximum supported priority */
static int  min_priority;		/* minimum supported priority */

jthread* currentJThread = NULL;
static jthread* firstThread = NULL;

/* Context switch related functions */
#ifndef JTHREAD_CONTEXT_SAVE
#define JTHREAD_CONTEXT_SAVE(buf)		JTHREAD_SETJMP((buf))
#endif
#ifndef JTHREAD_CONTEXT_RESTORE
#define JTHREAD_CONTEXT_RESTORE(buf, val)	JTHREAD_LONGJMP((buf), (val))
#endif

/* The arguments to a signal handler */
#ifndef SIGNAL_ARGS
#define SIGNAL_ARGS(sig, sc) int sig
#endif

/* Get a signal context pointer from signal arguments */
#ifndef GET_SIGNAL_CONTEXT_POINTER
#define GET_SIGNAL_CONTEXT_POINTER(x) 0
#endif

/* A signal context pointer type, used in parameter lists/declarations */
#ifndef SIGNAL_CONTEXT_POINTER
#define SIGNAL_CONTEXT_POINTER(x) void *x
#endif

/* Get the PC from a signal context pointer */
#ifndef SIGNAL_PC
#define SIGNAL_PC(scp) 0
#endif

/*
 * Function declarations.
 * Again, keep these static to ensure encapsulation.
 */
static void handleInterrupt(int sig, SIGNAL_CONTEXT_POINTER(sc));
static void interrupt(SIGNAL_ARGS(sig, sc));
static void childDeath(void);
static void handleIO(int);
static void killThread(jthread *jtid);
static void resumeThread(jthread* jtid);
static void reschedule(void);
static void restore_fds(void);
static void restore_fds_and_exit(void);
static void die(void);
static int jthreadedFileDescriptor(int fd);
static void intsDisable(void);
static void intsRestore(void);
static void addWaitQThread(jthread *jtid, KaffeNodeQueue **queue);
static void cleanupWaitQ(jthread *jtid);

/*
 * macros to set and extract stack pointer from jmp_buf
 * make sure SP_OFFSET has the correct value for your architecture!
 */
#define GET_SP(E)       (((void**)(E))[SP_OFFSET])
#define SET_SP(E, V)    ((void**)(E))[SP_OFFSET] = (V)
#define GET_FP(E)       (((void**)(E))[FP_OFFSET])
#define SET_FP(E, V)    ((void**)(E))[FP_OFFSET] = (V)

/*
 * Macros to set and extract backing store pointer from jmp_buf
 * (IA-64 specific)
 */
#if defined(__ia64__)
#define BSP_OFFSET	17
#define GET_BSP(E)	(((void**)(E))[BSP_OFFSET])
#define SET_BSP(E, V)	((void**)(E))[BSP_OFFSET] = (V)
#endif

/* Set the base pointer in a jmp_buf if we can (only a convenience) */
#if defined(BP_OFFSET)
#define SET_BP(E, V)    ((void**)(E))[BP_OFFSET] = (V)
#endif

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
 * Functions related to list manipulation and interrupt handling
 *
 */

/*
 * Check whether a thread is on a given list
 */
static int
isOnList(KaffeNodeQueue *list, jthread *t)
{
	for (; list != NULL; list = list->next) {
		if (JTHREADQ(list) == t) {
			return (1);
		}
	}
	return (0);
}


/*
 * yield to another thread
 */
static inline void
internalYield(void)
{
        int priority = currentJThread->priority; 
   
        if (threadQhead[priority] != 0 &&
		threadQhead[priority] != threadQtail[priority])
        {
                /* Get the first thread and move it to the end */
		KaffeNodeQueue *firstThreadNode = threadQhead[priority];
                threadQhead[priority] = firstThreadNode->next;  
                threadQtail[priority]->next = firstThreadNode;
                threadQtail[priority] = firstThreadNode;
                firstThreadNode->next = 0;
                needReschedule = true;
        }
}

static void
addToAlarmQ(jthread* jtid, jlong timeout)
{
	KaffeNodeQueue** tidp;
	KaffeNodeQueue* node;
	jlong ct;

	assert(intsDisabled());

	ct = currentTime();
	if( (timeout + ct) > ct ) {
		jtid->flags |= THREAD_FLAGS_ALARM;
		
		/* Get absolute time */
		jtid->time = timeout + ct;
		
		/* Find place in alarm list and insert it */
		for (tidp = &alarmList;
		     (*tidp) != 0;
		     tidp = &(*tidp)->next) {
		        if (JTHREADQ(*tidp)->time > jtid->time)
			{
				break;
			}
		}
		node = KaffePoolNewNode(queuePool);
		node->next = *tidp;
		node->element = jtid;
		*tidp = node;
		
		/* If I'm head of alarm list, restart alarm */
		if (tidp == &alarmList)
		{
			MALARM(timeout);
		}
	} else {
		/* Huge timeout value, ignore it. */
	}
}

static void
removeFromAlarmQ(jthread* jtid)
{
	KaffeNodeQueue** tidp;

	assert(intsDisabled());

	jtid->flags &= ~THREAD_FLAGS_ALARM;

	/* Find thread in alarm list and remove it */
	for (tidp = &alarmList; (*tidp) != 0; tidp = &(*tidp)->next)
	{
		if (JTHREADQ(*tidp) == jtid)
		{
			KaffeNodeQueue *node = *tidp;
		
			(*tidp) = node->next;
			KaffePoolReleaseNode(queuePool, node);
			break;
		}
	}
}

/*
 * check whether interrupts are disabled
 */
int
intsDisabled(void)
{
        return (blockInts > 0);
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
static inline void 
intsDisable(void)
{
        blockInts++;
}

static inline void
processSignals(void)
{
	int i;
	for (i = 1; i < NSIG; i++)
	{
		if (pendingSig[i])
		{
			pendingSig[i] = 0;
			handleInterrupt(i, 0);
		}
	}
	sigPending = 0;
}

/*
 * restore interrupts
 *
 * If interrupts are about to be reenabled, execute the handlers for all
 * signals that are pending.
 */
static inline void
intsRestore(void)
{ 
        /* KAFFE_VMDEBUG */
        assert(blockInts >= 1);

        if (blockInts == 1) {
                if (sigPending) {
			processSignals();
		}
 
		/* reschedule if necessary */
                if (needReschedule == true) {
                        reschedule(); 
		}
        }
        blockInts--;
}

/*
 * Prevent all other threads from running.
 * In this uniprocessor implementation, this is simple.
 */
void 
jthread_suspendall(void)
{
        intsDisable();
}

/*
 * Reallow other threads.
 * In this uniprocessor implementation, this is simple.
 */
void 
jthread_unsuspendall(void)
{
        intsRestore();
}  

/*
 * Handle an asynchronous signal (i.e. a software interrupt).
 *
 * This is the handler given to registerAsyncSignalHandler().
 *
 * It is guaranteed that all asynchronous signals are delayed when
 * this handler begins execution (see registerAsyncSignalHandler()).
 * There are two ways for the asynchronous signals to get unblocked:
 * (1) return from the function.  The OS will unblock them.  (2)
 * explicitly unblock the signals.  We must do this before performing
 * a thread context switch as the target thread should (obviously) not
 * be running with all signals blocked.
 */
static void
interrupt(SIGNAL_ARGS(sig, sc))
{
	if( currentJThread->status != THREAD_SUSPENDED )
	{
#ifdef ENABLE_JVMPI
		EXCEPTIONFRAME(jthread_current()->localData.topFrame, sc);
#endif
	}
	
	/*
	 * If ints are blocked, this might indicate an inconsistent state of
	 * one of the thread queues (either alarmList or threadQhead/tail).
	 *
	 * Record this interrupt as pending so that the forthcoming
	 * intsRestore() (the intsRestore() in the interrupted thread)
	 * will handle it.  Then return from the signal handler.
	 *
	 * Also mark the interrupt as pending if interrupts are not disabled,
	 * but the wouldlosewakeup flag is set.  This is the case before
	 * we go in select/poll.
	 */
	if (intsDisabled() || wouldlosewakeup) {
		char c;
		pendingSig[sig] = 1;
		sigPending = 1;
		
#if defined(KAFFE_XPROFILER)
		/*
		 * Since the regular handler won't run with the sig context we
		 * need to do the hit here
		 */
		if( sig == SIGVTALRM )
		{
			SIGNAL_CONTEXT_POINTER(scp) =
				GET_SIGNAL_CONTEXT_POINTER(sc);
				
			profileHit((char *)SIGNAL_PC(scp));
		}
#endif
		/*
		 * There is a race condition in handleIO() between
		 * zeroing blockints and going into select().
		 * sigPipe+wouldlosewakeup is the hack that avoids
		 * that race condition.  See handleIO().
		 *
		 * If we would lose the wakeup because we're about to go to
		 * sleep in select(), write into the sigPipe to ensure select
		 * returns.
		 */
		/*
		 * Write a byte in the pipe if we get a signal if 
		 * wouldlosewakeup is set.  
		 * Do not write more than one byte, however.
		 */
		if (wouldlosewakeup == 1) {
			write(sigPipe[1], &c, 1);
			bytesInPipe++;
			wouldlosewakeup++;
		}

#if defined(KAFFE_SIGNAL_ONE_SHOT)
		/*
		 * On some systems, signal handlers are a one-shot deal.
		 * Re-install the signal handler for those systems.
		 */
		restoreAsyncSignalHandler(sig, interrupt);
#endif

		/*
		 * Returning from the signal handler should restore
		 * all signal state (if the OS is not broken).
		 */
		return;
	}

	/*
	 * The interrupted code was not in a critical section,
	 * so we enter a critical section now.  Note that we
	 * will *not* be interrupted between the blockInts
	 * check above and the intsDisable() below because
	 * the signal mask delays all asynchronous signals.
	 */

	intsDisable();

#if defined(KAFFE_SIGNAL_ONE_SHOT)
	/* Re-enable signal if necessary */
        restoreAsyncSignalHandler(sig, interrupt);
#endif

	/*
	 * Restore the signal state.  This means unblock all
	 * asynchronous signals.  We can now context switch to another
	 * thread as the signal state for the Kaffe process is clear
	 * in the eyes of the OS.  Any asynchronous signals that come
	 * in because we just unblocked them will discover that
	 * blockInts > 0, and flag their arrival in the pendingSig[]
	 * array.
	 *
	 * We clear the signal's pending indicator before reallowing signals.
	 */
	pendingSig[sig] = 0;
	unblockAsyncSignals();

	/*
	 * Handle the signal.
	 */
	handleInterrupt(sig, (void*)GET_SIGNAL_CONTEXT_POINTER(sc));

	/*
	 * Leave the critical section.  This may or may not cause a
	 * reschedule.  (Depends on the side-effects of
	 * handleInterrupt()).
	 */
	intsRestore();
}

/*
 * handle a SIGVTALRM alarm.
 *
 * If preemption is disabled, we have the current thread so that it is
 * scheduled in a round-robin fashion with its peers who have the same
 * priority.
 */
static void 
handleVtAlarm(int sig UNUSED, SIGNAL_CONTEXT_POINTER(sc UNUSED))
{
	static int c;

#if defined(KAFFE_XPROFILER)
	if( sc )
		profileHit((char *)SIGNAL_PC(sc));
#endif
	if (preemptive) {
		internalYield();
	}

	/*
	 * This is kind of ugly: some fds won't send us SIGIO.
	 * Example: the pseudo-tty driver in FreeBSD won't send a signal
	 * if we blocked on a write because the output buffer was full, and
	 * the output buffer became empty again.
	 *
	 * So we check periodically, every 0.2 seconds virtual time.
	 */
	if (++c % 20 == 0) {
		handleIO(false);
	}
}

/*
 * handle a SIGALRM alarm.
 */
static void 
alarmException(void)
{
	jthread* jtid;
	jlong curTime;

	/* Wake all the threads which need waking */
	curTime = currentTime();
	while (alarmList != 0 && JTHREADQ(alarmList)->time <= curTime) {
	        KaffeNodeQueue* node = alarmList;
		/* Restart thread - this will tidy up the alarm and blocked
		 * queues.
		 */
		jtid = JTHREADQ(node);
		alarmList = node->next;
		KaffePoolReleaseNode(queuePool, node);
		
		resumeThread(jtid);
	}

	/* Restart alarm */
	if (alarmList != 0) {
		MALARM(JTHREADQ(alarmList)->time - curTime);
	}
}

/*
 * print thread flags in pretty form.
 */
static char*
printflags(unsigned i)
{
	static char b[256];	/* plenty */
	struct {
		int flagvalue;
		const char *flagname;
	} flags[] = {
	    { THREAD_FLAGS_GENERAL, "GENERAL" },
	    { THREAD_FLAGS_NOSTACKALLOC, "NOSTACKALLOC" },
	    { THREAD_FLAGS_KILLED, "KILLED" },
	    { THREAD_FLAGS_ALARM, "ALARM" },
	    { THREAD_FLAGS_EXITING, "EXITING" },
	    { THREAD_FLAGS_DONTSTOP, "DONTSTOP" },
	    { THREAD_FLAGS_DYING, "DYING" },
	    { THREAD_FLAGS_BLOCKEDEXTERNAL, "BLOCKEDEXTERNAL" },
	    { THREAD_FLAGS_INTERRUPTED, "INTERRUPTED" },
	    { 0, NULL }
	}, *f = flags;

	b[0] = '\0';
	while (f->flagname) {
		if (i & f->flagvalue) {
			strcat(b, f->flagname);
			strcat(b, " ");
		}
		f++;
	}
	return b;
}

/* 
 * dump information about a thread to stderr
 */
void
jthread_dumpthreadinfo(jthread_t tid)
{
	dprintf("tid %p, status %s flags %s\n", tid, 
		tid->status == THREAD_SUSPENDED ? "SUSPENDED" :
		tid->status == THREAD_RUNNING ? "RUNNING" :
		tid->status == THREAD_DEAD ? "DEAD" : "UNKNOWN!!!", 
		printflags(tid->flags));
	if (tid->blockqueue != NULL) {
		int i;

		dprintf(" blocked");
		if (isOnList(waitForList, tid)) {
			dprintf(": waiting for children");
		}
#if 0
		/* XXX FIXME: alarmList uses nextalarm, but isOnList iterates
		 * using nextQ
		 */
		if (isOnList(alarmList, tid)) {
			dprintf(": sleeping");
		}
#endif
		for (i = 0; i < FD_SETSIZE; i++) {
			if (isOnList(readQ[i], tid)) {
				dprintf(": reading from fd %d ", i);
				break;
			}
			if (isOnList(writeQ[i], tid)) {
				dprintf(": writing to fd %d ", i);
				break;
			}
		}

#if 0
		dprintf("@%p (%p->", tid->blockqueue,
					     t = *tid->blockqueue);
		while (t && t->nextQ) {
			t = t->nextQ; 
			dprintf("%p->", t);
		}
		dprintf("|) ");
#endif
	}
}

/*
 * print info about a java thread (hopefully we'll get this to include more
 * detail, such as the actual stack traces for each thread)
 *
 */
static void
dumpJavaThreadLocal(jthread_t thread, UNUSED void *p)
{
	Hjava_lang_VMThread *tid = (Hjava_lang_VMThread *)jthread_get_data(thread)->jlThread;
	dprintf("`%s' ", nameThread(tid));
	jthread_dumpthreadinfo(thread);
	dprintf("\n");
}

static void
dumpThreadsLocal(void)
{
	dprintf("Dumping live threads:\n");
	jthread_walkLiveThreads(dumpJavaThreadLocal, NULL);
}


/*
 * handle an interrupt.
 * 
 * this function is either invoked from within a signal handler, or as the
 * result of intsRestore.
 */
static void 
handleInterrupt(int sig, SIGNAL_CONTEXT_POINTER(sc))
{
	switch(sig) {
	case SIGALRM:
		alarmException();
		break;

	case SIGUSR1:
		ondeadlock();
		break;

	case SIGUSR2:
		dumpThreadsLocal();
		break;

#if defined(SIGVTALRM)
	case SIGVTALRM:
		handleVtAlarm(sig, sc);
		break;
#endif

	case SIGCHLD:
		childDeath();
		break;

	case SIGIO:
		handleIO(false);
		break;

	default:
		dprintf("unknown signal %d\n", sig);
		exit(-1);
	}
}

/*============================================================================
 *
 * Functions related to run queue manipulation
 */


/*
 * Resume a thread running.
 * This routine has to be called only from locations which ensure
 * run / block queue consistency. There is no check for illegal resume
 * conditions (like explicitly resuming an IO blocked thread). 
 */
static void
resumeThread(jthread* jtid)
{
	KaffeNodeQueue** ntid;

DBG(JTHREAD,	dprintf("resumeThread %p\n", jtid);	);
	intsDisable();

	if (jtid->status != THREAD_RUNNING) {

		CLEAR_BLOCKED_ON_EXTERNAL(jtid);

		/* Remove from alarmQ if necessary */
		if ((jtid->flags & THREAD_FLAGS_ALARM) != 0) {
			removeFromAlarmQ(jtid);
		}
		/* Remove from lockQ if necessary */
		if (jtid->blockqueue != 0) {
		  KaffeNodeQueue **queue;

		  for (queue= &jtid->blockqueue;
		       *queue != 0;
		       queue = &(*queue)->next)
		    {
		      for (ntid = (KaffeNodeQueue **)((*queue)->element); 
			   *ntid != 0;
			   ntid = &(*ntid)->next) 
			{
			  if (JTHREADQ(*ntid) == jtid) {
			    KaffeNodeQueue *node = *ntid;
			    
			    *ntid = node->next;
			    KaffePoolReleaseNode(queuePool, node);
			    break;
			  }
			}
		    }
		    KaffePoolReleaseList(queuePool, jtid->blockqueue);
		    jtid->blockqueue = NULL;
		}

		jtid->status = THREAD_RUNNING;

		/* Place thread on the end of its queue */
		if (jtid->suspender != NULL) {
			/* Need to wait for the suspender to resume them. */
		}
		else if (threadQhead[jtid->priority] == 0) {
			threadQhead[jtid->priority] = KaffePoolNewNode(queuePool);
			threadQhead[jtid->priority]->element = jtid;
			threadQtail[jtid->priority] = threadQhead[jtid->priority];
			if (jtid->priority > currentJThread->priority) {
				needReschedule = true;
			}
		}
		else {
		        KaffeNodeQueue *queue = KaffePoolNewNode(queuePool);
			
			queue->element = jtid;
			threadQtail[jtid->priority]->next = queue;
			threadQtail[jtid->priority] = queue;
		}
	} else {
DBG(JTHREAD,		dprintf("Re-resuming %p\n", jtid); );
	}
	intsRestore();
}

/*
 * Add a new waiting queue for this thread.
 * Assert: ints must be disabled to avoid reschedule
 * while we set up the waiting queues.
 */
static void
addWaitQThread(jthread *jtid, KaffeNodeQueue **queue)
{
	KaffeNodeQueue *node;

	assert(intsDisabled()); 
	assert(queue != NULL);
	assert(jtid != NULL);
	
	/* Insert onto head of lock wait Q */
	node = KaffePoolNewNode(queuePool);
	node->next = *queue;
	node->element = jtid;
	*queue = node;
	
	/* Add the new queue to the list of registered blocking queues */
	node = KaffePoolNewNode(queuePool);
	node->next = jtid->blockqueue;
	node->element = queue;
	
	jtid->blockqueue = node;
}

static void
cleanupWaitQ(jthread *jtid)
{
	KaffeNodeQueue **ntid;
	
	if (jtid->blockqueue != 0) {
		KaffeNodeQueue **queue;
		
		for (queue= &jtid->blockqueue;
		     *queue != 0;
		     queue = &(*queue)->next)
			{
				KaffeNodeQueue **one_wait_queue = (KaffeNodeQueue **)((*queue)->element);
				
				for (ntid = one_wait_queue; 
				     *ntid != 0;
				     ntid = &(*ntid)->next) 
					{
						KaffeNodeQueue *node = *ntid;
						
						if (JTHREADQ(node) == jtid) {
							*ntid = node->next;
							KaffePoolReleaseNode(queuePool, node);
							break;
						}
					}
			}
		KaffePoolReleaseList(queuePool, jtid->blockqueue);
		jtid->blockqueue = NULL;
	}
}

/*
 * Suspend a thread on a queue.
 * Return true if thread was interrupted.
 */
static int
suspendOnQThread(jthread* jtid, KaffeNodeQueue** queue, long timeout)
{
	int rc = false;
	KaffeNodeQueue** ntid;
	KaffeNodeQueue* last;

DBG(JTHREAD,	dprintf("suspendOnQThread %p %p (%ld) bI %d\n",
	jtid, queue, timeout, blockInts); );

	assert(timeout >= 0 || timeout == NOTIMEOUT);
	assert(intsDisabled()); 

	if (timeout == 0)
		return false;

	if (jtid->status != THREAD_SUSPENDED) {
		jtid->status = THREAD_SUSPENDED;

#ifdef ENABLE_JVMPI
		FIRSTFRAME(currentJThread->localData.topFrame, 0);
#endif

		last = 0;
		for (ntid = &threadQhead[jtid->priority]; 
			*ntid != 0; 
			ntid = &(*ntid)->next) 
		{
			if (JTHREADQ(*ntid) == jtid) {
			        KaffeNodeQueue *node = (*ntid);
				
				/* Remove thread from runq */
				*ntid = node->next;
				KaffePoolReleaseNode(queuePool, node);
				if (*ntid == 0) {
					threadQtail[jtid->priority] = last;
				}

				/* Insert onto head of lock wait Q */
				if (queue != 0) {
				  addWaitQThread(jtid, queue);
				}

				/* If I have a timeout, insert into alarmq */
				if (timeout != NOTIMEOUT) {
				  addToAlarmQ(jtid, (jlong)timeout);
				}

				/* If I was running, reschedule */
				if (jtid == currentJThread) {
					reschedule();
					if (jtid->flags & THREAD_FLAGS_INTERRUPTED) {
						rc = true;
					}
				}
				break;
			}
			last = *ntid;
		}
	} else {
	DBG(JTHREAD,
		dprintf("Re-suspending %p on %p\n", jtid, *queue); );
	}
	return (rc);
}

/*
 * Kill thread.
 */
static void 
killThread(jthread *tid)
{
	KaffeNodeQueue **ntid;
	KaffeNodeQueue* last;

	intsDisable();

	/* allow thread to perform any action before being killed -
	 * such as notifying on the object 
	 */
	if (destructor1)
		(*destructor1)(tid->localData.jlThread);

DBG(JTHREAD,	
	dprintf("killThread %p kills %p\n", currentJThread, tid); );

	if (tid->status != THREAD_DEAD) {

		/* Get thread off runq (if it needs it) */
		if (tid->status == THREAD_RUNNING) {
			int pri = tid->priority;
			last = 0;

			for (ntid = &threadQhead[pri]; 
				*ntid != 0; 
				ntid = &(*ntid)->next) 
			{
				if (JTHREADQ(*ntid) == tid) {
				        KaffeNodeQueue *node = (*ntid);
					
					*ntid = node->next;
					KaffePoolReleaseNode(queuePool, node);
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

		/* Now that we are off the runQ, it is safe to leave
		 * the list of live threads and be GCed.
		 */
		/* Remove thread from live list so it can be garbaged */
		for (ntid = &liveThreads; *ntid != 0; ntid =
			     &(*ntid)->next) { 
			if (tid == JTHREADQ(*ntid)) {
			  KaffeNodeQueue *node = (*ntid);
			  
			  (*ntid) = node->next;
			  KaffePoolReleaseNode(queuePool, node);
			  break;
			}
		}

		cleanupWaitQ(tid);

		/* Dead Jim - let the GC pick up the remains */
		tid->status = THREAD_DEAD;
	}

	intsRestore();
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
newThreadCtx(size_t stackSize)
{
	jthread *ct;

	ct = KGC_malloc(threadCollector, sizeof(jthread) + 16 + stackSize, KGC_ALLOC_THREADCTX);
	if (ct == 0) {
		return 0;
	}

	KGC_addRef(threadCollector, ct);
#if defined(__ia64__)
	/* (gb) Align jmp_buf on 16-byte boundaries */
	ct = (jthread *)((((unsigned long)(ct)) & 15) ^ (unsigned long)(ct));
#endif
	ct->stackBase = (ct + 1);
	ct->stackEnd = (char *) ct->stackBase + stackSize;
	ct->restorePoint = ct->stackEnd;
	ct->status = THREAD_SUSPENDED;

DBG(JTHREAD,
	dprintf("allocating new thread, stack base %p-%p\n", 
	    ct->stackBase, ct->stackEnd); );

	return (ct);
}

/*
 * free a thread context and its stack
 */
void    
jthread_destroy(jthread *jtid)
{
	KaffeNodeQueue *x;

	if (DBGEXPR(JTHREAD, true, false)) {
		for (x = liveThreads; x; x = x->next)
			assert(JTHREADQ(x) != jtid);
	}
	/* We do not free explicitly as it should be done by the GC. */
	KGC_rmRef(threadCollector, jtid);
}

/*
 * iterate over all live threads
 */
void
jthread_walkLiveThreads(void (*func)(jthread_t,void*), void *priv)
{
        KaffeNodeQueue* liveQ;

        for (liveQ = liveThreads; liveQ != NULL; liveQ = liveQ->next) {
                func(JTHREADQ(liveQ), priv);
        }
}

void
jthread_walkLiveThreads_r(void (*func)(jthread_t,void*), void *priv)
{
	jthread_walkLiveThreads(func, priv);
}

/*
 * determine the interesting stack range for a conservative gc
 */
int
jthread_extract_stack(jthread *jtid, void **from, unsigned *len)
{
	assert(jtid != NULL);
#if defined(STACK_GROWS_UP)
	*from = jtid->stackBase;
	*len = jtid->restorePoint - jtid->stackBase;
#else   
	*from = jtid->restorePoint;
	*len = (char *) jtid->stackEnd - (char *) jtid->restorePoint;
#endif
	return (1);
}

/* 
 * XXX this is supposed to count the number of stack frames 
 */
int
jthread_frames(jthread *thrd UNUSED)
{
        return (0);
}

void jthread_suspend(jthread_t jt, void *suspender)
{
	assert(jt != jthread_current());
	
	intsDisable();
	if( jt->suspender == suspender )
	{
		jt->suspendCount += 1;
	}
	else
	{
		assert(jt->suspender == NULL);
		
		jt->suspender = suspender;
		if( jt->status != THREAD_SUSPENDED )
		{
			suspendOnQThread(jt, NULL, NOTIMEOUT);
			jt->status = THREAD_RUNNING;
			jt->suspendCount = 1;
		}
	}
	intsRestore();
}

void jthread_resume(jthread_t jt, void *suspender)
{
	if( jt != currentJThread )
	{
		intsDisable();
		if( jt->suspender == suspender )
		{
			assert(jt->suspendCount > 0);
			
			jt->suspendCount -= 1;
			if( jt->suspendCount == 0 )
			{
				if( jt->status == THREAD_RUNNING )
				{
					jt->status = THREAD_SUSPENDED;
				}
				resumeThread(jt);
				jt->suspender = NULL;
			}
		}
		intsRestore();
	}
}

jthread_t jthread_from_data(threadData *td, void *suspender)
{
	jthread_t retval = NULL;

	intsDisable();
	{
		KaffeNodeQueue *curr;
		jthread_t tid;
		
		for( curr = liveThreads;
		     (curr != NULL) && (retval == NULL);
		     curr = curr->next)
		{
		        tid = JTHREADQ(curr);
			if( &tid->localData == td )
			{
				if( tid != currentJThread )
				{
					jthread_suspend(tid, suspender);
				}
				retval = tid;
			}
		}
	}
	intsRestore();
	
	return( retval );
}

/*============================================================================
 *
 * Functions for initialization and thread creation
 *
 */

#if defined(HAVE_SETITIMER) && defined(ITIMER_VIRTUAL)
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
 * deactivate virtual timer
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

/**
 * Thread allocation function alias.
 */
static void *thread_static_allocator(size_t bytes)
{
	return KGC_malloc(threadCollector, bytes, KGC_ALLOC_STATIC_THREADDATA);
}

static void thread_static_free(void *p)
{
	return KGC_free(threadCollector, p);
}

static void *thread_reallocator(void *p, size_t bytes)
{
	return KGC_realloc(threadCollector, p, bytes, KGC_ALLOC_STATIC_THREADDATA);
}

/*
 * Initialize the threading system. 
 */
void
jthread_init(int pre,
	int maxpr, int minpr,
	Collector *collector,
	void (*_destructor1)(void*),
	void (*_onstop)(void),
	void (*_ondeadlock)(void))
{
        jthread *jtid; 
	int i;

	threadCollector = collector;
	blockInts = 0;

	/* XXX this is f***ed.  On BSD, we get a SIGHUP if we try to put
	 * a process that has a pseudo-tty in async mode in the background
	 * So we'll just ignore it and keep running.  Note that this will
	 * detach us from the session too.
	 */
	KaffeJThread_ignoreSignal(SIGHUP);

	KaffeSetDefaultAllocator(thread_static_allocator, thread_static_free, thread_reallocator);
	queuePool = KaffeCreatePool();

#if defined(SIGVTALRM)
	registerAsyncSignalHandler(SIGVTALRM, interrupt);
#endif
	registerAsyncSignalHandler(SIGALRM, interrupt);
	registerAsyncSignalHandler(SIGIO, interrupt);
        registerAsyncSignalHandler(SIGCHLD, interrupt);
        registerAsyncSignalHandler(SIGUSR1, interrupt);
        registerAsyncSignalHandler(SIGUSR2, interrupt);

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
#ifndef _HURD_ASYNC_QUICKFIX_
	if (DBGEXPR(ANY, DBGEXPR(ASYNCSTDIO, true, false), true)) {
		for (i = 0; i < 3; i++) {
			if (i != jthreadedFileDescriptor(i)) {
				return;
			}
		}
	}
#endif

	/*
	 * On some systems, it is essential that we put the fds back
	 * in their non-blocking state
	 */
	atexit(restore_fds);
	registerTerminalSignal(SIGINT, restore_fds_and_exit);
	registerTerminalSignal(SIGTERM, restore_fds_and_exit);

	preemptive = pre;
	max_priority = maxpr;
	min_priority = minpr;
	onstop = _onstop;
	ondeadlock = _ondeadlock;
	destructor1 = _destructor1;
	threadQhead = (KaffeNodeQueue **)thread_static_allocator((maxpr + 1) * sizeof (KaffeNodeQueue *));
	threadQtail = (KaffeNodeQueue **)thread_static_allocator((maxpr + 1) * sizeof (KaffeNodeQueue *));
	for (i=0;i<FD_SETSIZE;i++) {
	  readQ[i] = writeQ[i] = NULL;
	  blockingFD[i] = true;
	}
	alarmList = NULL;
	waitForList = NULL;

	for (i=0;i<=maxpr;i++)
	  threadQhead[i] = threadQtail[i] = NULL;

	/* create the helper pipe for lost wakeup problem */
	if (pipe(sigPipe) != 0) {
		return;
	}
	if (maxFd == -1) {
		maxFd = sigPipe[0] > sigPipe[1] ? sigPipe[0] : sigPipe[1];
	}

	jtid = newThreadCtx(0);
	if (!jtid) {
		return;
	}

	/* Fake up some stack bounds until we get the real ones later - we're
	 * the only thread running so this isn't a problem.
	 */
	jtid->stackBase = 0;
	jtid->stackEnd = (char*)0 - 1;
#if defined(STACK_GROWS_UP)
        jtid->restorePoint = jtid->stackEnd;
#else
        jtid->restorePoint = jtid->stackBase;
#endif
        jtid->priority = maxpr;
        jtid->status = THREAD_SUSPENDED;
        jtid->flags = THREAD_FLAGS_NOSTACKALLOC;
        jtid->func = (void (*)(void*))jthread_init;
        
	liveThreads = KaffePoolNewNode(queuePool);
	liveThreads->element = jtid;
        jtid->time = 0;

        talive++;
        currentJThread = jtid;
        resumeThread(jtid);
#if defined(KAFFE_XPROFILER)
	/*
	 * The profiler is started at program startup, we take over from here
	 * on out so we disable whatever one was installed
	 */
	disableProfileTimer();
#endif
	/* Because of the handleVtAlarm hack (poll every 20 SIGVTALRMs) 
	 * we turn on the delivery of SIGVTALRM even if no actual time
	 * slicing is possible because only one Java thread is running.
	 * XXX We should be smarter about that.
	 */
	activate_time_slicing();

	jmutex_initialise(&GClock);
}

jthread_t
jthread_createfirst(size_t mainThreadStackSize, unsigned int prio, void* jlThread)
{
        jthread *jtid; 

	jtid = currentJThread;

	/*
	 * Note: the stackBase and stackEnd values are used for two purposes:
	 * - to report to the gc what area to scan (extract_stack)
	 * - to help in determining whether the next frame in the link chain
	 *   of frames is valid.  This is done by checking its range.
	 */
	detectStackBoundaries(jtid, mainThreadStackSize);

	jtid->localData.jlThread = jlThread;

	jthread_setpriority(jtid, prio);

	firstThread = jtid;

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
	if (currentJThread) {
	  /* DBG(JTHREAD,	dprintf("disable stop for thread  %p\n", currentJThread);	) */
		intsDisable();
		currentJThread->flags |= THREAD_FLAGS_DONTSTOP;
		currentJThread->stopCounter++;
		assert(currentJThread->stopCounter > 0);

		/* XXX Shouldn't recurse that much... ever... hopefully. */
		assert(currentJThread->stopCounter < 50);  

		intsRestore();
	}
}

/*
 * reallow cancellation and stop if cancellation pending
 */
void 
jthread_enable_stop(void)
{
	if (currentJThread) {
	  /* DBG(JTHREAD,	dprintf("enable stop for thread  %p\n", currentJThread);	) */
		intsDisable();
		if (--currentJThread->stopCounter == 0) {
			currentJThread->flags &= ~THREAD_FLAGS_DONTSTOP;
			if ((currentJThread->flags & THREAD_FLAGS_KILLED) != 0
			   && ((currentJThread->flags & THREAD_FLAGS_EXITING) 
			   	== 0))
			{
				die();
			}
		}
		assert(currentJThread->stopCounter >= 0);
		intsRestore();
	}
}

/*
 * interrupt a thread
 */
void
jthread_interrupt(jthread *jtid)
{
	intsDisable();

	/* mark thread as interrupted */
	jtid->flags |= THREAD_FLAGS_INTERRUPTED;

	/* make sure we only resume suspended threads 
	 * (and neither dead nor runnable threads) that
	 * are not trying to acquire a mutex.
	 */
	if ((jtid->status == THREAD_SUSPENDED) && !jthread_on_mutex(jtid)) {
		resumeThread(jtid);
	}
	intsRestore();
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

static void NONRETURNING
start_this_sucker_on_a_new_frame(void)
{
	/* all threads start with interrupts turned off */
	blockInts = 1;

	/* I might be dying already */
	if ((currentJThread->flags & THREAD_FLAGS_KILLED) != 0) {
		die();
	}

	intsRestore();
	assert(currentJThread->stopCounter == 0);
	currentJThread->func(currentJThread->localData.jlThread);
	jthread_exit(); 
}


/*
 * create a new jthread
 */
jthread *
jthread_create(unsigned int pri, void (*func)(void *), int isDaemon,
        void *jlThread, size_t threadStackSize)
{
        KaffeNodeQueue *liveQ;
	jthread *jtid; 
	void	*oldstack, *newstack;
#if defined(__ia64__)
	void	*oldbsp, *newbsp;
#endif
	size_t   page_size;

	/*
	 * Disable stop to protect the threadLock lock, and prevent
	 * the system from losing a new thread context (before the new
	 * thread is queued up).
	 */
	jthread_disable_stop();

	/* Adjust stack size */
	page_size = getpagesize();
	if (threadStackSize == 0)
		threadStackSize = THREADSTACKSIZE;
	threadStackSize = (threadStackSize + page_size - 1) & (uintp)-page_size;

	jmutex_lock(&threadLock);
	jtid = newThreadCtx(threadStackSize);
	if (!jtid) {
		jmutex_unlock(&threadLock);
		jthread_enable_stop();
		return 0;
	}
        jtid->priority = pri;
        jtid->localData.jlThread = jlThread;
        jtid->status = THREAD_SUSPENDED;
        jtid->flags = THREAD_FLAGS_GENERAL;
	jtid->blockqueue = NULL;

	liveQ = KaffePoolNewNode(queuePool);
	liveQ->next = liveThreads;
	liveQ->element = jtid;
	liveThreads = liveQ;

        talive++;       
        if ((jtid->daemon = isDaemon) != 0) {
                tdaemon++;
        }
DBG(JTHREAD,
	dprintf("creating thread %p, daemon=%d\n", jtid, isDaemon); );
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
        if (JTHREAD_CONTEXT_SAVE(jtid->env)) {
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
#if defined(__ia64__)
	oldbsp = GET_BSP(jtid->env);
#endif

#if defined(STACK_GROWS_UP)
	newstack = jtid->stackBase+STACK_COPY;
	memcpy(newstack-STACK_COPY, oldstack-STACK_COPY, STACK_COPY);
#else /* !STACK_GROWS_UP */
	newstack = jtid->stackEnd;
#if defined(__ia64__)
	/*
	 * The stack segment is split in the middle. The upper half is used
	 * as backing store for the register stack which grows upward.
	 * The lower half is used for the traditional memory stack which
	 * grows downward. Both stacks start in the middle and grow outward
	 * from each other.
	 */
	newstack = (void *)((uintp)newstack - (threadStackSize >> 1));
	newbsp = newstack;
	/* Make register stack 64-byte aligned */
	if ((unsigned long)newbsp & 0x3f)
		newbsp = newbsp + (0x40 - ((unsigned long)newbsp & 0x3f));
	newbsp += STACK_COPY;
	memcpy(newbsp-STACK_COPY, oldbsp-STACK_COPY, STACK_COPY);
#endif
	newstack = (void *)((uintp)newstack - STACK_COPY);
	memcpy(newstack, oldstack, STACK_COPY);
#endif /* !STACK_GROWS_UP */

#if defined(NEED_STACK_ALIGN)
	newstack = (void *) STACK_ALIGN(newstack);
#endif

	SET_SP(jtid->env, newstack);
#if defined(__ia64__)
	SET_BSP(jtid->env, newbsp);
#endif

#if defined(SET_BP)
	/*
	 * Clear the base pointer in the new thread's stack.
	 * Nice for debugging, but not strictly necessary.
	 */
	SET_BP(jtid->env, 0);
#endif


#if defined(FP_OFFSET)
	/* needed for: IRIX */
	SET_FP(jtid->env, newstack + ((void *)GET_FP(jtid->env) - oldstack));
#endif

        resumeThread(jtid);
	jthread_enable_stop();

        return jtid;
}

/*============================================================================
 *
 * Functions that are part of the user interface
 *
 */

/*
 * return the current thread
 */
jthread_t
jthread_current(void)
{
       return currentJThread;
}

/*
 * determine whether a location is on the stack of the current thread
 */
int
jthread_on_current_stack(void *bp)
{
        int rc = bp >= currentJThread->stackBase && bp < currentJThread->stackEnd;

DBG(JTHREADDETAIL,
       dprintf("on current stack: base=%p size=%ld bp=%p %s\n",
               currentJThread->stackBase,
               (long)((char *) currentJThread->stackEnd - (char *) currentJThread->stackBase),
               bp,
               (rc ? "yes" : "no"));
    );

        return rc;
}

/*
 * Check for room on stack.
 */
int
jthread_stackcheck(int left)
{
       int rc;
#if defined(STACK_GROWS_UP)
        rc = jthread_on_current_stack((char*)&rc + left);
#else
        rc = jthread_on_current_stack((char*)&rc - left);
#endif
       return (rc);
}

/*
 * Get the current stack limit.
 */

#define        REDZONE 1024

void
jthread_relaxstack(int yes)
{
       if( yes )
       {
#if defined(STACK_GROWS_UP)
               uintp end = (uintp) currentJThread->stackEnd;
               end += REDZONE;
               currentJThread->stackEnd = (void *) end;
#else
               uintp base = (uintp) currentJThread->stackBase;
               base -= REDZONE;
               currentJThread->stackBase = (void *) base;
#endif
       }
       else
       {
#if defined(STACK_GROWS_UP)
               uintp end = (uintp) currentJThread->stackEnd;
               end -= REDZONE;
               currentJThread->stackEnd = (void *) end;
#else
               uintp base = (uintp) currentJThread->stackBase;
               base += REDZONE;
               currentJThread->stackBase = (void *) base;
#endif
       }
}

void*
jthread_stacklimit(void)
{
#if defined(STACK_GROWS_UP)
        return (void*)((uintp)currentJThread->stackEnd - REDZONE);
#else
        return (void*)((uintp)currentJThread->stackBase + REDZONE);
#endif
}

/* Spinlocks: simple since we're uniprocessor */
/* ARGSUSED */
void
jthread_spinon(void *arg UNUSED)
{
       jthread_suspendall();
}

/* ARGSUSED */
void
jthread_spinoff(void *arg UNUSED)
{
       jthread_unsuspendall();
}

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
jthread_sleep(jlong millis)
{
        if (millis == 0) {
                return; 
        }
        intsDisable();
	BLOCKED_ON_EXTERNAL(currentJThread);
        suspendOnQThread(currentJThread, 0, (long)millis);
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
        if (jtid == 0
#if 0
	    /* this code makes kaffe behave like sun, but it means
	     * that Thread.join() after Thread.stop() is useless.
	     */
	    || (jtid->flags & (THREAD_FLAGS_KILLED | THREAD_FLAGS_DYING)) != 0 
#else
	    /* There seems to be a window in which death can be
	     * broadcast before it is waited for.  Basically,
	     * jthread_alive will be false immediately after
	     * Thread.stop(), unless stopping the thread was
	     * disabled.  Thread.alive() will become false as soon as
	     * the thread is on its way out.
	     */
	    || (jtid->flags & (THREAD_FLAGS_DYING | THREAD_FLAGS_EXITING))
#endif
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
	KaffeNodeQueue** ntid;
	KaffeNodeQueue* last;
	KaffeNodeQueue* node;

	if (jtid->status == THREAD_SUSPENDED) {
		jtid->priority = (unsigned char)prio;
		return;
	}

	intsDisable();

	/* Remove from current thread list */
	last = NULL;
	node = NULL;
	for (ntid = &threadQhead[jtid->priority]; 
		*ntid != 0; 
		ntid = &(*ntid)->next) 
	{
		if (JTHREADQ(*ntid) == jtid) {
			node = *ntid;
			*ntid = node->next;
			if (*ntid == 0) {
				threadQtail[jtid->priority] = last;
			}
			break;
		}
		last = *ntid;
	}

	assert(node != NULL);

	/* Insert onto a new one */
	jtid->priority = (unsigned char)prio;
	if (threadQhead[prio] == 0) {
		threadQhead[prio] = node;
		threadQtail[prio] = node;
	}
	else {
		threadQtail[prio]->next = node;
		threadQtail[prio] = node;
	}
	node->next = NULL;

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
	intsDisable();
	/* No reason to hit a dead man over the head */
	if (jtid->status != THREAD_DEAD) {
	  jtid->flags |= THREAD_FLAGS_KILLED;
	}

	/* if it's us, die */
	if (jtid == jthread_current()
	    && (jtid->flags & THREAD_FLAGS_DONTSTOP) == 0 && blockInts == 1)
	  die();

	/* We only have to resume the thread if it is not us. */
	if (jtid != jthread_current())
          resumeThread(jtid);

	intsRestore();
}

/*
 * Have a thread exit.  This function does not return.
 */
void
jthread_exit(void)
{
	jthread* tid;
	KaffeNodeQueue *liveQ;

DBG(JTHREAD,
	dprintf("jthread_exit %p\n", currentJThread);		);

	jthread_disable_stop();
	jmutex_lock(&threadLock);

	talive--;
	if (currentJThread->daemon)
	  {
	    tdaemon--;
	  }

	KaffeVM_unlinkNativeAndJavaThread();

	assert(!(currentJThread->flags & THREAD_FLAGS_EXITING));
	currentJThread->flags |= THREAD_FLAGS_EXITING;

	jmutex_unlock(&threadLock);
	jthread_enable_stop();

	/* If we only have daemons left, then we should exit. */
	if (talive == tdaemon) {
	  
	  DBG(JTHREAD,
	      dprintf("all done, closing shop\n");	);
	  
	  if (runOnExit != 0) {
	    runOnExit();
	  }
	  /* we disable interrupts while we go out to prevent a reschedule
	   * in killThread()
	   */
	  intsDisable();
	  
	  for (liveQ = liveThreads; liveQ != 0; liveQ = liveQ->next) {
	    tid = JTHREADQ(liveQ);
	    /* The current thread is still on the live
	     * list, and we don't want to recursively
	     * suicide.
	     */			
	    if (!(tid->flags & THREAD_FLAGS_EXITING) && tid != firstThread)
	      killThread(tid);
	  }

	  if (currentJThread == firstThread)
	  {
	      DBG(JTHREAD,  dprintf("jthread_exit(%p): we're the main thread, returning.\n", currentJThread); );
	      return;
	  }

	  /* Wake up the first thread */
	  DBG(JTHREAD, dprintf("jthread_exit(%p): waking up main thread.\n", currentJThread));
	  firstThread->suspender = NULL;
	  resumeThread(firstThread);

	} else if (currentJThread == firstThread) {
	  /* The main thread is not exiting. Remove the flag to prevent
	   * reschedule() from killing us. */
	  intsDisable();
	  currentJThread->flags &= ~THREAD_FLAGS_EXITING;
	  currentJThread->suspender = NULL;
	  suspendOnQThread(currentJThread, NULL, NOTIMEOUT);
	  assert(talive == tdaemon);
	  return;
	}

	/* we disable interrupts while we go out to prevent a reschedule
	 * in killThread()
	 */
	intsDisable();

	for (;;) {
		killThread(currentJThread);
		jthread_sleep((jlong) 1000);
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

	/* A reschedule in a non-blocked context is half way to hell */
	assert(intsDisabled());

	b = blockInts;

	for (;;) {
	        for (i = max_priority; i >= min_priority; i--) {
			if (threadQhead[i] == 0) 
			    continue;

			if (JTHREADQ(threadQhead[i]) != currentJThread) {
				lastThread = currentJThread;
				currentJThread = JTHREADQ(threadQhead[i]);

				{
					struct rusage ru;
					jlong ct;

					getrusage(RUSAGE_SELF, &ru);
					ct = ((jlong)ru.ru_utime.tv_sec *
					      1000000)
						+ ((jlong)ru.ru_utime.tv_usec);
					ct += ((jlong)ru.ru_stime.tv_sec *
					       1000000)
						+ ((jlong)ru.ru_stime.tv_usec);

					lastThread->totalUsed +=
						(ct - lastThread->startUsed);
					currentJThread->startUsed = ct;
				}
DBG(JTHREADDETAIL,
dprintf("switch from %p to %p\n", lastThread, currentJThread); );

				/* save and restore floating point state */
#if defined(SAVE_FP)
				SAVE_FP(lastThread->fpstate);
#endif
#if defined(CONTEXT_SWITCH)
				CONTEXT_SWITCH(lastThread, currentJThread);
#else
				if (JTHREAD_CONTEXT_SAVE(lastThread->env) == 0) {
				    lastThread->restorePoint = 
					GET_SP(lastThread->env);
				    JTHREAD_CONTEXT_RESTORE(currentJThread->env, 1);
				}
#endif
#if defined(LOAD_FP)
				LOAD_FP(currentJThread->fpstate);
#endif

				/* Restore ints */
				blockInts = b;

				assert(currentJThread == lastThread);

				/* Now handle external requests for cancelation
				 * We do not act upon them if:
				 * + The thread has the DONTSTOP flags set.
				 * + The threads is already exiting
				 */
				if ((currentJThread->flags & 
					THREAD_FLAGS_KILLED) != 0 && 
				    (currentJThread->flags & 
					THREAD_FLAGS_DONTSTOP) == 0 &&
				    (currentJThread->flags & 
					THREAD_FLAGS_EXITING) == 0 &&
				    blockInts == 1) 
				{
					die();
				}
			}
			/* Now kill the schedule */
			needReschedule = false;
			return;
		}

		/* since we set `wouldlosewakeup' first, we might write into
		 * the pipe but not go to handleIO at all.  That's okay ---
		 * all it means is that we'll return from the next select()
		 * for no reason.  handleIO will eventually drain the pipe.
		 */
		wouldlosewakeup = 1;
		if (sigPending) {
			wouldlosewakeup = 0;
			processSignals();
			continue;
		}

#if defined(DETECTDEADLOCK)
		if (tblocked_on_external == 0) {
			ondeadlock();
		}
#endif
		/* if we thought we should reschedule, but there's no thread
		 * currently runnable, reset needReschedule and wait for another
		 * event that will set it again.
		 */
		needReschedule = false;
		handleIO(true);
	}
}

/*============================================================================
 * 
 * I/O interrupt related functions
 *
 */

#if 0
static void
removeQueueFromBlockQueue(jthread *jtid, KaffeNodeQueue *queue)
{
  KaffeNodeQueue **thisQ;
  
  for (thisQ = &(jtid->blockqueue); *thisQ != 0; thisQ = &(*thisQ)->next) {
    KaffeNodeQueue *node = *thisQ;
    
    if (*((KaffeNodeQueue **)node->element) == queue) {
      *thisQ = node->next;
      KaffePoolReleaseNode(queuePool, node);
      break;
    }
  }
}
#endif

/*
 * resume all threads blocked on a given queue
 */
static void
resumeQueue(KaffeNodeQueue *queue)
{
	KaffeNodeQueue *tid;
	KaffeNodeQueue *ntid;
	
	for (tid = queue; tid != 0; tid = ntid) {
		ntid = tid->next;
		resumeThread(JTHREADQ(tid));
	}
}

/*
 * Process incoming SIGIO
 * return 1 if select was interrupted
 */
static
void
handleIO(int canSleep)
{
	int r;
	/** the wake-up time of the next thread on the alarm queue */
	jlong firstAlarm;
	/** how long do we want to sleep, at most */
	jlong maxWait;
	/* NB: both pollarray and rd, wr are thread-local */
#if USE_POLL
	/* for poll(2) */
	unsigned int nfd, i;
#if DONT_USE_ALLOCA
	struct pollfd pollarray[FD_SETSIZE];	/* huge (use alloca?) */
#else
	struct pollfd *pollarray = alloca(sizeof(struct pollfd) * (maxFd+1));
#endif
#else
	/* for select(2) */
	fd_set rd;
	fd_set wr;
	struct timeval zero = { 0, 0 };
	int i;
#endif
	int b = 0;

	assert(intsDisabled());

DBG(JTHREADDETAIL,
	dprintf("handleIO(sleep=%d)\n", canSleep);		);

#if USE_POLL
	/* Build pollarray from fd_sets.
	 * This is probably not the most efficient way to handle this.
	 */
	for (nfd = 0, i = 0; (int)i <= maxFd; i++) {
		short ev = 0;
		if (readQ[i] != 0) { 	/* FD_ISSET(i, &readsPending) */
			/* Check for POLLIN and POLLHUP for portability.
			 * Some poll(2) implementations return POLLHUP
			 * on EOF.
			 */
			ev |= POLLIN | POLLHUP;
		}
		if (writeQ[i] != 0) {   /* FD_ISSET(i, &writesPending) */
			ev |= POLLOUT;
			assert(FD_ISSET(i, &writesPending));
		}
		if (ev != 0) {
			pollarray[nfd].fd = i;
			pollarray[nfd].events = ev;
			nfd++;
		}
	}
#else
	FD_COPY(&readsPending, &rd);
	FD_COPY(&writesPending, &wr);
#endif

	/*
	 * figure out which fds are ready
	 */
retry:
	if (canSleep) {
		b = blockInts;
		/* NB: BEGIN unprotected region */
		blockInts = 0;
		/* add sigpipe[0] if needed */
#if USE_POLL
		pollarray[nfd].fd = sigPipe[0];
		pollarray[nfd].events = POLLIN;
		nfd++;
#else
		FD_SET(sigPipe[0], &rd);
#endif
	}

        /*
	 * find out if we have any threads waiting (and if so, when the first
	 * one will expire).  we use this to prevent indefinite waits in the
	 * poll / select
	 *
	 */
	firstAlarm = -1;
	if (alarmList != 0) {
		// sorted
		firstAlarm = JTHREADQ(alarmList)->time;
	}

	maxWait = (canSleep ? -1 : 0);
	if ( (firstAlarm != -1) && (canSleep) ) {
		jlong curTime = currentTime();
		if (curTime >= firstAlarm) {
			maxWait = 0;
		} else {
			maxWait = firstAlarm - curTime;
		}
		DBG(JTHREADDETAIL, dprintf("handleIO(sleep=%d) maxWait=%ld\n", canSleep, (long) maxWait); );
	}

#if USE_POLL
	r = poll(pollarray, nfd, maxWait);
#else
	if (maxWait <= 0) {
		r = select(maxFd+1, &rd, &wr, 0, &zero);
	} else {
		struct timeval maxWaitVal = { maxWait/1000, (maxWait % 1000) * 1000 };
		r = select(maxFd+1, &rd, &wr, 0, &maxWaitVal);
	}
#endif
	/* Reset wouldlosewakeup here */
	wouldlosewakeup = 0; 

	if (canSleep) {
		int can_read_from_pipe = 0;
		blockInts = b;
		/* NB: END unprotected region */

#if USE_POLL
		can_read_from_pipe = (pollarray[--nfd].revents & POLLIN);
#else
		can_read_from_pipe = FD_ISSET(sigPipe[0], &rd);
#endif
		/* drain helper pipe if a byte was written */
		if (r > 0 && can_read_from_pipe) {
			char c;

			/* NB: since "rd" is a thread-local variable, it can 
			 * still say that we should read from the pipe when 
			 * in fact another thread has already read from it.
			 * That's why we count how many bytes go in and out.
			 */
			if (bytesInPipe > 0) {
				read(sigPipe[0], &c, 1);
				bytesInPipe--;
			}
		}

		if (sigPending) {
			processSignals();
		}
	}
	if ((r < 0 && errno == EINTR) && !canSleep) 
		goto retry;

	if (r <= 0)
		return;

DBG(JTHREADDETAIL,
	dprintf("Select returns %d\n", r);			);

#if USE_POLL
	for (i = 0; r > 0 && i < nfd; i++) {
		int fd;
		register short rev = pollarray[i].revents;
		if (rev == 0) {
			continue;
		}

		fd = pollarray[i].fd;
		needReschedule = true;
		r--;

		/* If there's an error, we don't know whether to wake
		 * up readers or writers.  So wake up both if so.
		 * Note that things such as failed connect attempts
		 * are reported as errors, not a read or write readiness.
		 */
		/* wake up readers when not just POLLOUT */
		if (rev != POLLOUT && readQ[fd] != 0) {
			resumeQueue(readQ[fd]);
			readQ[fd] = 0;
		}
		/* wake up writers when not just POLLIN */
		if (rev != POLLIN && writeQ[fd] != 0) {
			resumeQueue(writeQ[fd]);
			writeQ[fd] = 0;
		}
	}
#else
	for (i = 0; r > 0 && i <= maxFd; i++) {
		if (readQ[i] != 0 && FD_ISSET(i, &rd)) {
			needReschedule = true;
			resumeQueue(readQ[i]);
			readQ[i] = 0;
			r--;
		}
		if (writeQ[i] != 0 && FD_ISSET(i, &wr)) {
			needReschedule = true;
			resumeQueue(writeQ[i]);
			writeQ[i] = 0;
			r--;
		}
	}
#endif
	return;
}

/*
 * A file I/O operation could not be completed. Sleep until we are woken up
 * by the SIGIO handler.
 *
 * Interrupts are disabled on entry and exit.
 * fd is assumed to be valid.
 * Returns true if operation was interrupted.
 */
static int
blockOnFile(int fd, int op, int timeout)
{
	int rc = false;
DBG(JTHREAD,
	dprintf("blockOnFile(%d,%s)\n", fd, op == TH_READ ? "r":"w"); );

	assert(intsDisabled());
	BLOCKED_ON_EXTERNAL(currentJThread);

	if (fd > maxFd) {
		maxFd = fd;
	}
	if (op == TH_READ) {
		FD_SET(fd, &readsPending);
		rc = suspendOnQThread(currentJThread, &readQ[fd], timeout);
		FD_CLR(fd, &readsPending);
	}
	else {
		FD_SET(fd, &writesPending);
		rc = suspendOnQThread(currentJThread, &writeQ[fd], timeout);
		FD_CLR(fd, &writesPending);
	}
	return (rc);
}

/*============================================================================
 * 
 * locking subsystem
 *
 */

void 
jmutex_initialise(jmutex *lock)
{
	lock->holder = NULL;
	lock->waiting = NULL;
}

void
jmutex_lock(jmutex *lock)
{
DBG(JTHREAD,
	dprintf("jmutex_lock(%p)\n", lock); );
	intsDisable();
	jthread_current()->flags |= THREAD_FLAGS_WAIT_MUTEX;
	while (lock->holder != NULL)
		suspendOnQThread(jthread_current(), &lock->waiting, NOTIMEOUT);
	jthread_current()->flags &= ~THREAD_FLAGS_WAIT_MUTEX;

	lock->holder = jthread_current();			
	intsRestore();
}

void
jmutex_unlock(jmutex *lock)
{
DBG(JTHREAD,
	dprintf("jmutex_unlock(%p)\n", lock); );
	intsDisable();
	lock->holder = NULL;
	if (lock->waiting != 0) {
		jthread* tid;
		KaffeNodeQueue* node = lock->waiting;

		tid = JTHREADQ(node);
		lock->waiting = node->next;
		KaffePoolReleaseNode(queuePool, node);
		assert(tid->status != THREAD_RUNNING);
		resumeThread(tid);
	}
	intsRestore();
}

void
jmutex_destroy(jmutex *lock)
{
	assert(lock->holder == NULL);
	assert(lock->waiting == NULL);
}

/* JThreads does not have special ambiguities concerning the
 * handling of the GC lock.
 */

void
jthread_lockGC(void)
{
  jmutex_lock(&GClock);
}

void
jthread_unlockGC(void)
{
  jmutex_unlock(&GClock);
}

void
jcondvar_initialise(jcondvar *cv)
{
	*cv = NULL;
}

jboolean
jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout)
{
	jthread *current = jthread_current();
	jboolean r;

	intsDisable();

	/* give up mutex */
	lock->holder = NULL;
	if (lock->waiting != NULL) {
		jthread* tid;
		KaffeNodeQueue* node = lock->waiting;

		tid = JTHREADQ(node);
		lock->waiting = node->next;
		KaffePoolReleaseNode(queuePool, node);

		assert(tid->status != THREAD_RUNNING);
		resumeThread(tid);
	}

#if defined(DETECTDEADLOCK)
	/* a limited wait should not cause us to scream deadlock */
	if (timeout != 0) {
		BLOCKED_ON_EXTERNAL(currentJThread);
	}
#endif

	/* wait to be signaled */
	current->flags |= THREAD_FLAGS_WAIT_CONDVAR;
	r = suspendOnQThread(current, cv, (long) timeout);
	current->flags &= ~THREAD_FLAGS_WAIT_CONDVAR;
	/* reacquire mutex */
	current->flags |= THREAD_FLAGS_WAIT_MUTEX;
	while (lock->holder != NULL) {
		suspendOnQThread(current, &lock->waiting, NOTIMEOUT);
	}
	current->flags &= ~THREAD_FLAGS_WAIT_MUTEX;
	lock->holder = current;
	intsRestore();

	return (r);
}

void
jcondvar_signal(jcondvar *cv, jmutex *lock)
{
	intsDisable();
	if (*cv != NULL) {
		KaffeNodeQueue* condQ;
		/* take off condvar queue */
		condQ = *cv;
		*cv = condQ->next;

		/* put on lock queue */
		condQ->next = lock->waiting;
		lock->waiting = condQ;
	}
	intsRestore();
}

void
jcondvar_broadcast(jcondvar *cv, jmutex *lock)
{
	intsDisable();
	if (*cv != NULL) {
		/* splice the lists `*cv' and `lock->waiting' */

		KaffeNodeQueue** condp;
		/* advance to last element in cv list */
		for (condp = cv; *condp != 0; condp = &(*condp)->next)
			;
		(*condp) = lock->waiting;
		lock->waiting = *cv;
		*cv = NULL;
	}
	intsRestore();
}

void
jcondvar_destroy(jcondvar* cv)
{
	assert(*cv == NULL);
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
	int r;
#if (defined(FIOSSAIOSTAT) && !((defined(hpux) || defined (__hpux__)) && defined(FIOASYNC))) || \
    (defined(FIOASYNC) && !defined(linux))
	int on = 1;
#endif
#if (defined(FIOSSAIOOWN) && !((defined(hpux) || defined (__hpux__)) && defined(F_SETOWN))) || \
    defined(F_SETOWN)
	/* cache pid to accommodate antique C libraries */
	static int pid = -1;
	
	if (pid == -1)
		pid = getpid();
#endif
	if (fd == -1)
		return (fd);

#if defined(F_SETFD)
	/* set close-on-exec flag for this file descriptor */
	if ((r = fcntl(fd, F_SETFD, 1)) < 0) {
		perror("F_SETFD");
		return (r);
	}
#endif

	/* Make non-blocking */
	if ((r = fcntl(fd, F_GETFL, 0)) < 0) {
		perror("F_GETFL");
		return (r);
	}

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

#if defined(FIOSSAIOSTAT) && !((defined(hpux) || defined (__hpux__)) && defined(FIOASYNC))
	r = ioctl(fd, FIOSSAIOSTAT, &on);
	if (r < 0 && errno != ENOTTY) {
		/* Defines ENOTTY to be an acceptable error */
		perror("FIOSSAIOSTAT");
 		return (r);
	}
#elif defined(FIOASYNC) && !defined(linux)
	/* Don't do this on Linux because Linux version < 2.2.5 doesn't
	 * know what FIOASYNC means.  It thinks FIOASYNC == O_SYNC. I kid you
	 * not.  You can imagine what that means. ;-)
	 * Never mind, FASYNC work as expected and is already set :)
	 */
	/* 
	 * This ioctl fails for so many systems on so many occasions.
	 * Reasons include ENXIO, ENOTTY, EINVAL(?)
	 */
	r = ioctl(fd, FIOASYNC, &on);
	if (r < 0) {
		DBG(JTHREAD, perror("FIOASYNC"); );
        }
#endif

#if !(defined(O_ASYNC) || defined(FIOASYNC) ||  \
      defined(FASYNC) || defined(FIOSSAIOSTAT))
#error	Could not put socket in async mode
#endif


	/* Allow socket to signal this process when new data is available */
#if defined(FIOSSAIOOWN) && !((defined(hpux) || defined (__hpux__)) && defined(F_SETOWN))
	r = ioctl(fd, FIOSSAIOOWN, &pid);
        if (r == -1 && errno != ENOTTY) {
		perror("Error doing FIOSSAIOWN");
	}
	
#elif defined(F_SETOWN)
	/* On some systems, this will flag an error if fd is not a socket */
	r = fcntl(fd, F_SETOWN, pid);
	if (r < 0) {
		DBG(JTHREAD, perror("F_SETOWN"); );
	}
#endif
	return (fd);
}

/*
 * clear non-blocking flag for a file descriptor
 */
void
jthreadRestoreFD(int fd)
{
	/* clear nonblocking flag */
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);
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
	for (i = 0; i < 3; i++) {
		jthreadRestoreFD(i);
    	}
}

static void
restore_fds_and_exit()
{
	restore_fds();
	/* technically, we should restore the original handler and rethrow
	 * the signal.
	 */
	KAFFEVM_EXIT(-1);		/* XXX */
}

/*
 * Threaded socket create.
 */
int
jthreadedSocket(int af, int type, int proto, int *out)
{
	int r;

	intsDisable();
	r = socket(af, type, proto);
	if (r == -1) {
		r = errno;
	} else {
		*out = jthreadedFileDescriptor(r);
		r = 0;
	}
	intsRestore();
	return (r);
}

/*
 * Return thread-specific data for a given jthread.
 */

threadData*
jthread_get_data(jthread_t tid)
{
	return (&tid->localData);
}

/*
 * Return thread status.
 */

int jthread_get_status(jthread_t jt)
{
	return (jt->status);
}

/*
 * Check if thread is interrupted.
 */
int jthread_is_interrupted(jthread_t jt)
{
	return (jt->flags & THREAD_FLAGS_INTERRUPTED);
}

int jthread_interrupted(jthread_t jt)
{
	if (jt->flags & THREAD_FLAGS_INTERRUPTED)
	{
		jt->flags &= ~THREAD_FLAGS_INTERRUPTED;
		return 1;
	}

	return 0;
}

int jthread_on_mutex(jthread_t jt)
{
	return (jt->flags & THREAD_FLAGS_WAIT_MUTEX);
}

int jthread_on_condvar(jthread_t jt)
{
	return (jt->flags & THREAD_FLAGS_WAIT_CONDVAR);
}

void jthread_clear_run(jthread_t jt)
{
	jt->startUsed = 0;
}

int jthread_has_run(jthread_t jt)
{
	return (jt->startUsed != 0);
}

/*
 * Threaded file open.
 */
int
jthreadedOpen(const char* path, int flags, int mode, int *out)
{
	int r;

	intsDisable();
	/* Cygnus WinNT requires this */
	r = open(path, flags|O_BINARY, mode);
	if (r == -1) {
		r = errno;
	} else {
		*out = jthreadedFileDescriptor(r);
		r = 0;
	}
	intsRestore();
	return (r);
}

/*
 * various building blocks for timeout system call functions
 */
#define SET_DEADLINE(deadline, timeout) 		\
	if (timeout != NOTIMEOUT) { 			\
		jlong ct = currentTime();		\
		deadline = timeout + ct;		\
		if( deadline < ct ) {			\
			deadline = 0;			\
			timeout = NOTIMEOUT;		\
		}					\
	}

#define BREAK_IF_LATE(deadline, timeout)		\
	if (timeout != NOTIMEOUT) {			\
		if (currentTime() >= deadline) {	\
			errno = ETIMEDOUT;		\
			break;				\
		}					\
	}

#define IGNORE_EINTR(r)					\
	if (r == -1 && errno == EINTR) {		\
		continue;				\
	}

#define SET_RETURN(r)					\
	if (r == -1) {					\
		r = errno;				\
	} 

#define SET_RETURN_OUT(r, out, ret)			\
	if (r == -1) {					\
		r = errno;				\
	} else {					\
		*out = ret;				\
		r = 0;					\
	}

#define CALL_BLOCK_ON_FILE(A, B, C)				\
	if (blockOnFile(A, B, C)) {				\
		/* interrupted via jthread_interrupt() */ 	\
		errno = EINTR; 					\
		break;						\
	}
/*
 * Threaded socket connect.
 */
int
jthreadedConnect(int fd, struct sockaddr* addr, int len, int timeout)
{
	int r;
	jlong deadline = 0;
	int inProgress = 0;

	intsDisable();
	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = connect(fd, addr, (socklen_t)len);
		if (r == 0 || !(errno == EINPROGRESS 
				|| errno == EINTR || errno == EISCONN)) {
			break;	/* success or real error */
		}
		if (r == -1 && errno == EISCONN) {
			/* On Solaris 2.5, after getting EINPROGRESS
			   from a non-blocking connect request, we
			   won't ever get success.  When we're waken
			   up, we'll either get EISCONN, which should
			   be taken as success, or a real failure.
			   However, we can't map EISCONN to success
			   inconditionally, because attempting to
			   connect the same socket again should raise
			   an exception.

			   Mapping EISCONN to success might lead to
			   false positives if connect fails and
			   another thread succeeds to connect this
			   socket before this one is waken up.  Let's
			   just hope it doesn't happen for now.  */
			if (inProgress) {
				r = 0;
			}
			break;
		} else if (r == -1 && errno == EINPROGRESS) {
			inProgress = 1;
			if (!blockingFD[fd]) {
				intsRestore();
				return (EWOULDBLOCK);
			}
		}
		IGNORE_EINTR(r)
		CALL_BLOCK_ON_FILE(fd, TH_CONNECT, timeout)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN(r)
	intsRestore();
	return (r);
}

/*
 * Threaded socket accept.
 */
int
jthreadedAccept(int fd, struct sockaddr* addr, socklen_t* len, 
		int timeout, int* out)
{
	/* absolute time at which time out is reached */
	jlong deadline = 0;
	int r;

	intsDisable();
	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = accept(fd, addr, len);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
				|| errno == EAGAIN)) {
			break;	/* success or real error */
		}
		if (r == EWOULDBLOCK && !blockingFD[fd]) {
			intsRestore();
			return (EWOULDBLOCK);
		}
		IGNORE_EINTR(r)
		CALL_BLOCK_ON_FILE(fd, TH_ACCEPT, timeout)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN_OUT(r, out, jthreadedFileDescriptor(r))
	intsRestore();
	return (r);
}

/*
 * Threaded read with timeout
 */
int
jthreadedTimedRead(int fd, void* buf, size_t len, int timeout, ssize_t *out)
{
	ssize_t r = -1;
	/* absolute time at which timeout is reached */
	jlong deadline = 0;


	assert(timeout >= 0 || timeout == NOTIMEOUT);
	intsDisable();
	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = read(fd, buf, len);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
				|| errno == EAGAIN)) {
			break;	/* real error or success */
		}
		IGNORE_EINTR(r)
		CALL_BLOCK_ON_FILE(fd, TH_READ, timeout)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN_OUT(r, out, r)
	intsRestore();
	return (r);
}

/*
 * Threaded write with timeout
 */
int
jthreadedTimedWrite(int fd, const void* buf, size_t len, int timeout, ssize_t *out)
{
	ssize_t r = 1;
	/* absolute time at which timeout is reached */
	jlong deadline = 0;
	const char *ptr = buf;

	assert(timeout >= 0 || timeout == NOTIMEOUT);
	intsDisable();
	SET_DEADLINE(deadline, timeout)
	while (len > 0 && r > 0) {
		r = write(fd, ptr, len);
		if (r >= 0) {
			ptr += r;
			len -= r;
			r = ptr - (const char *) buf;
			continue;
		}
		if (!(errno == EWOULDBLOCK || errno == EINTR 
				|| errno == EAGAIN)) {
			break;	/* real error or success */
		}
		if (errno == EINTR) {
			r = 1;
			continue;
		}
		if (blockOnFile(fd, TH_WRITE, timeout)) {
			/* interrupted by jthread_interrupt() */
			errno = EINTR;
			*out = ptr - (const char *) buf;
			break;
		}
		BREAK_IF_LATE(deadline, timeout)
		r = 1;
	}
	SET_RETURN_OUT(r, out, r)
	intsRestore();
	return (r);
}

/*
 * Threaded read with no time out
 */
int
jthreadedRead(int fd, void* buf, size_t len, ssize_t *out)
{
	return (jthreadedTimedRead(fd, buf, len, NOTIMEOUT, out));
}

/*
 * Threaded write
 */
int
jthreadedWrite(int fd, const void* buf, size_t len, ssize_t *out)
{
	ssize_t r = 1;
	const char* ptr;

	ptr = buf;

	intsDisable();
	while (len > 0 && r > 0) {
		r = (ssize_t)write(fd, ptr, len);
		if (r >= 0) {
			ptr += r;
			len -= r;
			r = ptr - (const char *) buf;
			continue;
		}
		if (errno == EINTR) {
			/* ignore */
			r = 1;
			continue;
		}
		if (!(errno == EWOULDBLOCK || errno == EAGAIN)) {
			/* real error */
			break;
		}
		/* must be EWOULDBLOCK or EAGAIN */
		if (!blockingFD[fd]) {
			errno = EWOULDBLOCK;
			*out = ptr - (const char *) buf;
			break;
		}
		if (blockOnFile(fd, TH_WRITE, NOTIMEOUT)) {
			/* interrupted by jthread_interrupt() */
			errno = EINTR;
			*out = ptr - (const char *) buf;
			break;
		}
		r = 1;
	}
	SET_RETURN_OUT(r, out, r)
	intsRestore();
	return (r); 
}

/*
 * Threaded recvfrom 
 */
int 
jthreadedRecvfrom(int fd, void* buf, size_t len, int flags, 
	struct sockaddr* from, socklen_t* fromlen, int timeout, ssize_t *out)
{
	int r;
	jlong deadline = 0;
 
	SET_DEADLINE(deadline, timeout)
	intsDisable();
	for (;;) {
		r = recvfrom(fd, buf, len, flags, from, fromlen);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
					|| errno == EAGAIN)) {
			break;
		}
		IGNORE_EINTR(r)
		/* else EWOULDBLOCK or EAGAIN */
		CALL_BLOCK_ON_FILE(fd, TH_READ, timeout)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN_OUT(r, out, r)
	intsRestore();
	return (r);
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
		resumeQueue(waitForList);
	}
}

/* 
 * Wait for a child process.
 */
int
jthreadedWaitpid(int wpid, int* status, int options, int *outpid)
{
#if defined(HAVE_WAITPID)
	int npid;
	int ret = 0;

DBG(JTHREAD,
	dprintf("waitpid %d current=%p\n", wpid, currentJThread); );

	intsDisable();
	for (;;) {
	        int procStatus;

		wouldlosewakeup = 1;
		npid = waitpid(wpid, &procStatus, options|WNOHANG);
		if (npid > 0) {
			*outpid = npid;
			if (WIFEXITED(procStatus))
			  *status = WEXITSTATUS(procStatus);
			else if (WIFSIGNALED(procStatus))
			  *status=128+WTERMSIG(procStatus);
			else
			  *status = -1;		/* shouldn't happen */
			break;
		}
		if ((npid == -1) && (errno == ECHILD)) {
			/* child does not exist */
			ret = -1;
			break;
		}
		BLOCKED_ON_EXTERNAL(currentJThread);
		if (suspendOnQThread(currentJThread, &waitForList, NOTIMEOUT)) {
			ret = EINTR;
			break;
		}
	}
	intsRestore();
	return (ret);
#else
	return (EOPNOTSUPPORT);
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
jthreadedForkExec(char **argv, char **arge,
	int ioes[4], int *outpid, const char *dir)
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
    );
	/* Create the pipes to communicate with the child */
	/* Make sure fds get closed if we can't create all pipes */
	for (nfd = 0; nfd < 8; nfd += 2) {
		int e;
		err = pipe(fds + nfd);
		e = errno;
		if (err == -1) {
			close_fds(fds, nfd);
			return (e);
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
			clearSignal(i);
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

		/* change working directory */
#if defined(HAVE_CHDIR)
		(void)chdir(dir);
#endif
 
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
		err = errno;
		/* Close all pipe fds */
		close_fds(fds, 8);
		sigprocmask(SIG_UNBLOCK, &nsig, 0);
		return (err);

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
		*outpid = pid;
		return (0);
	}

	exit(-1);
	/* NEVER REACHED */	
}

int
jthreadedSelect(int a, fd_set* b, fd_set* c, fd_set* d, 
		struct timeval* e, int* out)
{
	int rc = 0;
	struct timeval tval;
	int i;
	long time_milli;
	int second_time = 0;
	fd_set dummy_sets[3];
	
	assert(a < FD_SETSIZE);
	
	tval.tv_sec = 0;
	tval.tv_usec = 0;

	if (e != NULL)
		time_milli = e->tv_usec / 1000 + e->tv_sec * 1000;
	else
		time_milli = NOTIMEOUT;


	if (b == NULL) {
		FD_ZERO(&dummy_sets[0]);
		b = &dummy_sets[0];
	}

	if (c == NULL) {
		FD_ZERO(&dummy_sets[1]);
		c = &dummy_sets[1];
	}
	if (d == NULL) {
		FD_ZERO(&dummy_sets[2]);
		d = &dummy_sets[2];
	}

	intsDisable();

	for (;;) {
		fd_set tmp_b, tmp_c, tmp_d;

		FD_COPY(b, &tmp_b);
		FD_COPY(c, &tmp_c);
		FD_COPY(d, &tmp_d);

		if ((*out = select(a, &tmp_b, &tmp_c, &tmp_d, &tval)) == -1) {
			rc = errno;
			break;
		}
		if ((*out == 0 && second_time) || *out != 0) {
			FD_COPY(&tmp_b, b);
			FD_COPY(&tmp_c, c);
			FD_COPY(&tmp_d, d);
			break;
		}

		if (time_milli != 0) {
			int interrupted;

			BLOCKED_ON_EXTERNAL(currentJThread);

			if (a - 1 > maxFd)
				maxFd = a - 1;

			for (i=0;i<a;i++) {
				if (b && FD_ISSET(i, b)) {
					FD_SET(i, &readsPending);
					addWaitQThread(currentJThread, &readQ[i]);
				}
				if (c && FD_ISSET(i, c)) {
					FD_SET(i, &writesPending);
					addWaitQThread(currentJThread, &writeQ[i]);
				}
			}

			interrupted = suspendOnQThread(currentJThread, NULL, time_milli);

			for (i=0;i<a;i++) {
				if (b && FD_ISSET(i, b))
					FD_CLR(i, &readsPending);
				if (c && FD_ISSET(i, c))
					FD_CLR(i, &writesPending);
			}
			if (interrupted) {
				rc = EINTR;
				*out = 0;
				FD_ZERO(b);
				FD_ZERO(c);
				FD_ZERO(d);
				break;
			}
		}
		second_time = 1;
	}
	
	intsRestore();
	return (rc);
}

int jthreadedPipeCreate(int *read_fd, int *write_fd)
{
	int r;
	int pairs[2];

	intsDisable();
	/* Cygnus WinNT requires this */
	r = pipe(pairs);
	if (r == -1) {
		r = errno;
	} else {
		*read_fd = jthreadedFileDescriptor(pairs[0]);
		*write_fd = jthreadedFileDescriptor(pairs[1]);
		r = 0;
	}
	intsRestore();
	return (r);
}


void jthread_set_blocking(int fd, int blocking)
{
	assert(fd < FD_SETSIZE);

	intsDisable();
	blockingFD[fd] = blocking;
	intsRestore();
}

int jthread_is_blocking(int fd)
{
	assert(fd < FD_SETSIZE);

	return blockingFD[fd];
}

jlong jthread_get_usage(jthread_t jt)
{
       jlong retval;

       if( jt == jthread_current() )
       {
               struct rusage ru;
               jlong ct;

               getrusage(RUSAGE_SELF, &ru);
               ct = ((jlong)ru.ru_utime.tv_sec * 1000000)
                       + ((jlong)ru.ru_utime.tv_usec);
               ct += ((jlong)ru.ru_stime.tv_sec * 1000000)
                       + ((jlong)ru.ru_stime.tv_usec);

               retval = jt->totalUsed + (ct - jt->startUsed);
       }
       else
       {
               retval = jt->totalUsed;
       }
       retval *= 1000; /* Convert to nanos */
       return( retval );
}
