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

#define	lockMutex			_lockMutex
#define	unlockMutex			_unlockMutex
#define	waitCond			_waitCond
#define	signalCond			_signalCond
#define	broadcastCond			_broadcastCond
#define	holdMutex			_holdMutex

#define	initStaticLock(THING)		__initLock((THING))
#define	lockStaticMutex(THING)		__lockMutex((THING))
#define	unlockStaticMutex(THING)	__unlockMutex((THING))
#define	waitStaticCond(THING, TIME)	__waitCond((THING), (TIME))
#define	signalStaticCond(THING)		__signalCond((THING))
#define	broadcastStaticCond(THING)	__broadcastCond((THING))

struct Hjava_lang_Thread;

typedef struct _iLock {
	void*			address;
	struct _iLock*		next;
	int			ref;
	void*			holder;
	int			count;
	void*			mux;
	void*			cv;
} iLock;

extern iLock*	getLock(void*);
extern void	__initLock(iLock* lk);
extern void	__lockMutex(iLock*);
extern void	__unlockMutex(iLock*);
extern int	__waitCond(iLock*, jlong);
extern void	__signalCond(iLock*);
extern void	__broadcastCond(iLock*);
extern int	__holdMutex(iLock*);

extern void	_lockMutex(void*);
extern void	_unlockMutex(void*);
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

#endif
