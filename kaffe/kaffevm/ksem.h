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

#include "config-std.h"
#include "gtypes.h"
#include "jsyscall.h"

/*
 * The ksem interface.
 */
struct Ksem;

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

extern void ksem_init(struct Ksem* sem);
extern void ksem_put(volatile struct Ksem* sem);
extern jboolean ksem_get(volatile struct Ksem* sem, jlong timeout);
extern void ksem_destroy(struct Ksem* sem);

/*
 * Present POSIX mutex+condvar as a binary semaphore.
 */
typedef struct Ksem {
	jmutex		mux;
	jcondvar	cv;
	int		count;
} Ksem;

#endif /* !defined(JTHREAD_HAS_KSEM) */
#endif /* kaffevm_ksem_h */
