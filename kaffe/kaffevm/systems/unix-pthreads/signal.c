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
#include "gtypes.h"
#include "jsignal.h"
#include "md.h"
#include "stackTrace-impl.h"
#include "thread-internal.h"
#include "files.h"
#include "kaffe/jmalloc.h"
#include "lerrno.h"
#ifdef KAFFE_BOEHM_GC
#include "boehm-gc/boehm/include/gc.h"
#endif

#if defined(INTERPRETER)
#define	DEFINEFRAME()		/* Does nothing */
#define	EXCEPTIONPROTO		int sig
#define	EXCEPTIONFRAME(f, c)	/* Does nothing */
#define	EXCEPTIONFRAMEPTR	0
#elif defined(TRANSLATOR)
#include "jit-md.h"
#define	DEFINEFRAME()		exceptionFrame frame
#define	EXCEPTIONFRAMEPTR	&frame
#endif /* TRANSLATOR */

#ifndef STACK_STRUCT
#define STACK_STRUCT stack_t
#endif

#if defined(__WIN32__)
#define SIG_T   void(*)()
#else
#define SIG_T   void*
#endif

static void ignoreSignal(int sig);

static exchandler_t nullHandler;
static exchandler_t floatingHandler;
static exchandler_t stackOverflowHandler;

/*
 * Null exception - catches bad memory accesses.
 */
static void
nullException(SIGNAL_ARGS(sig, ctx))
{
        void *stackptr;
	jthread_t current_thread;

	DEFINEFRAME();

	/* Restore the signal handler if necessary */
	restoreSyncSignalHandler(sig, nullException);

	/* Clean up the synchronous signal state (just unblock this signal) */
	unblockSignal(sig);

	EXCEPTIONFRAME(frame, ctx);
#if defined(STACK_POINTER)
	current_thread = jthread_current();
	stackptr = (void *)STACK_POINTER(GET_SIGNAL_CONTEXT_POINTER(ctx));
	/* Here we have a stupid heuristic which may not work rightfully
	 * if kaffe allocates a big buffer using alloca (in that case we
	 * will get an NPE). But it is better than the previous case which was
	 * heating nearly all NPEs on Darwin.
	 */
#if defined(STACK_GROWS_UP)
	if (current_thread != NULL && stackptr >= current_thread->stackMax &&
	    stackptr <= (void *)((uintp)current_thread->stackMax+1024))
#else
	if (current_thread != NULL && stackptr <= current_thread->stackMin &&
	    stackptr >= (void *)((uintp)current_thread->stackMax-1024))
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
		ignoreSignal(SIGPIPE);
#endif
	}
}


/* -----------------------------------------------
 * OS signal handling code.  See FAQ/FAQ.jsignal for information.
 * ----------------------------------------------- */

static void *
registerSignalHandler(int sig, void* handler, int isAsync)
{
#if defined(HAVE_SIGACTION)
	struct sigaction newact, oldact;

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
	return registerSignalHandler(sig, handler, true);
}

/*
 * Register a signal handler for a synchronous signal.
 */
void *
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
	return registerSignalHandler(sig, handler, false);
}

/*
 * Restore an asynchronous signal handler.  
 * Only necesary on some platforms which don't provide SIGACTION
 */
#if defined(HAVE_SIGACTION)
void
restoreAsyncSignalHandler(int sig UNUSED, void* handler UNUSED)
{
}
#else
void
restoreAsyncSignalHandler(int sig, void* handler)
{
	/* XXX need a configure-time test for this. */
	registerAsyncSignalHandler(sig, handler);
}
#endif

/*
 * Restore a synchronous signal handler.  
 * Only necesary on some platforms.
 */
#if defined(HAVE_SIGACTION)
void
restoreSyncSignalHandler(int sig UNUSED, void* handler UNUSED)
{
}
#else
void
restoreSyncSignalHandler(int sig, void* handler)
{
	/* XXX need a configure-time test for this. */
	registerSyncSignalHandler(sig, handler);
}
#endif

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
static void
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
	sigprocmask(SIG_UNBLOCK, &nsig, NULL);
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
	sigprocmask(SIG_UNBLOCK, &nsig, NULL);
	
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
	sigprocmask(SIG_BLOCK, &nsig, NULL);
	
}

/* ----------------------------------------------------------------------
 * STACK BOUNDARY DETECTORS
 * ----------------------------------------------------------------------
 */
#if defined(KAFFE_BOEHM_GC)

/*
 * The Boehm-Weiser GC has already the stack bottom. Use the
 * value it has detected.
 */
void
detectStackBoundaries(jthread_t jtid, size_t mainThreadStackSize)
{
        void *stackPointer;

	stackPointer = GC_stackbottom;

	jtid->stackMax = stackPointer;
	jtid->stackMin = (char *)jtid->stackMax - mainThreadStackSize;
        jtid->stackCur = jtid->stackMax;
}

#elif defined(KAFFEMD_STACKBASE)

/*
 * The OS gives us the stack base. Get it and adjust the pointers.
 */

void
detectStackBoundaries(jthread_t jtid, size_t mainThreadStackSize)
{
        void *stackPointer;

	stackPointer = mdGetStackBase();

	jtid->stackMin = stackPointer;
	jtid->stackMax = (char *)jtid->stackMin + mainThreadStackSize;
        jtid->stackCur = jtid->stackMax;
}

#elif defined(KAFFEMD_STACKEND) // KAFFEMD_STACKBASE

/*
 * Here the OS gives us the position of the end of stack. Get it
 * and adjust our internal pointers.
 */

void
detectStackBoundaries(jthread_t jtid, size_t mainThreadStackSize)
{
        void *stackPointer;

	stackPointer = mdGetStackEnd();

	jtid->stackMax = stackPointer;
	jtid->stackMin = (char *)jtid->stackMax - mainThreadStackSize;
        jtid->stackCur = jtid->stackMax;
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
detectStackBoundaries(jthread_t jtid, size_t mainThreadStackSize)
{
	static volatile char *guessPointer;
	void *handler_segv, *handler_bus;

#if defined(SIGSEGV)
	handler_segv = registerSyncSignalHandler(SIGSEGV, stackOverflowDetector);
#endif
#if defined(SIGBUS)
	handler_bus = registerSyncSignalHandler(SIGBUS, stackOverflowDetector);
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
	jtid->stackMin = guessPointer;
	jtid->stackMax = (char *)jtid->stackMin + mainThreadStackSize;
	jtid->stackCur = jtid->stackMax;
#else
	jtid->stackMax = guessPointer;
	jtid->stackMin = (char *)jtid->stackMax - mainThreadStackSize;
	jtid->stackCur = jtid->stackMin;
#endif

	/* We restore the old signal handlers (SIG_DFL and SIG_IGN are included.
	 */ 
#if defined(SIGSEGV)
	registerSignalHandler(SIGSEGV, handler_segv, false);
#endif
#if defined(SIGBUS)
	registerSignalHandler(SIGBUS, handler_bus, false);
#endif
}

#else

/*
 * This is the worse heuristic in terms of precision. But
 * this may be the only one working on this platform.
 */

void
detectStackBoundaries(jthread_t jtid, size_t mainThreadStackSize)
{
#if defined(STACK_GROWS_UP)
	jtid->stackMin = (void*)(uintp)(&jtid - 0x100);
	jtid->stackMax = (char *)jtid->stackMin + mainThreadStackSize;
        jtid->stackCur = jtid->stackMax;
#else
	jtid->stackMax = (void*)(uintp)(&jtid + 0x100);
        jtid->stackMin = (char *) jtid->stackMax - mainThreadStackSize;
        jtid->stackCur = jtid->stackMin;
#endif
}

#endif
