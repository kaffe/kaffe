/*
 * locks.h
 *
 * Manage the 'fastlock' locking system.
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __locks_h
#define __locks_h

#include "thread-impl.h"

#include "md.h"
#include "support.h"
#include "ksem.h"

struct _iLock;

#define	LOCKOBJECT			struct _iLock**
#define	lockMutex(O)			(KTHREAD(disable_stop)(), locks_internal_lockMutex(&(O)->lock, NULL))
#define	unlockMutex(O)			do { locks_internal_unlockMutex(&(O)->lock, NULL); KTHREAD(enable_stop)(); } while (0)
#define	waitCond(O,T)			locks_internal_waitCond(&(O)->lock, NULL, (T))
#define	signalCond(O)			locks_internal_signalCond(&(O)->lock, NULL)
#define	broadcastCond(O)		locks_internal_broadcastCond(&(O)->lock, NULL)

#define	lockStaticMutex(THING)		(KTHREAD(disable_stop)(), locks_internal_lockMutex(&(THING)->lock, &(THING)->heavyLock))
#define	unlockStaticMutex(THING)	do { locks_internal_unlockMutex(&(THING)->lock, &(THING)->heavyLock); KTHREAD(enable_stop)(); } while(0)
#define	waitStaticCond(THING, TIME)	locks_internal_waitCond(&(THING)->lock, &(THING)->heavyLock, (TIME))
#define	signalStaticCond(THING)		locks_internal_signalCond(&(THING)->lock, &(THING)->heavyLock)
#define	broadcastStaticCond(THING)	locks_internal_broadcastCond(&(THING)->lock, &(THING)->heavyLock)

struct Hjava_lang_Object;

/**
 * The "heavy" alternative when fast-locking encounters true
 * contention, and for some of the global locks.  The _iLock
 * works like a monitor (i.e. Java locks).  The "holder" field
 * is a pointer into the stack frame of the thread which
 * acquired the lock (used for validating the holder on an
 * unlock and for distinguishing recursive invocations).
 */
typedef struct _iLock {
  uintp         in_progress;
  uintp         num_wait;
  void*		holder;
  jthread_t	mux;
  jthread_t	cv;
  Ksem          sem;
  uint8         lockCount;
  void*	hlockHolder;
} iLock;

typedef struct _iStaticLock {
	iLock	*lock;
	iLock	heavyLock; 
} iStaticLock;

#define	LOCKFREE	((iLock*)0)

#define GET_HEAVYLOCK(p) ((iLock *)((uintp)(p) & ~(uintp)1))

extern void	initLocking(void);

extern void     initStaticLock(iStaticLock *slock);
extern void     destroyStaticLock(iStaticLock *slock);

/*
 * Java Object locking interface.
 */
extern void	lockObject(struct Hjava_lang_Object*);
extern void	unlockObject(struct Hjava_lang_Object*);
extern void 	slowLockObject(struct Hjava_lang_Object*);
extern void 	slowUnlockObject(struct Hjava_lang_Object*);

extern void	locks_internal_lockMutex(LOCKOBJECT, iLock *);
extern void	locks_internal_unlockMutex(LOCKOBJECT, iLock *);
extern jboolean	locks_internal_waitCond(LOCKOBJECT, iLock *, jlong);
extern void	locks_internal_signalCond(LOCKOBJECT, iLock *);
extern void	locks_internal_broadcastCond(LOCKOBJECT, iLock *);
extern void	locks_internal_slowUnlockMutexIfHeld(LOCKOBJECT, iLock *);

extern void	dumpLocks(void);

extern void KaffeLock_destroyLock(struct _Collector *, void *);

#endif
