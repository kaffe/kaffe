/*
 * jni-base.c
 * Java Native Interface - Basic exported JNI functions.
 *
 * Copyright (c) 1996, 1997, 2004
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
#include "config-mem.h"
#include "jni.h"
#include "jnirefs.h"
#include "jni_i.h"
#include "threadData.h"
#include "thread.h"
#include "baseClasses.h"

/*
 * Define the version of JNI we support.
 */
int Kaffe_Java_Major_Version = 1;
int Kaffe_Java_Minor_Version = 1;

/*
 * Keep track of how many VM's are active. Right now
 * we only support one at a time.
 */
static int Kaffe_NumVM = 0;

extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern JavaVMInitArgs Kaffe_JavaVMInitArgs;
extern JavaVM Kaffe_JavaVM;

jint
JNI_GetDefaultJavaVMInitArgs(JavaVMInitArgs* args)
{
	if (args->version != ((Kaffe_Java_Major_Version << 16) | Kaffe_Java_Minor_Version)) {
		return (-1);
	}
	memcpy(args, &Kaffe_JavaVMInitArgs, sizeof(JavaVMInitArgs));
	args->version = (Kaffe_Java_Major_Version << 16) | Kaffe_Java_Minor_Version;
	return (0);
}

jint
JNI_CreateJavaVM(JavaVM** vm, JNIEnv** env, JavaVMInitArgs* args)
{
	if (args->version != ((Kaffe_Java_Major_Version << 16) | Kaffe_Java_Minor_Version)) {
		return (-1);
	}

	/* We can only init. one KVM */
	if (Kaffe_NumVM != 0) {
		return (-1);
	}

	/* Setup the machine */
	Kaffe_JavaVMArgs[0] = *args;
	initialiseKaffe();

	/* Setup JNI for main thread */
#if defined(NEED_JNIREFS)
	THREAD_DATA()->jnireferences = (jnirefs *)gc_malloc(sizeof(jnirefs), &gcNormal);
#endif

	/* Return the VM and JNI we're using */
	*vm = &Kaffe_JavaVM;
	*env = THREAD_JNIENV();
	Kaffe_NumVM++;

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JVM_INIT_DONE) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_JVM_INIT_DONE;
		jvmpiPostEvent(&ev);
	}
#endif
	
	return (0);
}

jint
JNI_GetCreatedJavaVMs(JavaVM** vm, jsize buflen UNUSED, jsize* nvm)
{
	vm[0] = &Kaffe_JavaVM;
	*nvm = Kaffe_NumVM;

	return (0);
}
