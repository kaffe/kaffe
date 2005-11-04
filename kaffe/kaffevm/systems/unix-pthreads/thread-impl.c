/*
 * thread-impl.c - pthread based ThreadInterface implementation
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2005
 *      Kaffe.org contributors.  See ChangeLog for details. 
 *      All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "lerrno.h"
#include <limits.h>

#include "config.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-setjmp.h"
#include "config-io.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include "locks.h"
#include "thread-impl.h"
#include "debug.h"
#include "md.h"
#include "gc.h"
#include "thread.h"
#ifdef KAFFE_BOEHM_GC
#include "boehm-gc/boehm/include/gc.h"
#endif

/* define __USE_GNU for pthread_yield on linux */
#define __USE_GNU
#include <pthread.h>

#if !defined(HAVE_PTHREAD_YIELD) && defined(HAVE_SCHED_YIELD)
#if defined(HAVE_SCHED_H)
#include <sched.h>
#endif // SCHED_H
#endif // SCHED_YIELD && !PTHREAD_YIELD

#ifndef MAINSTACKSIZE
#define MAINSTACKSIZE (1024*1024)
#endif

#if defined(KAFFE_VMDEBUG)

static char stat_act[]   = { ' ', 'a' };
static char stat_susp[]  = { ' ', 's', ' ', 'r', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
static char stat_block[] = { ' ', 'T', 'm', ' ', 'c', ' ', ' ', ' ', 't', ' ', ' ' };

#define TMSG_SHORT(_msg,_nt)     \
   dprintf(_msg" %p [tid:%4lx, java:%p]\n", \
    _nt, _nt->tid, _nt->data.jlThread)

#define TMSG_LONG(_msg,_nt)      \
   dprintf(_msg" %p [tid:%4lx, java:%p], stack [%p..%p..%p], state: %c%c%c\n",         \
        _nt, _nt->tid, _nt->data.jlThread, _nt->stackMin, _nt->stackCur, _nt->stackMax,  \
        stat_act[_nt->active], stat_susp[_nt->suspendState], stat_block[_nt->blockState])

#define CHECK_CURRENT_THREAD(_nt)                                          \
  if ( ((uintp) &_nt < (uintp) _nt->stackMin) ||           \
       ((uintp) &_nt > (uintp) _nt->stackMax) ) {          \
    printf( "?? inconsistent current thread: %x [tid: %d, java: %x]\n",    \
                    _nt, _nt->tid, _nt->data.jlThread);                                   \
    tDump();                                                               \
  }

#endif /* KAFFE_VMDEBUG */

/***********************************************************************
 * typedefs & defines
 */

/*
 * This is the configurable section. Note that SCHED_FIFO is the only
 * schedule policy which conforms to the "old" Java thread model (with
 * stringent priorities), but it usually isn't available on desktop
 * OSes (or imposes certain restrictions, e.g. root privileges).
 */
#if defined(HAVE_SCHED_OTHER_IN_SCHED)
#define SCHEDULE_POLICY     SCHED_OTHER
#else
#undef SCHEDULE_POLICY
#endif

/* our upper limit for cached threads (0 = no caching at all) */
#define MAX_CACHED_THREADS 0


/*
 * Flag to say whether the thread subsystem has been initialized.
 */
static char jthreadInitialized = false;

/*
 * Signal to use to suspend all threads.
 */
static int sigSuspend;

/*
 * Signal to use to resume all threads.
 */
static int sigResume;

/*
 * Signal to use to dump thread internal state.
 */
static int sigDump;

/*
 * Signal to use to produce an interrupt.
 */
static int sigInterrupt;

/*
 * Signals used by the thread system to restard/cancel threads.
 */
static int psigRestart;
static int psigCancel;

/**
 * This variable holds a pointer to the garbage collector which
 * will be used to allocate thread data.
 */
static Collector *threadCollector;

/***********************************************************************
 * global data
 */

/** We keep a list of all active threads, so that we can enumerate them */
static jthread_t	activeThreads;

/** This mutex lock prevents somebody to modify or read the active thread
 * concurrently with some other threads. This prevents some bug that may appear 
 * when a thread die, is created or is being walked.
 */
static pthread_mutex_t		activeThreadsLock = PTHREAD_MUTEX_INITIALIZER;

/** This mutex lock protects calls into non-reentrant system services.
 */
static pthread_mutex_t		systemMutex = PTHREAD_MUTEX_INITIALIZER;

/** We don't throw away threads when their user func terminates, but suspend
 * and cache them for later re-use */
static jthread_t	cache;

/** The notorious first thread, which has to be handled differently because
 * it isn't created explicitly */
static jthread_t	firstThread;

/** Number of active non-daemon threads (the last terminating nonDaemon
 * causes the process to shut down */
static int		nonDaemons;

/** Number of system threads (either running (activeThreads) or
 * blocked (cache). We need this to implement our own barrier, since
 * many kernel thread systems don't behave graceful on exceeding their limit */
static int		nSysThreads;

/** number of currently cached threads */
static int		nCached;

/** map the Java priority levels to whatever the pthreads impl gives us */
static int		*priorities;

/** thread-specific-data key to retrieve 'nativeData' */
pthread_key_t		ntKey;

/** a hint to avoid unnecessary pthread_creates (with pending exits) */
static volatile int	pendingExits;

/** level of critical sections (0 = none) */
static int		critSection;

/** helper semaphore to signal completion of critical section enter/exit */
static repsem_t		critSem;

/** Signal set which contains important signals for suspending threads. */
static sigset_t		suspendSet;

/** This callback is to be called when a thread exits. */
static void (*threadDestructor)(void *);

/** This callback is called when all non-daemon threads exit. */
static void (*runOnExit)(void);

#ifdef KAFFE_VMDEBUG
/** an optional deadlock watchdog thread (not in the activeThread list),
 * activated by KAFFE_VMDEBUG topic JTHREAD */
static pthread_t	deadlockWatchdog;

/**
 * This is a debugging variable to analyze possible deadlock when dumping thread states.
 * It retains a pointer to the thread holding the thread list lock.
 */
static jthread_t        threadListOwner;

#endif /* KAFFE_VMDEBUG */

static void suspend_signal_handler ( int sig );
static void resume_signal_handler ( int sig );
static void tDispose ( jthread_t nt );

static void *
thread_malloc(size_t bytes)
{
	return KGC_malloc(threadCollector, bytes, KGC_ALLOC_THREADCTX);
}

static inline void
protectThreadList(jthread_t cur)
{
  cur->blockState |= BS_THREAD;
  jmutex_lock(&activeThreadsLock);
#ifdef KAFFE_VMDEBUG
  threadListOwner = cur;
#endif
}

static inline void
unprotectThreadList(jthread_t cur)
{
#ifdef KAFFE_VMDEBUG
  threadListOwner = NULL;
#endif
  jmutex_unlock(&activeThreadsLock);
  cur->blockState &= ~BS_THREAD;
}

/***********************************************************************
 * internal functions
 */


#if defined(KAFFE_VMDEBUG)
/*
 * dump a thread list, marking the supposed to be current thread
 */
static void
tDumpList ( jthread_t cur, jthread_t list )
{
  int		i;
  char		a1, a2, a3;
  jthread_t	t;

  for ( t=list, i=0; t; t=t->next, i++ ){
	/* the supposed to be current thread? */
	a1 = (t == cur) ? '*' : ' ';
	/* the current thread from a stack point-of view? */
	a2 = (((uintp)&i > (uintp)t->stackMin) &&
		  ((uintp)&i < (uintp)t->stackMax)) ? 'S' : ' ';
	/* the first one? */
	a3 = (t == firstThread) ? '1' : ' ';

	dprintf("%4d: %c%c%c %c%c%c   %p [tid: %4ld, java: %p]  "
		"stack: [%p..%p..%p]\n",
		i, a1, a2, a3, stat_act[t->active], stat_susp[t->suspendState],
		stat_block[t->blockState], t, (long)t->tid, t->data.jlThread,
		t->stackMin, t->stackCur, t->stackMax);
  }
}
#endif /* defined(KAFFE_VMDEBUG) */

/*
 * dump the state of the threading system
 */
static void
tDump (void)
{
  DBG(JTHREAD, {
	jthread_t	cur = jthread_current();

	dprintf("\n======================== thread dump =========================\n");

	dprintf("thread list lock owner: %p\n", threadListOwner);

	protectThreadList(cur);

	dprintf("state:  nonDaemons: %d, critSection: %d\n",
					 nonDaemons, critSection);

	dprintf("active threads:\n");
	tDumpList( cur, activeThreads);

	dprintf("\ncached threads:\n");
	tDumpList( cur, cache);

	unprotectThreadList(cur);

	dprintf("====================== end thread dump =======================\n");
  });
}

/*
 * On demand debug signal to dump the current thread state(s) (requested
 * by a external "kill -s <sigDump> <proc-id>"
 */
static
void
dump_signal_handler (int sig UNUSED)
{
  tDump();
}

#ifdef KAFFE_VMDEBUG
static
void* tWatchdogRun (void* p UNUSED)
{
  jthread_t t;
  int life;

  while ( nonDaemons ) {
	life = 0;
	for ( t=activeThreads; t != NULL; t = t->next ){
	  /*
	   * if we have a single thread that is not blocked at all, is in a
	   * timeout wait, and is not suspended, we are still safe (even though
	   * the timeout value might effectively be a deadlock)
	   */
	  if ( (!t->blockState || (t->blockState == BS_SYSCALL) || (t->blockState == BS_CV_TO)) && !t->suspendState ){
		life = 1;
		break;
	  }
	}

	if ( !life ) {
	  DBG( JTHREAD, dprintf("deadlock\n"));
	  tDump();
	  KAFFEVM_ABORT();
	}

	usleep( 5000);
  }

  return NULL;
}

static
void tStartDeadlockWatchdog (void)
{
  pthread_attr_t attr;
#if defined(SCHEDULE_POLICY)
  struct sched_param sp;

  sp.sched_priority = priorities[0];  /* looow */
#endif

  pthread_attr_init( &attr);
#if defined(SCHEDULE_POLICY)
  pthread_attr_setschedparam( &attr, &sp);
#endif
  pthread_attr_setstacksize( &attr, 4096);

  pthread_create( &deadlockWatchdog, &attr, tWatchdogRun, NULL);
}
#endif /* KAFFE_VMDEBUG */


/***********************************************************************
 * thread system initialization
 */

/**
 * Static initialisation of signal handlers. This function is called once.
 */
static void
tInitSignalHandlers (void)
{
  struct sigaction saSuspend, saResume, saInterrupt, saDump;
  unsigned int flags = 0;

#if defined(SA_RESTART)
  flags |= SA_RESTART;
#endif

  saSuspend.sa_flags = flags;
  saSuspend.sa_handler = suspend_signal_handler;
  sigemptyset( &saSuspend.sa_mask);
  sigaddset( &saSuspend.sa_mask, sigSuspend);
  sigaddset( &saSuspend.sa_mask, sigResume);
  if (psigRestart > 0)
    sigaddset( &saSuspend.sa_mask, psigRestart);

  if (psigCancel > 0)
    sigaddset( &saSuspend.sa_mask, psigCancel);

  sigaddset( &saSuspend.sa_mask, SIGSTOP);
  sigaddset( &saSuspend.sa_mask, SIGCONT);
  sigaddset( &saSuspend.sa_mask, SIGWINCH);

#ifndef KAFFE_BOEHM_GC
  sigaction( sigSuspend, &saSuspend, NULL);
#endif

  saResume.sa_flags = 0; /* Note that we do not want restart here. */
  saResume.sa_handler = resume_signal_handler;
  saResume.sa_mask = saSuspend.sa_mask;
#if !defined(KAFFE_BOEHM_GC) && !defined(KAFFE_BUGGY_NETBSD_SIGWAIT)
  sigaction( sigResume, &saResume, NULL);
#endif

  saInterrupt.sa_flags = flags;
  saInterrupt.sa_handler = SIG_IGN;
  sigemptyset(&saInterrupt.sa_mask);
  sigaction( sigInterrupt, &saInterrupt, NULL);

  saDump.sa_flags = flags;
  saDump.sa_handler = dump_signal_handler;
  sigemptyset( &saDump.sa_mask);
  sigaction( sigDump, &saDump, NULL);
}

/**
 * Initialize signal numbers to use depending of realtime signal availabilities.
 *
 * ORIGINAL NOTE:
 *
 * Now it starts to get hackish - we have to pick some signals
 * for suspend/resume (enter/exitCritSect) which don't interfere
 * with pthread implementations. Note that we can't rely on when
 * a suspend signal is delivered, and it's therefore not safe
 * to mulitplex a sinle signal for both suspend & resume purposes
 */
static void
tInitSignals(void)
{
#if !defined(SIGRTMIN)
#define SIGRTMIN -1
#define SIGRTMAX -1
#endif

  if (SIGRTMAX - SIGRTMIN < 7)
    {
#if defined(OLD_LINUXTHREADS)
      sigSuspend = SIGURG;
      sigResume  = SIGTSTP;
      sigDump    = SIGXCPU;
/*
 * Sneak these signal in from the thread library.
 */
      psigRestart = SIGUSR1;
      psigCancel  = SIGUSR2;


#else // OLD_LINUXTHREADS

      sigSuspend  = SIGUSR1;
      sigResume   = SIGUSR2;
      sigDump     = SIGURG;

// PSIG_RESTART and PSIG_CANCEL are left undefined.
      psigRestart = -1;
      psigCancel  = -1;

#endif

      if (SIGRTMIN < 0)
	sigInterrupt = SIGCONT;
    }
  else
    {
      sigSuspend = SIGRTMIN+6;
      sigResume  = SIGRTMIN+5;
      sigDump    = SIGURG;
/*
 * Sneak these signal in from the thread library.
 */
      psigRestart = SIGRTMIN;
      psigCancel  = SIGRTMIN+1;

      sigInterrupt = SIGRTMIN+2;
    }
}


/*
 * static init set up of Java-to-pthread priority mapping (pthread prioritiy levels
 * are implementation dependent)
 */
static
void tMapPriorities (int npr)
{
  int     d, max, min, i;
  float   r;

#if defined(HAVE_SCHED_GET_PRIORITY_MIN) && defined(SCHEDULE_POLICY)
  min = sched_get_priority_min( SCHEDULE_POLICY);
#else
  min = 0;
#endif /* defined(HAVE_SCHED_GET_PRIORITY_MIN) */

#if defined(HAVE_SCHED_GET_PRIORITY_MAX) && defined(SCHEDULE_POLICY)
  max = sched_get_priority_max( SCHEDULE_POLICY);
#else
  max = 0;
#endif /* defined(HAVE_SCHED_GET_PRIORITY_MAX) */

  d = max - min;
  r = (float)d / (float)npr;

  for ( i=0; i<npr; i++ ) {
	priorities[i] = (int)(i*r + 0.5) + min;
  }
}


/*
 * per-native thread init of semaphore
 */
static
void
tInitLock ( jthread_t nt )
{
  /* init a non-shared (process-exclusive) semaphore with value '0' */
  repsem_init( &nt->sem, 0, 0);
}

/*
 * We must have a certain amount of credible thread information setup
 * as soon as possible.
 */
static
void
tSetupFirstNative(void)
{
  jthread_t nt;

  /*
   * We need to have a native thread context available as soon as possible.
   */
  nt = thread_malloc( sizeof(struct _jthread));
  KGC_addRef(threadCollector, nt);
  nt->tid = pthread_self();
  pthread_setspecific( ntKey, nt);
  nt->stackMin  = (void*)0;
  nt->stackMax  = (void*)-1;
}

/**
 * The global, one-time initialization goes here. This is a
 * alternative to scattered pthread_once() calls
 */
void
jthread_init(int pre UNUSED,
        int maxpr, int minpr UNUSED,
	Collector *thread_collector,
	void (*destructor)(void *),
        void (*_onstop)(void) UNUSED,
        void (*_ondeadlock)(void) UNUSED)
{
  DBG(JTHREAD, dprintf("initialized\n"));

  threadCollector = thread_collector;
  threadDestructor = destructor;

  tInitSignals();

  pthread_key_create( &ntKey, NULL);
  repsem_init( &critSem, 0, 0);

  priorities = (int *)KGC_malloc (threadCollector, (maxpr+1) * sizeof(int), KGC_ALLOC_STATIC_THREADDATA);

  tMapPriorities(maxpr+1);
  tInitSignalHandlers();

  sigemptyset( &suspendSet);
  sigaddset( &suspendSet, sigResume);

  tSetupFirstNative();

  jthreadInitialized = true;

  DBG( JTHREAD, tStartDeadlockWatchdog() );
}

jthread_t
jthread_createfirst(size_t mainThreadStackSize, unsigned int pri UNUSED,
		    void* jlThread)
{
  jthread_t      nt;
  int            oldCancelType;

  nt = jthread_current();

  /* we can't use nt->attr, because it wasn't used to create this thread */
  pthread_attr_init( &nt->attr);

  nt->tid    = pthread_self();
  nt->data.jlThread = jlThread;
  nt->suspendState = 0;
  nt->active = 1;
  nt->func   = NULL;
  nt->next   = NULL;
  nt->daemon = false;
  pthread_mutex_init(&nt->suspendLock, NULL);

  /* Get stack boundaries. Note that this is just an approximation
   * which should cover all gc-relevant stack locations
   */
  detectStackBoundaries(nt, mainThreadStackSize);

  DBG( JTHREAD, TMSG_SHORT( "create first ", nt));

  /* init our cv and mux fields for locking */
  tInitLock( nt);

  /* Link native and Java thread objects
   * We already are executing in the right thread, so we can set the specific
   * data straight away
   */
  pthread_setspecific( ntKey, nt);
  pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &oldCancelType);
  
  /* if we aren't the first one, we are in trouble */
  assert( activeThreads == 0);
 
  activeThreads = firstThread = nt;
  nonDaemons=1;
  nSysThreads=1;

  return (nt);
}

/**
 * Interrupt a thread.
 * 
 * If tid is currently blocked its interrupted flag is set
 * and the blocking operation is canceled.
 */
void jthread_interrupt(jthread_t tid)
{
  pthread_mutex_lock(&tid->suspendLock);

  tid->interrupting = 1;

  if ((tid->blockState & (BS_CV|BS_CV_TO)) != 0)
    {
      pthread_cond_signal (&tid->data.sem.cv);
    }
  else if (tid->blockState == 0 || (tid->blockState & BS_SYSCALL) != 0)
    {
      /* We need to send some signal to interrupt syscalls. */
      pthread_kill(tid->tid, sigInterrupt);
    }

  pthread_mutex_unlock(&tid->suspendLock);
}

/**
 * Peek at the interrupted flag of a thread.
 *
 * @return true iff jt was interrupted.
 */
int jthread_is_interrupted(jthread_t jt)
{
  return jt->interrupting;
}

/**
 * Read and clear the interrupted flag of a thread.
 *
 * @return true iff jt was interrupted.
 */
int jthread_interrupted(jthread_t jt)
{
  int i = jt->interrupting;

  jt->interrupting = 0;
  return i;
}

bool jthread_attach_current_thread (bool isDaemon)
{
  jthread_t		nt;
  rlim_t		stackSize;

  /* create the jthread* thingy */
  nt = thread_malloc( sizeof(struct _jthread) );

  nt->func         = NULL;
  nt->suspendState = 0;
#if defined(KAFFEMD_STACKSIZE)
  stackSize = mdGetStackSize();
  
  if (stackSize == KAFFEMD_STACK_ERROR)
    {
      fprintf(stderr, "WARNING: Impossible to retrieve the real stack size\n");
      fprintf(stderr, "WARNING: You may experience deadlocks\n");
    }
  else if (stackSize == KAFFEMD_STACK_INFINITE)
    {
      fprintf(stderr, "WARNING: Kaffe may experience problems with unlimited\n"
	      "WARNING: stack sizes (e.g. deadlocks).\n");
      stackSize = MAINSTACKSIZE;
    }
#else
  stackSize = MAINSTACKSIZE;
#endif
  detectStackBoundaries(nt, stackSize);
  nt->stackCur     = NULL; 
  nt->daemon       = isDaemon;

  /* link everything together */
  nt->tid = pthread_self();
  pthread_setspecific( ntKey, nt);

  /* and done */
  return true;
}

bool jthread_detach_current_thread (void)
{
  jthread_t	cur = jthread_current ();

  tDispose (cur);

  return true;
}

/*
 * This is our thread function wrapper, which we need because of two
 * reasons. First, there is no way to set thread specific data from
 * outside the current thread (and it should be propperly set before we
 * enter the *real* thread func). Second, this is a better place to handle
 * the recycle looping than Texit (would be strange to loop in there)
 */
static
void* tRun ( void* p )
{
  jthread_t	cur = (jthread_t)p;
  jthread_t	t;
  size_t	ss;
  int		oldCancelType;

  /* get the stack boundaries */
  pthread_attr_getstacksize( &cur->attr, &ss);

#if defined(STACK_GROWS_UP)
  cur->stackMin = &cur;
  cur->stackMax = (void*) ((unsigned long)cur->stackMin + ss);
#else
  cur->stackMax = &cur;
  cur->stackMin = (void*) ((unsigned long)cur->stackMax - ss);
#endif

  pthread_setspecific( ntKey, cur);
  pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &oldCancelType);

  cur->tid = pthread_self();

  /* we are reasonably operational now, flag our creator that it's safe to give
   * up the thread lock */
  repsem_post( &cur->sem);

  while ( 1 ) {
	DBG( JTHREAD, TMSG_LONG( "calling user func of: ", cur));


	/* Now call our thread function, which happens to be firstStartThread(),
	 * which will call TExit before it returns */
	cur->func(cur->data.jlThread);

	DBG( JTHREAD, TMSG_LONG( "exiting user func of: ", cur));

	if (threadDestructor)
	  threadDestructor(cur->data.jlThread);

	protectThreadList(cur);

	/* remove from active list */
	if ( cur == activeThreads ){
	  activeThreads = cur->next;
	}
	else {
	  for ( t=activeThreads; t->next && (t->next != cur); t=t->next );
	  assert( t->next != 0 );
	  t->next = cur->next;
	}

	/* unlink Java and native thread */
	cur->data.jlThread = NULL;
	cur->suspendState = 0;

	/* link into cache list (if still within limit) */
	if ( ++nCached < MAX_CACHED_THREADS ){
	  cur->next = cache;
	  cache = cur;

	  DBG( JTHREAD, TMSG_SHORT( "cached thread ", cur));
	}

	pendingExits--;

	unprotectThreadList(cur);

	if ( nCached >= MAX_CACHED_THREADS ){
	  break;
	}

	if (cur->status == THREAD_KILL)
	  break;

	/* Wait until we get re-used (by TcreateThread). No need to update the
	 * blockState, since we aren't active anymore */
	repsem_wait( &cur->sem);
	if (cur->status == THREAD_KILL)
	  break;

	/*
	 * we have already been moved back to the activeThreads list by
	 * Tcreate (saves us a lock)
	 */
	DBG( JTHREAD, TMSG_SHORT( "reused thread ", cur));
  }

  tDispose( cur);

  return NULL;
}


/*
 * Create a new native thread for a given Java Thread object. Note
 * that we are called from Thread.start(), i.e. we are already allowed to
 * call func() (which happens to be threads.c:firstStartThread)
 *
 * Our call graph looks like this:
 *
 *     Thread.start
 *       startThread
 *         Kaffe_ThreadInterface.create (firstStartThread) == Tcreate
 *           pthread_create(..tRun..)
 *                                                 creator-thread
 *   --------------------------------------------------------------
 *                                                 created-thread
 *     tRun
 *       firstStartThread
 *         Thread.run
 *         exitThread
 *           Thread.finish
 *           Kaffe_ThreadInterface.exit == Texit
 */

jthread_t
jthread_create ( unsigned int pri, void* func, int isDaemon, void* jlThread, size_t threadStackSize )
{
  jthread_t		cur = jthread_current();
  jthread_t		nt;
#if defined(SCHEDULE_POLICY)
  struct sched_param	sp;
#endif

  /* if we are the first one, it's seriously broken */
  assert( activeThreads != 0 );

  /*
   * This is a safeguard to avoid creating too many new threads
   * because of a high Tcreate call frequency from a high priority
   * thread (which doesn't give exiters a chance to aquire the lock
   * to update the cache list).
   */
  if ( cache == 0 ) {
	while ( pendingExits && (cache == 0) )
	  sched_yield();
  }

#if defined(SCHEDULE_POLICY)
  sp.sched_priority = priorities[pri];
#endif

  protectThreadList(cur);
  if ( !isDaemon ) 
	nonDaemons++;
  unprotectThreadList(cur);

  if ( cache ) {
	protectThreadList(cur);

	/* move thread from the cache to the active list */
	nt = cache;
	cache = cache->next;
	nCached--;

	nt->next = activeThreads;
	activeThreads = nt;

	nt->data.jlThread = jlThread;
	nt->daemon = isDaemon;
	nt->func = func;
	nt->stackCur = NULL;
	nt->status = THREAD_RUNNING;

#if defined(SCHEDULE_POLICY)
	pthread_setschedparam( nt->tid, SCHEDULE_POLICY, &sp);
#endif

	DBG( JTHREAD, TMSG_SHORT( "create recycled ", nt));

	/* resurrect it */
	nt->active = 1;
	repsem_post( &nt->sem);

	unprotectThreadList(cur);
  }
  else {
	int creation_succeeded;

	nt = thread_malloc( sizeof(struct _jthread) );
	KGC_addRef(threadCollector, nt);

	pthread_attr_init( &nt->attr);
#if defined(SCHEDULE_POLICY)
	pthread_attr_setschedparam( &nt->attr, &sp);
#if defined(HAVE_PTHREAD_ATTR_SETSCHEDPOLICY)
	pthread_attr_setschedpolicy( &nt->attr, SCHEDULE_POLICY);
#endif /* defined(HAVE_PTHREAD_ATTR_SETSCHEDPOLICY) */
#endif /* defined(SCHEDULE_POLICY) */
	pthread_attr_setstacksize( &nt->attr, threadStackSize);

	nt->data.jlThread       = jlThread;
	nt->func         = func;
	nt->suspendState = 0;
	nt->stackMin     = NULL;
	nt->stackMax     = NULL;
	nt->stackCur     = NULL;
	nt->daemon	 = isDaemon;
	nt->status = THREAD_RUNNING;
	pthread_mutex_init(&nt->suspendLock, NULL);
	
	DBG( JTHREAD, TMSG_SHORT( "create new ", nt));

	/* init our cv and mux fields for locking */
	tInitLock( nt);

	/* Link the new one into the activeThreads list. We lock until
	 * the newly created thread is set up correctly (i.e. is walkable)
	 */
	protectThreadList(cur);
	nt->active = 1;
	nt->next = activeThreads;
	activeThreads = nt;

	/* Note that we don't directly start 'func' because we (a) still need to
	 * set the thread specifics, and (b) we need a looper for our thread
	 * recycling. We create the new thread while still holding the lock, because
	 * we otherwise might have a invalid tid in the activeList. The new thread
	 * in turn doesn't need the lock until it exits
	 */
	creation_succeeded = pthread_create( &nt->tid, &nt->attr, tRun, nt);

	/* If the creation of the new thread failed for some reason,
	 * print the reason, clean up and bail out.
	 */
	if (creation_succeeded != 0) {
	  switch(creation_succeeded) {
	  case EAGAIN: 
	    DBG( JTHREAD, dprintf( "too many threads (%d)\n", nSysThreads));
	    break;
	  case EINVAL:
	    DBG( JTHREAD, dprintf( "invalid value for nt.attr\n"));
	    break;
	  case EPERM:
	    DBG( JTHREAD, dprintf( "no permission to set scheduling\n"));
	    break;
	  default:
	    break;
	  }

	  repsem_destroy( &nt->sem);
	  KGC_rmRef(threadCollector, nt);
	  nt->active = 0;
	  activeThreads = nt->next;
	  unprotectThreadList(cur);
	  return NULL;
	}
	/* wait until the thread specific data has been set, and the new thread
	 * is in a suspendable state */
	repsem_wait( &nt->sem);

	/* The key is installed. We can now let the signals coming. */
	unprotectThreadList(cur);
  }
  return (nt);
}


/***********************************************************************
 * thread exit & cleanup
 */

/*
 * Native thread cleanup. This is just called in case a native thread
 * is not cached
 */
static
void tDispose ( jthread_t nt )
{
  /* We must lock the GC to prevent any garbage collection in this
   * function.
   */
  jthread_lockGC();

  /* Remove the static reference so the thread context may be freed. */
  KGC_rmRef(threadCollector, nt);

  KaffeVM_unlinkNativeAndJavaThread();

  pthread_detach( nt->tid);
  pthread_mutex_destroy (&nt->suspendLock);

  repsem_destroy( &nt->sem);

  /* The context is not freed explictly as it may cause troubles
   * with the locking system which is invoked by the GC in that case.
   * The thread context will be automatically freed by the GC in its 
   * thread context.
   */
  jthread_unlockGC();
}

/**
 * Function to be called (by threads.c firstStartThread) when the thread leaves
 * the user thread function. If the calling thread is the main thread then it
 * suspend the thread until all other threads has exited.
 */
void
jthread_exit ( void )
{
  jthread_t	cur = jthread_current();
  jthread_t	t;
  /*
   * We are leaving the thread user func, which means we are not
   * subject to GC, anymore (has to be marked here because the first thread
   * never goes through tRun)
   */
  cur->active = 0;

  DBG( JTHREAD, TMSG_SHORT( "exit ", cur));
  DBG( JTHREAD, dprintf("exit with %d non daemons (%x)\n", nonDaemons, cur->daemon));

  if ( !cur->daemon ) {
	/* the last non daemon should shut down the process */
	protectThreadList(cur);
	if ( --nonDaemons == 0 ) {
	  DBG( JTHREAD, dprintf("exit on last nonDaemon\n"));
	  if (runOnExit != NULL) {
	    unprotectThreadList(cur);
	    runOnExit();
	    protectThreadList(cur);
	  }

	  /*
	   * be a nice citizen, try to cancel all other threads before we
	   * bail out, to give them a chance to run their cleanup handlers
	   */
	  for ( t=cache; t != NULL; t = t->next ){
		t->status = THREAD_KILL;
		repsem_post(&t->sem);
	  }

	  t = activeThreads;
	  while (t != NULL) {
		/* We must not kill the current thread and the main thread
		 */
		if ( t != cur && t != firstThread && t->active) {
		  /* Mark the thread as to be killed. */
		  t->status = THREAD_KILL;
		  /* Send an interrupt event to the remote thread to wake up.
		   * This may not work in any cases. However that way we prevent a
		   * predictable deadlock on some threads implementation.
		   */
		  jthread_interrupt(t);
		  unprotectThreadList(cur);
		  pthread_join(t->tid, NULL);
		  protectThreadList(cur);

		  t = activeThreads;
		} else
		  t = t->next;
	  }

#if defined(KAFFE_VMDEBUG)
	  if ( deadlockWatchdog ){
		pthread_cancel( deadlockWatchdog);
	  }
#endif

	  if ( (cur != firstThread) && (firstThread->active == 0) ) {
		/* if the firstThread has already been frozen,
		 * it's not in the cache list. We must wake it up because
		 * this thread is the last one alive and it is exiting. */
		repsem_post (&firstThread->sem);
	  }

	  /* This is not the main thread so we may kill it. */
	  if (cur != firstThread)
	  {
	    unprotectThreadList(cur);
	    pthread_exit( NULL);

	    /* we shouldn't get here, this is a last safeguard */
	    KAFFEVM_EXIT(0);
	  }
	}
	unprotectThreadList(cur);
  }

  /*
   * We don't cache this one, but we have to remove it from the active list. Note
   * that the firstThread always has to be the last entry in the activeThreads list
   * (we just add new entries at the head)
   */
  protectThreadList(cur);
  if ( cur == firstThread && nonDaemons != 0) {

	/* if we would be the head, we would have been the last, too (and already exited) */
	assert( cur != activeThreads);

	for ( t=activeThreads; (t != NULL) && (t->next != cur); t=t->next );
	assert( t != NULL);
	t->next = NULL;

	unprotectThreadList(cur);

	/* Put the main thread in a frozen state waiting for the other
	 * real threads to terminate. The main thread gets the control back
	 * after that.
	 */
	while (repsem_wait( &cur->sem) != 0);
  }
  else if (cur != firstThread) {
	/* flag that we soon will get a new cache entry (would be annoying to
	 * create a new thread in the meantime) */
	pendingExits++;
	unprotectThreadList(cur);
  }
  else unprotectThreadList(cur);
}


/*
 * Thread is being finalized - free any held resource.
 */
void
jthread_destroy (jthread_t cur)
{
  DBG( JTHREAD, TMSG_SHORT( "finalize ", cur));
}


void 
jthread_sleep (jlong timeout) 
{
	struct timespec ts;

	ts.tv_sec  = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;

	nanosleep (&ts, NULL);
}

/***********************************************************************
 * scheduling and query
 */

/*
 * Change the scheduler priority of a running thread. Since we aren't
 * called directly, we can assume that 'prio' is within [MIN_PRIORITY..MAX_PRIORITY]
 */
#if defined(SCHEDUL_POLICY)
void
jthread_setpriority (jthread_t cur, jint prio)
{
  struct sched_param   sp;

  if ( cur ) {
	sp.sched_priority = priorities[prio];

	DBG( JTHREAD, dprintf("set priority: %p [tid: %4ld, java: %p) to %d (%d)\n",
			      cur, cur->tid, cur->data.jlThread, prio, priorities[prio]))
	pthread_setschedparam( cur->tid, SCHEDULE_POLICY, &sp);
  }
}
#else
void
jthread_setpriority (jthread_t cur UNUSED, jint prio UNUSED)
{
}
#endif

/**
 * yield.
 *
 */
void 
jthread_yield (void)
{
#if defined(HAVE_PTHREAD_YIELD)
  pthread_yield();
#elif defined(HAVE_SCHED_YIELD)
  sched_yield();
#endif
}


/******************************************************************************
 * the GC lock
 */

static pthread_mutex_t GClock = PTHREAD_MUTEX_INITIALIZER;

void jthread_lockGC(void)
{
  pthread_mutex_lock(&GClock);
}

void jthread_unlockGC(void)
{
  pthread_mutex_unlock(&GClock);
}

/*******************************************************************************
 * the suspend/resume mechanism
 */

/**
 * This function make the enters in deep suspend mode. It is generally called
 * suspend_signal_handler() and the locking mechanism when a thread has called
 * jthread_suspendall(). It temporarily changes the state of the signal mask
 * for the current thread.
 *
 * @param releaseMutex If true, the function is requested to change the signal
 * mask and to release the suspendLock mutex only after this. In that case we
 * keep in sync with jthread_suspendall()
 */
void KaffePThread_WaitForResume(int releaseMutex, unsigned int state)
{
  volatile jthread_t cur = jthread_current();
  int s;
  sigset_t oldset;

  if (releaseMutex)
    {
      pthread_sigmask(SIG_BLOCK, &suspendSet, &oldset);
      pthread_mutex_unlock(&cur->suspendLock);
      /*
       * In that particular case we have to release the mutex on the thread list
       * because it may cause deadlocks in the GC thread. It does not hurt as we
       * do not access the thread list but we will have to reacquire it before
       * returning.
       */
      if (cur->blockState & BS_THREAD)
	      pthread_mutex_unlock(&activeThreadsLock);
    }

  /* freeze until we get a subsequent sigResume */
  while( cur->suspendState == SS_SUSPENDED )
    {
      sigwait( &suspendSet, &s);
      /* Post something even if it is not the right thread. */
      if (cur->suspendState == SS_SUSPENDED)
        repsem_post(&critSem);
    }
  
  DBG( JTHREADDETAIL, dprintf("sigwait return: %p\n", cur));

  /* If the thread needs to be put back in a block state
   * we must not reset the stack pointer.
   */
  if (state == 0)
    cur->stackCur     = NULL;
  cur->suspendState = 0;
  cur->blockState |= state;
  
  /* notify the critSect owner we are leaving the handler */
  repsem_post( &critSem);

  if (releaseMutex)
    {
      if (cur->blockState & BS_THREAD)
	pthread_mutex_lock(&activeThreadsLock);
      pthread_sigmask(SIG_SETMASK, &oldset, NULL);
    }
}

/**
 * The suspend signal handler, which we need to implement critical sections.
 * It is used for two purposes: (a) to block all active threads which might
 * get rescheduled during a critical section (we can't rely on priority-fifo
 * scheduling, it might be SCHED_OTHER), and (b) to get a GC-limit of the current
 * thread stack so that we don't have to scan the whole thing. Since this
 * assumes that we execute on the threads normal stack, make sure we have no
 * SA_ONSTACK / signalstack() in effect
 */
void
suspend_signal_handler (int sig UNUSED)
{
  volatile jthread_t   cur = jthread_current();

  DBG( JTHREAD, dprintf("suspend signal handler: %p\n", cur));

  /* signals are global things and might come from everywhere, anytime */
  if ( !cur || !cur->active )
	return;

  KaffePThread_AckAndWaitForResume(cur, 0);
}

void KaffePThread_AckAndWaitForResume(volatile jthread_t cur, unsigned int state)
{
  if ( cur->suspendState == SS_PENDING_SUSPEND ){
    JTHREAD_JMPBUF env;
    
    /*
     * Note: We're not gonna do a longjmp to this place, we just want
     * to do something that will save all of the registers onto the stack.
     */
    JTHREAD_SETJMP(env);
    
    /* assuming we are executing on the thread stack, we record our current pos */
    cur->stackCur     = (void*)&env;
    cur->suspendState = SS_SUSPENDED;
    cur->blockState  &= ~state;
    
    /* notify the critSect owner that we are now suspending in the handler */
    repsem_post( &critSem);
    
    KaffePThread_WaitForResume(false, state);
  }
}


/**
 * The resume signal handler, which we mainly need to get the implicit sigreturn
 * call (i.e. to unblock a preceeding sigwait).
 */
void
resume_signal_handler ( int sig UNUSED )
{
  /* we don't do anything, here - all the action is in the suspend handler */
}

/**
 * Temporarily suspend all threads but the current one. This is
 * a dangerous operation, but it is more safe than to rely on
 * fixed priority scheduling (which most desktop OSes don't provide).
 * Make sure no suspended thread blocks on a resource (mutexed non-reentrant
 * lib func) we use within the critical section, or we have a classical deadlock.
 * Critical section can be nested, but that's usually not a good idea (increases
 * chances for deadlocks or trojan threads)
 */
void
jthread_suspendall (void)
{
  int		status;
  jthread_t	cur = jthread_current();
  volatile jthread_t	t;

  if (!jthreadInitialized)
    return;
 
  /* don't allow any new thread to be created or recycled until this is done */
  protectThreadList(cur);

  DBG( JTHREAD, dprintf("enter crit section[%d] from: %p [tid:%4ld, java:%p)\n",
			critSection, cur, (unsigned long)cur->tid,
			cur->data.jlThread));

  if ( ++critSection == 1 ){

#if !defined(KAFFE_BOEHM_GC)
	int val;
	int numPending = 0;

	repsem_getvalue(&critSem, &val);
	assert(val == 0);

	for ( t=activeThreads; t; t = t->next ){
	  /*
	   * make sure we don't suspend ourselves, and we don't expect suspend
	   * signals handled by threads which are blocked on someting else
	   * than the thread lock (which we soon release)
	   */
	  pthread_mutex_lock(&t->suspendLock);
	  if ( (t != cur) && (t->suspendState == 0) && (t->active != 0) ) {
		DBG( JTHREAD, dprintf("signal suspend: %p (susp: %d blk: %d)\n",
				      t, t->suspendState, t->blockState));

		t->suspendState = SS_PENDING_SUSPEND;

		if ((t->blockState & (BS_SYSCALL|BS_CV|BS_MUTEX|BS_CV_TO)) != 0)
		  {
		    /* The thread is already stopped.
		     */
		    assert(t->stackCur != NULL);
		    t->suspendState = SS_SUSPENDED;
		  }
		else
		  {
		    if ((status = pthread_kill( t->tid, sigSuspend)) != 0)
		      {
			dprintf("Internal error: error sending SUSPEND signal to %p: %d (%s)\n", t, status, strerror(status));
			KAFFEVM_ABORT();
		      }
		    else
		      {
			/* BAD: Empirical workaround for lost signals (with accumulative syncing)
			 * It shouldn't be necessary (posix sems are accumulative), and it
			 * is bad, performancewise (at least n*2 context switches per suspend)
			 * but it works more reliably on linux 2.2.x */
			numPending++;
		      }
		  }
	  }
	  pthread_mutex_unlock(&t->suspendLock);
	}

	/* Now that all signals has been sent we may wait for all concerned
	 * threads to handle them.
	 */
	while (numPending > 0)
	  {
	    repsem_wait( &critSem);
	    numPending--;
	  }

#else
	/*
	 * Here, we must use the special Boehm's stop world routine.
	 * However we continue to update our own thread state flag.
	 */
	GC_stop_world();

	for ( t=activeThreads; t; t = t->next ) {
	  if ( (t != cur) && (t->suspendState == 0) && (t->active) ) {
		t->suspendState = SS_PENDING_SUSPEND;
	  }
	}
#endif
  }

  unprotectThreadList(cur);

  DBG( JTHREAD, dprintf("critical section (%d) established\n", critSection));
}


/**
 * Resume all temporarily suspended threads. Just take action if this
 * is the outmost exit
 */
void
jthread_unsuspendall (void)
{
  jthread_t	cur = jthread_current();
  jthread_t	t;
  int		status;

  if ( !jthreadInitialized || !critSection )
	return;

  if ( --critSection == 0 ){
	  int val;
	/* No need to sync, there's nobody else running. However it seems
	 * we cannot use mutexes as they cause a deadlock when the world
	 * is suspended.
	 */
	protectThreadList(cur);
	repsem_getvalue(&critSem, &val);
	assert(val == 0);

#if !defined(KAFFE_BOEHM_GC)
	for ( t=activeThreads; t; t = t->next ){
	  pthread_mutex_lock(&t->suspendLock);
	  if ( (t->suspendState & (SS_PENDING_SUSPEND | SS_SUSPENDED)) != 0 )
	    {
	      
	      DBG( JTHREAD, dprintf("signal resume: %p (sus: %d blk: %d)\n",
				    t, t->suspendState, t->blockState));

	      t->suspendState = SS_PENDING_RESUME;
	      if ((t->blockState & (BS_SYSCALL|BS_CV|BS_CV_TO|BS_MUTEX)) == 0)
		{
		  DBG (JTHREADDETAIL, dprintf("  sending sigResume\n"));
		  do
		    {
		      status = pthread_kill( t->tid, sigResume);
		      if ( status )
			{
			  DBG( JTHREAD, dprintf("error sending RESUME signal to %p: %d\n", t, status));
			}		  
		      /* ack wait workaround, see jthread_suspendall remarks */
		      repsem_wait( &critSem);
		    }
		  while (t->suspendState == SS_PENDING_RESUME);
		}
	      else
		{
		  DBG (JTHREADDETAIL, dprintf("  clearing suspendState\n"));
		  t->suspendState = 0;
		}
	    }
	  pthread_mutex_unlock(&t->suspendLock);
	}

#else
	for ( t=activeThreads; t; t = t->next ){
	  if ( t->suspendState & (SS_PENDING_SUSPEND | SS_SUSPENDED) ){
		t->suspendState = 0;
	  }
	}

	GC_start_world();

#endif
	repsem_getvalue(&critSem, &val);
	assert(val == 0);
	  unprotectThreadList(cur);

  }

  DBG( JTHREAD, dprintf("exit crit section (%d)\n", critSection));
}


/*******************************************************************************
 * GC related stuff
 */

/**
 * Walk stacks of all threads, except of the current one (doesn't
 * make much sense since that's the GC itself, and it's task is to
 * get rid of garbage, not to pin it down - besides the fact that
 * its stack is a moving target).
 *
 * The call chain looks like this
 *
 *  startGC
 *    Kaffe_ThreadInterface.GcWalkThreads == TwalkThreads
 *      walkMemory
 *        gcFunctions[..].walk == walkThread
 *
 */
void
jthread_walkLiveThreads (void(*func)(jthread_t,void*), void *private)
{
  jthread_t t;

  DBG( JTHREAD, dprintf("start walking threads\n"));

  for ( t = activeThreads; t != NULL; t = t->next) {
	func(t, private);
  }

  DBG( JTHREAD, dprintf("end walking threads\n"));
}

void
jthread_walkLiveThreads_r (void(*func)(jthread_t, void *), void *private)
{
  jthread_t cur = jthread_current();

  protectThreadList(cur);
  jthread_walkLiveThreads (func, private);
  unprotectThreadList(cur);
}

int
jthread_is_blocking (int fd)
{
  int r;
 
  r = fcntl(fd, F_GETFL, 0);
  if (r < 0) {
    perror("fcntl(F_GETFL)");
    return 0;
  }
		  
  return (r & O_NONBLOCK) != 0;
}

void
jthread_set_blocking (int fd, int blocking)
{
  int r;
  /* This code has been copied from jthreadedFileDescriptor in
     unix-jthreads/jthread.c
  */
  if (!blocking) {
    /* Make non-blocking */
    if ((r = fcntl(fd, F_GETFL, 0)) < 0) {
      perror("F_GETFL");
      return;
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
  } else {
    /* clear nonblocking flag */
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);
  }
}

/*
 * Get the current stack limit.
 * Adapted from kaffe/kaffevm/systems/unix-jthreads/jthread.h
 */

/**
 * This function increments the pointer stored in "p" by "inc" bytes.
 * 
 * @param p Pointer to be incremented.
 * @param inc Amount of bytes to increment the pointer.
 */
static inline void incrPointer(void **p, int inc)
{
  *p = (void *)((uintp)*p + inc);
}


/**
 * This function is meant to relax stack boundaries so the virtual machine
 * may execute some task before the final abort.
 *
 * @param yes If it is true, it relaxes the boundaries. If false, it shortens
 * the stack.
 */
void jthread_relaxstack(int yes)
{
  if( yes )
    {
#if defined(STACK_GROWS_UP)
      incrPointer(&jthread_current()->stackMax, STACKREDZONE);
#else
      incrPointer(&jthread_current()->stackMin, -STACKREDZONE);
#endif
    }
  else
    {
#if defined(STACK_GROWS_UP)
      incrPointer(&jthread_current()->stackMax, -STACKREDZONE);
#else
      incrPointer(&jthread_current()->stackMin, STACKREDZONE);
#endif
    }
}

/**
 * Test whether an address is on the stack of the calling thread.
 *
 * @param p the address to check
 *
 * @return true if address is on the stack
 *
 * Needed for locking and for exception handling.
 */
bool jthread_on_current_stack(void* p)
{
  jthread_t nt = jthread_current();

DBG(JTHREADDETAIL, dprintf("on current stack: base=%p size=%ld bp=%p",
                        nt->stackMin,
                        (long)((char *)nt->stackMax - (char *)nt->stackMin),
                        p); );

  if (nt == 0 || (p > nt->stackMin && p < nt->stackMax)) {
DBG(JTHREADDETAIL, dprintf(" yes\n"); );
        return (true);
  }
  else {
DBG(JTHREADDETAIL, dprintf(" no\n"); );
        return (false);
  }
}


/**
 * Lock a mutex which will be used for critical sections when entering
 * non-reentrant system code, for example.
 *
 * @param dummy unused pointer
 */
void jthread_spinon(void *dummy UNUSED)
{
  pthread_mutex_lock(&systemMutex);
}

/**
 * Unock a mutex used for critical sections when entering non-reentrant system
 * code.
 *
 * @param dummy unused pointer
 */
void jthread_spinoff(void *dummy UNUSED)
{
  pthread_mutex_unlock(&systemMutex);
}

/**
 * Sets a function to be run when all non-daemon threads have exited.
 *
 * @param func the function to be called when exiting.
 */
void jthread_atexit(void (* func)(void))
{
  runOnExit = func;
}

int KaffePThread_getSuspendSignal(void)
{
  return sigSuspend;
}

/**
 * Extract the range of the stack that's in use.
 * 
 * @param tid the thread whose stack is to be examined
 * @param from storage for the address of the start address
 * @param len storage for the size of the used range
 *
 * @return true if successful, otherwise false
 *
 * Needed by the garbage collector.
 */

bool jthread_extract_stack(jthread_t tid, void** from, unsigned* len)
{
  if (tid->active == 0) {
    return false;
  }
  assert(tid->suspendState == SS_SUSPENDED);
#if defined(STACK_GROWS_UP)
  *from = tid->stackMin;
  *len = (uintp)tid->stackCur - (uintp)tid->stackMin;
#else
  *from = tid->stackCur;
  *len = (uintp)tid->stackMax - (uintp)tid->stackCur;
#endif
  return true;
}

/**
 * Returns the current native thread.
 *
 */
jthread_t jthread_current(void)      
{
  if (!jthreadInitialized)
    return NULL;

  return (jthread_t)pthread_getspecific(ntKey);
}

/**
 * Disable stopping the calling thread.
 *
 * Needed to avoid stopping a thread while it holds a lock.
 */
void jthread_disable_stop(void)
{
}

/**
 * Enable stopping the calling thread.
 *
 * Needed to avoid stopping a thread while it holds a lock.
 */
void jthread_enable_stop(void)
{
}

/** 
 * Stop a thread.
 * 
 * @param tid the thread to stop.
 */
void jthread_stop(jthread_t tid UNUSED)
{
}

/**
 * Dump some information about a thread to stderr.
 *
 * @param tid the thread whose info is to be dumped.
 */
void jthread_dumpthreadinfo(jthread_t tid UNUSED)
{
}

/**
 * Return the java.lang.Thread instance attached to a thread
 *
 * @param tid the native thread whose corresponding java thread
 *            is to be returned.
 * @return the java.lang.Thread instance.
 */
threadData *jthread_get_data(jthread_t tid)
{
        return (&tid->data);
}

/**
 * Check for room on stack.
 *
 * @param left number of bytes that are needed
 *
 * @return true if @left bytes are free, otherwise false 
 *
 * Needed by intrp in order to implement stack overflow checking.
 */
bool jthread_stackcheck(int left)
{
	int rc;
#if defined(STACK_GROWS_UP)
        rc = jthread_on_current_stack((char*)&rc + left);
#else
        rc = jthread_on_current_stack((char*)&rc - left);
#endif
	return (rc);
}

/**
 * Returns the upper bound of the stack of the calling thread.
 *
 * Needed by support.c in order to implement stack overflow checking. 
 */
void* jthread_stacklimit(void)
{
  jthread_t nt = jthread_current();
#if defined(STACK_GROWS_UP)
  return (void *)((uintp)nt->stackMax - STACKREDZONE);
#else
  return (void *)((uintp)nt->stackMin + STACKREDZONE);
#endif
}

int jthread_on_condvar(jthread_t jt)
{
	return (jt->blockState & (BS_CV|BS_CV_TO)) != 0;
}

int jthread_on_mutex(jthread_t jt)
{
	return (jt->blockState & (BS_MUTEX)) != 0;
}

int jthread_get_status(jthread_t jt)
{
  if (jt->status == THREAD_KILL)
    return THREAD_DEAD;

  return jt->status;
}

int jthread_has_run(jthread_t jt UNUSED)
{
  return 1;
}

void jthread_clear_run(jthread_t jt UNUSED)
{
}

void jthread_suspend(jthread_t jt UNUSED, void *suspender UNUSED)
{
	/* TODO */
}

void jthread_resume(jthread_t jt UNUSED, void *suspender UNUSED)
{
	/* TODO */
}

jthread_t jthread_from_data(threadData *td, void *suspender UNUSED)
{
  jthread_t cur = jthread_current();
  jthread_t iterator;

  protectThreadList(cur);
  iterator = activeThreads;
  while (iterator != NULL)
    {
      if (td == &iterator->data)
	{
	  unprotectThreadList(cur);
	  /* Thread handles are garbage collected so the stack is protecting
	   * us.
	   */
	  return iterator;
	}
      iterator = iterator->next;
    }

  unprotectThreadList(cur);
  return NULL;
}

jlong jthread_get_usage(jthread_t jt UNUSED)
{
	/* TODO */
	return 0;
}
