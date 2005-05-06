/*
 * signal.c
 * Implement signal-based exception handling
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-mem.h"
#include "config-setjmp.h"
#include "jthread.h"
#include "md.h"

#if defined(INTERPRETER)
#define	DEFINEFRAME()		/* Does nothing */
#define	EXCEPTIONPROTO		int sig
#define	EXCEPTIONFRAME(f, c)	/* Does nothing */
#define	EXCEPTIONFRAMEPTR	0
#elif defined(TRANSLATOR)
#define	DEFINEFRAME()		exceptionFrame frame
#define	EXCEPTIONFRAMEPTR	&frame
#endif /* TRANSLATOR */

#define SIG_T   void*

static void nullException(EXCEPTIONPROTO);
static void floatingException(EXCEPTIONPROTO);

static exchandler_t nullHandler;
static exchandler_t floatingHandler;

/*
 * Setup a signal handler.
 */
void
catchSignal(int sig, void* handler)
{
	sigset_t nsig;

#if defined(HAVE_SIGACTION)

	struct sigaction newact;

	newact.sa_handler = (SIG_T)handler;
	sigemptyset(&newact.sa_mask);
	/* we cannot afford to have our signal handlers preempted before
	 * they are able to disable interrupts.
	 */
	sigaddset(&newact.sa_mask, SIGIO);
	sigaddset(&newact.sa_mask, SIGALRM);
#if defined(SIGVTALRM)
	sigaddset(&newact.sa_mask, SIGVTALRM);
#endif
#if defined(SIGUNUSED)
	sigaddset(&newact.sa_mask, SIGUNUSED);
#endif
	newact.sa_flags = 0;
#if defined(SA_SIGINFO)
	newact.sa_flags |= SA_SIGINFO;
#endif
	sigaction(sig, &newact, NULL);

#elif defined(HAVE_SIGNAL)

	signal(sig, (SIG_T)handler);

#else
	KAFFEVM_ABORT();
#endif

	/* Unblock this signal */
	sigemptyset(&nsig);
	sigaddset(&nsig, sig);
	sigprocmask(SIG_UNBLOCK, &nsig, 0);
}

/*
 * Setup the internal exceptions.
 */
void
jthread_initexceptions(exchandler_t _nullHandler,
		       exchandler_t _floatingHandler,
		       exchandler_t _stackOverflowHandler UNUSED)
{
	nullHandler = _nullHandler;
	floatingHandler = _floatingHandler;

	if (DBGEXPR(EXCEPTION, false, true)) {
		/* Catch signals we need to convert to exceptions */
#if defined(SIGSEGV)
		catchSignal(SIGSEGV, nullException);
#endif
#if defined(SIGBUS)
		catchSignal(SIGBUS, nullException);
#endif
#if defined(SIGFPE)
		catchSignal(SIGFPE, floatingException);
#endif
#if defined(SIGPIPE)
		catchSignal(SIGPIPE, SIG_IGN);
#endif
	}
}

/*
 * Null exception - catches bad memory accesses.
 */
static void
nullException(EXCEPTIONPROTO)
{
	DEFINEFRAME();

	/* These threads are internal to pthreads and shouldn't blow up like this. */
	assert(pthread_self() != 0);

	/* don't catch the signal if debugging exceptions */
	if (DBGEXPR(EXCEPTION, false, true)) {
		catchSignal(sig, nullException);
	}

	EXCEPTIONFRAME(frame, ctx);
	nullHandler(EXCEPTIONFRAMEPTR);
}

/*
 * Division by zero.
 */
static void
floatingException(EXCEPTIONPROTO)
{
	DEFINEFRAME();

	/* These threads are internal to pthreads and shouldn't blow up like this. */
	assert(pthread_self() != 0);

	/* don't catch the signal if debugging exceptions */
	if (DBGEXPR(EXCEPTION, false, true)) {
		catchSignal(sig, floatingException);
	}

	EXCEPTIONFRAME(frame, ctx);
	floatingHandler(EXCEPTIONFRAMEPTR);
}

/*
 * we don't really block/unblock any signals here
 */
void
blockAsyncSignals(void)
{
}

void
unblockAsyncSignals(void)
{
}
