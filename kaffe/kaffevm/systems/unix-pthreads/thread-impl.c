/*
 * thread-impl.c - pthread based ThreadInterface implementation
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

#include "config.h"
#include "config-std.h"
#include "config-signal.h"

#include "java_lang_Throwable.h"
#include "gtypes.h"
#include "thread.h"
#include "gc.h"
#include "jni.h"
#include "locks.h"

#define	DBG(X,Y)

/* these are required for handling exceptions */
#include "exception.h"

#if defined(INTERPRETER)
#define DEFINEFRAME()           /* Does nothing */
#define EXCEPTIONPROTO          int sig
#define EXCEPTIONFRAME(f, c)    /* Does nothing */
#define EXCEPTIONFRAMEPTR       0
#elif defined(TRANSLATOR)
#define DEFINEFRAME()           exceptionFrame frame
#define EXCEPTIONFRAMEPTR       &frame
#endif /* TRANSLATOR */

/* Some systems need special setups for the exception handling */
#if !defined(EXCEPTIONSTART)
#define EXCEPTIONSTART()
#endif
#if !defined(EXCEPTIONEND)
#define EXCEPTIONEND()
#endif

#if defined(DEBUG)
char stat_act[]   = { ' ', 'a' };
char stat_susp[]  = { ' ', 's', ' ', 'r', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
char stat_block[] = { ' ', 'T', 'm', ' ', 'c', ' ', ' ', ' ', 't', ' ', ' ' };
#endif


/***********************************************************************
 * typedefs & defines
 */

#include <bits/local_lim.h>

/*
 * This is the configurable section. Note that SCHED_FIFO is the only
 * schedule policy which conforms to the "old" Java thread model (with
 * stringent priorities), but it usually isn't available on desktop
 * OSes (or imposes certain restrictions, e.g. root privileges).
 */
#define SCHEDULE_POLICY     SCHED_OTHER

/* our upper create limit, to ensure we don't blow the system */
#define MAX_SYS_THREADS     _POSIX_THREAD_THREADS_MAX - 1

/* our upper limit for cached threads (0 = no caching at all) */
#define MAX_CACHED_THREADS  MAX_SYS_THREADS - 3

/*
 * Now it starts to get hackish - we have to pick some signals
 * for suspend/resume (enter/exitCritSect) which don't interfere
 * with pthread implementations. Note that we can't rely on when
 * a suspend signal is delivered, and it's therefore not safe
 * to mulitplex a sinle signal for both suspend & resume purposes
 */
#if !defined(__SIGRTMIN) || (__SIGRTMAX - __SIGRTMIN < 3)
#define          SIG_SUSPEND   SIGURG
#define          SIG_RESUME    SIGTSTP
#define          SIG_DUMP      SIGXCPU

/*
 * Sneak these signal in from the thread library.
 */
#define		 PSIG_RESTART	SIGUSR1
#define		 PSIG_CANCEL	SIGUSR2

#else

#define          SIG_SUSPEND   SIGUSR1
#define          SIG_RESUME    SIGUSR2
#define          SIG_DUMP      SIGXCPU

/*
 * Sneak these signal in from the thread library.
 */
#define		 PSIG_RESTART	(__SIGRTMIN)
#define		 PSIG_CANCEL	(__SIGRTMIN+1)

#endif


/***********************************************************************
 * global data
 */

/* We keep a list of all active threads, so that we can enumerate them */
nativeThread     *activeThreads;

/* We don't throw away threads when their user func terminates, but suspend
 * and cache them for later re-use */
nativeThread     *cache;

/* The notorious first thread, which has to be handled differently because
 * it isn't created explicitly */
nativeThread     *firstThread;

/* Number of active non-daemon threads (the last terminating nonDaemon
 * causes the process to shut down */
int              nonDaemons;

/* Number of system threads (either running (activeThreads) or
 * blocked (cache). We need this to implement our own barrier, since
 * many kernel thread systems don't behave graceful on exceeding their limit */ 
int              nSysThreads;

/* number of currently cached threads */
int              nCached;

/* map the Java priority levels to whatever the pthreads impl gives us */
int              priorities[java_lang_Thread_MAX_PRIORITY]; 

/* thread-specific-data key to retrieve 'nativeData' */
pthread_key_t    ntKey;

/* our lock to protect list manipulation/iteration */
static iLock*    tLock;

/* a hint to avoid unnecessary pthread_creates (with pending exits) */
volatile int     pendingExits;

/* level of critical sections (0 = none) */
int              critSection;

/* helper semaphore to signal completion of critical section enter/exit */
sem_t            critSem;

sigset_t         suspendSet;

/* an optional deadlock watchdog thread (not in the activeThread list),
 * activated by DEBUG topic vm_thread */
pthread_t        deadlockWatchdog;


void suspend_signal_handler ( int sig );
void resume_signal_handler ( int sig );
static void tDispose ( nativeThread* nt );

static void* (*thread_malloc)(size_t);
static void (*thread_free)(void*);

extern void nullException(int);

#define LOCKSLOT  int iLockRoot
#define TLOCK(_nt) do {            \
  (_nt)->blockState |= BS_THREAD;  \
  lockStaticMutex(&tLock);         \
} while (0)

#define TUNLOCK(_nt) do {          \
  unlockStaticMutex(&tLock);       \
  (_nt)->blockState &= ~BS_THREAD; \
} while (0)


/***********************************************************************
 * internal functions
 */

/*
 * On demand debug signal to dump the current thread state(s) (requested
 * by a external "kill -s <SIG_DUMP> <proc-id>"
 */
void
dump_signal_handler ( int sig ) 
{
  tDump();
}

/*
 * dump a thread list, marking the supposed to be current thread
 */
void
tDumpList ( nativeThread *cur, nativeThread* list )
{
  int             i;
  char            a1, a2, a3;
  nativeThread    *t;

  for ( t=list, i=0; t; t=t->next, i++ ){
	/* the supposed to be current thread? */
	a1 = (t == cur) ? '*' : ' ';
	/* the current thread from a stack point-of view? */
	a2 = (((uintp)&i > (uintp)t->stackMin) &&
		  ((uintp)&i < (uintp)t->stackMax)) ? 'S' : ' ';
	/* the first one? */
	a3 = (t == firstThread) ? '1' : ' ';

	DBG( vm_thread, ("%4d: %c%c%c %c%c%c   %p [tid: %4d, java: %p]  "
					 "lock: [mux: %p cv: %p] stack: [%p..%p..%p]\n",
					 i, a1, a2, a3,
					 stat_act[t->active], stat_susp[t->suspendState], stat_block[t->blockState],
					 t, t->tid, t->thread,
					 &t->mux, &t->cv,
					 t->stackMin, t->stackCur, t->stackMax));
  }
}

/*
 * dump the state of the threading system
 */
void
tDump (void)
{
  DBG_ACTION( vm_thread, {
	nativeThread     *cur = pthread_getspecific( ntKey);
	void             *lock   = tLock;
	void             *holder = tLock->holder;
	void             *mux    = tLock->mux;
	void             *muxNat = tLock->mux ? unhand(tLock->mux)->PrivateInfo : 0;
	void             *cv     = tLock->cv;
	void             *cvNat  = tLock->cv ? unhand(tLock->cv)->PrivateInfo : 0;
  
	TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */
	
	DBG( vm_thread,("\n======================== thread dump =========================\n"));

	DBG( vm_thread, ("state:  nonDaemons: %d, critSection: %d\n",
					 nonDaemons, critSection));

	DBG( vm_thread, ("tLock:       %p [holder: %p, mux: %p (native: %p), cv: %p (native: %p)]\n",
					 lock, holder, mux, muxNat, cv, cvNat));

	DBG( vm_thread, ("active threads:\n"));
	tDumpList( cur, activeThreads);

	DBG( vm_thread, ("\ncached threads:\n"));
	tDumpList( cur, cache);

	DBG( vm_thread, ("====================== end thread dump =======================\n"));
	
	TUNLOCK( cur); /* ------------------------------------------------------ tLock */
  });
}


static
void*
tWatchdogRun (void* p)
{
  nativeThread *t;
  int life;

  while ( nonDaemons ) {
	life = 0;
	for ( t=activeThreads; t != NULL; t = t->next ){
	  /*
	   * if we have a single thread that is not blocked at all, is in a
	   * timeout wait, and is not suspended, we are still safe (even though
	   * the timeout value might effectively be a deadlock)
	   */
	  if ( (!t->blockState || (t->blockState == BS_CV_TO)) && !t->suspendState ){
		life = 1;
		break;
	  }
	}

	if ( !life ) {
	  DBG( vm_thread, ("deadlock\n"));
	  tDump();
	  ABORT();
	}

	usleep( 5000);
  }

  return 0;
}

void
tStartDeadlockWatchdog (void)
{
  pthread_attr_t attr;
  struct sched_param sp;

  sp.sched_priority = priorities[0];  /* looow */

  pthread_attr_init( &attr);
  pthread_attr_setschedparam( &attr, &sp);
  pthread_attr_setstacksize( &attr, 4096);

  pthread_create( &deadlockWatchdog, &attr, tWatchdogRun, 0);
}



/***********************************************************************
 * thread system initialization
 */

/*
 * static init of signal handlers
 */
void
tInitSignalHandlers (void)
{
  struct sigaction sigSuspend, sigResume, sigSegv, sigDump;
  unsigned int flags = 0;

#if defined(SA_RESTART)
  flags |= SA_RESTART;
#endif

  sigSuspend.sa_flags = flags;
  sigSuspend.sa_handler = suspend_signal_handler;
  sigemptyset( &sigSuspend.sa_mask);
  sigaddset( &sigSuspend.sa_mask, SIG_SUSPEND);
  sigaddset( &sigSuspend.sa_mask, SIG_RESUME);
  sigaddset( &sigSuspend.sa_mask, PSIG_RESTART);
  sigaddset( &sigSuspend.sa_mask, PSIG_CANCEL);

  sigaddset( &sigSuspend.sa_mask, SIGSTOP);
  sigaddset( &sigSuspend.sa_mask, SIGCONT);
  sigaddset( &sigSuspend.sa_mask, SIGWINCH);

  sigaction( SIG_SUSPEND, &sigSuspend, NULL);

  sigResume.sa_flags = flags;
  sigResume.sa_handler = resume_signal_handler;
  sigResume.sa_mask = sigSuspend.sa_mask;
  sigaction( SIG_RESUME, &sigResume, NULL);

#if defined(SIG_DUMP)
  sigDump.sa_flags = flags;
  sigDump.sa_handler = dump_signal_handler;
  sigemptyset( &sigDump.sa_mask);
  sigaction( SIG_DUMP, &sigDump, NULL);
#endif

  sigSegv.sa_flags = flags;
#if defined(SA_SIGINFO)
  sigSegv.sa_flags |= SA_SIGINFO;
#endif
#if defined(SA_NOMASK)
  sigSegv.sa_flags |= SA_NOMASK;
#endif
  sigSegv.sa_handler = (void(*)(int)) nullException;
  sigemptyset( &sigSegv.sa_mask);
  sigaction( SIGSEGV, &sigSegv, NULL);
}

/*
 * static init set up of Java-to-pthread priority mapping (pthread prioritiy levels
 * are implementation dependent)
 */
static
void
tMapPriorities (void)
{
  int     d, min, max, n, i;
  float   r;

  min = sched_get_priority_min( SCHEDULE_POLICY);
  max = sched_get_priority_max( SCHEDULE_POLICY);

  d = max - min;
  n = sizeof( priorities);
  r = (float)d / (float)n;

  for ( i=0; i<n; i++ ) {
	priorities[i] = (int)(i*r + 0.5) + min;
  }
}


/*
 * per-native thread init of condvars, mutexes and semaphores
 */
static
void
tInitLock ( nativeThread* nt )
{
  pthread_condattr_t    cvAttr;
  pthread_mutexattr_t   muxAttr;

  /* init a process private condvar */
  pthread_condattr_init( &cvAttr);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  pthread_condattr_setpshared( &cvAttr, PTHREAD_PROCESS_PRIVATE);
#endif
  pthread_cond_init( &nt->cv, &cvAttr);

  /* init a process private mutex. We deal with priority inversion
   * by means of inheriting the highest priority of any thread who
   * requests the mutex
   */
  pthread_mutexattr_init( &muxAttr);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  pthread_mutexattr_setpshared( &muxAttr, PTHREAD_PROCESS_PRIVATE);
#endif
#if defined(_POSIX_THREAD_PRIO_INHERIT_POSIX_THREAD_PRIO_PROTECT)
  pthread_mutexattr_setprotocol( &muxAttr, PTHREAD_PRIO_INHERIT);
#endif
  pthread_mutex_init( &nt->mux, &muxAttr);

  /* init a non-shared (process-exclusive) semaphore with value '0' */
  sem_init( &nt->sem, 0, 0);
}

/*
 * init the Java thread locking facilities (sem) from the native thread
 * condvar and mutex
 */
static
void
tSetLock ( nativeThread* nt )
{
  sem2posixLock *lk;

  lk = (sem2posixLock*)unhand(nt->thread)->sem;
  lk->cv  = &nt->cv;
  lk->mux = &nt->mux;
  lk->thd = nt;
}

/*
 * We must have a certain amount of credible thread information setup
 * as soon as possible.
 */
static
void
tSetupFirstNative(void)
{
  nativeThread* nt;

  /*
   * We need to have a native thread context available as soon as possible.
   */
  nt = thread_malloc( sizeof(nativeThread));
  nt->tid = pthread_self();
  pthread_setspecific( ntKey, nt);
  nt->stackMin  = (void*)0;
  nt->stackMax  = (void*)-1;
}

/*
 * The global, one-time initialization goes here. This is a
 * alternative to scattered pthread_once() calls
 */
void
jthread_init(int pre,
        int maxpr, int minpr,
        void *(*_allocator)(size_t),
        void (*_deallocator)(void*),
        void (*_destructor1)(void*),
        void (*_onstop)(void),
        void (*_ondeadlock)(void))
{
  DBG( vm_thread, ("initialized\n"));

  thread_malloc = _allocator;
  thread_free = _deallocator;

  pthread_key_create( &ntKey, NULL);
  sem_init( &critSem, 0, 0);

  tMapPriorities();
  tInitSignalHandlers();

  sigfillset( &suspendSet);
  sigdelset( &suspendSet, SIG_RESUME);

  tSetupFirstNative();

  DBG_ACTION( vm_thread, { tStartDeadlockWatchdog(); });
}

nativeThread*
jthread_createfirst(size_t mainThreadStackSize, unsigned char pri, void* jlThread)
{
  Hjava_lang_Thread* thread;
  nativeThread* nt;
  int            oldCancelType;

  thread = (Hjava_lang_Thread*)jlThread;
  nt = GET_CURRENT_THREAD( &nt );

  /* we can't use nt->attr, because it wasn't used to create this thread */
  pthread_attr_init( &nt->attr);

  nt->tid    = pthread_self();
  nt->thread = thread;
  nt->suspendState = 0;
  nt->active = 1;
  nt->func   = NULL;
  nt->next   = NULL;

  /* Get stack boundaries. Note that this is just an approximation
   * which should cover all gc-relevant stack locations
   */
#if defined(STACK_GROWS_UP)
  nt->stackMin  = (void*) ((uintp)&nt & -0x1000)
  nt->stackMax  = (void*) ((uintp) nt->stackMin + mainThreadStackSize);
#else
  nt->stackMax  = (void*) (((uintp)&nt + 0x1000 - 1) & -0x1000);
  nt->stackMin  = (void*) ((uintp) nt->stackMax - mainThreadStackSize);
#endif

  DBG( vm_thread, TMSG_SHORT( "create first ", nt));

  /* init our cv and mux fields for locking */
  tInitLock( nt);
  tSetLock( nt);

  /* Link native and Java thread objects
   * We already are executing in the right thread, so we can set the specific
   * data straight away
   */
  pthread_setspecific( ntKey, nt); 
  unhand(thread)->PrivateInfo = (struct Hkaffe_util_Ptr*)nt;
  pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &oldCancelType);
  
  /* if we aren't the first one, we are in trouble */
  assert( activeThreads == 0);
  activeThreads = firstThread = nt;
  nonDaemons=1;
  nSysThreads=1;

  return (nt);
}


/*
 * This is our thread function wrapper, which we need because of two
 * reasons. First, there is no way to set thread specific data from
 * outside the current thread (and it should be propperly set before we
 * enter the *real* thread func). Second, this is a better place to handle
 * the recycle looping than Texit (would be strange to loop in there)
 */
static
void*
tRun ( void* p )
{
  nativeThread     *cur = (nativeThread*)p;
  nativeThread     *t;
  size_t           ss;
  int              oldCancelType;
  int		   iLockRoot;

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
  sem_post( &cur->sem);

  while ( 1 ) {
	DBG( vm_thread, TMSG_LONG( "calling user func of: ", cur));

	/* Now call our thread function, which happens to be firstStartThread(),
	 * which will call TExit before it returns */
	cur->func(cur->thread);

	DBG( vm_thread, TMSG_LONG( "exiting user func of: ", cur));

	TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

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
	unhand(cur->thread)->PrivateInfo = 0;
	cur->thread = 0;
	cur->suspendState = 0;

	/* link into cache list (if still within limit) */
	if ( ++nCached < MAX_CACHED_THREADS ){
	  cur->next = cache;
	  cache = cur;

	  DBG( vm_thread, TMSG_SHORT( "cached thread ", cur));
	}

	pendingExits--;

	TUNLOCK( cur); /* ---------------------------------------------------- tLock */

	if ( nCached >= MAX_CACHED_THREADS ){
	  break;
	}

	/* Wait until we get re-used (by TcreateThread). No need to update the
	 * blockState, since we aren't active anymore */
	sem_wait( &cur->sem);

	/*
	 * we have already been moved back to the activeThreads list by
	 * Tcreate (saves us a lock)
	 */
	DBG( vm_thread, TMSG_SHORT( "reused thread ", cur));
  }

  tDispose( cur);

  return 0;
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

nativeThread*
jthread_create ( unsigned char pri, void* func, int daemon, void* jlThread, size_t threadStackSize )
{
  nativeThread         *cur = GET_CURRENT_THREAD( &self);
  nativeThread         *nt;
  struct sched_param   sp;
  Hjava_lang_Thread*   thread = (Hjava_lang_Thread*)jlThread;
  int		       iLockRoot;

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

  sp.sched_priority = priorities[unhand(thread)->priority];
  if ( !unhand(thread)->daemon )
	nonDaemons++;

  if ( cache ) {
	TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

	/* move thread from the cache to the active list */
	nt = cache;
	cache = cache->next;
	nCached--;

	nt->next = activeThreads;
	activeThreads = nt;

	nt->thread = thread;
	nt->func = func;
	nt->stackCur = 0;
	unhand(thread)->PrivateInfo = (struct Hkaffe_util_Ptr*)nt;
	tSetLock( nt);

	pthread_setschedparam( nt->tid, SCHEDULE_POLICY, &sp);

	DBG( vm_thread, TMSG_SHORT( "create recycled ", nt));

	/* resurrect it */
	nt->active = 1;
	sem_post( &nt->sem);

	TUNLOCK( cur); /* ---------------------------------------------------- tLock */
  }
  else {
	if ( nSysThreads++ > MAX_SYS_THREADS ){
	  // bail out, we exceeded our physical thread limit
	  DBG( vm_thread, ( "too many threads (%d)\n", nSysThreads));
	  return (0);
	}

	nt = thread_malloc( sizeof(nativeThread) );

	pthread_attr_init( &nt->attr);
	pthread_attr_setschedparam( &nt->attr, &sp);
	pthread_attr_setschedpolicy( &nt->attr, SCHEDULE_POLICY);
	pthread_attr_setstacksize( &nt->attr, threadStackSize);

	nt->thread       = thread;
	nt->func         = func;
	nt->suspendState = 0;
	nt->stackMin     = 0;
	nt->stackMax     = 0;
	nt->stackCur     = 0;

	unhand(thread)->PrivateInfo = (struct Hkaffe_util_Ptr*)nt;

	DBG( vm_thread, TMSG_SHORT( "create new ", nt));

	/* init our cv and mux fields for locking */
	tInitLock( nt);
	tSetLock( nt);

	/* Link the new one into the activeThreads list. We lock until
	 * the newly created thread is set up correctly (i.e. is walkable)
	 */
	TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

	nt->active = 1;
	nt->next = activeThreads;
	activeThreads = nt;

	/* Note that we don't directly start 'func' because we (a) still need to
	 * set the thread specifics, and (b) we need a looper for our thread
	 * recycling. We create the new thread while still holding the lock, because
	 * we otherwise might have a invalid tid in the activeList. The new thread
	 * in turn doesn't need the lock until it exits
	 */
	pthread_create( &nt->tid, &nt->attr, tRun, nt);

	/* wait until the thread specific data has been set, and the new thread
	 * is in a suspendable state */
	sem_wait( &nt->sem); 

	TUNLOCK( cur); /* ---------------------------------------------------- tLock */
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
void
tDispose ( nativeThread* nt )
{
  pthread_detach( nt->tid);

  pthread_mutex_destroy( &nt->mux);
  pthread_cond_destroy( &nt->cv);
  sem_destroy( &nt->sem);

  thread_free( nt);
}

/*
 * Function to be called (by threads.c firstStartThread) when the thread leaves
 * the user thread function
 */
void
jthread_exit ( void )
{
  Hjava_lang_Thread    *thread = tCurrentJava();
  nativeThread         *cur = NATIVE_THREAD( thread);
  nativeThread         *t;
  int		       iLockRoot;

  /*
   * We are leaving the thread user func, which means we are not
   * subject to GC, anymore (has to be marked here because the first thread
   * never goes through tRun)
   */
  cur->active = 0;

  DBG( vm_thread, TMSG_SHORT( "exit ", cur));  

  if ( !unhand(thread)->daemon ){
	/* the last non daemon should shut down the process */
	if ( --nonDaemons == 0 ) {
	  TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

	  DBG( vm_thread, ("exit on last nonDaemon\n"));

	  /*
	   * be a nice citizen, try to cancel all other threads before we
	   * bail out, to give them a chance to run their cleanup handlers
	   */
	  for ( t=cache; t != NULL; t = t->next ){
		pthread_cancel( t->tid);
	  }

	  for ( t=activeThreads; t != NULL; t = t->next ){
		if ( t != cur ) {
		  pthread_cancel( t->tid);
		}
	  }

	  if ( deadlockWatchdog ){
		pthread_cancel( deadlockWatchdog);
	  }

	  if ( (cur != firstThread) && (firstThread->active == 0) ) {
		/* if the firstThread has already been frozen, it's not in the cache list */
		pthread_cancel( firstThread->tid);
	  }

	  pthread_exit( 0);

	  /* pretty useless, but clean */
	  TUNLOCK( cur); /* -------------------------------------------------- tLock */

	  /* we shouldn't get here, this is a last safeguard */
	  EXIT(0);
	}
  }

  if ( cur == firstThread ) {
	/*
	 * We don't cache this one, but we have to remove it from the active list. Note
	 * that the firstThread always has to be the last entry in the activeThreads list
	 * (we just add new entries at the head)
	 */
	TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

	/* if we would be the head, we would have been the last, too (and already exited) */
	assert( cur != activeThreads);

	for ( t=activeThreads; (t != NULL) && (t->next != cur); t=t->next );
	assert( t != NULL);
	t->next = 0;

	TUNLOCK( cur); /* ---------------------------------------------------- tLock */  

	/*
	 * Put us into a permanent freeze to avoid shut down of the whole process (it's
	 * not clear if this is common pthread behavior or just a implementation
	 * linux-threads "feature")
	 */
	sem_wait( &cur->sem);
  }
  else {
	/* flag that we soon will get a new cache entry (would be annoying to
	 * create a new thread in the meantime) */
	pendingExits++;
  }
}


/*
 * Thread is being finalized - free any held resource.
 */
void    
jthread_destroy (nativeThread* cur)
{
  DBG_ACTION( vm_thread, {
	DBG( vm_thread, TMSG_SHORT( "finalize ", cur));
  });
}       


/***********************************************************************
 * scheduling and query
 */

/*
 * Change the scheduler priority of a running thread. Since we aren't
 * called directly, we can assume that 'prio' is within [MIN_PRIORITY..MAX_PRIORITY]
 */
void    
jthread_setpriority (nativeThread* cur, jint prio)
{
  struct sched_param   sp;

  if ( cur ) {
	sp.sched_priority = priorities[prio];

	DBG( vm_thread, ("set priority: %p [tid: %d, java: %x) to %d (%d)\n",
					 cur, cur->tid, cur->thread, prio, priorities[prio]));
	pthread_setschedparam( cur->tid, SCHEDULE_POLICY, &sp);
  }
}            

/*******************************************************************************
 * the suspend/resume mechanism
 */

/*
 * The suspend signal handler, which we need to implement critical sections.
 * It is used for two purposes: (a) to block all active threads which might
 * get rescheduled during a critical section (we can't rely on priority-fifo
 * scheduling, it might be SCHED_OTHER), and (b) to get a GC-limit of the current
 * thread stack so that we don't have to scan the whole thing. Since this
 * assumes that we execute on the threads normal stack, make sure we have no
 * SA_ONSTACK / signalstack() in effect
 */
void
suspend_signal_handler ( int sig )
{
  nativeThread  *cur = GET_CURRENT_THREAD(&cur);

  DBG( vm_thread_sig, ("suspend signal handler: %p\n", cur));

  /* signals are global things and might come from everywhere, anytime */
  if ( !cur || !cur->active )
	return;

  if ( cur->suspendState == SS_PENDING_SUSPEND ){

	/* assuming we are executing on the thread stack, we record our current pos */
	cur->stackCur     = (void*)&cur;
	cur->suspendState = SS_SUSPENDED;

	/* notify the critSect owner that we are now suspending in the handler */
	sem_post( &critSem);

	/* freeze until we get a subsequent SIG_RESUME */
	sigsuspend( &suspendSet);

	DBG( vm_thread_sig, ("sigsuspend return: %p\n", cur));

	cur->stackCur     = 0;
	cur->suspendState = 0;

	/* notify the critSect owner we are leaving the handler */
	sem_post( &critSem);
  }
}

/*
 * The resume signal handler, which we mainly need to get the implicit sigreturn
 * call (i.e. to unblock a preceeding sigsuspend).
 */
void
resume_signal_handler ( int sig )
{
  /* we don't do anything, here - all the action is in the suspend handler */
}


/*
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
  int           stat;
  nativeThread  *cur = GET_CURRENT_THREAD( &cur);
  nativeThread  *t;
  int		iLockRoot;
  //int           nSuspends;

  /* don't allow any new thread to be created or recycled until this is done */
  TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

  DBG( vm_thread, ("enter crit section[%d] from: %p [tid:%d, java:%p)\n",
				   critSection, cur, cur->tid, cur->thread));

  if ( ++critSection == 1 ){
	//nSuspends = 0;

	for ( t=activeThreads; t; t = t->next ){
	  /*
	   * make sure we don't suspend ourselves, and we don't expect suspend
	   * signals handled by threads which are blocked on someting else
	   * than the thread lock (which we soon release)
	   */
	  if ( (t != cur) && (t->suspendState == 0) && (t->active) ) {
		DBG( vm_thread_sig, ("signal suspend: %p (susp: %d blk: %d)\n",
							 t, t->suspendState, t->blockState));
		t->suspendState = SS_PENDING_SUSPEND;

		if ( (stat = pthread_kill( t->tid, SIG_SUSPEND)) ){
		  DBG( vm_thread, ("error sending SUSPEND signal to %p: %d\n", t, stat));
		}
		else {
		  //nSuspends++;

		  /* BAD: Empirical workaround for lost signals (with accumulative syncing)
		   * It shouldn't be necessary (posix sems are accumulative), and it
		   * is bad, performancewise (at least n*2 context switches per suspend)
		   * but it works more reliably on linux 2.2.x */
		  sem_wait( &critSem);
		}
	  }
	}

#ifdef NEVER
	/* wait until all signals we've sent out have been handled */
	while ( nSuspends ){
	  sem_wait( &critSem);
	  nSuspends--;
	}
#endif
  }

  TUNLOCK( cur); /* ------------------------------------------------------ tLock */

  DBG( vm_thread, ( "critical section (%d) established\n", critSection))
}


/*
 * Resume all temporarily suspended threads. Just take action if this
 * is the outmost exit
 */
void
jthread_unsuspendall (void)
{
  nativeThread  *cur = GET_CURRENT_THREAD( &cur);
  nativeThread *t;
  int          stat;
  int		iLockRoot;
  //int          nResumes;

  if ( !critSection )
	return;

  if ( --critSection == 0 ){
	//nResumes = 0;

	/* No need to sync, there's nobody else running. It's just a matter of
	 * defensive programming (and we use our fast locks)
	 */
	TLOCK( cur); /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++ tLock */

	for ( t=activeThreads; t; t = t->next ){
	  if ( t->suspendState & (SS_PENDING_SUSPEND | SS_SUSPENDED) ){
		//nResumes++;

		DBG( vm_thread, ("signal resume: %p (sus: %d blk: %d)\n",
						 t, t->suspendState, t->blockState));

		t->suspendState = SS_PENDING_RESUME;
		stat = pthread_kill( t->tid, SIG_RESUME);
		if ( stat ) {
		  DBG( vm_thread, ("error sending RESUME signal to %p: %d\n", t, stat));
		}

		/* ack wait workaround, see TentercritSect remarks */
		sem_wait( &critSem);
	  }
	}

#ifdef NEVER
	/* wait until all signals we've sent out have been handled */
	while ( nResumes ){
	  sem_wait( &critSem);
	  nResumes--;
	}
#endif

	TUNLOCK( cur); /*----------------------------------------------------- tLock */
  }

  DBG( vm_thread, ("exit crit section (%d)\n", critSection));
}


/*******************************************************************************
 * GC related stuff
 */

/*
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
jthread_walkLiveThreads (void(*func)(void*))
{
  nativeThread *t;

  DBG( vm_thread, ("start walking threads\n"));

  for ( t = activeThreads; t != NULL; t = t->next) {
	func((void*)t->thread);
  }

  DBG( vm_thread, ("end walking threads\n"));
}

#if 0
/*      
 * Walk the thread's internal context. The stack is actually "walked down"
 * [base+size..base].
 * ? how useful is this for the GC thread itself ?
 */
void
TwalkThread ( Hjava_lang_Thread* thread )
{
  nativeThread  *t;
  nativeThread  *nt = NATIVE_THREAD( thread);
  void          *base;
  long          size;

  if ( !nt || !nt->active || !nt->stackMin ){
	return;
  }

  DBG( vm_thread, TMSG_LONG( "walking ", nt));

  if ( !nt->suspendState && !nt->blockState ){
	t = GET_CURRENT_THREAD(&t);
	if ( t == nt ){
	  /* Ok, we do walk the gcMan for now, but just up to this point */
	  nt->stackCur = &t;
	}
	else {
	  /* everything else should be blocked or suspended by now */
	  DBG( vm_thread, ("walking a running thread %p\n", nt));
	  //tDump();
	  //ABORT();
	  return;
	}
  }
  else if ( nt->suspendState == SS_PENDING_SUSPEND ){
	DBG( vm_thread, ("pending suspend, walk whole stack\n"));
	/*
	 * Assuming the very next thing after a context switch to this thread
	 * would be calling the signal handler, we accept that case. Unfortunately,
	 * we don't have a stackCur, yet
	 */
#if defined(STACK_GROWS_UP)
	nt->stackCur = nt->stackMax;
#else
	nt->stackCur = nt->stackMin;
#endif	
  }

  if ( ((uintp) nt->stackCur < (uintp) nt->stackMin) ||
	   (((uintp) nt->stackCur > (uintp) nt->stackMax)) ) {
	DBG( vm_thread, ("inconsistent stack\n"));
	tDump();
	ABORT();
  }

  markObject( unhand(thread)->jnireferences);
  markObject( unhand(thread)->exceptObj);

#if defined(STACK_GROWS_UP)
  base = nt->stackMin;
  size = (unsigned long)nt->stackCur - (unsigned long) base;
#else
  base = nt->stackCur;
  size = (uintp)nt->stackMax - (uintp) base;
#endif

  walkConservative( base, size);
}
#endif
