/*
 * internal.c
 * Internal threading system support
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)
#define	SDBG(s)

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
#include "internal.h"
#include "locks.h"
#include "exception.h"
#include "support.h"
#include "external.h"
#include "errors.h"
#include "gc.h"
#include "md.h"
#include "lerrno.h"
#define NOUNIXPROTOTYPES
#include "jsyscall.h"

static Hjava_lang_Thread* threadQhead[THREAD_MAXPRIO + 1];
static Hjava_lang_Thread* threadQtail[THREAD_MAXPRIO + 1];
static int maxFd = -1;
static fd_set readsPending;
static fd_set writesPending;
static Hjava_lang_Thread* readQ[FD_SETSIZE];
static Hjava_lang_Thread* writeQ[FD_SETSIZE];
static struct timeval zerotimeout = { 0, 0 };
static bool alarmBlocked;
static struct Hjava_lang_Thread* liveThreads;
static iLock threadLock;
static Hjava_lang_Thread* alarmList;
static Hjava_lang_Thread* currentThread;
static int talive;
static int tdaemon;

static void addToAlarmQ(Hjava_lang_Thread*, jlong);
static void removeFromAlarmQ(Hjava_lang_Thread*);
static void checkEvents(void);
static void childDeath(int);
static void alarmException(int);
static void resumeThread(Hjava_lang_Thread*);

int blockInts;
bool needReschedule;
jbool runFinalizerOnExit;

extern iLock waitlock;
extern gcFuncs gcThread;

void reschedule(void);
void Tspinon(void*);
void Tspinoff(void*);

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

#if !defined(STACK_COPY)
#define	STACK_COPY 128
#endif

#define TCTX(t) ((ctx*)unhand(t)->PrivateInfo)

/*
 * Resume a thread running.
 * This routine has to be called only from locations which ensure
 * run / block queue consistency. There is no check for illegal resume
 * conditions (like explicitly resuming an IO blocked thread). There also
 * is no update of any blocking queue. Both has to be done by the caller
 */
static
void
resumeThread(Hjava_lang_Thread* tid)
{
	Hjava_lang_Thread** ntid;

DBG(	printf("resumeThread %x\n", tid);			)
	Tspinon(0);

	if (TCTX(tid)->status != THREAD_RUNNING) {

		/* Remove from alarmQ if necessary */
		if ((TCTX(tid)->flags & THREAD_FLAGS_ALARM) != 0) {
			removeFromAlarmQ(tid);
		}
		/* Remove from lockQ if necessary */
		if (TCTX(tid)->blockqueue != 0) {
			for (ntid = TCTX(tid)->blockqueue; *ntid != 0; ntid = &TCTX(*ntid)->nextQ) {
				if (*ntid == tid) {
					*ntid = TCTX(tid)->nextQ;
					break;
				}
			}
			TCTX(tid)->blockqueue = 0;
		}

		TCTX(tid)->status = THREAD_RUNNING;

		/* Place thread on the end of its queue */
		if (threadQhead[TCTX(tid)->priority] == 0) {
			threadQhead[TCTX(tid)->priority] = tid;
			threadQtail[TCTX(tid)->priority] = tid;
			if (TCTX(tid)->priority > TCTX(currentThread)->priority) {
				needReschedule = true;
			}
		}
		else {
			TCTX(threadQtail[TCTX(tid)->priority])->nextQ = tid;
			threadQtail[TCTX(tid)->priority] = tid;
		}
		TCTX(tid)->nextQ = 0;
	}
SDBG(	else {
		printf("Re-resuming 0x%x\n", tid);
	}							)
	Tspinoff(0);
}

/*
 * Suspend a thread on a queue.
 */
static
void
suspendOnQThread(Hjava_lang_Thread* tid, Hjava_lang_Thread** queue, jlong timeout)
{
	Hjava_lang_Thread** ntid;
	Hjava_lang_Thread* last;

DBG(	printf("suspendOnQThread %x %x (%d)\n", tid, queue, (int)timeout); )

	assert(blockInts > 0);

	if (TCTX(tid)->status != THREAD_SUSPENDED) {
		TCTX(tid)->status = THREAD_SUSPENDED;

		last = 0;
		for (ntid = &threadQhead[TCTX(tid)->priority]; *ntid != 0; ntid = &TCTX(*ntid)->nextQ) {
			if (*ntid == tid) {

				/* Remove thread from runq */
				*ntid = TCTX(tid)->nextQ;
				if (*ntid == 0) {
					threadQtail[TCTX(tid)->priority] = last;
				}

				/* Insert onto head of lock wait Q */
				if (queue != 0) {
					TCTX(tid)->nextQ = *queue;
					*queue = tid;
					TCTX(tid)->blockqueue = queue;
				}

				/* If I have a timeout, insert into alarmq */
				if (timeout > NOTIMEOUT) {
					addToAlarmQ(tid, timeout);
				}

				/* If I was running, reschedule */
				if (tid == currentThread) {
					reschedule();
				}
				break;
			}
			last = *ntid;
		}
	}
SDBG(	else {
		printf("Re-suspending 0x%x on %x\n", tid, *queue);
	}							)
}

/*
 * Kill thread.
 */
static
void
killThread(Hjava_lang_Thread* tid)
{
	Hjava_lang_Thread** ntid;

	Tspinon(0);

	/* Notify on the object just in case anyone is waiting */
	lockMutex(&tid->base);
	broadcastCond(&tid->base);
	unlockMutex(&tid->base);

DBG(	printf("killThread %x\n", tid);			)

	if (TCTX(tid)->status != THREAD_DEAD) {

		/* Get thread off runq (if it needs it) */
		if (TCTX(tid)->status == THREAD_RUNNING) {
			for (ntid = &threadQhead[TCTX(tid)->priority]; *ntid != 0; ntid = &TCTX(*ntid)->nextQ) {
				if (*ntid == tid) {
					*ntid = TCTX(tid)->nextQ;
					break;
				}
			}
		}

		/* Run something else */
		needReschedule = true;
		blockInts = 1;

		/* Dead Jim - let the GC pick up the remains */
		TCTX(tid)->status = THREAD_DEAD;
	}

	Tspinoff(0);
}

static
void
addToAlarmQ(Hjava_lang_Thread* tid, jlong timeout)
{
	Hjava_lang_Thread** tidp;

	assert(blockInts > 0);

	TCTX(tid)->flags |= THREAD_FLAGS_ALARM;

	/* Get absolute time */
	TCTX(tid)->time = timeout + currentTime();

	/* Find place in alarm list and insert it */
	for (tidp = &alarmList; (*tidp) != 0; tidp = &TCTX(*tidp)->nextalarm) {
		if (TCTX(*tidp)->time > TCTX(tid)->time) {
			break;
		}
	}
	TCTX(tid)->nextalarm = *tidp;
	*tidp = tid;

	/* If I'm head of alarm list, restart alarm */
	if (tidp == &alarmList) {
		MALARM(timeout);
	}
}

static
void
removeFromAlarmQ(Hjava_lang_Thread* tid)
{
	Hjava_lang_Thread** tidp;

	assert(blockInts >= 1);

	TCTX(tid)->flags &= ~THREAD_FLAGS_ALARM;

	/* Find thread in alarm list and remove it */
	for (tidp = &alarmList; (*tidp) != 0; tidp = &TCTX(*tidp)->nextalarm) {
		if ((*tidp) == tid) {
			(*tidp) = TCTX(tid)->nextalarm;
			TCTX(tid)->nextalarm = 0;
			break;
		}
	}
}

/*
 * Handle alarm.
 * This routine uses a different meaning of "blockInts". Formerly, it was just
 * "don't reschedule if you don't have to". Now it is "don't do ANY
 * rescheduling actions due to an expired timer". An alternative would be to
 * block SIGALARM during critical sections (by means of sigprocmask). But
 * this would be required quite often (for every outmost Tspinon(0),
 * Tspinoff(0)) and therefore would be much more expensive than just
 * setting an int flag which - sometimes - might cause an additional
 * setitimer call.
 */
static
void
alarmException(int sig)
{
	Hjava_lang_Thread* tid;
	Hjava_lang_Thread** ntid;
	jlong time;

	/* Re-enable signal - necessary for SysV */
	catchSignal(sig, alarmException);

	Tspinon(0);

	/*
	 * If ints are blocked, this might indicate an inconsistent state of
	 * one of the thread queues (either alarmList or threadQhead/tail).
	 * We better don't touch one of them in this case and come back later.
	 */
	if (blockInts > 1) {
		MALARM(50);
		Tspinoff(0);
		return;
	}

	/* Wake all the threads which need waking */
	time = currentTime();
	while (alarmList != 0 && TCTX(alarmList)->time <= time) {
		/* Restart thread - this will tidy up the alarm and blocked
		 * queues.
		 */
		tid = alarmList;
		alarmList = TCTX(alarmList)->nextalarm;
		resumeThread(tid);
	}

	/* Restart alarm */
	if (alarmList != 0) {
		MALARM(TCTX(alarmList)->time - time);
	}

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
	Tspinoff(0);
	alarmBlocked = false;
}

/*
 * Child process has died.
 */
static
void
childDeath(int sig)
{
	static bool withchild = false;

	/* This bizzare bit of code handles the SYSV machines which re-throw SIGCHLD
	 * when you reset the handler.  It also works for those which don't.
	 * Perhaps there's a way to detech this in the configuration process?
	 */
	if (withchild == true) {
		return;
	}
	withchild = true;
	catchSignal(sig, childDeath);
	withchild = false;

	lockStaticMutex(&waitlock);
	signalStaticCond(&waitlock);
	unlockStaticMutex(&waitlock);
}

/*
 * Reschedule the thread.
 * Called whenever a change in the running thread is required.
 */
void
reschedule(void)
{
	int i;
	Hjava_lang_Thread* lastThread;
	int b;
	sigset_t nsig;

	/* A reschedule in a non-blocked context is half way to hell */
	assert(blockInts > 0);
	b = blockInts;

	for (;;) {
	        for (i = THREAD_MAXPRIO; i >= 0; i--) {
			if (threadQhead[i] != 0) {
				if (threadQhead[i] != currentThread) {
					lastThread = currentThread;
					currentThread = threadQhead[i];

					if (setjmp(TCTX(lastThread)->env) == 0) {
						TCTX(lastThread)->restorePoint = GET_SP(TCTX(lastThread)->env);
						longjmp(TCTX(currentThread)->env, 1);
					}

					/* Alarm signal may be blocked - if so
					 * unblock it.
					 */
					if (alarmBlocked == true) {
						alarmBlocked = false;
						sigemptyset(&nsig);
						sigaddset(&nsig, SIGALRM);
						sigprocmask(SIG_UNBLOCK, &nsig, 0);
					}

					/* Restore ints */
					blockInts = b;

					/* I might be dying */
					if ((TCTX(lastThread)->flags & THREAD_FLAGS_KILLED) != 0 && blockInts == 1) {
						TCTX(lastThread)->flags &= ~THREAD_FLAGS_KILLED;
						blockInts = 0;
						throwException(ThreadDeath);
						assert("Rescheduling dead thread" == 0);
					}
				}
				/* Now kill the schedule */
				needReschedule = false;
				return;
			}
		}
		/* Nothing to run - wait for external event */
		checkEvents();
	}
}

/*
 * Wait for some file descriptor or other event to become ready.
 */
static
void
checkEvents(void)
{
	int r;
	fd_set rd;
	fd_set wr;
	Hjava_lang_Thread* tid;
	Hjava_lang_Thread* ntid;
	int i;
	int b;

DBG(	printf("checkEvents\n");					)

	FD_COPY(&readsPending, &rd);
	FD_COPY(&writesPending, &wr);

	/*
	 * Select() is called with indefinite wait, but we have to make sure
	 * we can get interrupted by timer events.  However, we should *NOT*
	 * reschedule.
	 */
	needReschedule = false;
	b = blockInts;
	blockInts = 0;
	r = (*Kaffe_SystemCallInterface._select)(maxFd+1, &rd, &wr, 0, 0);
	blockInts = b;

	/* If select get's interrupted, just return now */
	if (r < 0 && errno == EINTR) {
		return;
	}

	/* We must be holding off interrupts before we start playing with
	 * the read and write queues.  This should be already done but a
	 * quick check never hurt anyone.
	 */
	assert(blockInts > 0);

	/* On a select error, mark all threads in error and release
	 * them all.  They should try to re-select and pick up their
	 * own individual errors.
	 */
	if (r < 0) {
		for (i = 0; i <= maxFd; i++) {
			for (tid = readQ[i]; tid != 0; tid = ntid) {
				ntid = TCTX(tid)->nextQ;
				TCTX(tid)->flags |= THREAD_FLAGS_ERROR;
				resumeThread(tid);
			}
			for (tid = writeQ[i]; tid != 0; tid = ntid) {
				ntid = TCTX(tid)->nextQ;
				TCTX(tid)->flags |= THREAD_FLAGS_ERROR;
				resumeThread(tid);
			}
			writeQ[i] = 0;
			readQ[i] = 0;
		}
		return;
	}

DBG(	printf("Select returns %d\n", r);				)

	for (i = 0; r > 0 && i <= maxFd; i++) {
		if (readQ[i] != 0 && FD_ISSET(i, &rd)) {
			for (tid = readQ[i]; tid != 0; tid = ntid) {
				ntid = TCTX(tid)->nextQ;
				resumeThread(tid);
			}
			readQ[i] = 0;
			r--;
		}
		if (writeQ[i] != 0 && FD_ISSET(i, &wr)) {
			for (tid = writeQ[i]; tid != 0; tid = ntid) {
				ntid = TCTX(tid)->nextQ;
				resumeThread(tid);
			}
			writeQ[i] = 0;
			r--;
		}
	}
}

/*
 * An attempt to access a file would block, so suspend the thread until
 * it will happen.
 */
int
blockOnFile(int fd, int op)
{
	fd_set fset;
	int r;

DBG(	printf("blockOnFile()\n");					)

	/* Trap obviously invalid file descriptors */
	if (fd < 0) {
		errno = EBADF;
		return (-1);
	}

	Tspinon(0);

	retry:

	/* First a quick check to see if the file handle is usable.
	 * This saves going through all that queuing stuff.
	 */
	FD_ZERO(&fset);
	FD_SET(fd, &fset);
	r = (*Kaffe_SystemCallInterface._select)(fd+1, (op == TH_READ ? &fset : 0), (op == TH_WRITE ? &fset : 0), 0, &zerotimeout);

	/* Select got interrupted - do it again */
	if (r < 0 && errno == EINTR) {
		goto retry;
	}
	/* If r != 0 then either its and error and we should return it, or the
	 * file is okay to use so we should use it.  Either way, return now.
	 */
	if (r != 0) {
		Tspinoff(0);
		assert(blockInts == 0);
		return (r);
	}

	if (fd > maxFd) {
		maxFd = fd;
	}
	if (op == TH_READ) {
		FD_SET(fd, &readsPending);
		suspendOnQThread(currentThread, &readQ[fd], NOTIMEOUT);
		FD_CLR(fd, &readsPending);
	}
	else {
		FD_SET(fd, &writesPending);
		suspendOnQThread(currentThread, &writeQ[fd], NOTIMEOUT);
		FD_CLR(fd, &writesPending);
	}

	/* If we have an error flagged, retry the whole thing. */
	if ((TCTX(currentThread)->flags & THREAD_FLAGS_ERROR) != 0) {
		TCTX(currentThread)->flags &= ~THREAD_FLAGS_ERROR;
		goto retry;
	}

	Tspinoff(0);
	assert(blockInts == 0);

	return (1);
}

/*
 * Allocate a new thread context and stack.
 */
static
ctx*
newThreadCtx(int stackSize)
{
	ctx *ct;

	ct = gc_malloc(sizeof(ctx) + stackSize, GC_ALLOC_THREADCTX);
	ct->status = THREAD_SUSPENDED;
	ct->stackBase = (uint8*)(ct + 1);
	ct->stackEnd = ct->stackBase + stackSize;
	ct->restorePoint = ct->stackEnd;
	return (ct);
}

static
void
allocThreadCtx(Hjava_lang_Thread* tid, int stackSize)
{
	void* mem;

	mem = newThreadCtx(stackSize);
	unhand(tid)->PrivateInfo = mem;
	GC_WRITE(tid, mem);
}

/*
 * Walk the thread's internal context.
 */
static
void
TwalkThread(Hjava_lang_Thread* tid)
{
	ctx* ct;

	if (unhand(tid)->PrivateInfo == 0) {
		return;
	}

	ct = TCTX(tid);
	markObject(unhand(tid)->exceptObj);
#if defined(STACK_GROWS_UP)
	walkConservative(ct->stackBase, ct->restorePoint - ct->stackBase);
#else
	walkConservative(ct->restorePoint, ct->stackEnd - ct->restorePoint);
#endif
}

static
void
TcreateFirst(Hjava_lang_Thread* tid)
{
	ctx* c;

	/* We do our general initialisation first */
	initStaticMutex(&threadLock);

        /* Plug in the alarm handler */
#if defined(SIGALRM)
	catchSignal(SIGALRM, alarmException);
#endif
#if defined(SIGCHLD)
	catchSignal(SIGCHLD, childDeath);
#endif

	allocThreadCtx(tid, 0);
	liveThreads = tid;
	talive = 1;

	c = TCTX(tid);
	c->priority = unhand(tid)->priority;
	c->status = THREAD_SUSPENDED;
	c->flags = THREAD_FLAGS_GENERAL;
	currentThread = tid;
	resumeThread(tid);

#if defined(STACK_GROWS_UP)
	c->stackBase = (void*)(((uintp)&c) - STACK_COPY);
	c->stackEnd = c->stackBase + threadStackSize;
	c->restorePoint = c->stackEnd;
#else
	c->stackEnd = (void*)(STACK_COPY + ((uintp)&c));
	c->stackBase = c->stackEnd - threadStackSize;
	c->restorePoint = c->stackBase;
#endif
}

static
void
Tcreate(Hjava_lang_Thread* tid, void* func)
{
	ctx* c;
	int cidx;

	lockStaticMutex(&threadLock);

	talive++;
	if (unhand(tid)->daemon) {
		tdaemon++;
	}

	allocThreadCtx(tid, threadStackSize);

	c = TCTX(tid);
	c->priority = unhand(tid)->priority;
	c->status = THREAD_SUSPENDED;
	c->flags = THREAD_FLAGS_GENERAL;
	c->nextlive = liveThreads;
	liveThreads = tid;

	unlockStaticMutex(&threadLock);

	assert(func != 0);

	cidx = setjmp(c->env);
	if (cidx == 0) {
		memcpy(c->stackEnd - STACK_COPY, GET_SP(c->env), STACK_COPY);
		SET_SP(c->env, c->stackEnd - STACK_COPY);
		c->func = func;
	}
	else {
		(*((ctx*)unhand(currentThread)->PrivateInfo)->func)(0);
	}

	resumeThread(tid);
}

static
void
Tsetup(void* arg)
{
	/* Every thread starts with the interrupts off */
	Tspinoff(0);
}

static
void
Tsleep(jlong time)
{
	Tspinon(0);
	suspendOnQThread(currentThread, 0, time);
	Tspinoff(0);
}

static
void
Tyield(void)
{
	int cprio;

	Tspinon(0);

	cprio = TCTX(currentThread)->priority;

	if (threadQhead[cprio] != threadQtail[cprio]) {
		/* Get the next thread and move me to the end */
		threadQhead[cprio] = TCTX(currentThread)->nextQ;
		TCTX(threadQtail[cprio])->nextQ = currentThread;
		threadQtail[cprio] = currentThread;
		TCTX(currentThread)->nextQ = 0;
		needReschedule = true;
	}

	Tspinoff(0);
}

static
void
Tprio(Hjava_lang_Thread* tid, jint prio)
{
	Hjava_lang_Thread** ntid;
	Hjava_lang_Thread* last;

	/* Not natifve thread yet */
	if (unhand(tid)->PrivateInfo == 0) {
		return;
	}

	if (TCTX(tid)->status == THREAD_SUSPENDED) {
		TCTX(tid)->priority = (uint8)prio;
		return;
	}

	Tspinon(0);

	/* Remove from current thread list */
	last = 0;
	for (ntid = &threadQhead[TCTX(tid)->priority]; *ntid != 0; ntid = &TCTX(*ntid)->nextQ) {
		if (*ntid == tid) {
			*ntid = TCTX(tid)->nextQ;
			if (*ntid == 0) {
				threadQtail[TCTX(tid)->priority] = last;
			}
			break;
		}
		last = *ntid;
	}

	/* Insert onto a new one */
	unhand(tid)->priority = prio;
	TCTX(tid)->priority = (uint8)unhand(tid)->priority;
	if (threadQhead[prio] == 0) {
		threadQhead[prio] = tid;
		threadQtail[prio] = tid;
	}
	else {
		TCTX(threadQtail[prio])->nextQ = tid;
		threadQtail[prio] = tid;
	}
	TCTX(tid)->nextQ = 0;

	/* If I was reschedulerd, or something of greater priority was,
	 * insist on a reschedule.
	 */
	if (tid == currentThread || prio > TCTX(currentThread)->priority) {
		needReschedule = true;
	}

	Tspinoff(0);
}

static
void
Tstop(Hjava_lang_Thread* tid)
{
	TCTX(tid)->flags |= THREAD_FLAGS_KILLED;
	resumeThread(tid);
}

static
void
Texit(void)
{
	Hjava_lang_Thread* tid;
	Hjava_lang_Thread** ntid;

	lockStaticMutex(&threadLock);

	talive--;
	if (unhand(currentThread)->daemon) {
		tdaemon--;
	}

	/* Remove thread from live list so it can be garbaged */
	for (ntid = &liveThreads; *ntid != 0; ntid = &TCTX(*ntid)->nextlive) {
		if (currentThread == (*ntid)) {
			(*ntid) = TCTX(currentThread)->nextlive;
			break;
		}
	}

	unlockStaticMutex(&threadLock);

	/* If we only have daemons living the we should exit */
        if (talive == tdaemon) {
		if (runFinalizerOnExit != 0) {
			invokeFinalizer();
		}
		/* Kill any daemon threads */
		for (tid = liveThreads; tid != 0; tid = TCTX(tid)->nextlive) {
			killThread(tid);
                }
		/* Am I suppose to close things down nicely ?? */
		EXIT(0);
        }
        else {
		for (;;) {
			killThread(currentThread);
			sleepThread(1000);
		}
	}
}

static
bool
Talive(Hjava_lang_Thread* tid)
{
	bool status;

	Tspinon(0);
	if (unhand(tid)->PrivateInfo == 0 || TCTX(tid)->status == THREAD_DEAD) {
		status = false;
	}
	else {
		status = true;
	}
	Tspinoff(0);

	return (status);
}

static
jint
Tframes(Hjava_lang_Thread* tid)
{
	return (0);	/* ??? */
}

static
void
Tfinalize(Hjava_lang_Thread* tid)
{
	if (unhand(tid)->PrivateInfo != 0) {
		gc_free(TCTX(tid));
		unhand(tid)->PrivateInfo = 0;
	}
}

static
Hjava_lang_Thread*
TcurrentJava(void)
{
	return (currentThread);
}

static
void*
TcurrentNative(void)
{
	return (currentThread);
}

static
void
TwalkThreads(void)
{
	Hjava_lang_Thread* tid;

	for (tid = liveThreads; tid != NULL; tid = TCTX(tid)->nextlive) {
		walkMemory(tid);
	}
}

static
void*
TnextFrame(void* fm)
{
#if defined(TRANSLATOR)
	exceptionFrame* nfm;

	nfm = (exceptionFrame*)(((exceptionFrame*)fm)->retbp);
	if (FRAMEOKAY(nfm)) {
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


static
void
Linit(iLock* lk)
{
	lk->mux = NULL;
	lk->cv = NULL;
}

static
void
Llock(iLock* lk)
{
	Tspinon(0);
	while (lk->holder != NULL) {
		suspendOnQThread(currentThread, (Hjava_lang_Thread**)&lk->mux, NOTIMEOUT);
	}
	lk->holder = currentThread;
	Tspinoff(0);
}

static
void
Lunlock(iLock* lk)
{
	Hjava_lang_Thread* tid;

	Tspinon(0);
	lk->holder = NULL;
	if (lk->mux != 0) {
		tid = lk->mux;
		lk->mux = TCTX(tid)->nextQ;
		assert(TCTX(tid)->status != THREAD_RUNNING);
		resumeThread(tid);
	}
	Tspinoff(0);
}

static
void
Lwait(iLock* lk, jlong timeout)
{
	Hjava_lang_Thread* tid;

	Tspinon(0);
	lk->holder = NULL;
	if (lk->mux != NULL) {
		tid = lk->mux;
		lk->mux = TCTX(tid)->nextQ;
		assert(TCTX(tid)->status != THREAD_RUNNING);
		resumeThread(tid);
	}
	suspendOnQThread(currentThread, (Hjava_lang_Thread**)&lk->cv, timeout);
	while (lk->holder != NULL) {
		suspendOnQThread(currentThread, (Hjava_lang_Thread**)&lk->mux, NOTIMEOUT);
	}
	lk->holder = currentThread;
	Tspinoff(0);
}

static
void
Lsignal(iLock* lk)
{
	Hjava_lang_Thread* tid;

	Tspinon(0);
	if (lk->cv != NULL) {
		tid = lk->cv;
		lk->cv = TCTX(tid)->nextQ;
		TCTX(tid)->nextQ = lk->mux;
		lk->mux = tid;
	}
	Tspinoff(0);
}

static
void
Lbroadcast(iLock* lk)
{
	Hjava_lang_Thread** tidp;

	Tspinon(0);
	if (lk->cv != NULL) {
		for (tidp = (Hjava_lang_Thread**)&lk->cv; *tidp != 0; tidp = &TCTX(*tidp)->nextQ)
			;
		(*tidp) = lk->mux;
		lk->mux = lk->cv;
		lk->cv = NULL;
	}
	Tspinoff(0);
}

void
Tspinon(void* arg)
{
	blockInts++;
}

void
Tspinoff(void* arg)
{
	if (blockInts == 1 && needReschedule == true) {
		reschedule();
	}
	blockInts--;
}

/*
 * Define the thread interface.
 */
ThreadInterface Kaffe_ThreadInterface = {

	TcreateFirst,
	Tcreate,
	Tsetup,
	Tsleep,
	Tyield,
	Tprio,
	Tstop,
	Texit,
	Talive,
	Tframes,
	Tfinalize,

	TcurrentJava,
	TcurrentNative,

	TwalkThreads,
	TwalkThread,

	TnextFrame,
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
