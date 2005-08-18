/*
 * jni-refs.c
 * Java Native Interface - References handling.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004-2005
 * 	The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jni.h"
#include "jnirefs.h"
#include "jni_i.h"
#include "jni_funcs.h"
#include "gc.h"
#include "jvmpi_kaffe.h"

static void
postError(JNIEnv* env, errorInfo* info)
{
	(*env)->Throw(env, error2Throwable(info));
}

void
KaffeJNI_addJNIref(jref obj)
{
	jnirefs* table;
	int idx;

	table = THREAD_DATA()->jnireferences;

	if (table->used == table->frameSize) {
	  KaffeJNI_FatalError(THREAD_JNIENV(), "No more room for local references");
	}

	idx = table->next;
	for (;;) {
		if (table->objects[idx] == 0) {
			table->objects[idx] = obj;
			table->used++;
			table->next = (idx + 1) % table->frameSize;
			return;
		}
		idx = (idx + 1) % table->frameSize;
	}
}

void
KaffeJNI_removeJNIref(jref obj)
{
	int idx;
	jnirefs* table;

	table = THREAD_DATA()->jnireferences;

	for (idx = 0; idx < table->frameSize; idx++) {
		if (table->objects[idx] == obj) {
			table->objects[idx] = NULL;
			table->used--;
			return;
		}
	}
}

void
KaffeJNI_DeleteGlobalRef(JNIEnv* env UNUSED, jref obj)
{
  jref obj_local;
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JNI_GLOBALREF_FREE) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_JNI_GLOBALREF_FREE;
		ev.u.jni_globalref_free.ref_id = obj;
		jvmpiPostEvent(&ev);
	}
#endif

	obj_local = unveil(obj);

	gc_rm_ref(obj_local);
}

void
KaffeJNI_DeleteLocalRef(JNIEnv* env UNUSED, jref obj)
{
	jref obj_local;

	obj_local = unveil(obj);

	REMOVE_REF(obj_local);
}

jboolean
KaffeJNI_IsSameObject(JNIEnv* env UNUSED, jobject obj1, jobject obj2)
{
	if (obj1 == obj2) {
		return (JNI_TRUE);
	}
	else {
		return (JNI_FALSE);
	}
}

jref
KaffeJNI_NewGlobalRef(JNIEnv* env, jref obj)
{
	jref obj_local;
	BEGIN_EXCEPTION_HANDLING(NULL);

	obj_local = unveil(obj);

	if (!gc_add_ref(obj_local)) {
		errorInfo info;
		postOutOfMemory(&info);
		postError(env, &info);
	}
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JNI_GLOBALREF_ALLOC) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_JNI_GLOBALREF_ALLOC;
		ev.u.jni_globalref_alloc.obj_id = obj_local;
		ev.u.jni_globalref_alloc.ref_id = obj_local;
		jvmpiPostEvent(&ev);
	}
#endif
	END_EXCEPTION_HANDLING();
	return obj_local;
}

jint
KaffeJNI_EnsureLocalCapacity(JNIEnv* env, jint capacity)
{
  jint ret = 0;

  BEGIN_EXCEPTION_HANDLING(-1);

  if (thread_data->jnireferences->used+capacity > 
      thread_data->jnireferences->frameSize)
    ret = KaffeJNI_PushLocalFrame(env, capacity);

  END_EXCEPTION_HANDLING();

  return ret;
}

jint
KaffeJNI_PushLocalFrame(JNIEnv* env, jint capacity)
{
  jnirefs *table;

  BEGIN_EXCEPTION_HANDLING(-1);

  if (capacity <= 0)
    return -1;  

  table = gc_malloc
    (sizeof(jnirefs) + sizeof(jref)*capacity,
     KGC_ALLOC_STATIC_THREADDATA);
  if (table == NULL)
    {
      errorInfo info;
      postOutOfMemory(&info);
      postError(env, &info);
      return -1;
    }

  table->prev = thread_data->jnireferences;
  table->frameSize = capacity;
  table->localFrames = thread_data->jnireferences->localFrames+1;

  thread_data->jnireferences = table;

  END_EXCEPTION_HANDLING();
  
  return 0;
}

jobject
KaffeJNI_PopLocalFrame(JNIEnv* env UNUSED, jobject obj)
{
  int localFrames;
  int i;
  jnirefs *table;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj_local = unveil(obj);

  table = thread_data->jnireferences;
  localFrames = table->localFrames;

  /* We must not delete the top JNI local frame as it is done by
   * the native wrapper.
   */
  if (localFrames == 1)
    goto popframe_end;
  
  localFrames = table->localFrames;
  for (localFrames = table->localFrames; localFrames >= 1; localFrames--)
    {
      thread_data->jnireferences = table->prev;
      gc_free(table);
      table = thread_data->jnireferences;
    }
  
  if (obj_local != NULL)
    {
      for (i = 0; i < table->frameSize; i++)
	if (table->objects[i] == obj_local)
	  break;
      
      /* If the object is not already referenced, add a new reference to it.
       */
      if (i == table->frameSize)
	ADD_REF(obj_local); 
    }
  
  END_EXCEPTION_HANDLING();

 popframe_end:
  return obj_local;
} 

jobject
KaffeJNI_NewLocalRef(JNIEnv *env UNUSED, jobject ref)
{
  jobject ref_local;
  BEGIN_EXCEPTION_HANDLING(NULL);

  ref_local = unveil(ref);

  if (ref_local != NULL)
    ADD_REF(ref_local);

  END_EXCEPTION_HANDLING();

  return ref_local;
}

jweak KaffeJNI_NewWeakGlobalRef(JNIEnv *env UNUSED, jobject obj)
{
  jweak ref;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(NULL);

  obj_local = unveil(obj);

  ref = KGC_malloc(main_collector, KGC_ALLOC_VMWEAKREF, sizeof(jobject));

  *((jobject *)ref) = obj_local;
  KGC_addWeakRef(main_collector, ref, obj_local);

  ref = (jweak) ((uintp)ref | 1);

#if defined(ENABLE_JVMPI)
  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JNI_WEAK_GLOBALREF_ALLOC) )
    {
      JVMPI_Event ev;
      
      ev.event_type = JVMPI_EVENT_JNI_WEAK_GLOBALREF_ALLOC;
      ev.u.jni_globalref_alloc.obj_id = obj_local;
      ev.u.jni_globalref_alloc.ref_id = ref;
      jvmpiPostEvent(&ev);
    }
#endif
  END_EXCEPTION_HANDLING();

  return ref;
}

void KaffeJNI_DeleteWeakGlobalRef(JNIEnv *env UNUSED, jweak ref)
{
  jobject obj;

  BEGIN_EXCEPTION_HANDLING_VOID();

  assert(KGC_getObjectIndex(main_collector, ref) == KGC_ALLOC_VMWEAKREF);

#if defined(ENABLE_JVMPI)
  if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JNI_WEAK_GLOBALREF_FREE) )
    {
      JVMPI_Event ev;
      
      ev.event_type = JVMPI_EVENT_JNI_WEAK_GLOBALREF_FREE;
      ev.u.jni_globalref_free.ref_id = ref;
      jvmpiPostEvent(&ev);
    }
#endif

  obj = unveil(ref);

  KGC_rmWeakRef(main_collector, ref, obj);
  KFREE(obj);

  END_EXCEPTION_HANDLING();
}
