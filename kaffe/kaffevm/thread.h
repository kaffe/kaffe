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

#include "config.h"
#include "md.h"

#include "native.h"
#include "errors.h"
#include "java_lang_Throwable.h"
#include "java_lang_Thread.h"
#include "java_lang_ThreadGroup.h"

#define	THREADCLASS			"java/lang/Thread"
#define	THREADGROUPCLASS		"java/lang/ThreadGroup"
#define	THREADDEATHCLASS		"java/lang/ThreadDeath"

#define THREAD_MAXPRIO  		(java_lang_Thread_MAX_PRIORITY+1)

/*
 * Interface to the thread system.
 */
void	initThreads(void);
void    initNativeThreads(int nativestacksize);
void	yieldThread(void);
void	sleepThread(jlong);
void	exitThread(void);
bool	aliveThread(Hjava_lang_Thread*);
jint	framesThread(Hjava_lang_Thread*);
void	setPriorityThread(Hjava_lang_Thread*, jint);
void	startThread(Hjava_lang_Thread*);
void	stopThread(Hjava_lang_Thread*, Hjava_lang_Object*);
void	interruptThread(Hjava_lang_Thread*);
void	finalizeThread(Hjava_lang_Thread*);
char*	nameThread(Hjava_lang_Thread*);
char*	nameNativeThread(void*);
Hjava_lang_Thread* getCurrentThread(void);
Hjava_lang_Thread* createDaemon(void*, const char*, void *arg, int,
				size_t, struct _errorInfo *);
void	attachFakedThreadInstance (const char *name, int isDaemon);

extern  Hjava_lang_Class* ThreadClass;
struct  _Collector;

extern	jbool deadlockDetection;

#define THREAD_NATIVE()         ((void*)jthread_current())

#define THREAD_JNIENV()         (&jthread_get_data(jthread_current())->jniEnv)

#define THREAD_DATA()		(jthread_get_data(jthread_current()))

#if !defined(KAFFEH)
/*
 * Inject the ThreadInterface implementation header.
 */
#include "thread-impl.h"
#endif

#endif
