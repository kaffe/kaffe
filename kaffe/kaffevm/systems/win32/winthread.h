/*
 * winthread.h
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __winthread_h
#define __winthread_h

/*
 * Thread structure
 */
typedef struct nativeThread {
	DWORD				task;
	HANDLE				hand;
	Hjava_lang_Thread*		next;
	uintp				stkbase;
	uintp				stkend;
	void				(*func)(void);
	HAND				sem;
} nativeThread;

#define	JAVAPRIO2WIN32PRIO(P)	(P)

#define	NATIVE_THREAD(T)	((nativeThread*)(unhand(T)->PrivateInfo))

typedef nativeThread* jthread_t;

#endif
