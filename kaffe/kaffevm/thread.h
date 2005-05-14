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
#include "java_lang_VMThread.h"
#include "java_lang_ThreadGroup.h"

#define	THREADCLASS			"java/lang/Thread"
#define VMTHREADCLASS			"java/lang/VMThread"
#define	THREADGROUPCLASS		"java/lang/ThreadGroup"

#define THREAD_MAXPRIO  		(java_lang_Thread_MAX_PRIORITY+1)

/*
 * Interface to the VM thread system.
 */
void	initThreads(void);
void    initNativeThreads(int nativestacksize);
void	yieldThread(void);
void	exitThread(void);
void	setPriorityThread(Hjava_lang_VMThread*, jint);
void	startThread(Hjava_lang_VMThread*);
void	stopThread(Hjava_lang_VMThread*, Hjava_lang_Object*);
void	interruptThread(Hjava_lang_VMThread*);
void	finalizeThread(Hjava_lang_VMThread*);
char*	nameThread(Hjava_lang_VMThread*);
char*	nameNativeThread(void*);
void	KaffeVM_unlinkNativeAndJavaThread(void);



Hjava_lang_Thread* getCurrentThread(void);
Hjava_lang_VMThread* getCurrentVMThread(void);
Hjava_lang_Thread* createDaemon(void*, const char*, void *arg, int,
				size_t, struct _errorInfo *);
void	KaffeVM_attachFakedThreadInstance (const char *name, int isDaemon);

extern  Hjava_lang_Class* ThreadClass;
struct  _Collector;

extern  void KaffeVM_setDeadlockDetection(jboolean detection);

#define THREAD_NATIVE()         ((void*)KTHREAD(current)())

#define THREAD_JNIENV()         (&KTHREAD(get_data)(KTHREAD(current)())->jniEnv)

#define THREAD_DATA()		(KTHREAD(get_data)(KTHREAD(current)()))

#if !defined(KAFFEH)
/*
 * Inject the ThreadInterface implementation header.
 */
#include "thread-impl.h"
#endif

#endif
