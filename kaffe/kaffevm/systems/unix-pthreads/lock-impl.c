/*
 * lock-impl.c - pthread-based LockInterface implementation (Posix style)
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include <errno.h>
#include "debug.h"

#include "jthread.h"
/* For NOTIMEOUT */
#include "jsyscall.h"
#ifdef KAFFE_BOEHM_GC
#include "boehm-gc/boehm/include/gc.h"
#endif

void
jmutex_lock( jmutex* lk )
{
  jthread_t cur = jthread_current ();

  cur->stackCur  = (void*)&cur;
  cur->blockState |= BS_MUTEX;

  pthread_mutex_lock( lk );

  cur->blockState &= ~BS_MUTEX;
}

/*
 * Wait on the condvar with a given relative timeout in ms (which we
 * have to convert into a absolute timespec now)
 */
jboolean
jcondvar_wait ( jcondvar* cv, jmutex *mux, jlong timeout )
{
  jthread_t cur = jthread_current();
  int             status;
  struct timespec abst;
  struct timeval  now;

  //CHECK_LOCK( cur,lk);

  cur->stackCur  = (void*)&cur;
  jthread_current()->interrupting = 0;

  if ( timeout == NOTIMEOUT ) {
	/* we handle this as "wait forever"	*/
	cur->blockState |= BS_CV;
	status = pthread_cond_wait( cv, mux );
	cur->blockState &= ~BS_CV;
  }
  else {
	/* timeout is in millisecs, timeval in microsecs, and timespec in nanosecs */
	gettimeofday( &now, 0);
	abst.tv_sec = now.tv_sec + (timeout / 1000);
	if( abst.tv_sec < now.tv_sec ) {
	    /* huge timeout value, we handle this as "wait forever" */
	    cur->blockState |= BS_CV;
	    status = pthread_cond_wait( cv, mux );
	    cur->blockState &= ~BS_CV;
	}
	else {
	    abst.tv_nsec = (now.tv_usec * 1000) + (timeout % 1000) * 1000000;
	    
	    if (abst.tv_nsec > 1000000000) {
                abst.tv_sec  += 1;
                abst.tv_nsec -= 1000000000;
	    }
	    
	    cur->blockState |= BS_CV_TO;
	    status = pthread_cond_timedwait( cv, mux, &abst);
	    cur->blockState &= ~BS_CV_TO;
	}

  }
  jthread_current()->interrupting = (status == EINTR);
  
  return (status == 0);
}
