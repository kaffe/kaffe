/*
 * ksem.h
 *
 * Copyright (c) 200
 *	University of Utah.  All Rights Reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef kaffevm_ksem_h
#define kaffevm_ksem_h

/*
 * The ksem interface.
 */
struct Ksem;

static inline void ksemInit(struct Ksem* sem) __UNUSED__;
static inline void ksemPut(struct Ksem* sem) __UNUSED__;
static inline jboolean ksemGet(struct Ksem* sem, jlong timeout) __UNUSED__;
static inline void ksemDestroy(struct Ksem* sem) __UNUSED__;

/*
 * Include the system locking layer interface.  See if it gives us
 * Ksem's or jmutex/jcondvar's (see FAQ.locks).
 */

#include "lock-impl.h"

/*
 * Threading packages for Kaffe have two choices for implementing
 * Ksems.  They can either implement the ksem interface directly
 * (useful if the underlying system has semaphore support).  Or they
 * can provide the jmutex/jcondvar interface.  See FAQ/FAQ.locks
 * for more information.
 *
 * If it doesn't provide KSEM, then assume it provides jmutex/jcondvar
 * and use that.
 */
#ifndef THREAD_SYSTEM_HAS_KSEM

/*
 * Present POSIX mutex+condvar as a binary semaphore.
 */
typedef struct Ksem {
	jmutex		mux;
	jcondvar	cv;
	int		count;
} Ksem;

/*
 * Initialize the just-allocated Ksem.  This function is only invoked
 * by the threading system when a new thread is allocated.
 */
static inline void
ksemInit(Ksem* sem)
{
	assert(sem);
	
	jmutex_initialise(&(sem->mux));
	jcondvar_initialise(&(sem->cv));
	sem->count = 0;
}

/*
 * Use a stored wakeup from the semaphore.  Block if none
 * are available.  Can wait with a timeout.  (If timeout is 0, then
 * do not timeout in wait.)
 * Returns true if the semaphore was acquired, returns false if
 * we timed-out in wait and semaphore still wasn't available.
 *
 * XXX spurious wakeups are not handled here.  Should they be?
 */
static inline jboolean
ksemGet(Ksem* sem, jlong timeout)
{
	jboolean r;

	assert(sem);
	
	r = true;

	jmutex_lock(&sem->mux);
	/* If no stored wakeups, then sleep. */
	if (sem->count == 0) {
		(void)jcondvar_wait(&sem->cv, &sem->mux, timeout);
	}

	/* Use a stored wakeup if available. */
	if (sem->count == 1) {
		sem->count = 0;
		r = true;
	}
	else {
		/* Still no stored wakeup means we waited and timeout. */
		assert(sem->count == 0);
		r = false;
	}
	jmutex_unlock(&sem->mux);
	return (r);
}

/*
 * Store a wakeup in the semaphore.  Wakeup one thread blocked
 * on the cv (if any).
 */
static inline void
ksemPut(Ksem* sem)
{
	jmutex_lock(&sem->mux);
	assert(sem->count == 0);
        sem->count = 1;
	jcondvar_signal(&sem->cv, &sem->mux);
	jmutex_unlock(&sem->mux);
}

static inline void
ksemDestroy(Ksem* sem)
{
	assert(sem);
	jmutex_destroy(&(sem->mux));
	jcondvar_destroy(&(sem->cv));
}



#endif /* !defined(JTHREAD_HAS_KSEM) */
#endif /* kaffevm_ksem_h */
