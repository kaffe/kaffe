/*
 * thread.h
 * Thread support.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __thread_h
#define __thread_h

#include "md.h"

#include "native.h"
#include "java_lang_Thread.h"
#include "java_lang_ThreadGroup.h"

#define	THREADCLASS			"java/lang/Thread"
#define	THREADGROUPCLASS		"java/lang/ThreadGroup"
#define	THREADDEATHCLASS		"java/lang/ThreadDeath"

#define	NOTIMEOUT			0

#define THREAD_MAXPRIO  (java_lang_Thread_MAX_PRIORITY+1)

/*
 * Interface to the thread system.
 */
void	initThreads(void);
void	yieldThread(void);
void	sleepThread(jlong);
void	exitThread(void) __NORETURN__;
bool	aliveThread(Hjava_lang_Thread*);
jint	framesThread(Hjava_lang_Thread*);
void	setPriorityThread(Hjava_lang_Thread*, jint);
void	startThread(Hjava_lang_Thread*);
void	stopThread(Hjava_lang_Thread*, Hjava_lang_Object*);
void	finalizeThread(Hjava_lang_Thread*);
Hjava_lang_Thread* getCurrentThread(void);

typedef struct ThreadInterface {

	void			(*init)(int nativestacksize);
	void			(*createFirst)(Hjava_lang_Thread*);
	void			(*create)(Hjava_lang_Thread*, void*);
	void			(*sleep)(jlong);
	void			(*yield)(void);
	void			(*setPriority)(Hjava_lang_Thread*, jint);
	void			(*stop)(Hjava_lang_Thread*);
	void			(*exit)(void) __NORETURN__;
	bool			(*alive)(Hjava_lang_Thread*);
	jint			(*frames)(Hjava_lang_Thread*);
	void			(*finalize)(Hjava_lang_Thread*);

	Hjava_lang_Thread*	(*currentJava)(void);
	void*			(*currentNative)(void);

	void			(*GcWalkThreads)(void);
	void			(*GcWalkThread)(Hjava_lang_Thread*);

	void*			(*nextFrame)(void*);

} ThreadInterface;

extern ThreadInterface Kaffe_ThreadInterface;

#endif
