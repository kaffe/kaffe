/*
 * lock-impl.c - pthread-based LockInterface implementation (Posix style)
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <pthread.h>

#include "config.h"
#include "config-std.h"
#include "gtypes.h"

#include "thread.h"
#include "locks.h"


/***********************************************************************
 * Interface functions
 *
 * All of them should eventually turned into compile time interfaces
 * (exported inlined funcs)
 */

/*
 * Lock the mutex associated with the sem2posixLock
 */
void  
Llock ( sem2posixLock* lk )
{
  lLock( lk);
}

/*
 * Unlock the mutex associated with the sem2posixLock
 */
void    
Lunlock ( sem2posixLock* lk )
{
  lUnlock( lk);
}

/*
 * Wait for the condvar associated with the sem2posixLock, with
 * a given relative timeout in ms (which we have to convert into
 * a absolute timespec now)
 */
jboolean
Lwait ( sem2posixLock* lk, jlong timeout )
{
  nativeThread    *cur = GET_CURRENT_THREAD(&cur);
  int             stat;
  struct timespec abst;
  struct timeval  now;

  //CHECK_LOCK( cur,lk);

  cur->stackCur  = (void*)&cur;

  if ( timeout == 0 ) {
	/* we handle this as "wait forever"	*/
	cur->blockState |= BS_CV;
	stat = pthread_cond_wait( (pthread_cond_t*)lk->cv, (pthread_mutex_t*)lk->mux);
	cur->blockState &= ~BS_CV;
  }
  else {
	/* timeout is in millisecs, timeval in microsecs, and timespec in nanosecs */
	gettimeofday( &now, 0);
	abst.tv_sec = now.tv_sec + (timeout / 1000);
	abst.tv_nsec = (now.tv_usec * 1000) + (timeout % 1000) * 1000000;
 
	cur->blockState |= BS_CV_TO;
	stat = pthread_cond_timedwait( (pthread_cond_t*)lk->cv, (pthread_mutex_t*)lk->mux, &abst);
	cur->blockState &= ~BS_CV_TO;
  }
  
  return (stat == 0);
}

/*
 * Signal the condvar associated with the sem2posixLock, to wake one of
 * the threads waiting on it
 */
void    
Lsignal ( sem2posixLock* lk )
{
  lSignal( lk);
}       
