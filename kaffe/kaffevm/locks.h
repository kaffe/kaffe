/*
 * fastlocks.h
 * Manage locking system.
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __fastlocks_h
#define __fastlocks_h

#include "md.h"

struct _iLock;

#define	LOCKOBJECT			struct _iLock**
#define	lockMutex(O)			_lockMutex(&(O)->lock, &iLockRoot)
#define	unlockMutex(O)			_unlockMutex(&(O)->lock, &iLockRoot)
#define	waitCond(O,T)			_waitCond(&(O)->lock, (T))
#define	signalCond(O)			_signalCond(&(O)->lock)
#define	broadcastCond(O)		_broadcastCond(&(O)->lock)
#define	holdMutex(O)			_holdMutex(&(O)->lock)

#define	lockStaticMutex(THING)		_lockMutex((THING), &iLockRoot)
#define	unlockStaticMutex(THING)	_unlockMutex((THING), &iLockRoot)
#define	waitStaticCond(THING, TIME)	_waitCond((THING), (TIME))
#define	signalStaticCond(THING)		_signalCond((THING))
#define	broadcastStaticCond(THING)	_broadcastCond((THING))

#define lockJavaMutex           _lockMutex
#define unlockJavaMutex         _unlockMutex

struct Hjava_lang_Thread;
struct Hjava_lang_Object;

typedef struct _iLock {
	void*				holder;
	struct Hjava_lang_Thread*	mux;
	struct Hjava_lang_Thread*	cv;
} iLock;

#define	LOCKINPROGRESS	((iLock*)-1)
#define	LOCKFREE	((iLock*)0)

extern void	_lockMutex(LOCKOBJECT, void*);
extern void	_unlockMutex(LOCKOBJECT, void*);
extern jboolean	_waitCond(LOCKOBJECT, jlong);
extern void	_signalCond(LOCKOBJECT);
extern void	_broadcastCond(LOCKOBJECT);
extern void	_slowUnlockMutexIfHeld(LOCKOBJECT, void*);
extern void	lockObject(struct Hjava_lang_Object*);
extern void	unlockObject(struct Hjava_lang_Object*);
extern void* 	_releaseLock(iLock**);
extern void 	_acquireLock(iLock**, void*);

/*
 * Unblock an object and call a function. When the function returns reclaim
 * the object.
 */
#define	UNBLOCK_EXECUTE(OBJ, FUNC) { \
		void* st = _releaseLock(&((Hjava_lang_Object*)(OBJ))->lock); \
		FUNC; \
		_acquireLock(&((Hjava_lang_Object*)(OBJ))->lock, st); \
	}

/*
 * Used to convert POSIX locks to semaphores.
 */
typedef struct {
	void*	mux;
	void*	cv;
	int	count;
	void*	thd;
} sem2posixLock;

#if 0
/*
 * Lock interface.  Either define the lock/unlock/wait/signal/broadcast
 * set of operations (for POSIX style locking) or else define the
 * semget/semput operations for semaphore style locking.
 */
typedef struct LockInterface {

	void	(*lock)(sem2posixLock*);
	void	(*unlock)(sem2posixLock*);
	jboolean (*wait)(sem2posixLock*, jlong);
	void	(*signal)(sem2posixLock*);

	jboolean (*semget)(void*, jlong);
	void	(*semput)(void*);

} LockInterface;

EXTERN_C LockInterface Kaffe_LockInterface;

#endif

#if !defined(KAFFEH)
/*
 * Inject the LockInterface implementation header.
 */
#include "lock-impl.h"
#endif

#endif
