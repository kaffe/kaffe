/*
 * jthread.h
 *
 * Thread support using internal system.
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

#ifndef __jthread_h
#define __jthread_h

/*
 * The file is independent of Kaffe, but if compiled as part of the Kaffe
 * build process, it will use Kaffe's constants and config options.
 * This should make it machine-independent or at least aid in porting it.
 *
 * We use the preprocessor constant "HAVE_CONFIG_H" to determine whether that is
 * the case or not.
 */
#if defined(HAVE_CONFIG_H)

/*======== 	begin of definitions that apply to Kaffe 	     ========*/

#include "debug.h"
#include "config.h"
#include "config-setjmp.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "gtypes.h"		/* for jlong */
#include "lerrno.h"
#include "support.h"		/* XXX: for currentTime */
#include "md.h"
#include "threadData.h"

#include <sys/resource.h>

#if defined (HAVE_SYS_POLL_H) || defined(HAVE_POLL_H)
#define USE_POLL	1
#endif

#if defined(__WIN32__)
#define SIG_T   void(*)()
#else
#define SIG_T   void*
#endif

/*======== 	end of definitions that apply to Kaffe 	     	     ========*/

#else	/* !PACKAGE_VERSION */

/*
 * all definitions for compilation under plain UNIX are in this file
 */
#include "config-jthreads.h"

/*======== end of definitions that apply to plain UNIX only ========*/

#endif  /* !PACKAGE_VERSION */

#include "jqueue.h"
#include "lock-impl.h"

/* thread status */
#define THREAD_SUSPENDED                0
#define THREAD_RUNNING                  1
#define THREAD_DEAD                     2

/* thread flags */
#define THREAD_FLAGS_GENERAL            0
#define THREAD_FLAGS_NOSTACKALLOC       1   /* this flag is not used anymore */
#define THREAD_FLAGS_KILLED             2
#define THREAD_FLAGS_ALARM              4
#define THREAD_FLAGS_EXITING        	8
#define THREAD_FLAGS_DONTSTOP        	16
#define THREAD_FLAGS_DYING        	32
#define THREAD_FLAGS_BLOCKEDEXTERNAL	64
#define THREAD_FLAGS_INTERRUPTED	128
#define THREAD_FLAGS_WAIT_MUTEX		256
#define THREAD_FLAGS_WAIT_CONDVAR	512

/*
 * This is our internal structure representing the "native" threads.
 * This used to be called "ctx".
 */
typedef struct _jthread {
	threadData			localData;
	unsigned char			status;
	unsigned char			priority;
	void*				restorePoint;
	void*				stackBase;
	void*				stackEnd;
	void*				suspender;
	unsigned int			suspendCount;
	jlong				time;
	jlong				startUsed;
	jlong				totalUsed;
        KaffeNodeQueue*                 blockqueue; 
	unsigned long			flags;
	void				(*func)(void *);
	int				daemon;
	int				stopCounter;

	JTHREAD_JMPBUF			env;
#if defined(SAVED_FP_SIZE)
	char				fpstate[SAVED_FP_SIZE];
#endif
	/* 
	 * note that this causes gdb under Solaris to crash when trying to
	 * print a struct jthread
	 */
	/* for alignment (Gcc extension) */
	double				align[0];
} jthread, *jthread_t;

extern jthread_t currentJThread;

/****************************************************************************
 *
 * The following functions form the interface provided by the 
 * revised internal threading system.
 */

/* 
 * initialize the threading system
 */
struct _Collector;
void 
jthread_init(
	int preemptive,				/* preemptive scheduling */
	int maxpr, 				/* maximum priority */
	int minpr, 				/* minimum priority */
	struct _Collector *collector,
	void (*_destructor1)(void*),		/* called when a thread exits */ 
	void (*_onstop)(void),			/* called when a thread is stopped */
	void (*_ondeadlock)(void));		/* called when we detect deadlock */

/*
 * Create the first thread - actually bind the first thread to the java
 * context.
 */
jthread_t
jthread_createfirst(size_t mainThreadStackSize,
	unsigned int prio, void* jlThread);

/*
 * create a thread with a given priority
 */
jthread_t
jthread_create(unsigned int pri, 	/* initial priority */
	void (*func)(void *), 		/* start function */
	int isDaemon, 			/* is this thread a daemon? */
	void *jlThread, 		/* cookie for this thread */
	size_t threadStackSize);	/* stack size to be allocated */

struct _exceptionFrame;
typedef void (*exchandler_t)(struct _exceptionFrame*);

/*
 * Register handlers for null pointer accesses and floating point exceptions
 */
void
jthread_initexceptions(exchandler_t _nullHandler,
                       exchandler_t _floatingHandler,
		       exchandler_t _stackOverflowHandler);

/*
 * set a function to be run when last non-daemon dies 
 * this is used to run the finalizer on exit.
 */
void 	jthread_atexit(void (*f)(void));

/**
 * walk all live threads, and invoke `func', passing in their cookie
 * this is used with `func' set to walkMemory
 */
void 	jthread_walkLiveThreads(void (*func)(jthread_t, void*), void*);

/**
 * walk all live threads, and invoke `func', passing in their cookie
 * this is used with `func' set to walkMemory.
 * This function is reentrant.
 */
void 	jthread_walkLiveThreads_r(void (*func)(jthread_t, void*), void*);

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
void 	jthread_yield(void);

/*
 * sleep for time milliseconds
 */
void 	jthread_sleep(jlong millis);

/* 
 * return the current thread 
 */
jthread_t jthread_current(void);

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
void 	jthread_exit(void);

/*
 * determine whether a location is on the stack of the current thread
 */
int     jthread_on_current_stack(void *bp);

/* 
 * Check for room on stack.
 */
int     jthread_stackcheck(int left);

/*
 * Get the current stack limit.
 */
void jthread_relaxstack(int yes);

void* jthread_stacklimit(void);

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
 * functions to disable and restore interrupts
 * These are *not* part of the public interface.
 */
#define	INTS_DISABLED()		intsDisabled()
int 	intsDisabled(void);

/*
 * Prevent all other threads from running.
 * In this uniprocessor implementation, this is simple.
 */
void jthread_suspendall(void);

/*
 * Reallow other threads.
 * In this uniprocessor implementation, this is simple.
 */
void jthread_unsuspendall(void);

/** 
 * Special GC mutex lock.
 */
void jthread_lockGC(void);

/**
 * Special GC mutex unlock.
 */
void jthread_unlockGC(void);

/*
 * Print info about a given jthread to stderr
 */
void jthread_dumpthreadinfo(jthread_t tid);

/*
 * return thread-specific data for a given jthread
 */
threadData* jthread_get_data(jthread_t tid);

/*
 * API related to I/O
 */
int jthreadedOpen(const char* path, int flags, int mode, int*);
int jthreadedSocket(int af, int type, int proto, int*);
int jthreadedConnect(int fd, struct sockaddr* addr, int len, int timeout);
int jthreadedAccept(int fd, struct sockaddr* addr, socklen_t* len, 
	int timeout, int *);
int jthreadedRead(int fd, void* buf, size_t len, ssize_t *);
int jthreadedTimedRead(int fd, void* buf, size_t len, int timeout, ssize_t *);
int jthreadedTimedWrite(int fd, const void* buf, size_t len, int timeout, ssize_t *);
int jthreadedWrite(int fd, const void* buf, size_t len, ssize_t *);
int jthreadedRecvfrom(int fd, void* buf, size_t len, int flags,
        struct sockaddr* from, socklen_t* fromlen, int timeout, ssize_t *);
int jthreadedWaitpid(int wpid, int* status, int options, int *);
int jthreadedForkExec(char **argv, char **arge,
		      int ioes[/* 4 */], int *, const char *);
int
jthreadedSelect(int a, fd_set* b, fd_set* c, fd_set* d, 
		struct timeval* e, int* out);
int jthreadedPipeCreate(int *read_fd, int *write_fd);

void jthread_set_blocking(int fd, int blocking);

int jthread_is_blocking(int fd);

/* restore an fd, i.e., put it in blocking state without async I/O */
#define JTHREAD_RESTORE_FD
void jthreadRestoreFD(int fd);

/* Spinlocks: simple since we're uniprocessor */
void jthread_spinon(void *arg);

void jthread_spinoff(void *arg);

void jthread_suspend(jthread_t jt, void *suspender);
void jthread_resume(jthread_t jt, void *suspender);

jthread_t jthread_from_data(threadData *td, void *suspender);

jlong jthread_get_usage(jthread_t jt);

int jthread_get_status(jthread_t jt);

int jthread_is_interrupted(jthread_t jt);

int jthread_interrupted(jthread_t jt);

int jthread_on_mutex(jthread_t jt);

int jthread_on_condvar(jthread_t jt);

void jthread_clear_run(jthread_t jt);

int jthread_has_run(jthread_t jt);

/* let main thread loop until all threads finish, for tests */
void 	jthread_exit_when_done(void);

static inline
bool jthread_attach_current_thread(UNUSED bool isDaemon)
{
	return false;
}

static inline
bool jthread_detach_current_thread (void)
{
 	return false;
}

#endif
