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
#define	COMPARE_AND_EXCHANGE(A,O,N) \
	(*(A) == (O) ? *(A) = (N), 1 : 0)
#endif
#endif

/* We need to treat the gc locks special since they guard memory allocation
 * and cannot be acquired by the gc thread otherwise.
 * I'm not proud of this - it's a hack waiting for a better idea - TIM.
 */
extern iLock* gc_lock;
extern iLock* gcman;
extern iLock* finman;
static struct {
	iLock**	key;
	iLock	lock;
} specialLocks[] = {
	{ &gc_lock,	{ 0, 0, 0 } },
	{ &gcman,	{ 0, 0, 0 } },
	{ &finman,	{ 0, 0, 0 } },
	{ &stringLock,	{ 0, 0, 0 } },
	{ &utf8Lock,	{ 0, 0, 0 } },
};
#define	NR_SPECIAL_LOCKS	(sizeof(specialLocks)/sizeof(specialLocks[0]))

/* Count number of backoffs.  XXX move into the STAT infrastructure. */
int backoffcount = 0;

/*
 * Initialise the locking system.
 */
void
initLocking(void)
{
}

#if defined(DEBUG)
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
getHeavyLock(iLock** lkp)
{
	iLock* old;
	iLock* lk;
	Hjava_lang_Thread* tid;
	jlong timeout;
	int i;

DBG(SLOWLOCKS,
    	dprintf("  getHeavyLock(**lkp=%p, *lk=%p, th=%p)\n",
		lkp, *lkp, jthread_current());
)
 
	timeout = 1;
	for (;;) {
		/* Get the current lock and replace it with LOCKINPROGRESS to indicate
		 * changes are in progress.
		 */
		old = *lkp;
		if (old == LOCKINPROGRESS || !COMPARE_AND_EXCHANGE(lkp, old, LOCKINPROGRESS)) {
			/* Someone else put the lock in LOCKINPROGRESS state */
			tid = getCurrentThread();
			backoffcount++;
			ksemGet((Ksem*)(unhand(tid)->sem), timeout);
			/* Back off */
			timeout = (timeout << 1)|timeout;
			continue;
		}

		/* If bottom bit is set, strip off and use pointer as pointer to heavy lock */
		if ((((uintp)old) & 1) == 1) {
DBG(SLOWLOCKS,
    			dprintf("    got cached lock\n");
)
			lk = (iLock*)(((uintp)old) & (uintp)-2);
		}
		else {
			/* Create a heavy lock object for others to find. */
			lk = 0;
			for (i = 0; i < NR_SPECIAL_LOCKS; i++) {
				if (specialLocks[i].key == lkp) {
					lk = &specialLocks[i].lock;
					break;
				}
			}
DBG(SLOWLOCKS,
    			dprintf("    got %s lock\n",
				(lk == 0) ? "new" : "special");
)
			if (lk == 0) {
				lk = (iLock*)jmalloc(sizeof(iLock));
			}
			lk->holder = (void*)old;
			lk->mux = 0;
			lk->cv = 0;
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
		lkp, lk, jthread_current());
)

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
slowLockMutex(iLock** lkp, void* where)
{
	iLock* lk;
	Hjava_lang_Thread* tid;

DBG(SLOWLOCKS,
    	dprintf("slowLockMutex(**lkp=%p, where=%p, th=%p)\n",
	       lkp, where, jthread_current());
)
	jthread_disable_stop(); /* protect the heavy lock, and its queues */

	for (;;) {
		lk = getHeavyLock(lkp);

		/* If I hold the heavy lock then just keep on going */
		if (jthread_on_current_stack(lk->holder)) {
			putHeavyLock(lkp, lk);
			jthread_enable_stop();
			return;
		}

		/* If no one holds the heavy lock then claim it */
		if (lk->holder == 0) {
			lk->holder = where;
			putHeavyLock(lkp, lk);
			jthread_enable_stop();
			return;
		}

		/* Otherwise wait for holder to release it */
		tid = getCurrentThread();
		unhand(tid)->nextlk = lk->mux;
		lk->mux = tid;
		putHeavyLock(lkp, lk);
		ksemGet((Ksem*)(unhand(tid)->sem), 0);
	}
}

/*
 * Slowly unlock a mutex.  If there's someone waiting then we wake them up
 * so they can claim the lock.  If no one is waiting we revert the lock to
 * a fast thin lock.
 */
static void
slowUnlockMutex(iLock** lkp, void* where)
{
	iLock* lk;
	Hjava_lang_Thread* tid;
	int i;

DBG(SLOWLOCKS,
    	dprintf("slowUnlockMutex(**lkp=%p, where=%p, th=%p)\n",
	       lkp, where, jthread_current());
)
	jthread_disable_stop(); /* protect the heavy lock, and its queues */
	lk = getHeavyLock(lkp);

	/* Only the lock holder can be doing an unlock */
	if (!jthread_on_current_stack(lk->holder)) {
		putHeavyLock(lkp, lk);
		jthread_enable_stop();
		throwException(IllegalMonitorStateException);
	}

	/* If holder isn't where we are now then this isn't the final unlock */
	if (lk->holder > where) {
		putHeavyLock(lkp, lk);
		jthread_enable_stop();
		return;
	}

	/* Final unlock - if someone is waiting for it now would be a good
	 * time to tell them.
	 */
	if (lk->mux != 0) {
		tid = lk->mux;
		lk->mux = unhand(tid)->nextlk;
		unhand(tid)->nextlk = 0;
		lk->holder = 0;
		putHeavyLock(lkp, lk);
		ksemPut((Ksem*)(unhand(tid)->sem));
	}
	/* If someone's waiting to be signaled keep the heavy in place */
	else if (lk->cv != 0) {
		lk->holder = 0;
		putHeavyLock(lkp, lk);
	}
	else {
		for (i = 0; i < NR_SPECIAL_LOCKS; i++) {
			if (specialLocks[i].key == lkp) {
				lk = 0;
				break;
			}
		}
		if (lk != 0) {
			jfree(lk);
		}
		putHeavyLock(lkp, LOCKFREE);
	}
	jthread_enable_stop();
}

void
_slowUnlockMutexIfHeld(iLock** lkp, void* where)
{
	iLock* lk;
	void* holder;

DBG(SLOWLOCKS,
    	dprintf("slowUnlockMutexIfHeld(**lkp=%p, where=%p, th=%p)\n",
	       lkp, where, jthread_current());
)

	lk = getHeavyLock(lkp);
	holder = lk->holder;
	putHeavyLock(lkp, lk);

	if (jthread_on_current_stack(holder)) {
		slowUnlockMutex(lkp, where);
	}
}

void*
_releaseLock(iLock** lkp)
{
	iLock* lk;
	void* holder;

DBG(SLOWLOCKS,
    	dprintf("_releaseLock(**lkp=%p, th=%p)\n",
	       lkp, jthread_current());
)

	lk = getHeavyLock(lkp);
	holder = lk->holder;

	/* I must be holding the damn thing */
	assert(jthread_on_current_stack(holder));

	putHeavyLock(lkp, lk);
	slowUnlockMutex(lkp, holder);

	return (holder);
}

void
_acquireLock(iLock** lkp, void* holder)
{
	slowLockMutex(lkp, holder);
}

jboolean
_waitCond(iLock** lkp, jlong timeout)
{
	iLock* lk;
	void* holder;
	Hjava_lang_Thread* tid;
	Hjava_lang_Thread** ptr;
	jboolean r;

DBG(SLOWLOCKS,
    	dprintf("_waitCond(**lkp=%p, timeout=%ld, th=%p)\n",
	       lkp, (long)timeout, jthread_current());
)

	lk = getHeavyLock(lkp);
	holder = lk->holder;

	/* I must be holding the damn thing */
	if (!jthread_on_current_stack(holder)) {
		putHeavyLock(lkp, holder);
		throwException(IllegalMonitorStateException);
	}

	tid = getCurrentThread();
	unhand(tid)->nextlk = lk->cv;
	lk->cv = tid;
	putHeavyLock(lkp, lk);
	slowUnlockMutex(lkp, holder);
	r = ksemGet((Ksem*)unhand(tid)->sem, timeout);

	/* Timeout */
	if (r == false) {
		lk = getHeavyLock(lkp);
		/* Remove myself from CV or MUX queue - if I'm * not on either
		 * then I should wait on myself to remove any pending signal.
		 */
		for (ptr = &lk->cv; *ptr != 0; ptr = &unhand(*ptr)->nextlk) {
			if ((*ptr) == tid) {
				*ptr = unhand(tid)->nextlk;
				goto found;
			}
		}
		for (ptr = &lk->mux; *ptr != 0; ptr = &unhand(*ptr)->nextlk) {
			if ((*ptr) == tid) {
				*ptr = unhand(tid)->nextlk;
				goto found;
			}
		}
		/* Not on list - so must have been signalled after all -
		 * decrease the semaphore to avoid problems.
		 */
		ksemGet((Ksem*)(unhand(tid)->sem), 0);

		found:;
		putHeavyLock(lkp, lk);
	}

	slowLockMutex(lkp, holder);

	return (r);
}

void
_signalCond(iLock** lkp)
{
	iLock* lk;
	Hjava_lang_Thread* tid;

DBG(SLOWLOCKS,
    	dprintf("_signalCond(**lkp=%p, th=%p)\n",
	       lkp, jthread_current());
)

	lk = getHeavyLock(lkp);

	if (!jthread_on_current_stack(lk->holder)) {
		putHeavyLock(lkp, lk);
		throwException(IllegalMonitorStateException);
	}

	/* Move one CV's onto the MUX */
	tid = lk->cv;
	if (tid != 0) {
		lk->cv = unhand(tid)->nextlk;
		unhand(tid)->nextlk = lk->mux;
		lk->mux = tid;
	}

	putHeavyLock(lkp, lk);
}

void
_broadcastCond(iLock** lkp)
{
	iLock* lk;
	Hjava_lang_Thread* tid;

DBG(SLOWLOCKS,
    	dprintf("_broadcastCond(**lkp=%p, th=%p)\n",
	       lkp, jthread_current());
)

	lk = getHeavyLock(lkp);

	if (!jthread_on_current_stack(lk->holder)) {
		putHeavyLock(lkp, lk);
		throwException(IllegalMonitorStateException);
	}

	/* Move all the CV's onto the MUX */
	while (lk->cv != 0) {
		tid = lk->cv;
		lk->cv = unhand(tid)->nextlk;
		unhand(tid)->nextlk = lk->mux;
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
_lockMutex(iLock** lkp, void* where)
{
	uintp val;

	val = (uintp)*lkp;

	if (val == 0) {
		if (!COMPARE_AND_EXCHANGE(lkp, 0, (iLock*)where)) {
			slowLockMutex(lkp, where);
		}
	}
	else if (val - (uintp)where > 1024) {
		/* XXX count this in the stats area */
		slowLockMutex(lkp, where);
	}
}

/*
 * Unlock a mutex - try to do this quickly but if we failed then
 * we've got contention so fall back on a slow lock.
 */
void
_unlockMutex(iLock** lkp, void* where)
{
	uintp val;

	val = (uintp)*lkp;

	if ((val & 1) != 0) {
		slowUnlockMutex(lkp, where);
	}
	else if ((val == (uintp)where) /* XXX squirrely bit */
		&& !COMPARE_AND_EXCHANGE(lkp, (iLock*)where, LOCKFREE)) {
		slowUnlockMutex(lkp, where);
	}
}

void
lockObject(Hjava_lang_Object* obj)
{
	_lockMutex(&obj->lock, &obj);
}

void
unlockObject(Hjava_lang_Object* obj)
{
	_unlockMutex(&obj->lock, &obj);
}

void
slowLockObject(Hjava_lang_Object* obj, void* where)
{
	slowLockMutex(&obj->lock, where);
}

void
slowUnlockObject(Hjava_lang_Object* obj, void* where)
{
	slowUnlockMutex(&obj->lock, where);
}

void
dumpLocks(void)
{
}

