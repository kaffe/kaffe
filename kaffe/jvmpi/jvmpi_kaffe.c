
#include "config.h"

#if defined(ENABLE_JVMPI)

#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-hacks.h"
#include "gtypes.h"
#include "native.h"
#include "object.h"
#include "jni.h"
#include "code.h"
#include "classMethod.h"
#include "java_lang_Thread.h"
#include "thread.h"
#include "stackTrace.h"

#include <assert.h>

#include "jvmpi_kaffe.h"

JVMPI_Interface *jvmpiCreateInterface(jint version)
{
	JVMPI_Interface *retval;

	retval = &jvmpi_data.jk_Interface;
	retval->version = version;
	return( retval );
}

void jvmpiPostEvent(JVMPI_Event *ev)
{
	assert(ev->event_type >= 0);
	assert(ev->event_type < JVMPI_EVENT_COUNT);

	ev->env_id = THREAD_JNIENV();
	switch( ev->event_type )
	{
	case JVMPI_EVENT_CLASS_LOAD:
	case JVMPI_EVENT_CLASS_UNLOAD:
	case JVMPI_EVENT_OBJECT_ALLOC:
		gc_disableGC();
		break;
	default:
		break;
	}
	jvmpi_data.jk_Interface.NotifyEvent(ev);
	switch( ev->event_type )
	{
	case JVMPI_EVENT_CLASS_LOAD:
	case JVMPI_EVENT_CLASS_UNLOAD:
	case JVMPI_EVENT_OBJECT_ALLOC:
		gc_enableGC();
		break;
	default:
		break;
	}
}

void jvmpiConvertField(JVMPI_Field *dst, fields *src)
{
	assert(dst != NULL);
	assert(src != NULL);

	dst->field_name = src->name->data;
	dst->field_signature = src->signature->data;
}

void jvmpiConvertMethod(JVMPI_Method *dst, methods *src)
{
	assert(dst != NULL);
	assert(src != NULL);

	dst->method_name = src->name->data;
	dst->method_signature = src->parsed_sig->signature->data;
	if( src->lines != NULL )
	{
		dst->start_lineno = src->lines->entry[0].line_nr;
		dst->end_lineno =
			src->lines->entry[src->lines->length].line_nr;
	}
	else
	{
		dst->start_lineno = -1;
		dst->end_lineno = -1;
	}
	dst->method_id = src;
}

void jvmpiConvertLineno(JVMPI_Lineno *dst,
			lineNumberEntry *src,
			void *start_pc)
{
	assert(dst != NULL);
	assert(src != NULL);

	dst->offset = src->start_pc - (uintp)start_pc;
	dst->lineno = src->line_nr;
}

static jint jvmpiCreateSystemThread(char *name,
				    jint priority,
				    void (*f)(void *))
{
	jint retval;

	if( (priority != JVMPI_NORMAL_PRIORITY) &&
	    (priority != JVMPI_MAXIMUM_PRIORITY) &&
	    (priority != JVMPI_MINIMUM_PRIORITY) )
	{
		retval = JNI_ERR;
	}
	else
	{
		jint mapped_priority = 0;
		Hjava_lang_Thread *th;
		errorInfo einfo;

		switch( priority )
		{
		case JVMPI_NORMAL_PRIORITY:
			mapped_priority = java_lang_Thread_NORM_PRIORITY;
			break;
		case JVMPI_MAXIMUM_PRIORITY:
			mapped_priority = java_lang_Thread_MAX_PRIORITY;
			break;
		case JVMPI_MINIMUM_PRIORITY:
			mapped_priority = java_lang_Thread_MIN_PRIORITY;
			break;
		default:
			assert(0);
			break;
		}
		if( (th = createDaemon(f,
				       name,
				       NULL,
				       mapped_priority,
				       32 * 1024, // XXX
				       &einfo)) != NULL )
		{
			retval = JNI_OK;
		}
		else
		{
			discardErrorInfo(&einfo);
			retval = JNI_ERR;
		}
	}
	return( retval );
}

static jint jvmpiDisableEvent(jint event_type, void *arg)
{
	jint retval;

	switch( event_type )
	{
	case JVMPI_EVENT_HEAP_DUMP:
	case JVMPI_EVENT_MONITOR_DUMP:
	case JVMPI_EVENT_OBJECT_DUMP:
		retval = JVMPI_NOT_AVAILABLE;
		break;
	default:
		BITMAP_CLEAR(jvmpi_data.jk_EventMask, event_type);
		retval = JVMPI_SUCCESS;
		break;
	}
	return( retval );
}

static void jvmpiDisableGC(void)
{
	gc_disableGC();
}

static jint jvmpiEnableEvent(jint event_type, void *arg)
{
	jint retval;
	
	switch( event_type )
	{
	case JVMPI_EVENT_HEAP_DUMP:
	case JVMPI_EVENT_MONITOR_DUMP:
	case JVMPI_EVENT_OBJECT_DUMP:
		retval = JVMPI_NOT_AVAILABLE;
		break;
	default:
		BITMAP_SET(jvmpi_data.jk_EventMask, event_type);
		retval = JVMPI_SUCCESS;
		break;
	}
	return( retval );
}

static void jvmpiEnableGC(void)
{
	gc_enableGC();
}

static void jvmpiGetCallTrace(JVMPI_CallTrace *trace, jint depth)
{
	stackTraceInfo *sti = NULL;
	jthread_t jt;

	assert(trace != NULL);
	assert(trace->env_id != NULL);
	assert(trace->frames != NULL);
	assert(depth > 0);

	trace->num_frames = 0;
	if( (jt = jthread_from_data((threadData *)trace->env_id,
				    &jvmpi_data)) != NULL )
	{
		sti = (stackTraceInfo *)
			buildStackTrace(jt == jthread_current() ?
					NULL :
					&jthread_get_data(jt)->topFrame);
		jthread_resume(jt, &jvmpi_data);
	}
	if( sti != NULL )
	{
		int lpc;

		for( lpc = 0;
		     (sti[lpc].meth != ENDOFSTACK) && (depth > 0);
		     lpc++ )
		{
			JVMPI_CallFrame *cf;
			Method *meth;

			if( (meth = stacktraceFindMethod(&sti[lpc])) == NULL )
				continue;
			
			cf = &trace->frames[trace->num_frames];
			cf->lineno = -1;
			if( meth->lines != NULL )
			{
				uintp linepc = 0;
				int lpc2;
				
				for( lpc2 = 0;
				     lpc2 < meth->lines->length;
				     lpc2++ )
				{
					if( (sti[lpc].pc >=
					     meth->lines->entry[lpc2].
					     start_pc) &&
					    (linepc <=
					     meth->lines->entry[lpc2].
					     start_pc) )
					{
						cf->lineno = meth->lines->
							entry[lpc2].line_nr;
						linepc = meth->lines->
							entry[lpc2].start_pc;
					}
				}
			}
			cf->method_id = meth;
			trace->num_frames += 1;
			depth -= 1;
		}
	}
}

static jlong jvmpiGetCurrentThreadCpuTime(void)
{
	jlong retval;

	retval = jthread_get_usage(jthread_current());
	return( retval );
}

static jobjectID jvmpiGetMethodClass(jmethodID mid)
{
	jobjectID retval;

	assert(mid != NULL);
	
	retval = ((Method *)mid)->class;
	return( retval );
}

static void *jvmpiGetThreadLocalStorage(JNIEnv *env_id)
{
	void *retval = NULL;
	jthread_t jt;

	assert(env_id != NULL);
	
	if( (jt = jthread_from_data((threadData *)env_id,
				    &jvmpi_data)) != NULL )
	{
		retval = jthread_get_data(jt)->jvmpiData;
		jthread_resume(jt, &jvmpi_data);
	}
	return( retval );
}

static jobjectID jvmpiGetThreadObject(JNIEnv *env_id)
{
	jobjectID retval = NULL;
	jthread_t jt;

	assert(env_id != NULL);
	
	if( (jt = jthread_from_data((threadData *)env_id,
				    &jvmpi_data)) != NULL )
	{
		retval = jthread_get_data(jt)->jlThread;
		jthread_resume(jt, &jvmpi_data);
	}
	return( retval );
}

static jint jvmpiGetThreadStatus(JNIEnv *env_id)
{
	jthread_t jt;
	jint retval;

	assert(env_id != NULL);

	if( (jt = jthread_from_data((threadData *)env_id,
				    &jvmpi_data)) != NULL )
	{
		if( jthread_on_mutex(jt) )
		{
			retval = JVMPI_THREAD_MONITOR_WAIT;
		}
		else if( jthread_on_condvar(jt) )
		{
			retval = JVMPI_THREAD_CONDVAR_WAIT;
		}
		else
		{
			switch( jthread_get_status(jt) )
			{
			case THREAD_RUNNING:
				retval = JVMPI_THREAD_RUNNABLE;
				break;
			case THREAD_SUSPENDED:
				/* XXX Should be IO_WAIT or something. */
				retval = JVMPI_THREAD_MONITOR_WAIT;
				break;
			}
		}
		if( jthread_is_interrupted(jt) )
		{
			retval |= JVMPI_THREAD_INTERRUPTED;
		}
		jthread_resume(jt, &jvmpi_data);
	}
	else
	{
		retval = JVMPI_FAIL;
	}
	return( retval );
}

static void jvmpiProfilerExit(jint err_code)
{
	EXIT(err_code);
}

static JVMPI_RawMonitor jvmpiRawMonitorCreate(char *lock_name)
{
	JVMPI_RawMonitor retval;

	if( (retval = jmalloc(sizeof(struct _JVMPI_RawMonitor))) != NULL )
	{
		jmutex_initialise(&retval->mux);
		jcondvar_initialise(&retval->cv);
		retval->lock_name = lock_name;
	}
	return( retval );
}

static void jvmpiRawMonitorDestroy(JVMPI_RawMonitor lock_id)
{
	if( lock_id != NULL )
	{
		jmutex_destroy(&lock_id->mux);
		jcondvar_destroy(&lock_id->cv);
		jfree(lock_id);
	}
}

static void jvmpiRawMonitorEnter(JVMPI_RawMonitor lock_id)
{
	assert(lock_id != NULL);
	
	jmutex_lock(&lock_id->mux);
}

static void jvmpiRawMonitorExit(JVMPI_RawMonitor lock_id)
{
	assert(lock_id != NULL);
	
	jmutex_unlock(&lock_id->mux);
}

static void jvmpiRawMonitorNotifyAll(JVMPI_RawMonitor lock_id)
{
	assert(lock_id != NULL);
	
	jcondvar_broadcast(&lock_id->cv, &lock_id->mux);
}

static void jvmpiRawMonitorWait(JVMPI_RawMonitor lock_id)
{
	assert(lock_id != NULL);
	
	jcondvar_wait(&lock_id->cv, &lock_id->mux, 0);
}

static jint jvmpiRequestEvent(jint event_type, void *arg)
{
	jint retval = JVMPI_NOT_AVAILABLE;

	return( retval );
}

static void jvmpiResumeThread(JNIEnv *env)
{
	jthread_t jt;

	assert(env != NULL);

	if( (jt = jthread_from_data((threadData *)env, &jvmpi_data)) != NULL )
	{
		jthread_resume(jt, &jvmpi_data);
		jthread_resume(jt, &jvmpi_data);
	}
}

static void jvmpiResumeThreadList(jint reqCount, JNIEnv **reqList, jint *results)
{
	int lpc;

	/* XXX */
	for( lpc = 0; lpc < reqCount; lpc++ )
	{
		jvmpiResumeThread(reqList[lpc]);
		results[lpc] = 0;
	}
}

static void jvmpiRunGC(void)
{
	invokeGC();
}

static void jvmpiSetThreadLocalStorage(JNIEnv *env_id, void *ptr)
{
	jthread_t jt;

	assert(env_id != NULL);
	
	if( (jt = jthread_from_data((threadData *)env_id,
				    &jvmpi_data)) != NULL )
	{
		jthread_get_data(jt)->jvmpiData = ptr;
		jthread_resume(jt, &jvmpi_data);
	}
}

static void jvmpiSuspendThread(JNIEnv *env_id)
{
	jthread_from_data((threadData *)env_id, &jvmpi_data);
}

static void jvmpiSuspendThreadList(jint reqCount, JNIEnv **reqList, jint *results)
{
	int lpc;

	assert(reqCount > 0);
	assert(reqList != NULL);
	assert(results != NULL);

	/* XXX */
	for( lpc = 0; lpc < reqCount; lpc++ )
	{
		jvmpiSuspendThread(reqList[lpc]);
		results[lpc] = 0;
	}
}

static jboolean jvmpiThreadHasRun(JNIEnv *env)
{
	jboolean retval = JNI_TRUE;

	/* XXX */
	return( retval );
}

static jobject jvmpijobjectID2jobject(jobjectID jid)
{
	return( jid );
}

static jobjectID jvmpijobject2jobjectID(jobject j)
{
	return( j );
}

jvmpi_kaffe_t jvmpi_data = {
	{ 0 },
	{
		JVMPI_VERSION_1_2,
		
		NULL,
		
		jvmpiEnableEvent,
		jvmpiDisableEvent,
		jvmpiRequestEvent,
		
		jvmpiGetCallTrace,
		
		jvmpiProfilerExit,
		
		jvmpiRawMonitorCreate,
		jvmpiRawMonitorEnter,
		jvmpiRawMonitorExit,
		jvmpiRawMonitorWait,
		jvmpiRawMonitorNotifyAll,
		jvmpiRawMonitorDestroy,

		jvmpiGetCurrentThreadCpuTime,
		jvmpiSuspendThread,
		jvmpiResumeThread,
		jvmpiGetThreadStatus,
		jvmpiThreadHasRun,
		jvmpiCreateSystemThread,

		jvmpiSetThreadLocalStorage,
		jvmpiGetThreadLocalStorage,

		jvmpiDisableGC,
		jvmpiEnableGC,
		jvmpiRunGC,

		jvmpiGetThreadObject,
		jvmpiGetMethodClass,

		jvmpijobjectID2jobject,
		jvmpijobject2jobjectID,

		jvmpiSuspendThreadList,
		jvmpiResumeThreadList
		
	}
};

#endif
