/*
 * ksem.h
 *
 * Copyright (c) 200
 *	University of Utah.  All Rights Reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "ksem.h"

/*
 * Initialize the just-allocated Ksem.  This function is only invoked
 * by the threading system when a new thread is allocated.
 */
void
ksemInit(Ksem* sem)
{
	assert(sem != NULL);
	
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
 * Spurious wakeups are not handled here.
 */
jboolean
ksemGet(Ksem* volatile sem, jlong timeout)
{
	jboolean r;

	assert(sem != NULL);
	
	r = true;

	if (timeout == 0)
		timeout = NOTIMEOUT;

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
		/* Still no stored wakeup means we waited and timedout. */
		r = false;
	}
	assert(sem->count == 0);
	jmutex_unlock(&sem->mux);
	return (r);
}

/*
 * Store a wakeup in the semaphore.  Wakeup one thread blocked
 * on the cv (if any).
 */
void
ksemPut(Ksem* volatile sem)
{
	assert(sem != NULL);
	jmutex_lock(&sem->mux);
        sem->count = 1;
	jcondvar_signal(&sem->cv, &sem->mux);
	jmutex_unlock(&sem->mux);
}

void
ksemDestroy(Ksem* sem)
{
	assert(sem != NULL);
	jmutex_destroy(&(sem->mux));
	jcondvar_destroy(&(sem->cv));
}
