
#ifndef __thread_data_h
#define __thread_data_h

#include "jni.h"
#include "stackTrace-impl.h"

/*
 * Structure that defines any per-thread data needed by kaffe.
 */
typedef struct _threadData {
	JNIEnv jniEnv;
	void *jlThread;
	struct _exceptionFrame topFrame;
	void *jvmpiData;
} threadData;

#endif
