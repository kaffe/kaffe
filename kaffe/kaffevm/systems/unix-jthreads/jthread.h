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
 * We use the preprocessor constant "KVER" to determine whether that is
 * the case or not.
 */
#if defined(KVER)

/*======== 	begin of definitions that apply to Kaffe 	     ========*/

#include "config.h"
#include "config-setjmp.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "jtypes.h"
#include "gtypes.h"
#include "thread.h"
#include "md.h"
#include "lerrno.h"
/* 
 * make sure jsyscall doesn't redefine open, read, etc. 
 * XXX - this is not very clean
 */
#define NOUNIXPROTOTYPES
#include "jsyscall.h"
#define  select(A, B, C, D, E)	\
	(*Kaffe_SystemCallInterface._select)(A,B,C,D,E)

/*======== 	end of definitions that apply to Kaffe 	     	     ========*/

#else	/* !KVER */

/*======== begin of definitions that apply to FreeBSD user mode only ========*/

#include <assert.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define HAVE_SETITIMER	1
#define HAVE_WAITPID	1
#define THREADSTACKSIZE         (32 * 1024)

#if defined(__FreeBSD__)
/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  In FreeBSD, that's 2 - normally it's 4 for a i386
 * On NetBSD and OpenBSD it's 2 as well.
 */
#define SP_OFFSET               2
#include <sys/ttycom.h>
#include <sys/filio.h>	
#elif defined(__linux__)
#define SP_OFFSET		4
#define FD_COPY(from, to)	memcpy(from, to, sizeof(from))
#endif

/* define our own jlong and NOTIMEOUT */
typedef signed long long	jlong;
#define NOTIMEOUT	0
#define true		1
#define false		0

#define catchSignal(s, h)	signal(s, h)

static jlong currentTime()
{
    struct timeval tm;
    gettimeofday(&tm, 0);
    return (((jlong)tm.tv_sec * 1000L) + ((jlong)tm.tv_usec / 1000L));
}

/* let main thread loop until all threads finish, for tests */
void 	jthread_exit_when_done();

/*======== end of definitions that apply to FreeBSD user mode only ========*/

#endif  /* !KVER */

/*
 * a mutex is represented by a two-word structure
 */
typedef struct _jmutex
{
        struct _jthread *holder;
        struct _jthread *waiting;
} jmutex;

/*
 * a condition variable is simply expressed as a list of threads
 * waiting to be notified
 */
typedef struct _jthread *jcondvar;

/*
 * This is our internal structure representing the "native" threads.
 * This used to be called "ctx".
 */
typedef struct _jthread {
	unsigned char			status;
	unsigned char			priority;
	void*				restorePoint;
	void*				stackBase;
	void*				stackEnd;
	jlong				time;
	struct _jthread*		nextQ;
	struct _jthread*		nextlive;
	struct _jthread*		nextalarm;
	struct _jthread**		blockqueue;
	unsigned char			flags;
	void				(*func)(void *);
	int				daemon;

	/* this one is simply thread specific data, or a cookie -
	 * used to hold the current Java thread
	 */
	void*				jlThread;
	jmp_buf				env;
	/* for alignment (Gcc extension) */
	double				align[0];
} jthread;

#define GET_COOKIE(jtid)	((jtid)->jlThread)

/****************************************************************************
 *
 * The following functions form the interface provided by the 
 * revised internal threading system.
 */

/* 
 * initialize the threading system
 */
jthread *
jthread_init(
	int preemptive,			/* preemptive scheduling */
	int maxpr, 			/* maximum priority */
	int minpr, 			/* minimum priority */
	int mainthreadpr,		/* priority of current (main) thread */
	void *jlmainThread,     	/* cookie for main thread */
	size_t mainThreadStackSize,	/* assumed main stack size */
        void *(*_allocator)(size_t),	/* memory allocator */
	void (*_deallocator)(void*),	/* memory deallocator */
	void (*_destructor1)(void*),	/* called when a thread exits */ 
	void (*_onstop)());		/* called when a thread is stopped */

/*
 * create a thread with a given priority
 */
jthread *
jthread_create(unsigned char pri, 	/* initial priority */
	void (*func)(void *), 		/* start function */
	int daemon, 			/* is this thread a daemon? */
	void *jlThread, 		/* cookie for this thread */
	size_t threadStackSize);	/* stack size to be allocated */

/*
 * set a function to be run when last non-daemon dies 
 * this is used to run the finalizer on exit.
 */
void 	jthread_atexit(void (*f)());

/*
 * walk all live threads, and invoke `func', passing in their cookie
 * this is used with `func' set to walkMemory
 */
void 	jthread_walkLiveThreads(void (*func)(void *jlThread));

/* 
 * destroy this jthread structure 
 */
void	jthread_destroy(jthread *jtid);

/*
 * set the priority of a thread 
 */
void	jthread_setpriority(jthread* jtid, int prio);

/*
 * yield to another thread
 */
void 	jthread_yield();

/*
 * sleep for time milliseconds
 */
void 	jthread_sleep(jlong time);

/* 
 * return the current thread 
 */
static inline jthread *
jthread_current() 
{ 
	extern jthread* currentJThread;
	return currentJThread; 
}

/* 
 * count the number of stack frames - unimplemented 
 */
int 	jthread_frames(jthread *thrd);

/* 
 * return whether this thread is alive or not
 */
int 	jthread_alive(jthread *jtid);

/*
 * stop this thread
 */
void 	jthread_stop(jthread *jtid);

/*
 * have the current thread exit
 */
void 	jthread_exit();

/*
 * determine whether a location is on the stack of the current thread
 * (was FRAMEOKAY)
 */
int 	jthread_on_current_stack(void *bp);

/*
 * determine the "interesting" stack range a conservative gc must walk
 */
void jthread_extract_stack(jthread *jtid, void **from, unsigned *len);

/*
 * functions to disable and restore interrupts
 */
void 	intsDisable();
void 	intsRestore();
void	intsRestoreAll();
int 	intsDisabled();

/*
 * API related to I/O
 */
int jthreadedFileDescriptor(int fd);
int jthreadedOpen(const char* path, int flags, int mode);
int jthreadedSocket(int af, int type, int proto);
int jthreadedConnect(int fd, struct sockaddr* addr, size_t len);
int jthreadedAccept(int fd, struct sockaddr* addr, size_t* len);
ssize_t jthreadedRead(int fd, void* buf, size_t len);
ssize_t jthreadedWrite(int fd, const void* buf, size_t len);
ssize_t jthreadedRecvfrom(int fd, void* buf, size_t len, int flags,
        struct sockaddr* from, int* fromlen);
int jthreadedWaitpid(int wpid, int* status, int options);

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
