/*
 * java.lang.Thread.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include <native.h>
#include "../../../kaffe/kaffevm/thread.h"
#include "../../../kaffe/kaffevm/locks.h"
#include "../../../kaffe/kaffevm/support.h"

struct Hjava_lang_Thread*
java_lang_Thread_currentThread(void)
{
	return (getCurrentThread());
}

/*
 * Yield processor to another thread of the same priority.
 */
void
java_lang_Thread_yield(void)
{
	yieldThread();
}

/*
 * Put current thread to sleep for a time.
 */
void
java_lang_Thread_sleep0(jlong time)
{
	sleepThread(time);
}

/*
 * Start this thread running.
 */
void
java_lang_Thread_start(struct Hjava_lang_Thread* this)
{
	startThread(this);
}

/*
 * Is this thread alive?
 */
jbool
java_lang_Thread_isAlive(struct Hjava_lang_Thread* this)
{
	return (aliveThread(this));
}

/*
 * Number of stack.  One for the moment.
 */
jint
java_lang_Thread_countStackFrames(struct Hjava_lang_Thread* this)
{
	return (framesThread(this));
}

/*
 * Change thread priority.
 */
void
java_lang_Thread_setPriority0(struct Hjava_lang_Thread* this, jint prio)
{
	setPriorityThread(this, prio);
}

/*
 * Stop a thread in its tracks.
 */
void
java_lang_Thread_stop0(struct Hjava_lang_Thread* this, struct Hjava_lang_Object* obj)
{
	stopThread(this, obj);
}

/*
 * We suspend a thread by waiting on itself - this is going
 * away in JDK 1.2 anyhow.
 */
void
java_lang_Thread_suspend0(struct Hjava_lang_Thread* this)
{
	lockMutex(this);
	waitCond(this, 0);
	unlockMutex(this);
}

void
java_lang_Thread_resume0(struct Hjava_lang_Thread* this)
{
	lockMutex(this);
	signalCond(this);
	unlockMutex(this);
}

jbool
java_lang_Thread_isInterrupted(struct Hjava_lang_Thread* this, jbool val)
{
	unimp("java.lang.Thread:isInterrupted unimplemented");
}

void
java_lang_Thread_interrupt0(struct Hjava_lang_Thread* this)
{
	unimp("java.lang.Thread:interrupt0 unimplemented");
}
