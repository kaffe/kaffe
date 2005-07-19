/*
 * jvmpi.h
 * Java Virtual Machine Profiling Interface
 */

#ifndef _KAFFE_JVMPI_H
#define _KAFFE_JVMPI_H

#include <jni.h>

/* JVMPI version numbers. */
#define JVMPI_VERSION_1	0x10000001
#define JVMPI_VERSION_1_1	0x10000002
#define JVMPI_VERSION_1_2	0x10000003

typedef void *jobjectID;

enum {
	JVMPI_FAIL = -1,
	JVMPI_SUCCESS,
	JVMPI_NOT_AVAILABLE
};

enum {
	JVMPI_THREAD_RUNNABLE = 1,
	JVMPI_THREAD_MONITOR_WAIT,
	JVMPI_THREAD_CONDVAR_WAIT
};

enum {
	JVMPI_THREAD_INTERRUPTED = 0x4000,
	JVMPI_THREAD_SUSPENDED = 0x8000
};

enum {
	JVMPI_MINIMUM_PRIORITY = 1,
	JVMPI_NORMAL_PRIORITY = 5,
	JVMPI_MAXIMUM_PRIORITY = 10
};

enum {
	JVMPI_EVENT_METHOD_ENTRY = 1,
	JVMPI_EVENT_METHOD_ENTRY2,
	JVMPI_EVENT_METHOD_EXIT,
	
	JVMPI_EVENT_OBJECT_ALLOC = 4,
	JVMPI_EVENT_OBJECT_FREE,
	JVMPI_EVENT_OBJECT_MOVE,
	
	JVMPI_EVENT_COMPILED_METHOD_LOAD = 7,
	JVMPI_EVENT_COMPILED_METHOD_UNLOAD,
	
	JVMPI_EVENT_INSTRUCTION_START = 9,
	
	JVMPI_EVENT_THREAD_START = 33,
	JVMPI_EVENT_THREAD_END,
	
	JVMPI_EVENT_CLASS_LOAD_HOOK = 35,
	
	JVMPI_EVENT_HEAP_DUMP = 37,
	
	JVMPI_EVENT_JNI_GLOBALREF_ALLOC = 38,
	JVMPI_EVENT_JNI_GLOBALREF_FREE,
	JVMPI_EVENT_JNI_WEAK_GLOBALREF_ALLOC,
	JVMPI_EVENT_JNI_WEAK_GLOBALREF_FREE,
	
	JVMPI_EVENT_CLASS_LOAD = 42,
	JVMPI_EVENT_CLASS_UNLOAD,
	
	JVMPI_EVENT_DATA_DUMP_REQUEST = 44,
	JVMPI_EVENT_DATA_RESET_REQUEST,
	
	JVMPI_EVENT_JVM_INIT_DONE = 46,
	JVMPI_EVENT_JVM_SHUT_DOWN,
	
	JVMPI_EVENT_ARENA_NEW = 48,
	JVMPI_EVENT_ARENA_DELETE,
	
	JVMPI_EVENT_OBJECT_DUMP = 50,
	
	JVMPI_EVENT_RAW_MONITOR_CONTENDED_ENTER = 51,
	JVMPI_EVENT_RAW_MONITOR_CONTENDED_ENTERED,
	JVMPI_EVENT_RAW_MONITOR_CONTENDED_EXIT,
	JVMPI_EVENT_MONITOR_CONTENDED_ENTER,
	JVMPI_EVENT_MONITOR_CONTENDED_ENTERED,
	JVMPI_EVENT_MONITOR_CONTENDED_EXIT,
	JVMPI_EVENT_MONITOR_WAIT,
	JVMPI_EVENT_MONITOR_WAITED,
	JVMPI_EVENT_MONITOR_DUMP,
	
	JVMPI_EVENT_GC_START = 60,
	JVMPI_EVENT_GC_FINISH,
	
	JVMPI_EVENT_COUNT,
	
	JVMPI_REQUESTED_EVENT = 0x10000000
};

enum {
	JVMPI_DUMP_LEVEL_0,
	JVMPI_DUMP_LEVEL_1,
	JVMPI_DUMP_LEVEL_2
};

enum {
	JVMPI_NORMAL_OBJECT = 0,
	JVMPI_CLASS = 2,
	JVMPI_BOOLEAN = 4,
	JVMPI_CHAR = 5,
	JVMPI_FLOAT = 6,
	JVMPI_DOUBLE = 7,
	JVMPI_BYTE = 8,
	JVMPI_SHORT = 9,
	JVMPI_INT = 10,
	JVMPI_LONG = 11
};

enum {
	JVMPI_GC_ROOT_JNI_GLOBAL = 1,
	JVMPI_GC_ROOT_JNI_LOCAL,
	JVMPI_GC_ROOT_JAVA_FRAME,
	JVMPI_GC_ROOT_NATIVE_STACK,
	JVMPI_GC_ROOT_STICKY_CLASS,
	JVMPI_GC_ROOT_THREAD_BLOCK,
	JVMPI_GC_ROOT_MONITOR_USED,
	JVMPI_GC_ROOT_THREAD_OBJ,
	
	JVMPI_GC_CLASS_DUMP = 0x20,
	JVMPI_GC_INSTANCE_DUMP,
	JVMPI_GC_OBJ_ARRAY_DUMP,
	JVMPI_GC_PRIM_ARRAY_DUMP,
	
	JVMPI_GC_ROOT_UNKNOWN = 0xff
};

enum {
	JVMPI_MONITOR_JAVA = 1,
	JVMPI_MONITOR_RAW
};

typedef struct {
	jint lineno;
	jmethodID method_id;
} JVMPI_CallFrame;

typedef struct {
	JNIEnv *env_id;
	jint num_frames;
	JVMPI_CallFrame *frames;
} JVMPI_CallTrace;

typedef struct {
	const char *field_name;
	const char *field_signature;
} JVMPI_Field;

typedef struct {
	jint heap_dump_level;
} JVMPI_HeapDumpArg;

typedef struct {
	jint offset;
	jint lineno;
} JVMPI_Lineno;

typedef struct {
	const char *method_name;
	const char *method_signature;
	jint start_lineno;
	jint end_lineno;
	jmethodID method_id;
} JVMPI_Method;

typedef struct _JVMPI_RawMonitor *JVMPI_RawMonitor;

typedef struct {
	jint event_type;
	JNIEnv *env_id;
  
	union {
		struct {
			jint arena_id;
			char *arena_name;
		} new_arena;

		struct {
			jint arena_id;
		} delete_arena;
		
		struct {
			unsigned char *class_data;
			jint class_data_len;
			unsigned char *new_class_data;
			jint new_class_data_len;
			void * (*malloc_f)(unsigned int);
		} class_load_hook;

		struct {
			const char *class_name;
			char *source_name;
			jint num_interfaces;
			jint num_methods;
			JVMPI_Method *methods;
			jint num_static_fields;
			JVMPI_Field *statics;
			jint num_instance_fields;
			JVMPI_Field *instances;
			jobjectID class_id;
		} class_load;

		struct {
			jobjectID class_id;
		} class_unload;

		struct {
			jmethodID method_id;
			void *code_addr;
			jint code_size;
			jint lineno_table_size;
			JVMPI_Lineno *lineno_table;
		} compiled_method_load;

		struct { 
			jmethodID method_id;
		} compiled_method_unload;
		
		struct {
			jlong used_objects;
			jlong used_object_space;
			jlong total_object_space;
		} gc_info;

		struct {
			int dump_level;
			char *begin;
			char *end;
			jint num_traces;
			JVMPI_CallTrace *traces;
		} heap_dump;

		struct {
			jobjectID obj_id;
			jobject ref_id;
		} jni_globalref_alloc;

		struct {
			jobject ref_id;
		} jni_globalref_free;

		struct {
			jmethodID method_id;
		} method;
		
		struct {
			jmethodID method_id;
			jobjectID obj_id;
		} method_entry2;

		struct {
			jobjectID object;
		} monitor;

		struct {
			char *begin;
			char *end;
			jint num_traces;
			JVMPI_CallTrace *traces;
			jint *threads_status;
		} monitor_dump;

		struct {
			jobjectID object;
			jlong timeout;
		} monitor_wait;

		struct {
			jint arena_id;
			jobjectID class_id;
			jint is_array;
			jint size;
			jobjectID obj_id;
		} obj_alloc;

		struct {
			jobjectID obj_id;
		} obj_free;

		struct {
			jint data_len;
			char *data;
		} object_dump;

		struct {
			jint arena_id;
			jobjectID obj_id;
			jint new_arena_id;
			jobjectID new_obj_id;
		} obj_move;

		struct {
			char *name;
			JVMPI_RawMonitor id;
		} raw_monitor;

		struct {
			char *thread_name;
			char *group_name;
			char *parent_name;
			jobjectID thread_id;
			JNIEnv *thread_env_id;
		} thread_start;

		struct {
			jmethodID method_id;
			jint offset;
			union {
				struct {
					jboolean is_true;
				} if_info;
				struct {
					jint key;
					jint low;
					jint hi;
				} tableswitch_info;
				struct {
					jint chosen_pair_index;
					jboolean is_default;
				} lookupswitch_info;
			} u;
		} instruction;
	} u;
} JVMPI_Event;

typedef struct {
	jint version;

	void (*NotifyEvent)(JVMPI_Event *event);

	jint (*EnableEvent)(jint event_type, void *arg);
	jint (*DisableEvent)(jint event_type, void *arg);
	jint (*RequestEvent)(jint event_type, void *arg);

	void (*GetCallTrace)(JVMPI_CallTrace *trace, jint depth);

	void (*ProfilerExit)(jint);

	JVMPI_RawMonitor (*RawMonitorCreate)(char *lock_name);
	void (*RawMonitorEnter)(JVMPI_RawMonitor lock_id);
	void (*RawMonitorExit)(JVMPI_RawMonitor lock_id);
	void (*RawMonitorWait)(JVMPI_RawMonitor lock_id, jlong ms);
	void (*RawMonitorNotifyAll)(JVMPI_RawMonitor lock_id);
	void (*RawMonitorDestroy)(JVMPI_RawMonitor lock_id);

	jlong (*GetCurrentThreadCpuTime)(void);
	void (*SuspendThread)(JNIEnv *env);
	void (*ResumeThread)(JNIEnv *env);
	jint (*GetThreadStatus)(JNIEnv *env);
	jboolean (*ThreadHasRun)(JNIEnv *env);
	jint (*CreateSystemThread)(char *name, jint priority, void (*f)(void *));
	void (*SetThreadLocalStorage)(JNIEnv *env_id, void *ptr);
	void * (*GetThreadLocalStorage)(JNIEnv *env_id);

	void (*DisableGC)(void);
	void (*EnableGC)(void);
	void (*RunGC)(void);

	jobjectID (*GetThreadObject)(JNIEnv *env);
	jobjectID (*GetMethodClass)(jmethodID mid);

	/* JNI handle <-> object ID conversions; VERSION_1_1 and newer */

	jobject   (*jobjectID2jobject)(jobjectID jid);
	jobjectID (*jobject2jobjectID)(jobject   j);

	/* VERSION_1_2 and newer: */

	void (*SuspendThreadList)(jint reqCount, JNIEnv **reqList, jint *results);
	void (*ResumeThreadList)(jint reqCount, JNIEnv **reqList, jint *results);
} JVMPI_Interface;

#endif
