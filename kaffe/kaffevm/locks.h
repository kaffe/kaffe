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

#ifndef KAFFEH
#include "jthread.h"
#endif

#include "md.h"

struct _iLock;

#define	LOCKOBJECT			struct _iLock**
#define	lockMutex(O)			(jthread_disable_stop(), _lockMutex(&(O)->lock, &iLockRoot))
#define	unlockMutex(O)			do { _unlockMutex(&(O)->lock, &iLockRoot); jthread_enable_stop(); } while (0)
#define	waitCond(O,T)			_waitCond(&(O)->lock, (T))
#define	signalCond(O)			_signalCond(&(O)->lock)
#define	broadcastCond(O)		_broadcastCond(&(O)->lock)
#define	holdMutex(O)			_holdMutex(&(O)->lock)

#define	lockStaticMutex(THING)		(jthread_disable_stop(), _lockMutex((THING), &iLockRoot))
#define	unlockStaticMutex(THING)	do { _unlockMutex((THING), &iLockRoot); jthread_enable_stop(); } while(0)
#define	waitStaticCond(THING, TIME)	_waitCond((THING), (TIME))
#define	signalStaticCond(THING)		_signalCond((THING))
#define	broadcastStaticCond(THING)	_broadcastCond((THING))

struct Hjava_lang_Thread;
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
	void*				holder;
	struct Hjava_lang_Thread*	mux;
	struct Hjava_lang_Thread*	cv;
} iLock;

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

extern void	_lockMutex(LOCKOBJECT, void*);
extern void	_unlockMutex(LOCKOBJECT, void*);
extern jboolean	_waitCond(LOCKOBJECT, jlong);
extern void	_signalCond(LOCKOBJECT);
extern void	_broadcastCond(LOCKOBJECT);
extern void	_slowUnlockMutexIfHeld(LOCKOBJECT, void*);
extern void* 	_releaseLock(iLock**);
extern void 	_acquireLock(iLock**, void*);

extern void	dumpLocks(void);

/*
 * Unblock an object and call a function. When the function returns reclaim
 * the object.
 */
#define	UNBLOCK_EXECUTE(OBJ, FUNC) { \
		void* st = _releaseLock(&((Hjava_lang_Object*)(OBJ))->lock); \
		FUNC; \
		_acquireLock(&((Hjava_lang_Object*)(OBJ))->lock, st); \
	}

#endif
