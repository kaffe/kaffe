/*
 * java.lang.Object.c
 *
 * Copyright (c) 2003, 2004
 *      The Kaffe.org's developers. All Rights reserved.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "gtypes.h"
#include "locks.h"
#include "object.h"
#include "constants.h"
#include "access.h"
#include "soft.h"
#include "baseClasses.h"
#include "java_lang_Object.h"
#include "java_lang_Cloneable.h"
#include "java_lang_VMObject.h"
#include "thread.h"
#include "jvmpi_kaffe.h"
#include "debug.h"

/*
 * Return class object for this object.
 */
struct Hjava_lang_Class*
java_lang_VMObject_getClass(struct Hjava_lang_Object* o)
{
  return (OBJECT_CLASS(o));
}

/*
 * Notify threads waiting here.
 */
void
java_lang_VMObject_notifyAll(struct Hjava_lang_Object* o)
{
  broadcastCond(o);
}

/*
 * Notify a thread waiting here.
 */
void
java_lang_VMObject_notify(struct Hjava_lang_Object* o)
{
  signalCond(o);
}

/*
 * Clone me.
 */
struct Hjava_lang_Object*
java_lang_VMObject_clone(struct Hjava_lang_Cloneable* c)
{
  Hjava_lang_Object *o = (Hjava_lang_Object *)c;
  Hjava_lang_Object* obj;
  Hjava_lang_Class* class;
  
  class = OBJECT_CLASS(o);
  
  if (!CLASS_IS_ARRAY(class))
    {
      /* Check class is cloneable and throw exception if it isn't */
      if (soft_instanceof(CloneClass, o) == 0)
	{
	  SignalError("java.lang.CloneNotSupportedException", class->name->data);
	}
      /* Clone an object */
      obj = newObject(class);
      memcpy(OBJECT_DATA(obj), OBJECT_DATA(o), CLASS_FSIZE(class) - sizeof(Hjava_lang_Object));
    }
  else
    {
      /* Clone an array */
      obj = newArray(CLASS_ELEMENT_TYPE(class), ARRAY_SIZE(o));
      memcpy(ARRAY_DATA(obj), ARRAY_DATA(o),
	     (size_t) ARRAY_SIZE(o) * TYPE_SIZE(CLASS_ELEMENT_TYPE(class)));
    }
  return (obj);
}

/*
 * Wait for this object to be notified.
 */
void
java_lang_VMObject_wait(struct Hjava_lang_Object* o, jlong timeout, UNUSED jint ns)
{
  jthread_t cur = KTHREAD(current)();

  if(KTHREAD(interrupted)(cur))
    {
      throwException(InterruptedException);
    }

DBG(VMTHREAD, dprintf ("%p (%p) waiting for %p, %lli\n",
			cur, KTHREAD(get_data)(cur)->jlThread,
			o, timeout); );

#if defined(ENABLE_JVMPI)
  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_WAIT) )
    {
      JVMPI_Event ev;
      
      ev.event_type = JVMPI_EVENT_MONITOR_WAIT;
      if( o == (struct Hjava_lang_Object*) getCurrentThread()->vmThread )
	{
	  ev.u.monitor_wait.object = NULL;
	}
      else
	{
	  ev.u.monitor_wait.object = o;
	}
      ev.u.monitor_wait.timeout = timeout;
      jvmpiPostEvent(&ev);
    }
#endif
  waitCond(o, timeout);
#if defined(ENABLE_JVMPI)
  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_MONITOR_WAITED) )
    {
      JVMPI_Event ev;
      
      ev.event_type = JVMPI_EVENT_MONITOR_WAITED;
      if( o == (struct Hjava_lang_Object*) getCurrentThread()->vmThread )
	{
	  ev.u.monitor_wait.object = NULL;
	}
      else
	{
	  ev.u.monitor_wait.object = o;
	}
      ev.u.monitor_wait.timeout = timeout;
      jvmpiPostEvent(&ev);
    }
#endif

  if(KTHREAD(interrupted)(cur))
    {
      throwException(InterruptedException);
    }
}
