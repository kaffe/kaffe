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

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "object.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "thread.h"
#include "jthread.h"
#include "locks.h"
#include "errors.h"
#include "exception.h"
#include "gc.h"
#include "md.h"

/* Note:
 * It is wrong to call getCurrentJava() anywhere in
 * this file since it may not be initialized. 
 */

/* Note:
 * USE_LOCK_CACHE can be defined to if we are prepared to keep an extra
 * pointer in the object structure to speed lock location.
 */

#define	MAXLOCK		64
#define	HASHLOCK(a)	((((uintp)(a)) / sizeof(void*)) % MAXLOCK)
#define SPINON(addr) 	jthread_spinon(addr)
#define SPINOFF(addr) 	jthread_spinoff(addr)

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
		fprintf(stderr, "lock@%p %s ", lk->address, 
			describeObject(lk->address));
	}
#if !defined(JMUTEX_BLOCKED) || !defined(JCONDVAR_WAITING)
	/* Dumb version */
	fprintf(stderr, "held by `%s'\n .hd=%-9p .ct=%d .mx=%-9p .cv=%-9p\n",
		(lk->holder != 0) ? nameNativeThread(lk->holder) : "noone",
		lk->holder, lk->count, lk->mux, lk->cv);
#else
	/* We can do better if jmutex_blocked and jcondvar_blocked are
	 * supported by the threading system
	 */
	if (lk->holder != 0) {
		jthread_t *blocked;
		int nblocked = jmutex_blocked(lk->mux, &blocked);

		/* lock is held, say by whom and who's waiting */
		fprintf(stderr, "\n  held by `%s'\n  blocks threads: ", 
			    nameNativeThread(lk->holder));
		if (nblocked > 0) {
			int i;
			for (i = 0; i < nblocked; i++) {
				fprintf(stderr, "`%s'%c", 
					    nameNativeThread(blocked[i]), 
					    (i < nblocked - 1) ? ' ' : '\n');
			}
			KFREE(blocked);	   /* use thread deallocator here */
		} else {
			fprintf(stderr, "\n");
		}
	} else {
		fprintf(stderr, " (uncontended)\n");
	}
	/* now check for waiters on cond variable */
	{
		jthread_t *blocked;
		int nblocked = jcondvar_waiting(lk->cv, &blocked);

		if (nblocked > 0) {
			int i;
			fprintf(stderr, "  waiting to be signaled are: ");
			for (i = 0; i < nblocked; i++) {
				fprintf(stderr, "`%s'%c", 
					    nameNativeThread(blocked[i]), 
					    (i < nblocked - 1) ? ' ' : '\n');
			}
			KFREE(blocked);	   /* use thread deallocator here */
		}
	}
#endif
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
 * implementation of the locking subsystem based on jlocks
 *
 * Note that we keep track of lk->holder, and its type is void*.
 */
static          
void
initLock(iLock* lk)
{               
        static bool first = true;
        static jmutex first_mutex;
        static jcondvar first_condvar;
        
        /* The first lock init is for the memory manager - so we can't
         * use it yet.  Allocate from static space.
         */
        if (first == true) {
                first = false;
                lk->mux = &first_mutex;
                lk->cv = &first_condvar;
        }
        else {
                lk->mux = gc_malloc(sizeof(jmutex), GC_ALLOC_THREADCTX);
                lk->cv = gc_malloc(sizeof(jcondvar), GC_ALLOC_THREADCTX);
        } 
        jmutex_initialise(lk->mux);
        jcondvar_initialise(lk->cv);
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

	/* NB: we use a spinlock to quickly establish the case where
	 * we a lock is already in the hashtable.  However, if we don't
	 * find a lock, we must allocate one and initialize it.  Since
	 * this operation can cause a gc and hence take indefinitely,
	 * we must acquire a real lock then.
	 */
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
		initLock(lock);
                SPINON(lockHead->lock);

                lock->next = lockHead->head;
                lockHead->head = lock;

                /* Go back and see whether another thread has already
                 * entered the entry for this address.  If so, the losing 
		 * lock will be used to next time we need a lock.
		 * This is hopefully cheaper than using a real lock to
		 * we don't create locks unless we have to.
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
	initLock(lk);
}

/*
 * Lock the given lock.
 */
inline
void
__lockMutex(iLock* lk)
{
DBG(VMLOCKS,	dprintf("Lock 0x%x on iLock=0x%x\n", jthread_current(), lk);	    )

	/*
	 * Note: simply testing 'holder == currentNative' is not enough.
	 * If a thread systems uses the same value to which we initialized
	 * holder as a thread id (null), we might be fooled into thinking 
	 * we already hold the lock, when in fact we don't.
	 */
	if (lk->count > 0 && lk->holder == (void*)jthread_current()) {
		lk->count++;
	}
	else {
#ifdef DEBUG
		int trace = 0;
#endif
DBG(LOCKCONTENTION,
		if (lk->count != 0) {
		    dprintf("%p waiting for ", jthread_current());
		    dumpLock(lk);
		    trace = 1;
		}
    )
		jmutex_lock(lk->mux);
		lk->holder = (void *)jthread_current();
DBG(LOCKCONTENTION,
		if (trace) {
		    dprintf("%p got ", jthread_current());
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

DBG(VMLOCKS,	dprintf("Lock 0x%x on addr=0x%x\n", jthread_current(), addr);    )

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
DBG(VMLOCKS,	dprintf("Unlock 0x%x on iLock=0x%x\n", jthread_current(), lk);   )

#if defined(DEBUG)
	if (lk->count == 0) {
		dprintf("count == 0\n");
		dumpLock(lk);
		ABORT();
	}
	if (lk->holder != jthread_current()) {
		dprintf("HOLDER %p != ME %p\n", lk->holder, jthread_current());
		dumpLock(lk);
		ABORT();
	}
#endif
	assert(lk->count > 0 && lk->holder == (void*)jthread_current());
	lk->count--;
	if (lk->count == 0) {
		lk->holder = 0;
		jmutex_unlock(lk->mux);
	}
}

/*
 * Release a mutex by address.
 */
void
_unlockMutex(void* addr)
{
	iLock* lk;

DBG(VMLOCKS,	dprintf("Unlock 0x%x on addr=0x%x\n", jthread_current(), addr);  )

#if defined(USE_LOCK_CACHE)
	lk = ((Hjava_lang_Object*)addr)->lock;
#else
	lk = getLock(addr);
#endif
	__unlockMutex(lk);
	freeLock(lk);
}

/*
 * Release a given mutex and free it.
 */
void
_unlockMutexFree(iLock* lk)
{
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
       int count;
DBG(VMCONDS,	dprintf("Wait 0x%x on iLock=0x%x\n", jthread_current(), lk);	)

	if (lk == 0 || lk->holder != (void*)jthread_current()) {
		throwException(IllegalMonitorStateException);
	}

        /*
         * We must reacquire the Java lock before we're ready to die
         */
        jthread_disable_stop();
        count = lk->count;
        lk->count = 0;
	lk->holder = NULL; /* Debug only ? */
        jcondvar_wait(lk->cv, lk->mux, timeout);
        lk->holder = (void *)jthread_current();
        lk->count = count;
        
        /* now it's safe to start dying */
        jthread_enable_stop();

	return (0);
}

/*
 * Wait on a conditional variable.
 */
int
_waitCond(void* addr, jlong timeout)
{
	iLock* lk;

DBG(VMLOCKS,	dprintf("Wait 0x%x on addr=0x%x\n", jthread_current(), addr);    )

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
DBG(VMCONDS,	dprintf("Signal 0x%x on iLock=0x%x\n", jthread_current(), lk);)

	if (lk == 0 || lk->holder != (void*)jthread_current()) {
		throwException(IllegalMonitorStateException);
	}

	jcondvar_signal(lk->cv, lk->mux);
}

/*
 * Wake one thread on a conditional variable.
 */
void
_signalCond(void* addr)
{
	iLock* lk;

DBG(VMCONDS,	dprintf("Signal 0x%x on addr=0x%x\n", jthread_current(), addr);)

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
DBG(VMCONDS,	dprintf("Broadcast 0x%x on iLock=0x%x\n", jthread_current(), lk);)

	if (lk == 0 || lk->holder != (void*)jthread_current()) {
		throwException(IllegalMonitorStateException);
	}

	jcondvar_broadcast(lk->cv, lk->mux);
}

void
_broadcastCond(void* addr)
{
	iLock* lk;

DBG(VMCONDS,	dprintf("Broadcast 0x%x on addr=0x%x\n", jthread_current(), addr);)

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
	if (lk == 0 || lk->holder != (void*)jthread_current()) {
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
