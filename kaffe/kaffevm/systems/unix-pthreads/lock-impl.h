/*
 * lock-impl.h - this is the part of the LockInterface implementation
 * we want to expose (by means of injection into the abstraction locks.h)
 *
 * Use it only for two purposes:
 *  (1) redefine generic interface function macros of locks.h in order
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

#ifndef __lock_impl_h
#define __lock_impl_h

/* bring in thread-impl stuff by means of threads.h, to prevent us
 * from subsequently overloading the more specific macros by the generic ones
 */ 
#include "thread.h"


/***********************************************************************
 * inlined LockInterface functions
 */

static inline
void
lLock ( sem2posixLock* lk )
{
  nativeThread  *cur = GET_CURRENT_THREAD(&cur);

  cur->stackCur  = (void*)&cur;
  cur->blockState |= BS_MUTEX;

  pthread_mutex_lock( (pthread_mutex_t*)lk->mux);

  cur->blockState &= ~BS_MUTEX;
}

static inline  
void    
lUnlock ( sem2posixLock* lk )
{
  pthread_mutex_unlock( (pthread_mutex_t*)lk->mux);
}

static inline
void
lSignal ( sem2posixLock* lk )
{
  pthread_cond_signal( (pthread_cond_t*)lk->cv);
}       

extern jboolean Lwait ( sem2posixLock* lk, jlong timeout );

#define SEMGET          _SemGet
#define SEMPUT          _SemPut
#define LOCK(L)         lLock(L)
#define UNLOCK(L)       lUnlock(L)
#define SIGNAL(L)       lSignal(L)
#define WAIT(L,T)       Lwait((L), (T))

#define SETUP_POSIX_LOCKS(X)

#endif /* __lock_impl_h */
