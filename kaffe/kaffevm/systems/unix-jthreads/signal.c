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
#include "jsignal.h"
#include "md.h"
#include "gc.h"

#if defined(INTERPRETER)
#define	DEFINEFRAME()		/* Does nothing */
#define	EXCEPTIONPROTO		int sig
#define	EXCEPTIONFRAME(f, c)	/* Does nothing */
#define	EXCEPTIONFRAMEPTR	0
#elif defined(TRANSLATOR)
#define	DEFINEFRAME()		exceptionFrame frame
#define	EXCEPTIONFRAMEPTR	&frame
#endif /* TRANSLATOR */

static void nullException(EXCEPTIONPROTO);
static void floatingException(EXCEPTIONPROTO);

static exchandler_t nullHandler;
static exchandler_t floatingHandler;

/*
 * Setup the internal exceptions.
 */
void
jthread_initexceptions(exchandler_t _nullHandler,
		       exchandler_t _floatingHandler)
{
	nullHandler = _nullHandler;
	floatingHandler = _floatingHandler;

	if (DBGEXPR(EXCEPTION, false, true)) {
		/* Catch signals we need to convert to exceptions */
#if defined(SIGSEGV)
		registerSyncSignalHandler(SIGSEGV, nullException);
#endif
#if defined(SIGBUS)
		registerSyncSignalHandler(SIGBUS, nullException);
#endif
#if defined(SIGFPE)
		registerSyncSignalHandler(SIGFPE, floatingException);
#endif
#if defined(SIGPIPE)
		ignoreSignal(SIGPIPE);
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
#ifdef __FreeBSD__
	if ((uintp) ctx->sc_err > gc_heap_base) {
		dprintf("accessing free page %p (above %p)\n",
			(void*) ctx->sc_err, (void *) gc_heap_base);
		abort();
	}
#endif
	/* Restore the signal handler if necessary */
	restoreSyncSignalHandler(sig, nullException);

	/* Clean up the synchronous signal state (just unblock this signal) */
	unblockSignal(sig);

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

	/* Restore the signal handler if necessary */
	restoreSyncSignalHandler(sig, floatingException);

	/* Clean up the synchronous signal state (just unblock this signal) */
	unblockSignal(sig);

	EXCEPTIONFRAME(frame, ctx);
	floatingHandler(EXCEPTIONFRAMEPTR);
}

/* -----------------------------------------------
 * OS signal handling code.  See FAQ/FAQ.jsignal for information.
 * ----------------------------------------------- */

static void
registerSignalHandler(int sig, void* handler, int isAsync)
{
#if defined(HAVE_SIGACTION)
	struct sigaction newact;

	newact.sa_handler = (SIG_T)handler;
	sigemptyset(&newact.sa_mask);

	/*
	 * Define sa_mask to include the signals to block when
	 * running handler.
	 */
	if (isAsync) {
		sigaddset(&newact.sa_mask, SIGIO);
		sigaddset(&newact.sa_mask, SIGALRM);
		sigaddset(&newact.sa_mask, SIGCHLD);
#if defined(SIGVTALRM)
		sigaddset(&newact.sa_mask, SIGVTALRM);
#endif
	}

	newact.sa_flags = 0;
#if defined(SA_SIGINFO)
	newact.sa_flags |= SA_SIGINFO;
#endif
#if defined(SA_RESTART)
	newact.sa_flags |= SA_RESTART;
#endif
	sigaction(sig, &newact, NULL);

#elif defined(HAVE_SIGNAL)
#warning The default signal() semantics may not be sufficient for Kaffe.
	signal(sig, (SIG_T)handler);

#else
#error No signal handler support.  Jthreads requires signal support.	
#endif /* HAVE_SIGACTION */
}


/*
 * Register a handler for an asynchronous signal.
 */
void
registerAsyncSignalHandler(int sig, void* handler)
{
	int validSig = 
		(sig == SIGALRM)
#if defined(SIGVTALRM)
		|| (sig == SIGVTALRM) 
#endif
		|| (sig == SIGIO)
		|| (sig == SIGUSR1)
		|| (sig == SIGCHLD);

	/* Make sure its really an asynchronous signal being registered. */
	assert(handler != NULL);
	assert(validSig);  /* Can't have the #ifdef in a macro arg. */

	/*
	 * Register an asynchronous signal handler that will block all
	 * other asynchronous signals while the handler is running.
	 */
	registerSignalHandler(sig, handler, true);
}

/*
 * Register a signal handler for a synchronous signal.
 */
void
registerSyncSignalHandler(int sig, void* handler)
{
	int validSig = 0
#if defined(SIGFPE)	       
		|| (sig == SIGFPE)
#endif
#if defined(SIGSEGV)
		|| (sig == SIGSEGV)
#endif
#if defined(SIGBUS)
		|| (sig == SIGBUS)
#endif
		;
	
	assert(handler != NULL);
	assert(validSig);
	
	/* Register a synchronous signal handler */
	registerSignalHandler(sig, handler, false);
}

/*
 * Restore an asynchronous signal handler.  
 * Only necesary on some platforms which don't provide SIGACTION
 */
void
restoreAsyncSignalHandler(int sig, void* handler)
{
#if !defined(HAVE_SIGACTION)
	/* XXX need a configure-time test for this. */
	registerAsyncSignalHandler(sig, handler);
#endif
}


/*
 * Restore a synchronous signal handler.  
 * Only necesary on some platforms.
 */
void
restoreSyncSignalHandler(int sig, void* handler)
{
#if !defined(HAVE_SIGACTION)
	/* XXX need a configure-time test for this. */
	registerSyncSignalHandler(sig, handler);
#endif
}


/*
 * Register a handler for a terminal (i.e., process-killing) signal.
 * These handlers must exit().
 */
void
registerTerminalSignal(int sig, void* handler)
{
	assert((sig == SIGINT) || (sig == SIGTERM));
	registerSignalHandler(sig, handler, true);
}

/*
 * Clear the given signal (i.e., restore the default behavior of the signal)
 */
void
clearSignal(int sig)
{
	registerSignalHandler(sig, SIG_DFL, false);
}


/*
 * Ignore the given signal.
 */
void
ignoreSignal(int sig)
{
	registerSignalHandler(sig, SIG_IGN, false);
}

/*
 * Unblock the given signal.
 */
void
unblockSignal(int sig)
{
	sigset_t nsig;

	sigemptyset(&nsig);
	sigaddset(&nsig, sig);
	sigprocmask(SIG_UNBLOCK, &nsig, 0);
}


/*
 * Unblock all of the async signals.  (These are the
 * signals specified in the sa_mask in registerAsynchSignalHandler().)
 */
void 
unblockAsyncSignals(void)
{
	sigset_t nsig;

	sigemptyset(&nsig);
	sigaddset(&nsig, SIGIO);
	sigaddset(&nsig, SIGALRM);
	sigaddset(&nsig, SIGCHLD);
#if defined(SIGVTALRM)
	sigaddset(&nsig, SIGVTALRM);
#endif
	sigprocmask(SIG_UNBLOCK, &nsig, 0);
	
}

/*
 * Block all of the async signals.  (These are the
 * signals specified in the sa_mask in registerAsynchSignalHandler().)
 */
void 
blockAsyncSignals(void)
{
	sigset_t nsig;

	sigemptyset(&nsig);
	sigaddset(&nsig, SIGIO);
	sigaddset(&nsig, SIGALRM);
	sigaddset(&nsig, SIGCHLD);
#if defined(SIGVTALRM)
	sigaddset(&nsig, SIGVTALRM);
#endif
	sigprocmask(SIG_BLOCK, &nsig, 0);
	
}
