/*
 * fastlocks.c
 * Manage locking system
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	LDBG(s)

#include "config.h"
#include "config-std.h"
#include "object.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "thread.h"
#include "locks.h"
#include "errors.h"
#include "exception.h"
#include "md.h"
#include "jthread.h"

/*
 * If we don't have an atomic compare and exchange defined then make one
 * out of a simple atmoc exchange (using the LOCKINPROGRESS value the place
 * holder).  If we don't have that, we'll just fake it.
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

/* We need to treat the gc_lock special since it's guards memory allocation.
 */
extern iLock* gc_lock;
static iLock _gc_lock;

static iLock* freeLocks;
static jboolean _SemGet(void*, jlong);
static void _SemPut(void*);

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

	timeout = 1;
	for (;;) {
		/* Get the current lock and replace it with -1 to indicate
		 * changes are in progress.
		 */
		old = *lkp;
		if (old == LOCKINPROGRESS || !COMPARE_AND_EXCHANGE(lkp, old, LOCKINPROGRESS)) {
			tid = getCurrentThread();
			SEMGET(unhand(tid)->sem, timeout);
			/* Back off */
			timeout = (timeout << 1)|timeout;
			continue;
		}
		if ((((uintp)old) & 1) == 1) {
			lk = (iLock*)(((uintp)old) & (uintp)-2);
		}
		else {
			if (lkp == &gc_lock) {
				lk = &_gc_lock;
			}
			else if (freeLocks != 0) {
				lk = freeLocks;
				freeLocks = (iLock*)lk->holder;
			}
			else {
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
void
slowLockMutex(iLock** lkp, void* where)
{
	iLock* lk;
	Hjava_lang_Thread* tid;

LDBG(	printf("Slow lock\n");						)

	for (;;) {
		lk = getHeavyLock(lkp);

		/* If I hold the heavy lock then just keep on going */
		if (jthread_on_current_stack(lk->holder)) {
			putHeavyLock(lkp, lk);
			return;
		}

		/* If no one holds the heavy lock then claim it */
		if (lk->holder == 0) {
			lk->holder = where;
			putHeavyLock(lkp, lk);
			return;
		}

		/* Otherwise wait for holder to release it */
		tid = getCurrentThread();
		unhand(tid)->nextlk = lk->mux;
		lk->mux = tid;
		putHeavyLock(lkp, lk);
		SEMGET(unhand(tid)->sem, 0);
	}
}

/*
 * Slowly unlock a mutex.  If there's someone waiting then we wake them up
 * so they can claim the lock.  If no one is waiting we revert the lock to
 * a fast thin lock.
 */
void
slowUnlockMutex(iLock** lkp, void* where)
{
	iLock* lk;
	Hjava_lang_Thread* tid;

LDBG(	printf("Slow unlock\n");					)

	lk = getHeavyLock(lkp);

	/* Only the lock holder can be doing an unlock */
	if (!jthread_on_current_stack(lk->holder)) {
		putHeavyLock(lkp, lk);
		throwException(IllegalMonitorStateException);
	}

	/* If holder isn't where we are now then this isn't the final unlock */
	if (lk->holder > where) {
		putHeavyLock(lkp, lk);
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
		SEMPUT(unhand(tid)->sem);
	}
	/* If someone's waiting to be signaled keep the heavy in place */
	else if (lk->cv != 0) {
		lk->holder = 0;
		putHeavyLock(lkp, lk);
	}
	else {
		if (lk != &_gc_lock) {
			lk->holder = (void*)freeLocks;
			freeLocks = lk;
		}
		putHeavyLock(lkp, LOCKFREE);
	}
}

void
_slowUnlockMutexIfHeld(iLock** lkp, void* where)
{
	iLock* lk;
	void* holder;

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
	r = SEMGET(unhand(tid)->sem, timeout);

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
		SEMGET(unhand(tid)->sem, 0);

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
	if (*lkp == 0) {
		if (!COMPARE_AND_EXCHANGE(lkp, 0, (iLock*)where)) {
			slowLockMutex(lkp, where);
		}
	}
	else if ((uintp)*lkp - (uintp)where > 1024) {
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
	if ((((uintp)*lkp) & 1) != 0) {
		slowUnlockMutex(lkp, where);
	}
	else if ((uintp)*lkp == (uintp)where &&
			!COMPARE_AND_EXCHANGE(lkp, (iLock*)where, LOCKFREE)) {
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

/******************************************************************************
 *
 *  The following routines are used to convert POSIX style locks into the
 *  semaphores we actually need here.
 *
 ******************************************************************************/

#if defined(SETUP_POSIX_LOCKS)
static
jboolean
_SemGet(void* sem, jlong timeout)
{
	jboolean r;
	sem2posixLock* lk;

	r = true;
	lk = (sem2posixLock*)sem;

	LOCK(lk);
	if (lk->count == 0) {
		r = WAIT(lk, timeout);
	}
	if (r == true) {
		lk->count--;
	}
	UNLOCK(lk);
	return (r);
}

static
void
_SemPut(void* sem)
{
	sem2posixLock* lk;

	lk = (sem2posixLock*)sem;

	LOCK(lk);
        lk->count++;
	SIGNAL(lk);
	UNLOCK(lk);
}
#endif
