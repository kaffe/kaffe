/*
 * Copyright (c) 1998 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

#ifndef __jthread_h
#define __jthread_h

#include <pthread.h>
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

typedef pthread_mutex_t		jmutex;
typedef pthread_cond_t		jcondvar;

/*
 * This is our internal structure representing a "jthread" which is mapped
 * on top of a "pthread"
 */
typedef struct jthread {
	struct jthread*		nextlive;	  /* Live threads chain */
	int			daemon;		  /* Is a daemon thread? */
	void			(*func)(void *);  /* Startup function */
	void*			jlThread;         /* java_lang_thread ptr */
	pthread_t		native_thread;    /* pthread tid */
	unsigned char		status;		  /* Thread status */
	/* these four are for linux only */
	unsigned char		saved_status;	  /* Saved status */
	void*			base;		  /* approximate stack base */
	void*			end;		  /* approximate stack end */
	void*			sp;		  /* stack pointer */
} *jthread_t;

/*
 * Map the cookie and jthread to pthread_setspecific.
 * The cookie is the Java_lang_thread class structure for the current thread.
 * The jthread is the local jthread_t structure for the current thread.
 *
 * Why? Well, we have no knowledge of when the underlying pthread system
 * switches to a new thread, so must ask what thread is running whenever
 * this information is requested.
 */
extern pthread_key_t		cookie_key;
extern pthread_key_t		jthread_key;

#define GET_JTHREAD()		((jthread_t)pthread_getspecific(jthread_key))
#define SET_JTHREAD(x)		(pthread_setspecific(jthread_key, x))
#define GET_COOKIE()		(pthread_getspecific(cookie_key))

/*
 * This is somewhat bogus. Set jthread->jlthread at the same time so
 * as not to change the code in internal.c, which is going to be shared
 * at some point. 
 */
#define SET_COOKIE(x) \
	{ \
		jthread_t jthread = GET_JTHREAD();			\
									\
		jthread->jlThread = (x);				\
		pthread_setspecific(cookie_key, (x));			\
	}

/* 
 * return thread-specific data for a given native pthread (not a jthread!)
 * as returned by jthread_current().
 */
void* jthread_getcookie(void* tid);

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
	int mainthreadpr,		/* priority of current (main) thread */
	size_t mainThreadStackSize,	/* assumed main stack size */
        void *(*_allocator)(size_t),	/* memory allocator */
	void (*_deallocator)(void*),	/* memory deallocator */
	void (*_destructor1)(void*),	/* called when a thread exits */ 
	void (*_onstop)(void),		/* called when a thread is stopped */
	void (*_ondeadlock)(void));     /* called on deadlock */

/*
 * create a thread with a given priority
 */
jthread_t
jthread_create(unsigned int pri, 	/* initial priority */
	void (*func)(void *), 		/* start function */
	int daemon, 			/* is this thread a daemon? */
	void *jlThread, 		/* cookie for this thread */
	size_t threadStackSize);	/* stack size to be allocated */

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
static void 	
jthread_yield(void)
{ 
	sched_yield();
}

/*
 * sleep for time milliseconds
 */
void 	jthread_sleep(jlong time);

/* 
 * return the current native thread 
 */
static inline pthread_t
jthread_current(void) 
{ 
	return pthread_self();
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
void 	jthread_exit(void) __NORETURN__;

/*
 * determine whether a location is on the stack of the current thread
 * (was FRAMEOKAY)
 */
int 	jthread_on_current_stack(void *bp);

/*
 * Check for room on stack.
 */
int 	jthread_stackcheck(int left);

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
 * Locking API
 */
void jmutex_initialise(jmutex *lock);
void jmutex_lock(jmutex *lock);
void jmutex_unlock(jmutex *lock);

void jcondvar_initialise(jcondvar *cv);
void jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout);
void jcondvar_signal(jcondvar *cv, jmutex *lock);
void jcondvar_broadcast(jcondvar *cv, jmutex *lock);

#endif
