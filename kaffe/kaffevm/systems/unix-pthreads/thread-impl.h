/*
 * thread-impl.h - this is the part of the ThreadInterface implementation
 * we want to expose (by means of injection into the abstraction threads.h)
 *
 * Use it only for two purposes:
 *  (1) redefine generic interface function macros of threads.h in order
 *      to turn the runtime interface (completely or partly) into a
 *      compile-time interface
 *  (2) VERY RESTRICTIVE - define macros which can be used in clients to
 *      check for a certain subsystem (but JUST if there are strong reasons
 *      against using a clean interface function instead)
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __thread_impl_h
#define __thread_impl_h

/* always make sure we see the generic macros first */
#include "thread.h"

/* Get us the required internal types we need for our inline funcs. This seems
 * to be the wrong order (impl including/exposing internal), but there's no way
 * around that if we want any benefit from inlines/macros
 */
#include "thread-internal.h"

#if !defined(STACKREDZONE)
#define STACKREDZONE    8192
#endif

#define	DBG_ACTION(X,Y)

/***********************************************************************************
 * define the interface funtions we would like to provide as inlines
 */

static inline
void
jthread_yield (void)
{
  DBG_ACTION( vm_thread, {
	nativeThread *cur = GET_CURRENT_THREAD( &cur);
	DBG( vm_thread, TMSG_SHORT( "yield ", cur));
  });

  sched_yield();
}

static inline          
Hjava_lang_Thread*      
tCurrentJava (void)      
{
  nativeThread *cur = GET_CURRENT_THREAD( &cur);

  return cur->thread;
}


static inline          
nativeThread*
tCurrentNative(void)      
{
  nativeThread *cur = GET_CURRENT_THREAD( &cur);
  return (cur);
}

static inline          
nativeThread*
jthread_current(void)      
{
	return (tCurrentNative());
}

static inline
void
jthread_spinon(int dummy)
{
}

static inline
void
jthread_spinoff(int dummy)
{
}

extern void jthread_init(
        int preemptive,                 /* preemptive scheduling */
        int maxpr,                      /* maximum priority */
        int minpr,                      /* minimum priority */
        void *(*_allocator)(size_t),    /* memory allocator */
        void (*_deallocator)(void*),    /* memory deallocator */
        void (*_destructor1)(void*),    /* called when a thread exits */
        void (*_onstop)(void),          /* called when a thread is stopped */
        void (*_ondeadlock)(void));     /* called when we detect deadlock */
extern nativeThread* jthread_createfirst(size_t, unsigned char, void*);
extern nativeThread* jthread_create (
	unsigned char pri, void* func, int daemon,
	void* jlThread, size_t threadStackSize );
extern void jthread_setpriority (nativeThread* thread, jint prio);
extern void jthread_exit ( void );
extern void jthread_destroy (nativeThread* thread);
extern void jthread_suspendall (void);
extern void jthread_unsuspendall (void);
extern void jthread_walkLiveThreads (void(*)(void*));

static inline
jbool
jthread_on_current_stack(void* p)
{
  nativeThread* nt = GET_CURRENT_THREAD( &nt);
  if (nt == 0 || (p > nt->stackMin && p < nt->stackMax)) {
	return (true);
  }
  else {
	return (false);
  }
}

static inline
int
jthread_extract_stack(nativeThread* nt, void** from, unsigned* len)
{
  if (nt->active == 0) {
    return (0);
  }
  assert(nt->suspendState == SS_SUSPENDED);
#if defined(STACK_GROWS_UP)
  *from = nt->stackMin;
  *len = nt->stackCur - nt->stackMin;
#else
  *from = nt->stackCur;
  *len = nt->stackMax - nt->stackCur;
#endif
  return (1);
}

static inline
void*
jthread_stacklimit(void)
{
  nativeThread* nt = GET_CURRENT_THREAD( &nt);
#if defined(STACK_GROWS_UP)
  return (nt->stackMax - STACKREDZONE);
#else
  return (nt->stackMin + STACKREDZONE);
#endif
}

static inline
void
jthread_disable_stop(void)
{
}

static inline
void
jthread_enable_stop(void)
{
}

static inline
void
jthread_stop(jthread_t tid)
{
}

static inline
void
jthread_interrupt(jthread_t tid)
{
}

static inline
void
jthread_atexit(void* func)
{
}

static inline
void
jthread_dumpthreadinfo(nativeThread* tid)
{
}

static inline
void*
jthread_getcookie(jthread_t tid)
{
        return (tid->thread);
}


/***********************************************************************************
 * defines for identifying the subsystem. NOTE - using this violates the interface
 * encapsulation, and is just justified if we otherwise would end up with new,
 * non-orthogonal interface functions
 */

#define UNIX_PTHREADS	1

#endif /* __thread_impl_h */
