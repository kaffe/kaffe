/*
 * locks.h
 * Manage locking system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __locks_h
#define __locks_h

#include "md.h"

/*
 * These functions are declared here to avoid weird kaffeh problems.
 */
void	dontStopThread(void);
void	canStopThread(void);

/*
 * Locks come in three varieties.  Normal locks are associated with
 * java objects, and maintained by C code.  Static mutexs are not
 * associated with java objects and are also maintained by C code.
 * Java mutexes are assoicated with java objects and maintained by
 * java code.
 *
 * When a lock is maintained by java code, dispatchException is aware
 * of it and will release the lock as the stack is unwound.  But, when
 * a lock is maintained by C code, dispatchException cannot release
 * it.  For this reason, we must delay Thread.stop() until all normal
 * and static mutexes are released.
 */

#define	lockMutex(THING) (dontStopThread(), _lockMutex((THING)))
#define	unlockMutex(THING)			\
	_unlockMutex((THING));			\
	canStopThread()
#define	waitCond			_waitCond
#define	signalCond			_signalCond
#define	broadcastCond			_broadcastCond
#define	holdMutex			_holdMutex
#define	unlockKnownMutex(THING)			\
	_unlockMutexFree((THING));		\
	canStopThread()

#define	initStaticLock(THING)		__initLock((THING), #THING)
#define staticLockIsInitialized(THING)	((THING)->ref == -1)
#define	lockStaticMutex(THING)			\
	dontStopThread();			\
	__lockMutex((THING))
#define	unlockStaticMutex(THING)		\
	__unlockMutex((THING));			\
	canStopThread()
#define	waitStaticCond(THING, TIME)	__waitCond((THING), (TIME))
#define	signalStaticCond(THING)		__signalCond((THING))
#define	broadcastStaticCond(THING)	__broadcastCond((THING))

#define lockJavaMutex		_lockMutex
#define unlockJavaMutex		_unlockMutex
#define unlockKnownJavaMutex	_unlockMutexFree

struct Hjava_lang_Thread;

typedef struct _iLock {
	const void*		address;
	struct _iLock*		next;
	int			ref;
	void*			holder;
	int			count;
	void*			mux;
	void*			cv;
} iLock;

extern iLock*	getLock(void*);
extern void	__initLock(iLock* lk, const char *lkname);
extern void	__lockMutex(iLock*);
extern void	__unlockMutex(iLock*);
extern int	__waitCond(iLock*, jlong);
extern void	__signalCond(iLock*);
extern void	__broadcastCond(iLock*);
extern int	__holdMutex(iLock*);

extern iLock*	_lockMutex(void*);
extern void	_unlockMutex(void*);
extern void	_unlockMutexFree(iLock*lk);
extern int	_waitCond(void*, jlong);
extern void	_signalCond(void*);
extern void	_broadcastCond(void*);
extern int	_holdMutex(void*);

typedef struct LockInterface {

	void	(*init)(iLock*);
	void	(*lock)(iLock*);
	void	(*unlock)(iLock*);
	void	(*wait)(iLock*, jlong);
	void	(*signal)(iLock*);
	void	(*broadcast)(iLock*);

	void	(*spinon)(void*);
	void	(*spinoff)(void*);

} LockInterface;

extern LockInterface Kaffe_LockInterface;

extern void dumpLocks(void);
#endif
