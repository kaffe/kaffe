/*
 * jthread.h
 * Internal threading backend of Kaffe for DROPS
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#ifndef __l4_jthread_h
#define __l4_jthread_h

#include <l4/sys/types.h>
#include <l4/sys/syscalls.h>

#include "config.h"
#include "config-setjmp.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "gtypes.h"
#include "support.h"
#include "md.h"
#include "lerrno.h"
#include "lock-impl.h"
#include "threadData.h"

#include <l4/util/macros.h> /* l4util_idfmt, l4util_idstr */
//#include "thread.h"

#define GET_STACK_PTR(var) asm volatile ("mov %%esp, %%eax" : "=a" (var))

typedef struct _jthread {
        threadData data;        /* threadData from the JavaThreads*/
        int daemon;             /* a daemon thread? */
        struct _jthread * next; /* next live thread */
        struct _jthread * prev; /* prev live thread */
	void   (*func)(void *); /* Start function */
        l4_umword_t eip; 
        l4_umword_t esp;
        l4thread_t l4thread;    /* native thread id */
	int interrupting;       /* FLAG, if thread is in interrupting state */
#ifdef REALTIME_EXTENSION
        int threadtype;
        unsigned status;
#endif
} jthread, *jthread_t;

#include "l4thread.h"
extern __leaflet leaflet;

struct _Collector;
/* 
 * initialize the threading system
 */
jthread_t
jthread_init(
	int preemptive,			/* preemptive scheduling */
	int maxpr, 			/* maximum priority */
	int minpr, 			/* minimum priority */
        struct _Collector *collector,   /* memory de-/re- & allocator */
	void (*_destructor1)(void*),	/* called when a thread exits */ 
	void (*_onstop)(void),		/* called when a thread is stopped */
	void (*_ondeadlock)(void));     /* called on deadlock */

/*
 * Create the first thread - actually bind the first thread to the java
 * context.
 */
jthread_t
jthread_createfirst(size_t mainThreadStackSize, 
		    unsigned int prio, 
		    void* jlThread);


/*
 * create a thread with a given priority
 */
jthread_t
jthread_create(unsigned int pri, 	/* initial priority */
	void (*func)(void *),  		/* start function */
	int isDaemon, 			/* is this thread a daemon? */
	void *jlThread, 		/* cookie for this thread */
	size_t threadStackSize);	/* stack size to be allocated */

#ifdef REALTIME_EXTENSION
/*
 * create a realtime thread with a given priority
 */
jthread_t
jthread_createRT(unsigned int pri,
		 void (*func)(void *),
		 int isDaemon,
	         void *jlThread,
		 size_t threadStackSize,
		 int threadtype);
#endif /*REALTME_EXTENSION */

struct _exceptionFrame;
typedef void (*exchandler_t)(struct _exceptionFrame*);
                
/*                                                                      
 * Initialize handlers for null pointer accesses and div by zero        
 */             
void jthread_initexceptions(exchandler_t _nullHandler,
			    exchandler_t _floatingHandler,
			    exchandler_t _stackOverflowHandler);

/*
 * walk all live threads, and invoke `func', passing in their cookie
 * this is used with `func' set to walkMemory
 */
void jthread_walkLiveThreads   (void (*)(jthread_t,void*), void *);
void jthread_walkLiveThreads_r (void (*)(jthread_t,void*), void *);
 
inline static threadData* jthread_get_data(jthread_t tid)
{
  return (&tid->data);
}


/* 
 * destroy this jthread structure 
 */
void	jthread_destroy(jthread_t jtid);


/*
 * sleep for time milliseconds
 */
void 	jthread_sleep(jlong jtime);

/*
 * returns the Kaffe thread data structure of the current native L4 thread.
 */
inline static jthread_t jthread_current(void)
{
 return (jthread_t)l4thread_data_get_current(leaflet.datakey);
}


/*
 * set the priority of a thread 
 */
inline static void
jthread_setpriority(jthread_t jtid, int prio)
{
  if (l4thread_set_prio(jtid->l4thread,prio + leaflet.startPrio)!=0)
    assert(!"l4thread_set_prio failed !");
}

/* 
 * count the number of stack frames - unimplemented 
 */
int 	jthread_frames(jthread_t thrd);

/* 
 * return whether this thread is alive or not
 */
int 	jthread_alive(jthread_t jtid);

/*
 * stop this thread
 */
void 	jthread_stop(jthread_t jtid);

/*
 * interrupt a thread
 */
static inline void jthread_interrupt(jthread_t thread) {
  thread->interrupting = 1;

  /* bad hack, because of races ! */
  if (thread->data.sem.l4sem.counter < 0 ) {
    thread->data.sem.failed = true;
    ksem_put(&thread->data.sem);
  }

}


/*
 * have the current thread exit
 */
void 	jthread_exit(void) NONRETURNING;

/*
 * set a function to be run when last non-daemon dies 
 * this is used to run the finalizer on exit.
 */
void 	jthread_atexit(void (*f)(void));


/*
 * determine whether a location is on the stack of the current thread
 * (was FRAMEOKAY)
 */
static inline int
jthread_on_current_stack(void *bp) {
  int nReturn = 0;
  l4_addr_t low,high;
  l4_addr_t addr = (l4_addr_t)bp;

  if (l4thread_get_stack_current(&low,&high)){
    assert(!"l4thread_get_stack_current problem!");
  }else if (low <= addr && addr <= high) nReturn = 1; //on stack !
 
  return nReturn;
}


/*
 * Check for room on stack.
 */
static inline int
jthread_stackcheck(int need) {
  int nReturn = 0;
  l4_addr_t low,high;
  unsigned long old_esp;

  GET_STACK_PTR(old_esp);

  if (l4thread_get_stack_current(&low,&high)){
    assert(!"l4thread_get_stack_current problem!");
  }else{
    if (old_esp - need >= low) nReturn = 1;
  }

  return nReturn;
}

static inline void
jthread_relaxstack(int yes UNUSED) {
  printf("todo - jthread_relaxstack\n");
}

static inline void*
jthread_stacklimit(void) {
  printf("todo - jthread_stacklimit \n");
  return 0;
}

/*
 * determine the "interesting" stack range a conservative gc must walk
 */
int
jthread_extract_stack(jthread_t jtid, void **from, unsigned *len);
/*
 * Disallow cancellation for current thread
 *
 * Need while thread is holding a lock.
 */
static inline void jthread_disable_stop(void){};

/*
 * Reallow cancellation for current thread
 * If a cancellation is pending, the stop method will be called
 */
static inline void jthread_enable_stop(void){};

/*
 * Print info about a given jthread to stderr
 */
void jthread_dumpthreadinfo(jthread_t tid);

/*
 * Implement a spin lock on a given addr.
 * - Currently, addr is unused in Kaffe.
 * - Kaffe's AWT QT backend uses this functions for malloc.
 */
static inline void 
jthread_spinon(UNUSED void *arg)
{
  l4semaphore_down(&leaflet.systemLock);
}

static inline void 
jthread_spinoff(UNUSED void *arg)
{
  l4semaphore_up(&leaflet.systemLock);
}

/*
 * Prevent all other threads from running.
 */
void jthread_suspendall(void);

/*
 * Reallow other threads.
 */
void jthread_unsuspendall(void);

void drops_registerForExceptions(void);


/**
 * Specify the blocking state of a file descriptor
 */
void jthread_set_blocking (int fd, int blocking);

/**
 * Peek at the interrupted flag of a thread.
 *
 * @return true if thread was interrupted.
 */
static inline int jthread_is_interrupted(jthread_t thread) {
  return thread->interrupting;
}

/**
 * Read and clear the interrupted flag of a thread.
 *
 * @return true if thread was interrupted.
 */
static inline int jthread_interrupted(jthread_t thread)
{
  int i = thread->interrupting;
  thread->interrupting = 0;
  return i;
}

static inline bool jthread_attach_current_thread (bool isDaemon UNUSED) {
  return false; //to do
}

bool jthread_detach_current_thread (void);
	
/*
 * yield to another thread
 */
static inline void 	
jthread_yield(void)
{
  l4_yield();
}

static inline void jthread_lockGC(void)
{
//  l4semaphore_down(&leaflet.gcLock);
}

static inline void jthread_unlockGC(void)
{
//  l4semaphore_up(&leaflet.gcLock);
}


#ifdef REALTIME_EXTENSION
void * jthread_createarea(l4_size_t size);
void * jthread_reservearea(l4_size_t size);

/**
 * Consumed time (in us) of a thread reported by the kernel.
 */
jlong jthread_ctime(jthread_t thread);

jint jthread_startPeriodic(jlong start_ms,
                           jint  start_ns,
                           jlong period_ms,
                           jint  period_ns,
                           jlong wcet_ms,
                           jint  wcet_ns);

void jthread_stopPeriodic(void);
void jthread_nextPeriod(void);

/*
 * set a function to be run when deadline is missed or 
 * cost (wcet) is overruned.
 */
void jthread_misshandler(void (*f)(jthread_t, unsigned), unsigned costoverrun,
                         unsigned deadlineoverrun);
#endif /* REALTIME_EXTENSION */
#endif
