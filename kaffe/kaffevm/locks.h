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

struct _iLock;

#define	LOCKOBJECT			struct _iLock**
#define	lockMutex(O)			(KTHREAD(disable_stop)(), locks_internal_lockMutex(&(O)->lock, &iLockRoot, NULL))
#define	unlockMutex(O)			do { locks_internal_unlockMutex(&(O)->lock, &iLockRoot, NULL); KTHREAD(enable_stop)(); } while (0)
#define	waitCond(O,T)			locks_internal_waitCond(&(O)->lock, (T), NULL)
#define	signalCond(O)			locks_internal_signalCond(&(O)->lock, NULL)
#define	broadcastCond(O)		locks_internal_broadcastCond(&(O)->lock, NULL)

#define	lockStaticMutex(THING)		(KTHREAD(disable_stop)(), locks_internal_lockMutex(&(THING)->lock, &iLockRoot, &(THING)->heavyLock))
#define	unlockStaticMutex(THING)	do { locks_internal_unlockMutex(&(THING)->lock, &iLockRoot, &(THING)->heavyLock); KTHREAD(enable_stop)(); } while(0)
#define	waitStaticCond(THING, TIME)	locks_internal_waitCond(&(THING)->lock, (TIME), &(THING)->heavyLock)
#define	signalStaticCond(THING)		locks_internal_signalCond(&(THING)->lock, &(THING)->heavyLock)
#define	broadcastStaticCond(THING)	locks_internal_broadcastCond(&(THING)->lock, &(THING)->heavyLock)

struct Hjava_lang_Object;

/*
 * The "heavy" alternative when fast-locking encounters true
 * contention, and for some of the global locks.  The _iLock
 * works like a monitor (i.e. Java locks).  The "holder" field
 * is a pointer into the stack frame of the thread which
 * acquired the lock (used for validating the holder on an
 * unlock and for distinguishing recursive invocations).
 */
typedef struct _iLock {
	void*		holder;
	jthread_t	mux;
	jthread_t	cv;
} iLock;

typedef struct _iStaticLock {
	iLock	*lock;
	iLock	heavyLock; 
} iStaticLock;

#define KAFFE_STATIC_LOCK_INITIALIZER { NULL, { NULL, (jthread_t)0, (jthread_t)0 } }

#define	LOCKINPROGRESS	((iLock*)-1)
#define	LOCKFREE	((iLock*)0)

extern void	initLocking(void);

/*
 * Java Object locking interface.
 */
extern void	lockObject(struct Hjava_lang_Object*);
extern void	unlockObject(struct Hjava_lang_Object*);
extern void 	slowLockObject(struct Hjava_lang_Object*, void*);
extern void 	slowUnlockObject(struct Hjava_lang_Object*, void*);

extern void	locks_internal_lockMutex(LOCKOBJECT, void*, iLock *heavyLock);
extern void	locks_internal_unlockMutex(LOCKOBJECT, void*, iLock *heavyLock);
extern jboolean	locks_internal_waitCond(LOCKOBJECT, jlong, iLock *heavyLock);
extern void	locks_internal_signalCond(LOCKOBJECT, iLock *heavyLock);
extern void	locks_internal_broadcastCond(LOCKOBJECT, iLock *heavyLock);
extern void	locks_internal_slowUnlockMutexIfHeld(LOCKOBJECT, void*, iLock *heavyLock);

extern void	dumpLocks(void);

#endif
