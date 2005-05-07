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
#include "files.h"
#include "kaffe/jmalloc.h"

#if defined(INTERPRETER)
#define	DEFINEFRAME()		/* Does nothing */
#define	EXCEPTIONPROTO		int sig
#define	EXCEPTIONFRAME(f, c)	/* Does nothing */
#define	EXCEPTIONFRAMEPTR	0
#elif defined(TRANSLATOR)
#define	DEFINEFRAME()		exceptionFrame frame
#define	EXCEPTIONFRAMEPTR	&frame
#endif /* TRANSLATOR */

#ifndef STACK_STRUCT
#define STACK_STRUCT stack_t
#endif

static void nullException(SIGNAL_ARGS(sig, sc));
static void floatingException(EXCEPTIONPROTO);

static exchandler_t nullHandler;
static exchandler_t floatingHandler;
static exchandler_t stackOverflowHandler;

/*
 * Setup the internal exceptions.
 */
void
jthread_initexceptions(exchandler_t _nullHandler,
		       exchandler_t _floatingHandler,
		       exchandler_t _stackOverflowHandler)
{
	nullHandler = _nullHandler;
	floatingHandler = _floatingHandler;
	stackOverflowHandler = _stackOverflowHandler;

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
		KaffeJThread_ignoreSignal(SIGPIPE);
#endif
	}
}

/*
 * Null exception - catches bad memory accesses.
 */
static void
nullException(SIGNAL_ARGS(sig, ctx))
{
        void *stackptr;
  
	DEFINEFRAME();
	/* Restore the signal handler if necessary */
	restoreSyncSignalHandler(sig, nullException);

	/* Clean up the synchronous signal state (just unblock this signal) */
	unblockSignal(sig);

	EXCEPTIONFRAME(frame, ctx);
#if defined(STACK_POINTER)
	stackptr = (void *)STACK_POINTER(GET_SIGNAL_CONTEXT_POINTER(ctx));
#if defined(STACK_GROWS_UP)
	if (currentJThread != NULL && stackptr >= currentJThread->stackEnd)
#else
	if (currentJThread != NULL && stackptr <= currentJThread->stackBase)
#endif
	  stackOverflowHandler(EXCEPTIONFRAMEPTR);
	else
#endif // STACK_POINTER
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

static void *
registerSignalHandler(int sig, void* handler, int isAsync)
{
#if defined(HAVE_SIGACTION)
	struct sigaction newact;
	struct sigaction oldact;

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
#if defined(SA_ONSTACK)
	if (sig == SIGSEGV)
	  newact.sa_flags |= SA_ONSTACK;
#endif
#if defined(SA_SIGINFO)
	newact.sa_flags |= SA_SIGINFO;
#endif
#if defined(SA_RESTART)
	newact.sa_flags |= SA_RESTART;
#endif
	sigaction(sig, &newact, &oldact);

	return oldact.sa_handler;
#elif defined(HAVE_SIGNAL)
#warning The default signal() semantics may not be sufficient for Kaffe.
	return signal(sig, (SIG_T)handler);

#else
#error No signal handler support.  Jthreads requires signal support.	
#endif /* HAVE_SIGACTION */
}


/*
 * Register a handler for an asynchronous signal.
 */
void *
registerAsyncSignalHandler(int sig, void* handler)
{
#if !defined(NDEBUG)
	/* Only used in assert. */

	int validSig = 
		(sig == SIGALRM)
#if defined(SIGVTALRM)
		|| (sig == SIGVTALRM) 
#endif /* defined(SIGVTALRM) */
		|| (sig == SIGIO)
		|| (sig == SIGUSR1)
		|| (sig == SIGUSR2)
		|| (sig == SIGCHLD);
#endif /* !defined(NDEBUG) */

	/* Make sure its really an asynchronous signal being registered. */
	assert(handler != NULL);
	assert(validSig);  /* Can't have the #ifdef in a macro arg. */

	/*
	 * Register an asynchronous signal handler that will block all
	 * other asynchronous signals while the handler is running.
	 */
	return registerSignalHandler(sig, handler, true);
}

/*
 * Register a signal handler for a synchronous signal.
 */
void *
registerSyncSignalHandler(int sig, void* handler)
{
#if !defined(NDEBUG)
        /* Only used in assert. */

	int validSig = 0
#if defined(SIGFPE)	       
		|| (sig == SIGFPE)
#endif /* defined(SIGFPE) */
#if defined(SIGSEGV)
		|| (sig == SIGSEGV)
#endif /* defined(SIGSEGV) */
#if defined(SIGBUS)
		|| (sig == SIGBUS)
#endif /* defined(SIGBUS) */
		;
#endif /* !defined(NDEBUG) */
	
	assert(handler != NULL);
	assert(validSig);
	
	/* Register a synchronous signal handler */
	return registerSignalHandler(sig, handler, false);
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
KaffeJThread_ignoreSignal(int sig)
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

#if defined(HAVE_SIGALTSTACK) && defined(SA_ONSTACK)
static void
setupSigAltStack(void)
{
        STACK_STRUCT newstack;
	void *stackp;

	/*
	 * Signals has to have their own stack so we can solve
	 * stack problems.
	 */
	newstack.ss_size = THREADSTACKSIZE;
	newstack.ss_flags = 0;
	stackp = KMALLOC(newstack.ss_size);
#if defined(SIGALTSTACK_NEEDS_END)
	newstack.ss_sp = (void *)((uintp)stackp + newstack.ss_size);
#else
	newstack.ss_sp = stackp;
#endif
	if (sigaltstack(&newstack, NULL) < 0)
	  {
	    dprintf("Unexpected error calling sigaltstack: %s\n",
		    SYS_ERROR(errno));
	    KAFFEVM_EXIT(1);
	  }
}
#else
static void
setupSigAltStack(void)
{
}
#endif

/* ----------------------------------------------------------------------
 * STACK BOUNDARY DETECTORS
 * ----------------------------------------------------------------------
 */

#if defined(KAFFEMD_STACKBASE) // STACK_POINTER

/*
 * The OS gives us the stack base. Get it and adjust the pointers.
 */

void
detectStackBoundaries(jthread_t jtid, int mainThreadStackSize)
{
        void *stackPointer;

	stackPointer = mdGetStackBase();

	setupSigAltStack();

	jtid->stackBase = stackPointer;
	jtid->stackEnd = (char *)jtid->stackBase + mainThreadStackSize;
        jtid->restorePoint = jtid->stackEnd;

}

#elif defined(KAFFEMD_STACKEND) // KAFFEMD_STACKBASE

/*
 * Here the OS gives us the position of the end of stack. Get it
 * and adjust our internal pointers.
 */

void
detectStackBoundaries(jthread_t jtid, int mainThreadStackSize)
{
        void *stackPointer;

	setupSigAltStack();

	stackPointer = mdGetStackEnd();

	jtid->stackEnd = stackPointer;
	jtid->stackBase = (char *)jtid->stackEnd - mainThreadStackSize;
        jtid->restorePoint = jtid->stackEnd;
}

#elif defined(SA_ONSTACK) && defined(HAVE_SIGALTSTACK) && !defined(KAFFEMD_BUGGY_STACK_OVERFLOW)

static JTHREAD_JMPBUF outOfLoop;

/*
 * This function is called by the system when we go beyond stack boundaries
 * in infiniteLoop. We get the stack address using the stack pointer register
 * and then go back in detectStackBoundaries() using the old stack.
 */
static void NONRETURNING
stackOverflowDetector(SIGNAL_ARGS(sig UNUSED, sc))
{
  unblockSignal(SIGSEGV);
  JTHREAD_LONGJMP(outOfLoop, 1);
}

void kaffeNoopFunc(char c UNUSED)
{
}

/*
 * This is the first type of heuristic we can use to guess the boundaries.
 * Here we are provoking a SIGSEGV by overflowing the stack. Then we get
 * the faulty adress directly.
 */
void
detectStackBoundaries(jthread_t jtid, int mainThreadStackSize)
{
	static volatile char *guessPointer;
	void *old_sigsegv, *old_sigbus;

	setupSigAltStack();

#if defined(SIGSEGV)
	old_sigsegv = registerSyncSignalHandler(SIGSEGV, stackOverflowDetector);
#endif
#if defined(SIGBUS)
	old_sigbus = registerSyncSignalHandler(SIGBUS, stackOverflowDetector);
#endif
	
	if (JTHREAD_SETJMP(outOfLoop) == 0)
	{
	  uintp pageSize = getpagesize();

	  guessPointer = (char *)((uintp)(&jtid) & ~(pageSize-1));
	  
	  while (1)
	  {
#if defined(STACK_GROWS_UP)
	    guessPointer -= pageSize;
#else
	    guessPointer += pageSize;
#endif
	    kaffeNoopFunc(*guessPointer);
	  }
	}

	/* Here we have detected one the boundary of the stack.
	 * If stack grows up then it is the upper boundary. In the other
	 * case we have the lower boundary. As we know the stack size we
	 * may guess the other boundary.
	 */
#if defined(STACK_GROWS_UP)
	jtid->stackBase = guessPointer;
	jtid->stackEnd = (char *)jtid->stackBase + mainThreadStackSize;
	jtid->restorePoint = jtid->stackEnd;
#else
	jtid->stackEnd = guessPointer;
	jtid->stackBase = (char *)jtid->stackEnd - mainThreadStackSize;
	jtid->restorePoint = jtid->stackBase;
#endif

#if defined(SIGSEGV)
	registerSignalHandler(SIGSEGV, old_sigsegv, false);
#endif
#if defined(SIGBUS)
	registerSignalHandler(SIGBUS, old_sigbus, false);
#endif
}

#else

/*
 * This is the worse heuristic in terms of precision. But
 * this may be the only one working on this platform.
 */

void
detectStackBoundaries(jthread_t jtid, int mainThreadStackSize)
{
#if defined(STACK_GROWS_UP)
	jtid->stackBase = (void*)(uintp)(&jtid - 0x100);
	jtid->stackEnd = (char *)jtid->stackBase + mainThreadStackSize;
        jtid->restorePoint = jtid->stackEnd;
#else
	jtid->stackEnd = (void*)(uintp)(&jtid + 0x100);
        jtid->stackBase = (char *) jtid->stackEnd - mainThreadStackSize;
        jtid->restorePoint = jtid->stackBase;
#endif
}

#endif
