/*
 * locks.c
 * Manage locking system
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "object.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "thread.h"
#include "locks.h"
#include "ksem.h"
#include "errors.h"
#include "exception.h"
#include "md.h"
#include "jthread.h"
#include "debug.h"
#include "gc.h"
#include "jvmpi_kaffe.h"

/*
 * If we don't have an atomic compare and exchange defined then make
 * one out of a simple atomic exchange (using the LOCKINPROGRESS value
 * as the place holder).  If we don't have ATOMIC_EXCHANGE, we'll just
 * fake it.
 */
#if !defined(COMPARE_AND_EXCHANGE)
#if defined(ATOMIC_EXCHANGE)
#define	COMPARE_AND_EXCHANGE(A,O,N) \
	({ \
		iLock* val = LOCKINPROGRESS; \
		ATOMIC_EXCHANGE((A), val); \
		if (val == (O)) { \
			*(A) = (N); \
		} \
		else { \
			*(A) = (O); \
		} \
		(val == (O) ? 1 :  0); \
	})
#else
#error Please define COMPARE_AND_EXCHANGE or ATOMIC_EXCHANGE 
#endif
#endif

/* Count number of backoffs.  XXX move into the STAT infrastructure. */
int backoffcount = 0;

/*
 * Initialise the locking system.
 */
void
initLocking(void)
{
}

#if defined(KAFFE_VMDEBUG)
void
dumpObjectLocks(void)
{
}
#endif

/*
 * Get a heavy lock for the object and hold it.  If the object doesn't
 * have a heavy lock then we create one.
 */
static
iLock*
getHeavyLock(iLock* volatile * lkp, iLock *heavyLock)
{
	iLock* old;
	iLock* lk;
	jlong timeout;

DBG(SLOWLOCKS,
    	dprintf("  getHeavyLock(**lkp=%p, *lk=%p, th=%p)\n",
		lkp, *lkp, KTHREAD(current)());
);
 
	lk = heavyLock;
	timeout = 1;
	for (;;) {
		/* Get the current lock and replace it with LOCKINPROGRESS to indicate
		 * changes are in progress.
		 */
		old = *lkp;
		if (old == LOCKINPROGRESS || !COMPARE_AND_EXCHANGE(lkp, old, LOCKINPROGRESS)) {
			/* Someone else put the lock in LOCKINPROGRESS state */
			backoffcount++;
			KSEM(get)(&THREAD_DATA()->sem, timeout);
			/* Back off */
			timeout = (timeout << 1)|timeout;
			continue;
		}

		/* If bottom bit is set, strip off and use pointer as pointer to heavy lock */
		if ((((uintp)old) & 1) == 1) {
DBG(SLOWLOCKS,
    			dprintf("    got cached lock\n");
);
			if (lk != heavyLock) {
				gc_free (lk);
			}

			lk = (iLock*)(((uintp)old) & (uintp)-2);
		}
		else {
			if (lk == LOCKFREE) {
				/* Release the lock before we go into malloc.
				 * We have to reclaim the lock afterwards (at beginning
				 * of loop)
				 */
				*lkp = old;
				lk = (iLock*)gc_malloc(sizeof(iLock), KGC_ALLOC_LOCK);
				/* if that fails we're in trouble!! */
				assert(lk != 0);
				continue;
			}
DBG(SLOWLOCKS,
			dprintf("    got %s lock\n",
				(lk != heavyLock) ? "new" : "special");
);
			lk->holder = (void*)old;
			lk->mux = NULL;
			lk->cv = NULL;
		}
		return (lk);
	}
}

/*
 * Release the lock - only the one who has claimed it can do this
 * so there's no need for locked instructions.
 */
static
void
putHeavyLock(iLock** lkp, iLock* lk)
{
	assert(*lkp == LOCKINPROGRESS);

DBG(SLOWLOCKS,
	dprintf("  putHeavyLock(**lkp=%p, *lk=%p, th=%p)\n", 
		lkp, lk, KTHREAD(current)());
);

	if (lk == LOCKFREE) {
		*lkp = LOCKFREE;
	}
	else {
		*lkp = (iLock*)(1|(uintp)lk);
	}
}

/*
 * Slowly lock a mutex.  We get the heavy lock and lock that instead.
 * If we can't lock it we suspend until we can.
 */
static void
slowLockMutex(iLock** lkp, void* where, iLock *heavyLock)
{
	iLock* lk;
	jthread_t cur = KTHREAD(current) ();

DBG(SLOWLOCKS,
    	dprintf("slowLockMutex(**lkp=%p, where=%p, th=%p)\n",
	       lkp, where, KTHREAD(current)());
);
	KTHREAD(disable_stop)(); /* protect the heavy lock, and its queues */

	for (;;) {
		lk = getHeavyLock(lkp, heavyLock);

		/* If I hold the heavy lock then just keep on going */
		if (KTHREAD(on_current_stack)(lk->holder)) {
			putHeavyLock(lkp, lk);
			KTHREAD(enable_stop)();
			return;
		}

		/* If no one holds the heavy lock then claim it */
		if (lk->holder == 0) {
			lk->holder = where;
			putHeavyLock(lkp, lk);
			KTHREAD(enable_stop)();
			return;
		}

		/* Otherwise wait for holder to release it */
		KTHREAD(get_data)(cur)->nextlk = lk->mux;
		lk->mux = cur;
		putHeavyLock(lkp, lk);
		KSEM(get)(&KTHREAD(get_data)(cur)->sem, (jlong)0);
	}
}

/*
 * Slowly unlock a mutex.  If there's someone waiting then we wake them up
 * so they can claim the lock.  If no one is waiting we revert the lock to
 * a fast thin lock.
 */
static void
slowUnlockMutex(iLock** lkp, void* where, iLock *heavyLock)
{
	iLock* lk;
	jthread_t tid;

DBG(SLOWLOCKS,
    	dprintf("slowUnlockMutex(**lkp=%p, where=%p, th=%p)\n",
	       lkp, where, KTHREAD(current)());
);
	KTHREAD(disable_stop)(); /* protect the heavy lock, and its queues */
	lk = getHeavyLock(lkp, heavyLock);

	/* Only the lock holder can be doing an unlock */
	if (!KTHREAD(on_current_stack)(lk->holder)) {
		putHeavyLock(lkp, lk);
		KTHREAD(enable_stop)();
		throwException(IllegalMonitorStateException);
	}

	/* If holder isn't where we are now then this isn't the final unlock */
#if defined(STACK_GROWS_UP)
	if (lk->holder < where) {
#else
	if (lk->holder > where) {
#endif
		putHeavyLock(lkp, lk);
		KTHREAD(enable_stop)();
		return;
	}

	/* Final unlock - if someone is waiting for it now would be a good
	 * time to tell them.
	 */
	if (lk->mux != NULL) {
		tid = lk->mux;
		lk->mux = KTHREAD(get_data)(tid)->nextlk;
		KTHREAD(get_data)(tid)->nextlk = NULL;
		lk->holder = NULL;
		putHeavyLock(lkp, lk);
		KSEM(put)(&KTHREAD(get_data)(tid)->sem);
	}
	/* If someone's waiting to be signaled keep the heavy in place */
	else if (lk->cv != NULL) {
		lk->holder = NULL;
		putHeavyLock(lkp, lk);
	}
	else {
		if (lk != heavyLock) {
			gc_free(lk);
		}
		else {
			lk->holder = NULL;
		}
		putHeavyLock(lkp, LOCKFREE);
	}
	KTHREAD(enable_stop)();
}

void
locks_internal_slowUnlockMutexIfHeld(iLock** lkp, void* where, iLock *heavyLock)
{
	iLock* lk;
	void* holder;

DBG(SLOWLOCKS,
    	dprintf("slowUnlockMutexIfHeld(**lkp=%p, where=%p, th=%p)\n",
	       lkp, where, KTHREAD(current)());
);
	holder = *lkp;

	/* nothing to do if the lock is free */
	if (holder == LOCKFREE) {
		return;
	}

	/* if it's a thin lock and this thread owns it,
	 * try to free it the easy way
	 */
	if (KTHREAD(on_current_stack)(holder) &&
	    COMPARE_AND_EXCHANGE(lkp, holder, LOCKFREE)) {
		return;
	}

	/* ok, it is a heavy lock */
	lk = getHeavyLock(lkp, heavyLock);
	holder = lk->holder;
	putHeavyLock(lkp, lk);

	if (KTHREAD(on_current_stack)(holder)) {
		slowUnlockMutex(lkp, where, heavyLock);
	}
}

jboolean
locks_internal_waitCond(iLock** lkp, jlong timeout, iLock *heavyLock)
{
	iLock* lk;
	void* holder;
	jthread_t cur = KTHREAD(current)();
	jthread_t *ptr;
	jboolean r;

DBG(SLOWLOCKS,
    	dprintf("_waitCond(**lkp=%p, timeout=%ld, th=%p)\n",
	       lkp, (long)timeout, KTHREAD(current)());
);

	lk = getHeavyLock(lkp, heavyLock);
	holder = lk->holder;

	/* I must be holding the damn thing */
	if (!KTHREAD(on_current_stack)(holder)) {
		putHeavyLock(lkp, holder);
		throwException(IllegalMonitorStateException);
	}

	KTHREAD(get_data)(cur)->nextlk = lk->cv;
	lk->cv = cur;
	putHeavyLock(lkp, lk);
	slowUnlockMutex(lkp, holder, heavyLock);
	r = KSEM(get)(&KTHREAD(get_data)(cur)->sem, timeout);

	/* Timeout */
	if (r == false) {
		lk = getHeavyLock(lkp, heavyLock);
		/* Remove myself from CV or MUX queue - if I'm * not on either
		 * then I should wait on myself to remove any pending signal.
		 */
		for (ptr = &lk->cv; *ptr != 0; ptr = &KTHREAD(get_data)(*ptr)->nextlk) {
			if ((*ptr) == cur) {
				*ptr = KTHREAD(get_data)(cur)->nextlk;
				goto found;
			}
		}
		for (ptr = &lk->mux; *ptr != 0; ptr = &KTHREAD(get_data)(*ptr)->nextlk) {
			if ((*ptr) == cur) {
				*ptr = KTHREAD(get_data)(cur)->nextlk;
				goto found;
			}
		}
		/* Not on list - so must have been signalled after all -
		 * decrease the semaphore to avoid problems.
		 */
		KSEM(get)(&KTHREAD(get_data)(cur)->sem, (jlong)0);

		found:;
		putHeavyLock(lkp, lk);
	}

	slowLockMutex(lkp, holder, heavyLock);

	return (r);
}

void
locks_internal_signalCond(iLock** lkp, iLock *heavyLock)
{
	iLock* lk;
	jthread_t tid;

DBG(SLOWLOCKS,
    	dprintf("_signalCond(**lkp=%p, th=%p)\n",
	       lkp, KTHREAD(current)());
);

	lk = getHeavyLock(lkp, heavyLock);

	if (!KTHREAD(on_current_stack)(lk->holder)) {
		putHeavyLock(lkp, lk);
		throwException(IllegalMonitorStateException);
	}

	/* Move one CV's onto the MUX */
	tid = lk->cv;
	if (tid != 0) {
		lk->cv = KTHREAD(get_data)(tid)->nextlk;
		KTHREAD(get_data)(tid)->nextlk = lk->mux;
		lk->mux = tid;
	}

	putHeavyLock(lkp, lk);
}

void
locks_internal_broadcastCond(iLock** lkp, iLock *heavyLock)
{
	iLock* lk;
	jthread_t tid;

DBG(SLOWLOCKS,
    	dprintf("_broadcastCond(**lkp=%p, th=%p)\n",
	       lkp, KTHREAD(current)());
);

	lk = getHeavyLock(lkp, heavyLock);

	if (!KTHREAD(on_current_stack)(lk->holder)) {
		putHeavyLock(lkp, lk);
		throwException(IllegalMonitorStateException);
	}

	/* Move all the CV's onto the MUX */
	while (lk->cv != 0) {
		tid = lk->cv;
		lk->cv = KTHREAD(get_data)(tid)->nextlk;
		KTHREAD(get_data)(tid)->nextlk = lk->mux;
		lk->mux = tid;
	}

	putHeavyLock(lkp, lk);
}

/*
 * Lock a mutex - try to do this quickly but if we failed because
 * we can't determine if this is a multiple entry lock or we've got
 * contention then fall back on a slow lock.
 */
void
locks_internal_lockMutex(iLock** lkp, void* where, iLock *heavyLock)
{
	uintp val;

	val = (uintp)*lkp;

	if (val == 0) {
		if (!COMPARE_AND_EXCHANGE(lkp, 0, (iLock*)where)) {
			slowLockMutex(lkp, where, heavyLock);
		}
	}
	else if (!KTHREAD(on_current_stack)((void *)val)) {
		/* XXX count this in the stats area */
		slowLockMutex(lkp, where, heavyLock);
	}
}

/*
 * Unlock a mutex - try to do this quickly but if we failed then
 * we've got contention so fall back on a slow lock.
 */
void
locks_internal_unlockMutex(iLock** lkp, void* where, iLock *heavyLock)
{
	uintp val;

	val = (uintp)*lkp;

	if ((val & 1) != 0) {
		slowUnlockMutex(lkp, where, heavyLock);
	}
	else if ((val == (uintp)where) /* XXX squirrely bit */
		&& !COMPARE_AND_EXCHANGE(lkp, (iLock*)where, LOCKFREE)) {
		slowUnlockMutex(lkp, where, heavyLock);
	}
}

void
lockObject(Hjava_lang_Object* obj)
{
	locks_internal_lockMutex(&obj->lock, &obj, NULL);
}

void
unlockObject(Hjava_lang_Object* obj)
{
	locks_internal_unlockMutex(&obj->lock, &obj, NULL);
}

void
slowLockObject(Hjava_lang_Object* obj, void* where)
{
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_CONTENDED_ENTER) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_MONITOR_CONTENDED_ENTER;
		ev.u.monitor.object = obj;
		jvmpiPostEvent(&ev);
	}
#endif
	slowLockMutex(&obj->lock, where, NULL);
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_CONTENDED_ENTERED) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_MONITOR_CONTENDED_ENTERED;
		ev.u.monitor.object = obj;
		jvmpiPostEvent(&ev);
	}
#endif
}

void
slowUnlockObject(Hjava_lang_Object* obj, void* where)
{
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_CONTENDED_EXIT) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_MONITOR_CONTENDED_EXIT;
		ev.u.monitor.object = obj;
		jvmpiPostEvent(&ev);
	}
#endif
	slowUnlockMutex(&obj->lock, where, NULL);
}

void
dumpLocks(void)
{
}

