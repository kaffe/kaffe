/*
 * java.lang.Thread.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "java_lang_Thread.h"
#include "java_lang_VMThread.h"
#include "thread.h"
#include "locks.h"
#include "support.h"
#include "jthread.h"
#include "debug.h"

struct Hjava_lang_Thread*
java_lang_VMThread_currentThread(void)
{
  return (getCurrentThread());
}

/*
 * Yield processor to another thread of the same priority.
 */
void
java_lang_VMThread_yield(void)
{
  yieldThread();
}

/*
 * Start this thread running.
 */
void
java_lang_VMThread_start(struct Hjava_lang_VMThread* this, UNUSED jlong stacksize)
{
  startThread(this);
}

/*
 * Change thread priority.
 */
void
java_lang_VMThread_nativeSetPriority(struct Hjava_lang_VMThread* this, jint prio)
{
  setPriorityThread(this, prio);
}

void
java_lang_VMThread_interrupt(struct Hjava_lang_VMThread* this)
{
  interruptThread(this);
}

void
java_lang_VMThread_finalize(struct Hjava_lang_VMThread* this)
{
  finalizeThread(this);
}

jboolean java_lang_VMThread_interrupted(void)
{
  return jthread_interrupted(jthread_current());
}

jboolean java_lang_VMThread_isInterrupted(Hjava_lang_VMThread *this)
{
  return jthread_is_interrupted((jthread_t)unhand(this)->jthreadID);
}


void java_lang_VMThread_sleep(jlong timeout, UNUSED jint ns)
{
  jthread_t	cur = jthread_current();

  if(jthread_interrupted(cur))
    {
      throwException(InterruptedException);
    }

DBG(VMTHREAD, dprintf ("%p (%p) sleeping for %d\n", cur,
			jthread_get_data(cur)->jlThread, timeout); )

  /*
   * Using the semaphore of this thread for sleeping is safe, since
   * there are only two reasons for another thread to invoke ksemPut
   * on this semaphore:
   *
   *     - it releases a lock this thread is waiting for, or
   *     - it has to signal this thread that it has been started.
   *
   * None of these apply here (we're neither waiting for a lock nor
   * starting a new thread). Since the jthread implementation has to
   * be able to interrupt a thread waiting for its semaphore anyway,
   * this thread can still be interrupted.
   */
  ksemGet(&jthread_get_data(cur)->sem, timeout);

  if(jthread_interrupted(cur))
    {
      throwException(InterruptedException);
    }
}
