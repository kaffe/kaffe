/*
 * java.lang.Thread.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
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
java_lang_Thread_yield0(void)
{
	yieldThread();
}

/*
 * Start this thread running.
 */
void
java_lang_Thread_start0(struct Hjava_lang_Thread* this)
{
	startThread(this);
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

void
java_lang_Thread_interrupt0(struct Hjava_lang_Thread* this)
{
	interruptThread(this);
}

void
java_lang_Thread_finalize0(struct Hjava_lang_Thread* this)
{
	finalizeThread(this);
}
