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
#include "stats.h"

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

/*
 * Initialise the locking system.
 */
void
initLocking(void)
{
}

#if defined(KAFFE_STATS)
static timespent heavyLockTime;
static timespent locksTime;
#endif

/*
 * Get a heavy lock for the object and hold it.  If the object doesn't
 * have a heavy lock then we create one.
 */

#define IS_HEAVY_LOCK(ptr) ((((uintp)ptr)&1)==1)

static volatile iLock *
getHeavyLock(volatile iLock* volatile * lkp, volatile iLock *heavyLock)
{
  volatile iLock *lk;
  iLock *newLock;

  DBG(SLOWLOCKS,
      dprintf("  getHeavyLock(lk=%p, th=%p)\n",
	      *lkp, KTHREAD(current)());
      );

  for (;;)
    {
      lk = *lkp;

      /* Check if it has been allocated, if not acquire the lock by putting our
       * temporary structure.
       */
      if (!IS_HEAVY_LOCK(lk))
	{
	  startTiming(&heavyLockTime, "heavylock-handling");

	  if (heavyLock != NULL)
	    {
	      /* First we try to acquire the static heavy lock. */   
	      if (COMPARE_AND_EXCHANGE(&heavyLock->in_progress, 0, 1))
		{
		  /* We succeed. So anyway the other threads using this lock knows
		   * already about it.
		   */
		  /* Now we check whether the other thread holding the thin lock
		   * has released it or not. If it is so mark the holder as free.
		   * We loop until we are sure to have placed the heavylock and to have
		   * the correct value in lk.
		   */
		  while (!COMPARE_AND_EXCHANGE(lkp, lk, (iLock *)(((uintp)heavyLock) | 1)))
		    lk = *lkp;
		  
		  /* Remind the holder. */
		  heavyLock->holder = lk;
		  if (lk != LOCKFREE)
		    heavyLock->lockCount = 1;
		  
		  return heavyLock;
		}
	      lk = heavyLock;
	      break;
	    }

	  /* Build a temporary lock */
	  newLock = gc_malloc (sizeof(iLock), KGC_ALLOC_LOCK);

	  /* initialize the new lock. */
	  KSEM(init)(&newLock->sem);

	  // preserve current state of lock
	  newLock->holder = lk;
	  if (lk != LOCKFREE)
	    newLock->lockCount = 1;

	  if (!COMPARE_AND_EXCHANGE(lkp, lk, (iLock *)((uintp)newLock |1) ))
	    {
	      /* heavyLock exchange must always succeed as we have already exchanged it
	       * sooner.
	       */
	      newLock->lockCount = 0;
	      continue;
	    }

	  /* Now the lock is acquired by this thread and ready to use for the others.
	   */
	  lk = newLock;
	  stopTiming(&heavyLockTime);
	}
      else
	break;
    }

  lk = GET_HEAVYLOCK(lk);
  
  /* The lock is allocated and ready to use. */
  for (;;) {
    /* Try to acquire the lock. We try to do an "atomic" incrementation. */
    atomic_increment(&(lk->num_wait));
    if (!COMPARE_AND_EXCHANGE(&(lk->in_progress), 0, 1))
      {
	KSEM(get)(&lk->sem, (jlong)0);
        atomic_decrement(&(lk->num_wait));
	continue;
      }
    lk->hlockHolder = KTHREAD(current)();
    atomic_decrement(&(lk->num_wait));
    return lk;
  }
}

void initStaticLock(iStaticLock *slock)
{
  slock->lock = NULL;
  slock->heavyLock.num_wait = 0;
  slock->heavyLock.lockCount = 0;
  slock->heavyLock.mux = NULL;
  slock->heavyLock.cv = NULL;
  slock->heavyLock.in_progress = 0;
  slock->heavyLock.holder = NULL;
  KSEM(init)(&slock->heavyLock.sem);
}

void destroyStaticLock(iStaticLock *slock)
{
  assert(slock->lock == NULL || GET_HEAVYLOCK(slock->lock) == &slock->heavyLock);
  assert(slock->heavyLock.lockCount == 0);
  assert(slock->heavyLock.num_wait == 0);
  assert(slock->heavyLock.in_progress == 0);
  KSEM(destroy)(&slock->heavyLock.sem);
}

/*
 * Release the lock - only the one who has claimed it can do this
 * so there's no need for locked instructions.
 */
static void
putHeavyLock(volatile iLock* lk)
{
  DBG(SLOWLOCKS,
      dprintf("  putHeavyLock(lk=%p, th=%p)\n", 
	      lk, KTHREAD(current)());
      );

  assert(lk->in_progress == 1);
  
  lk->hlockHolder = NULL;
  lk->in_progress = 0;
  if (lk->num_wait != 0)
    KSEM(put)(&(lk->sem));
}

/*
 * Slowly lock a mutex.  We get the heavy lock and lock that instead.
 * If we can't lock it we suspend until we can.
 */
static void
slowLockMutex(volatile iLock* volatile * lkp, iLock *heavyLock)
{
  volatile iLock* lk;
  jthread_t cur = KTHREAD(current) ();
  threadData *tdata;

DBG(SLOWLOCKS,
    dprintf("slowLockMutex(lk=%p, th=%p)\n",
	    *lkp, KTHREAD(current)());
    );

 KTHREAD(disable_stop)(); /* protect the heavy lock, and its queues */

 tdata = KTHREAD(get_data)(cur);
 for (;;) {
   lk = getHeavyLock(lkp, heavyLock);
   
   /* If I hold the heavy lock then just keep on going */
   if (cur == lk->holder) {
     lk->lockCount++;
     putHeavyLock(lk);
     KTHREAD(enable_stop)();
     return;
   }
   
   /* If no one holds the heavy lock then claim it */
   if (lk->holder == NULL) {
     if (lk->lockCount != 0) {
       dprintf("Lockcount should be 0 for %p\n", lk);
       abort();
     }
     lk->holder = cur;
     lk->lockCount++;
     putHeavyLock(lk);
     KTHREAD(enable_stop)();
     return;
   }
   
   /* Otherwise wait for holder to release it */
   tdata->nextlk = lk->mux;
   lk->mux = cur;
   putHeavyLock(lk);
   KSEM(get)(&tdata->sem, (jlong)0);
 }
}

/*
 * Slowly unlock a mutex.  If there's someone waiting then we wake them up
 * so they can claim the lock.  If no one is waiting we revert the lock to
 * a fast thin lock.
 */
static void
slowUnlockMutex(volatile iLock* volatile * lkp, iLock *heavyLock)
{
  volatile iLock* lk;
  jthread_t tid;
  jthread_t cur = KTHREAD(current)();

  DBG(SLOWLOCKS,
      dprintf("slowUnlockMutex(lk=%p, th=%p)\n",
	      *lkp, KTHREAD(current)());
      );
  KTHREAD(disable_stop)(); /* protect the heavy lock, and its queues */
  lk = getHeavyLock(lkp, heavyLock);

  
  /* Only the lock holder can be doing an unlock */
  if (cur != lk->holder) {
    putHeavyLock(lk);
    KTHREAD(enable_stop)();
    throwException(IllegalMonitorStateException);
  }

  assert(lk->lockCount > 0);

  /* If holder isn't where we are now then this isn't the final unlock */
  lk->lockCount--;
  if (lk->lockCount != 0) {
    putHeavyLock(lk);
    KTHREAD(enable_stop)();
    return;
  }
    
  /* Final unlock - if someone is waiting for it now would be a good
   * time to tell them.
   */
  if (lk->mux != NULL) {
    threadData *tdata;

    tid = lk->mux;
    tdata = KTHREAD(get_data)(tid);
    lk->mux = tdata->nextlk;
    tdata->nextlk = NULL;
    lk->holder = NULL;
    KSEM(put)(&tdata->sem);
    putHeavyLock(lk);
  }
  else {
    lk->holder = NULL;
    putHeavyLock(lk);
  }
  KTHREAD(enable_stop)();
}

void
locks_internal_slowUnlockMutexIfHeld(iLock** lkp, iLock *heavyLock)
{
  volatile iLock* lk;
  void* holder;
  jthread_t cur = KTHREAD(current)();
  
  DBG(SLOWLOCKS,
      dprintf("slowUnlockMutexIfHeld(lkp=%p, th=%p)\n",
	      *lkp, KTHREAD(current)());
      );
  lk = *lkp;
  if (lk == LOCKFREE)
    return;

  /* Even if the lock is not allocated the holder is NULL. */
  if (!IS_HEAVY_LOCK(lk) &&
    /* if it's a thin lock and this thread owns it,
     * try to free it the easy way
     */
      !COMPARE_AND_EXCHANGE(lkp, (iLock*)cur, LOCKFREE))
    return;

  /* ok, it is a heavy lock and it is acquire by someone. */
  lk = getHeavyLock((volatile iLock *volatile *)lkp, heavyLock);
  holder = lk->holder;
  putHeavyLock(lk);
  
  if (holder == cur)
    slowUnlockMutex((volatile iLock *volatile *)lkp, heavyLock);
}

jboolean
locks_internal_waitCond(iLock** lkp, iLock *heavyLock, jlong timeout)
{
  volatile iLock* lk;
  void* holder;
  jthread_t cur = KTHREAD(current)();
  volatile jthread_t *ptr;
  jboolean r;
  threadData *tdata;
  unsigned int oldLockCount;
  
  DBG(SLOWLOCKS,
      dprintf("_waitCond(lk=%p, timeout=%ld, th=%p)\n",
	      *lkp, (long)timeout, KTHREAD(current)());
      );
  
  lk = getHeavyLock((volatile iLock *volatile *)lkp, heavyLock);
  holder = lk->holder;
  
  /* I must be holding the damn thing */
  if (holder != cur) {
    putHeavyLock(lk);
    throwException(IllegalMonitorStateException);
  }
  
  tdata = KTHREAD(get_data)(cur);
  tdata->nextlk = lk->cv;
  lk->cv = cur;
  /* Here we need to reduce the lock count to 1 to be sure
   * the lock is completely released when we invoke slowUnlockMutex.
   */
  oldLockCount = lk->lockCount;
  lk->lockCount = 1;

  putHeavyLock(lk);
  slowUnlockMutex((volatile iLock *volatile *)lkp, heavyLock);
  r = KSEM(get)(&tdata->sem, timeout);
  
  /* Timeout */
  if (r == false) {
    lk = getHeavyLock((volatile iLock *volatile *)lkp, heavyLock);
    /* Remove myself from CV or MUX queue - if I'm * not on either
     * then I should wait on myself to remove any pending signal.
     */
    for (ptr = &lk->cv; *ptr != 0; ptr = &KTHREAD(get_data)(*ptr)->nextlk) {
      if ((*ptr) == cur) {
	*ptr = tdata->nextlk;
	goto found;
      }
    }
    for (ptr = &lk->mux; *ptr != 0; ptr = &KTHREAD(get_data)(*ptr)->nextlk) {
      if ((*ptr) == cur) {
	*ptr = tdata->nextlk;
	goto found;
      }
    }
    /* Not on list - so must have been signalled after all -
     * decrease the semaphore to avoid problems.
     */
    KSEM(get)(&tdata->sem, (jlong)0);
    
  found:;
    putHeavyLock(lk);
  }
  
  slowLockMutex((volatile iLock *volatile *)lkp, heavyLock);
  /* This is safe as no other thread touches the lockcount if it is not
   * owning the lock.
   */
  lk->lockCount = oldLockCount;
  
  return (r);
}

void
locks_internal_signalCond(iLock** lkp, iLock *heavyLock)
{
  volatile iLock* lk;
  jthread_t tid;
  
  DBG(SLOWLOCKS,
      dprintf("_signalCond(lk=%p, th=%p)\n",
	      *lkp, KTHREAD(current)());
      );
  
  lk = getHeavyLock((volatile iLock *volatile *)lkp, heavyLock);
  
  if (lk->holder != KTHREAD(current)()) {
    putHeavyLock(lk);
    throwException(IllegalMonitorStateException);
  }
  
  /* Move one CV's onto the MUX */
  tid = lk->cv;
  if (tid != 0) {
    threadData *tdata = KTHREAD(get_data)(tid);

    lk->cv = tdata->nextlk;
    tdata->nextlk = lk->mux;
    lk->mux = tid;
  }
  
  putHeavyLock(lk);
}

void
locks_internal_broadcastCond(iLock** lkp, iLock *heavyLock)
{
  volatile iLock* lk;
  jthread_t tid;
  
  DBG(SLOWLOCKS,
      dprintf("_broadcastCond(lk=%p, th=%p)\n",
	      *lkp, KTHREAD(current)());
      );
  
  lk = getHeavyLock((volatile iLock *volatile *)lkp, heavyLock);

  if (lk->holder != KTHREAD(current)()) {
    putHeavyLock(lk);
    throwException(IllegalMonitorStateException);
  }
  
  /* Move all the CV's onto the MUX */
  while (lk->cv != 0) {
    threadData *tdata;

    tid = lk->cv;
    tdata = KTHREAD(get_data)(tid);
    lk->cv = tdata->nextlk;
    tdata->nextlk = lk->mux;
    lk->mux = tid;
  }
  
  putHeavyLock(lk);
}

/*
 * Lock a mutex - try to do this quickly but if we failed because
 * we can't determine if this is a multiple entry lock or we've got
 * contention then fall back on a slow lock.
 */
void
locks_internal_lockMutex(iLock** lkp, iLock *heavyLock)
{  
  if (!COMPARE_AND_EXCHANGE(lkp, LOCKFREE, (iLock *)KTHREAD(current)()))
      slowLockMutex((volatile iLock *volatile *)lkp, heavyLock);
}

/*
 * Unlock a mutex - try to do this quickly but if we failed then
 * we've got contention so fall back on a slow lock.
 */
void
locks_internal_unlockMutex(iLock** lkp, iLock *heavyLock)
{
  /* slowUnlockMutex should be fast enough. */
  if (!COMPARE_AND_EXCHANGE(lkp, (iLock *)KTHREAD(current)(), LOCKFREE))
    slowUnlockMutex((volatile iLock * volatile *)lkp, heavyLock);
}

void
lockObject(Hjava_lang_Object* obj)
{
  locks_internal_lockMutex(&obj->lock, NULL);
}

void
unlockObject(Hjava_lang_Object* obj)
{
  locks_internal_unlockMutex(&obj->lock, NULL);
}

void
slowLockObject(Hjava_lang_Object* obj)
{
#if defined(ENABLE_JVMPI)
  jboolean isContention = false;

  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_CONTENDED_ENTER) )
    {
      JVMPI_Event ev;
      
      /* 
       * There may be a race here. Let's hope the profiler
       * does not rely on the exact count of all contention
       * but acquiring the heavy lock here will be too heavy.
       */
      if (IS_HEAVY_LOCK(obj->lock))
	{
	  iLock *hlk = GET_HEAVYLOCK(obj->lock);
	  
	  if (hlk->lockCount != 0)
	    {
	      ev.event_type = JVMPI_EVENT_MONITOR_CONTENDED_ENTER;
	      ev.u.monitor.object = obj;
	      jvmpiPostEvent(&ev);

	      isContention = true;
	    }
	}
    }
#endif
  slowLockMutex((volatile iLock * volatile *)&obj->lock, NULL);
#if defined(ENABLE_JVMPI)
  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_CONTENDED_ENTERED) && isContention)
    {
      JVMPI_Event ev;
      
      ev.event_type = JVMPI_EVENT_MONITOR_CONTENDED_ENTERED;
      ev.u.monitor.object = obj;
      jvmpiPostEvent(&ev);
    }
#endif
}

void
slowUnlockObject(Hjava_lang_Object* obj)
{
#if defined(ENABLE_JVMPI)
  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_CONTENDED_EXIT) )
    {
      iLock *hlk = GET_HEAVYLOCK(obj->lock);
      JVMPI_Event ev;
      
      if (hlk->lockCount > 1)
	{
	  ev.event_type = JVMPI_EVENT_MONITOR_CONTENDED_EXIT;
	  ev.u.monitor.object = obj;
	  jvmpiPostEvent(&ev);
	}
    }
#endif
  slowUnlockMutex((volatile iLock *volatile *)&obj->lock, NULL);
}

void
dumpLocks(void)
{
}

void KaffeLock_destroyLock(Collector *gcif UNUSED, void *l)
{
  iLock *lock = (iLock *)l;

  assert(lock->lockCount == 0);
  assert(lock->num_wait == 0);
  assert(lock->in_progress == 0);
  KSEM(destroy)(&lock->sem);
}
