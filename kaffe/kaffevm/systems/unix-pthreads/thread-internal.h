/*
 * thread-impl.h - pthread based ThreadInterface implementation
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __thread_internal_h
#define __thread_internal_h

#include <pthread.h>
#if defined(HAVE_SEMAPHORE_H)
#include <semaphore.h>
#endif /* defined(HAVE_SEMAPHORE_H) */

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
  BS_CV_TO           =  0x08   /* blocked on a external convar timeout wait */
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

  /* convars and mutexes aren't useful in signal handlers, semaphores are */
  sem_t                 sem;

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
static inline          
jthread_t jthread_current(void)      
{
  return (jthread_t)pthread_getspecific(ntKey);
}

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
static inline
void jthread_disable_stop(void)
{
}

/**
 * Enable stopping the calling thread.
 *
 * Needed to avoid stopping a thread while it holds a lock.
 */
static inline
void jthread_enable_stop(void)
{
}

/** 
 * Stop a thread.
 * 
 * @param tid the thread to stop.
 */
static inline
void jthread_stop(UNUSED jthread_t tid)
{
}

/**
 * Interrupt a thread.
 * 
 * @param tid the thread to interrupt
 */
void jthread_interrupt(jthread_t tid);

/**
 * Register a function to be called when the vm exits.
 * 
 * @param func the func to execute.
 */
static inline
void jthread_atexit(UNUSED void (* func)(void))
{
}

/**
 * Dump some information about a thread to stderr.
 *
 * @param tid the thread whose info is to be dumped.
 */
static inline
void jthread_dumpthreadinfo(UNUSED jthread_t tid)
{
}

/**
 * Return the java.lang.Thread instance attached to a thread
 *
 * @param tid the native thread whose corresponding java thread
 *            is to be returned.
 * @return the java.lang.Thread instance.
 */
static inline
threadData *jthread_get_data(jthread_t tid)
{
        return (&tid->data);
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
static inline
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
static inline
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
 * Returns the upper bound of the stack of the calling thread.
 *
 * Needed by support.c in order to implement stack overflow checking. 
 */
static inline
void* jthread_stacklimit(void)
{
  jthread_t nt = jthread_current();
#if defined(STACK_GROWS_UP)
  return (void *)((uintp)nt->stackMax - STACKREDZONE);
#else
  return (void *)((uintp)nt->stackMin + STACKREDZONE);
#endif
}

/*
 * Get the current stack limit.
 */
void jthread_relaxstack(int yes);

/**
 * yield.
 *
 */
static inline
void jthread_yield (void)
{
  sched_yield();
}

/**
 * Acquire a spin lock.
 *
 */
static inline
void jthread_spinon(UNUSED int dummy)
{
}

/**
 * Release a spin lock.
 *
 */
static inline
void jthread_spinoff(UNUSED int dummy)
{
}

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
jthread_t jthread_createfirst(size_t, unsigned char, void*);

/**
 * Create a new native thread.
 *
 */
jthread_t jthread_create (unsigned char pri, void* func, int is_daemon,
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
 *
 */
void jthread_walkLiveThreads (void(*)(jthread_t,void*), void *);

/**
 * Return thread status
 */
int jthread_get_status (jthread_t thread);

/**
 * Specify the blocking state of a file descriptor
 */
void jthread_set_blocking (int fd, int blocking);

static inline void
jthread_suspend(UNUSED jthread_t jt, UNUSED void *suspender)
{
	/* TODO */
}

static inline void
jthread_resume(UNUSED jthread_t jt, UNUSED void *suspender)
{
	/* TODO */
}

static inline jthread_t
jthread_from_data(UNUSED threadData *td, UNUSED void *suspender)
{
	/* TODO */
	return NULL;
}

static inline
jlong jthread_get_usage(UNUSED jthread_t jt)
{
	/* TODO */
	return 0;
}

int jthread_is_interrupted(jthread_t jt);

int jthread_interrupted(jthread_t jt);

void KaffePThread_WaitForResume(int releaseMutex);

void detectStackBoundaries(jthread_t jtid, int mainThreadStackSize);

#endif /* __thread_impl_h */
