/*
 * locks.c
 * Manage locking system
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
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

/* Note:
 * It is wrong to call (*Kaffe_ThreadInterface.currentJava)() anywhere in
 * this file since it may not be initialized. 
 */

/* Note:
 * USE_LOCK_CACHE can be defined to if we are prepared to keep an extra
 * pointer in the object structure to speed lock location.
 */

#define	MAXLOCK		64
#define	HASHLOCK(a)	((((uintp)(a)) / sizeof(void*)) % MAXLOCK)
#define SPINON(addr) 	(*Kaffe_LockInterface.spinon)(addr)
#define SPINOFF(addr) 	(*Kaffe_LockInterface.spinoff)(addr)

static struct lockList {
	void*		lock;
	iLock*		head;
} lockTable[MAXLOCK];

/* a list in which we keep all static locks */
static iLock *staticLocks;

static void
dumpLock(iLock *lk)
{
	if (lk->ref == -1) {
		fprintf(stderr, "%s ", (char*)lk->address);
	} else {
		fprintf(stderr, "lock@%p ", lk->address);
	}
	fprintf(stderr, "held by `%s'\n .hd=%-9p .ct=%d .mx=%-9p .cv=%-9p\n",
		(lk->holder != 0) ? nameNativeThread(lk->holder) : "noone",
		lk->holder, lk->count, lk->mux, lk->cv);
}

/*
 * dump all locks
 */
void
dumpLocks(void)
{
	int i;
	iLock* lock;

	fprintf(stderr, "Dumping dynamic locks:\n");
	for (i = 0; i < MAXLOCK; i++) {
		for (lock = lockTable[i].head; lock; lock = lock->next) {
			if (lock->ref) {
				dumpLock(lock);
			}
		}
	}

	fprintf(stderr, "Dumping static locks:\n");
	for (lock = staticLocks; lock; lock = lock->next) {
		dumpLock(lock);
	}
}

/*
 * Retrieve a machine specific (possibly) locking structure associated with
 * this address.  If one isn't found, allocate it.
 */
iLock*
newLock(void* address)
{
	struct lockList* lockHead;
	iLock* lock;
	iLock* freelock;

	lockHead = &lockTable[HASHLOCK(address)];
	SPINON(lockHead->lock);

retry:;
	freelock = 0;

	/* See if there's a free lock for that slot */
	for (lock = lockHead->head; lock != NULL; lock = lock->next) {
		/* If so, increase ref count and return */
		if (lock->address == address) {
			lock->ref++;
			SPINOFF(lockHead->lock);
			return (lock);
		}
		if (lock->ref == 0 && freelock == 0) {
			freelock = lock;
		}
	}

	/* Allocate a new lock structure if needed */
	if (freelock == 0) {
		/* Both of these two function calls involve allocations. 
		 * They can block and cause a gc.  Thus, we cannot hold
		 * the spinlock here.
		 */
		SPINOFF(lockHead->lock);
		lock = gc_malloc(sizeof(iLock), GC_ALLOC_LOCK);
		(*Kaffe_LockInterface.init)(lock);
		SPINON(lockHead->lock);

		lock->next = lockHead->head;
		lockHead->head = lock;

		/* go back and see whether another thread has already
		 * entered the entry for this address.
		 */
		goto retry;
	}

	/* Fill in the details */
	freelock->address = address;
	freelock->ref = 1;
	freelock->holder = NULL;
	freelock->count = 0;
	SPINOFF(lockHead->lock);
	return (freelock);
}

/*
 * Retrieve a machine specific (possibly) locking structure associated with
 * this address.
 */
iLock*
getLock(void* address)
{
	struct lockList* lockHead;
	iLock* lock;

	lockHead = &lockTable[HASHLOCK(address)];

	for (lock = lockHead->head; lock != NULL; lock = lock->next) {
		if (lock->address == address) {
			break;
		}
	}
	return (lock);
}

/*
 * Free a lock if no longer in use.
 */
static void
freeLock(iLock* lk)
{
	struct lockList* lockHead;
	lockHead = &lockTable[HASHLOCK(lk->address)];

	SPINON(lockHead->lock);

	/* If lock no longer in use, release it for reallocation */
	lk->ref--;
	if (lk->ref == 0) {
		if (lk->count != 0)
		    printf("lk=%p addr=%p count is %d\n", lk, 
			lk->address, lk->count);
		assert(lk->count == 0);
		assert(lk->holder == NULL);
DBG(VMLOCKS,	dprintf("Freeing lock for addr=0x%x\n", lk->address);	)
	}

	SPINOFF(lockHead->lock);
}

/*
 * Initialise a new lock.
 */
void
__initLock(iLock* lk, const char *lkname)
{
	lk->ref = -1;
	lk->address = lkname;
	lk->next = staticLocks;
	staticLocks = lk;
	(*Kaffe_LockInterface.init)(lk);
}

/*
 * Lock the given lock.
 */
inline
void
__lockMutex(iLock* lk)
{
DBG(VMLOCKS,	dprintf("Lock 0x%x on iLock=0x%x\n", THREAD_NATIVE(), lk);	    )

	/*
	 * Note: simply testing 'holder == currentNative' is not enough.
	 * If a thread systems uses the same value to which we initialized
	 * holder as a thread id (null), we might be fooled into thinking 
	 * we already hold the lock, when in fact we don't.
	 */
	if (lk->count > 0 && lk->holder == (*Kaffe_ThreadInterface.currentNative)()) {
		lk->count++;
	}
	else {
#ifdef DEBUG
		int trace = 0;
#endif
DBG(LOCKCONTENTION,
		if (lk->count != 0) {
		    dprintf("%p waiting for ", THREAD_NATIVE());
		    dumpLock(lk);
		    trace = 1;
		}
    )
		(*Kaffe_LockInterface.lock)(lk);
DBG(LOCKCONTENTION,
		if (trace) {
		    dprintf("%p got ", THREAD_NATIVE());
		    dumpLock(lk);
		}
    )
		lk->count = 1;
	}
}

/*
 * Lock a mutex.  We use the address to find a lock.
 */
iLock*
_lockMutex(void* addr)
{
	iLock* lk;

DBG(VMLOCKS,	dprintf("Lock 0x%x on addr=0x%x\n", THREAD_NATIVE(), addr);    )

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
	if (lk->address != addr) {
		lk = newLock(addr);
		((Hjava_lang_Object*)addr)->lock = lk;
	}
#else
	lk = newLock(addr);
#endif
	__lockMutex(lk);
	return (lk);
}

/*
 * Release a given mutex.
 */
inline
void
__unlockMutex(iLock* lk)
{
DBG(VMLOCKS,	dprintf("Unlock 0x%x on iLock=0x%x\n", THREAD_NATIVE(), lk);   )

#if defined(DEBUG)
	if (lk->count == 0) {
		dprintf("count == 0\n");
		dumpLock(lk);
		ABORT();
	}
	if (lk->holder != (*Kaffe_ThreadInterface.currentNative)()) {
		dprintf("HOLDER %p != ME %p\n", lk->holder,
			(*Kaffe_ThreadInterface.currentNative)());
		dumpLock(lk);
		ABORT();
	}
#endif
	assert(lk->count > 0 && lk->holder == (*Kaffe_ThreadInterface.currentNative)());
	lk->count--;
	if (lk->count == 0) {
		(*Kaffe_LockInterface.unlock)(lk);
	}
}

/*
 * Release a mutex.
 */
void
_unlockMutex(void* addr)
{
	iLock* lk;

DBG(VMLOCKS,	dprintf("Unlock 0x%x on addr=0x%x\n", THREAD_NATIVE(), addr);  )

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
#else
	lk = getLock(addr);
#endif
	__unlockMutex(lk);
	freeLock(lk);
}

/*
 * Wait on a conditional variable.
 */
inline
int
__waitCond(iLock* lk, jlong timeout)
{
DBG(VMCONDS,	dprintf("Wait 0x%x on iLock=0x%x\n", THREAD_NATIVE(), lk);	)

	if (lk == 0 || lk->holder != (*Kaffe_ThreadInterface.currentNative)()) {
		throwException(IllegalMonitorStateException);
	}

	(*Kaffe_LockInterface.wait)(lk, timeout);
	return (0);
}

/*
 * Wait on a conditional variable.
 */
int
_waitCond(void* addr, jlong timeout)
{
	iLock* lk;

DBG(VMLOCKS,	dprintf("Wait 0x%x on addr=0x%x\n", THREAD_NATIVE(), addr);    )

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
#else
	lk = getLock(addr);
#endif
	__waitCond(lk, timeout);
	return (0);
}

/*
 * Wake one thread on a conditional variable.
 */
inline
void
__signalCond(iLock* lk)
{
DBG(VMCONDS,	dprintf("Signal 0x%x on iLock=0x%x\n", THREAD_NATIVE(), lk);)

	if (lk == 0 || lk->holder != (*Kaffe_ThreadInterface.currentNative)()) {
		throwException(IllegalMonitorStateException);
	}

	(*Kaffe_LockInterface.signal)(lk);
}

/*
 * Wake one thread on a conditional variable.
 */
void
_signalCond(void* addr)
{
	iLock* lk;

DBG(VMCONDS,	dprintf("Signal 0x%x on addr=0x%x\n", THREAD_NATIVE(), addr);)

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
#else
	lk = getLock(addr);
#endif
	__signalCond(lk);
}

/*
 * Wake all threads on a conditional variable.
 */
inline
void
__broadcastCond(iLock* lk)
{
DBG(VMCONDS,	dprintf("Broadcast 0x%x on iLock=0x%x\n", THREAD_NATIVE(), lk);)

	if (lk == 0 || lk->holder != (*Kaffe_ThreadInterface.currentNative)()) {
		throwException(IllegalMonitorStateException);
	}

	(*Kaffe_LockInterface.broadcast)(lk);
}

void
_broadcastCond(void* addr)
{
	iLock* lk;

DBG(VMCONDS,	dprintf("Broadcast 0x%x on addr=0x%x\n", THREAD_NATIVE(), addr);)

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
#else
	lk = getLock(addr);
#endif
	__broadcastCond(lk);
}

int
__holdMutex(iLock* lk)
{
	if (lk == 0 || lk->holder != (*Kaffe_ThreadInterface.currentNative)()) {
		return (0);
	}
	else {
		return (1);
	}
}

int
_holdMutex(void* addr)
{
	iLock* lk;

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
#else
	lk = getLock(addr);
#endif
	return (__holdMutex(lk));
}
