/*
 * lock-impl.h
 *
 * Unix pthreads implements the jmutex and jcondvar abstractions.
 * Most of the functions are inlined.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __lock_impl_h
#define __lock_impl_h

#include <pthread.h>

typedef pthread_mutex_t jmutex;
typedef pthread_cond_t jcondvar;

/* prototypes for jmutex interfaces.  All inlined except jcondvar_wait() */

extern void KaffePThread_setBlockingCall(void *sigdata);
extern void KaffePThread_clearBlockingCall(void *sigdata);

static inline void jmutex_initialise( jmutex* lk ) __UNUSED__;
extern void jmutex_lock(jmutex* lk );
static inline void jmutex_unlock( jmutex* lk ) __UNUSED__;

static inline void jcondvar_initialise( jcondvar* cv ) __UNUSED__;
static inline void jcondvar_signal( jcondvar* cv, jmutex *mux ) __UNUSED__;
extern jboolean jcondvar_wait(jcondvar* cv, jmutex* mux, jlong timeout );


/* inline jmutex/jcondvar functions.  */

static inline
void
jmutex_initialise( jmutex* lk )
{
  pthread_mutexattr_t   muxAttr;

  /* init a process private mutex. We deal with priority inversion
   * by means of inheriting the highest priority of any thread who
   * requests the mutex
   */
  pthread_mutexattr_init( &muxAttr);
#if defined(_POSIX_THREAD_PROCESS_SHARED) && _POSIX_THREAD_PROCESS_SHARED != -1
  pthread_mutexattr_setpshared( &muxAttr, PTHREAD_PROCESS_PRIVATE);
#endif
#if defined(_POSIX_THREAD_PRIO_INHERIT_POSIX_THREAD_PRIO_PROTECT)
  pthread_mutexattr_setprotocol( &muxAttr, PTHREAD_PRIO_INHERIT);
#endif
  pthread_mutex_init( lk, &muxAttr);
}

static inline  
void    
jmutex_unlock( jmutex* lk )
{
  pthread_mutex_unlock(lk);
}

static inline
void
jmutex_destroy (jmutex* lk) 
{
  pthread_mutex_destroy( lk );
}


static inline
void
jcondvar_initialise( jcondvar* cv )
{
  pthread_condattr_t    cvAttr;

  /* init a process private condvar */
  pthread_condattr_init( &cvAttr);
#if defined(_POSIX_THREAD_PROCESS_SHARED) && _POSIX_THREAD_PROCESS_SHARED != -1
  pthread_condattr_setpshared( &cvAttr, PTHREAD_PROCESS_PRIVATE);
#endif
  pthread_cond_init( cv, &cvAttr);
}

static inline
void
jcondvar_signal( jcondvar* cv, UNUSED jmutex* mux)
{
  pthread_cond_signal( cv );
}       

static inline
void
jcondvar_broadcast ( jcondvar* cv, UNUSED jmutex* mux)
{
  pthread_cond_broadcast( cv );
}

static inline
void
jcondvar_destroy( jcondvar *cv )
{
  pthread_cond_destroy( cv );
}

#endif /* __lock_impl_h */
