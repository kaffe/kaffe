/*
 * Copyright (c) 1998 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

/* Modified for BeOS R4 */

#ifndef __jthread_h
#define __jthread_h

#include <OS.h>

#include "config.h"
#include "config-setjmp.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "jtypes.h"
#include "gtypes.h"
#include "thread.h"
#include "support.h"
#include "md.h"
#include "lerrno.h"

/*
 * We set an arbitrary limit on the number of threads just so we can
 * use MAX_THREADS to determine the size of the "area" allocated for
 * per-thread information.  Because thread ids are hashed to indices
 * using a simple modulo function, in practice the number of threads
 * supported will be somewhat less than this number.
 */
#define MAX_THREADS 	2048

/*
 * This amount of memory must be large enough to contain an array
 * of MAX_THREADS instances of the per_thread_info_t structure
 * defined later.
 */
#define PTI_AREA_SIZE	(B_PAGE_SIZE * 6)

/*
 * This is our internal structure representing a "jthread" which is mapped
 * on top of a BeOS native thread.
 */
typedef struct jthread {
	struct jthread*		nextlive;	  /* Live threads chain */
	int			daemon;		  /* Is a daemon thread? */
	void			(*func)(void*);	  /* Startup function */
	void*			jlThread;         /* java_lang_thread ptr */
	thread_id		native_thread;    /* BeOS tid */
	unsigned char		status;		  /* Thread status */
	int32			stop_allowed;	  /* non-zero if STOPpable */
	int32			stop_pending;	  /* remember when STOPped */
	void*			stack_top;	  /* "static" address */
	void*			stack_bottom;	  /* this one, too */
} *jthread_t;

/*
 * The cookie is the Java_lang_thread class structure for the current thread.
 * The jthread is the local jthread_t structure for the current thread.
 *
 * These two are kept together in an array of structures, one per thread,
 * maintained in a BeOS "area" that is created by jthread_init.
 * The index of the entry for thread N is simply (N % MAX_THREADS),
 * at least for now.
 */
typedef struct {
	void*		cookie;
	jthread_t	jtid;
} per_thread_info_t;
extern per_thread_info_t* per_thread_info;	  /* addr of a BeOS area */

#define TIDX		(find_thread(NULL) % MAX_THREADS)
#define GET_JTHREAD()	(per_thread_info[TIDX].jtid)
#define SET_JTHREAD(x)	{ per_thread_info[TIDX].jtid = (x); }
#define GET_COOKIE()	(per_thread_info[TIDX].cookie)

/*
 * This is somewhat bogus. Set jthread->jlthread at the same time so
 * as not to change the code in internal.c, which is going to be shared
 * at some point. 
 */
#define SET_COOKIE(x) \
	{ \
		int i = TIDX;						\
		jthread_t jthread = per_thread_info[i].jtid;		\
									\
		jthread->jlThread = (x);				\
		per_thread_info[i].cookie = (x);			\
	}

/* 
 * return thread-specific data for a given native thread (not a jthread!)
 * as returned by jthread_current().
 */
static inline void*
jthread_getcookie(void* tid)
{
	return per_thread_info[(thread_id)tid % MAX_THREADS].cookie;
}

/****************************************************************************
 *
 * The following functions form the interface provided by the 
 * revised internal threading system.
 */

/* 
 * initialize the threading system
 */
jthread_t
jthread_init(
	int preemptive,			/* preemptive scheduling */
	int maxpr, 			/* maximum priority */
	int minpr, 			/* minimum priority */
        void *(*_allocator)(size_t),	/* memory allocator */
	void (*_deallocator)(void*),	/* memory deallocator */
	void (*_destructor1)(void*),	/* called when a thread exits */ 
	void (*_onstop)(void),		/* called when a thread is stopped */
	void (*_ondeadlock)(void));     /* called on deadlock */


/*
 * Create the first thread - actually bind the first thread to the java
 * context.
 */
jthread_t
jthread_createfirst(size_t mainThreadStackSize, 
		    unsigned char prio, 
		    void* jlThread);


/*
 * create a thread with a given priority
 */
jthread_t
jthread_create(unsigned int pri, 	/* initial priority */
	void (*func)(void *), 		/* start function */
	int daemon, 			/* is this thread a daemon? */
	void *jlThread, 		/* cookie for this thread */
	size_t threadStackSize);	/* stack size to be allocated */

struct _exceptionFrame;
typedef void (*exchandler_t)(struct _exceptionFrame*);
                
/*                                                                      
 * Initialize handlers for null pointer accesses and div by zero        
 */             
void    jthread_initexceptions(exchandler_t _nullHandler,
                               exchandler_t _floatingHandler,
			       exchandler_t _stackOverflowHandler);

/*
 * set a function to be run when last non-daemon dies 
 * this is used to run the finalizer on exit.
 */
void 	jthread_atexit(void (*f)(void));

/*
 * walk all live threads, and invoke `func', passing in their cookie
 * this is used with `func' set to walkMemory
 */
void 	jthread_walkLiveThreads(void (*func)(void *jlThread));

/* 
 * destroy this jthread structure 
 */
void	jthread_destroy(jthread_t jtid);

/*
 * set the priority of a thread 
 */
void	jthread_setpriority(jthread_t jtid, int prio);

/*
 * yield to another thread
 */
static inline void 	
jthread_yield(void)
{ 
	snooze(1);
}

/*
 * sleep for time milliseconds
 */
void 	jthread_sleep(jlong time);

/* 
 * return the current native thread 
 */
static inline thread_id
jthread_current(void) 
{ 
	return find_thread(NULL);
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
 * interrupt this thread
 */
void 	jthread_interrupt(jthread_t jtid);

/*
 * have the current thread exit
 */
void 	jthread_exit(void) NONRETURNING;

/*
 * determine whether a location is on the stack of the current thread
 * (was FRAMEOKAY)
 */
int 	jthread_on_current_stack(void *bp);

/*
 * Check for room on stack.
 */
int 	jthread_stackcheck(int left);

#define	REDZONE	1024

static inline void*
jthread_stacklimit(void)
{
	jthread_t currentJThread = GET_JTHREAD();
	return ((void*)((char*)currentJThread->stack_bottom + REDZONE));
}

/*
 * determine the "interesting" stack range a conservative gc must walk
 */
int jthread_extract_stack(jthread_t jtid, void **from, unsigned *len);

/*
 * Disallow cancellation for current thread
 */
void jthread_disable_stop(void);

/*
 * Reallow cancellation for current thread
 * If a cancellation is pending, the stop method will be called
 */
void jthread_enable_stop(void);

/*
 * Print info about a given jthread to stderr
 */
void jthread_dumpthreadinfo(jthread_t tid);

/*
 * Implement a spin lock on a given addr
 */
void jthread_spinon(void *arg);
void jthread_spinoff(void *arg);

/*
 * Prevent all other threads from running.
 */
void jthread_suspendall(void);

/*
 * Reallow other threads.
 */
void jthread_unsuspendall(void);

/*
 * Signals
 */
void catchSignal(int sig, void* handler);

#endif
