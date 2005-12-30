/*
 * thread-impl.h - pthread based ThreadInterface implementation
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

#ifndef __thread_internal_h
#define __thread_internal_h

#define __USE_GNU
#include <pthread.h>
#include "repsemaphore.h"

#include "gtypes.h"
#include "threadData.h"
#include "debug.h"

#if !defined(STACKREDZONE)
#define STACKREDZONE    8192
#endif

/* For jthread_get_status */
#define THREAD_SUSPENDED	0
#define THREAD_RUNNING		1
#define THREAD_DEAD		2
#define THREAD_KILL             3

/* suspend states (these are exclusive) */
typedef enum {
  SS_PENDING_SUSPEND =  0x01,  /* suspend signal has been sent, but not handled */
  SS_SUSPENDED       =  0x02,  /* suspend signal has been handled */
  SS_PENDING_RESUME  =  0x04   /* resume signal  has been sent */
} suspend_state_t;

/* blocking states (might be accumulative) */
typedef enum {
  BS_THREAD          =  0x01,  /* blocked on tLock (thread system internal) */
  BS_MUTEX           =  0x02,  /* blocked on a external mutex lock */
  BS_CV              =  0x04,  /* blocked on a external convar wait */
  BS_CV_TO           =  0x08,   /* blocked on a external convar timeout wait */
  BS_SYSCALL         =  0x10  /* blocked on a real blocking syscall */
} block_state_t;

/*
 * 'jthread' is our link between native and Java thread objects.
 * It also serves as a container for our pthread extensions (namely
 * enumeration, and inter-thread suspend)
 */
typedef struct _jthread {
  threadData		data;
  /* these are our links to the native pthread implementation */
  pthread_t             tid;
  pthread_attr_t        attr;

  /* To release the lock */
  pthread_mutex_t       suspendLock;

  /* wether this is a daemon thread */
  int			daemon;
  int                   interrupting;
  int                  status;

  /* convars and mutexes aren't useful in signal handlers, semaphores are */
  repsem_t                 sem;

  /* the following fields hold our extensions */
  int                   active;         /* are we in our user thread function 'func'? */
  suspend_state_t       suspendState;   /* are we suspended for a critSection?  */
  block_state_t         blockState;     /* are we in a Lwait or Llock (can handle signals)? */

  void                  (*func)(void*);  /* this kicks off the user thread func */
  void                  *stackMin;
  void                  *stackCur;      /* just useful if blocked or suspended */
  void                  *stackMax;

  struct _jthread	*next;
} *jthread_t;

extern pthread_key_t   ntKey;

/**
 * Returns the current native thread.
 *
 */
jthread_t jthread_current(void);

/**
 * Attaches the calling thread to the vm.
 *
 * @param is_daemon wether the thread is to be treated as a daemon thread
 *
 */
bool jthread_attach_current_thread (bool is_daemon);

/**
 * Detaches the calling thread from the vm.
 *
 */
bool jthread_detach_current_thread (void);

void jthread_sleep (jlong timeout);

/**
 * Disable stopping the calling thread.
 *
 * Needed to avoid stopping a thread while it holds a lock.
 */
void jthread_disable_stop(void);

/**
 * Enable stopping the calling thread.
 *
 * Needed to avoid stopping a thread while it holds a lock.
 */
void jthread_enable_stop(void);

/** 
 * Stop a thread.
 * 
 * @param tid the thread to stop.
 */
void jthread_stop(UNUSED jthread_t tid);

/**
 * Interrupt a thread.
 * 
 * @param tid the thread to interrupt
 */
void jthread_interrupt(jthread_t tid);

/**
 * Register a function to be called when the all non-daemon threads have exited.
 * 
 * @param func the func to execute.
 */
void jthread_atexit(void (* func)(void));

/**
 * Dump some information about a thread to stderr.
 *
 * @param tid the thread whose info is to be dumped.
 */
void jthread_dumpthreadinfo(UNUSED jthread_t tid);

/**
 * Return the java.lang.Thread instance attached to a thread
 *
 * @param tid the native thread whose corresponding java thread
 *            is to be returned.
 * @return the java.lang.Thread instance.
 */
threadData *jthread_get_data(jthread_t tid);

/**
 * Test whether an address is on the stack of the calling thread.
 *
 * @param p the address to check
 *
 * @return true if address is on the stack
 *
 * Needed for locking and for exception handling.
 */
bool jthread_on_current_stack(void* p);

/**
 * Check for room on stack.
 *
 * @param left number of bytes that are needed
 *
 * @return true if @left bytes are free, otherwise false 
 *
 * Needed by intrp in order to implement stack overflow checking.
 */
bool jthread_stackcheck(int left);

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
bool jthread_extract_stack(jthread_t tid, void** from, unsigned* len);

/**
 * Returns the upper bound of the stack of the calling thread.
 *
 * Needed by support.c in order to implement stack overflow checking. 
 */
void* jthread_stacklimit(void);

/*
 * Get the current stack limit.
 */
void jthread_relaxstack(int yes);

/**
 * yield.
 *
 */
void jthread_yield (void);

/**
 * Acquire a spin lock.
 *
 */
void jthread_spinon(UNUSED void *dummy);

/**
 * Release a spin lock.
 *
 */
void jthread_spinoff(UNUSED void *dummy);

struct _exceptionFrame;
typedef void (*exchandler_t)(struct _exceptionFrame*);

/**
 * Initialize handlers for null pointer accesses and div by zero        
 *
 */             
void jthread_initexceptions(exchandler_t _nullHandler,
			    exchandler_t _floatingHandler,
			    exchandler_t _stackOverflowHandler);

/**
 * Initialize the thread subsystem.
 *
 */
struct _Collector;
void jthread_init(int preemptive,                	/* preemptive scheduling */
		  int maxpr,                     	/* maximum priority */
		  int minpr,                     	/* minimum priority */
		  struct _Collector *collector,
		  void (*_destructor1)(void*),		/* called when a thread exits */
		  void (*_onstop)(void),		/* called when a thread is stopped */
		  void (*_ondeadlock)(void));		/* called when we detect deadlock */


/**
 * Bind the main thread of the vm to a java.lang.Thread instance.
 *
 */
jthread_t jthread_createfirst(size_t, unsigned int, void*);

/**
 * Create a new native thread.
 *
 */
jthread_t jthread_create (unsigned int pri, void* func, int is_daemon,
			  void* jlThread, size_t threadStackSize );


/**
 * Set the priority of a native thread.
 *
 */
void jthread_setpriority (jthread_t thread, jint prio);

/**
 * Called by thread.c when a thread is finished. 
 * 
 */
void jthread_exit ( void );

/**
 * Destroys the a native thread.
 *
 * @param thread the thread to destroy.
 *
 * Called when finalizing a java.lang.Thread instance.
 */
void jthread_destroy (jthread_t thread);

/**
 * Lock the special GC mutex. This mutex must be available to anyone doing
 * some GC sensitive operations (like intra-thread handling and the GC itself).
 * It may not be handled using the standard scheme as we can neither use fast
 * lock at some point of the thread handling nor the standard mutex for
 * the same reason. This call must be available in any circumstances after 
 * jthread_init.
 */
void jthread_lockGC(void);

/**
 * Unlock the special GC mutex.
 */
void jthread_unlockGC(void);

/**
 * Suspends all threads but the calling one. 
 *
 * Currently needed by the garbage collector.
 */
void jthread_suspendall (void);

/**
 * Unsuspends all threads but the calling one. 
 *
 * Currently needed by the garbage collector.
 */
void jthread_unsuspendall (void);

/**
 * Call a function once for each active thread.
 * Caution. This should only be used when all threads
 * are suspended using jthread_suspendall.
 */
void jthread_walkLiveThreads (void(*)(jthread_t,void*), void *);

/**
 * Call a function once for each active thread.
 * This is a reentrant version.
 */
void jthread_walkLiveThreads_r (void(*)(jthread_t,void*), void *);

/**
 * Return thread status
 */
int jthread_get_status (jthread_t thread);

/**
 * Specify the blocking state of a file descriptor
 */
void jthread_set_blocking (int fd, int blocking);

/**
 * Check the blocking state of a file descriptor
 */
int jthread_is_blocking (int fd);

/**
 * Check whether the specified thread is locked in a condvar.
 * 
 * @param th The thread to check.
 * @return 1 if it is locked, 0 else.
 */
int jthread_on_condvar (jthread_t th);

/**
 * Check whether the specified thread is locked in a mutex.
 *
 * @param th The thread to check.
 * @return 1 if it is locked, 0 else.
 */
int jthread_on_mutex (jthread_t th);

/**
 * Check whether the thread has been run since the last
 * clear.
 *
 * @param jt The thread to check the status of.
 * @return 1 if it has been run, 0 else.
 */
int jthread_has_run (jthread_t jt);

/**
 * Clear the run state of the specified thread.
 *
 * @param jt The thread to clear the run status of.
 */
void jthread_clear_run (jthread_t jt);

void jthread_suspend(UNUSED jthread_t jt, UNUSED void *suspender);

void jthread_resume(UNUSED jthread_t jt, UNUSED void *suspender);

jthread_t jthread_from_data(UNUSED threadData *td, UNUSED void *suspender);

jlong jthread_get_usage(UNUSED jthread_t jt);

int jthread_is_interrupted(jthread_t jt);

int jthread_interrupted(jthread_t jt);

void KaffePThread_WaitForResume(int releaseMutex, unsigned int state);
void KaffePThread_AckAndWaitForResume(jthread_t cur, unsigned int newState);
int KaffePThread_getSuspendSignal(void);

void detectStackBoundaries(jthread_t jtid, size_t mainThreadStackSize);

#endif /* __thread_impl_h */
