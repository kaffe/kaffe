#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include <jni.h>
#include "../../../kaffe/kaffevm/debug.h"

void
Java_kaffe_management_Debug_enable
	(JNIEnv *env, jclass thisClass, jobject list)
{
	const char *real_list = (*env)->GetStringUTFChars(env, list, 0);
	/* do we always have alloca? */
	char *copy = alloca(strlen(real_list) + 1);

	strcpy(copy, real_list);
	/* the argument really isn't const, since dbgSetMaskStr uses strtok*/
	dbgSetMaskStr(copy);
	(*env)->ReleaseStringUTFChars(env, list, real_list);
}

void
Java_kaffe_management_Debug_setVerboseGC
	(JNIEnv *env, jclass thisClass, jint level)
{
	Kaffe_JavaVMArgs[0].enableVerboseGC = level;
}

void
Java_kaffe_management_Debug_setVerboseJIT
	(JNIEnv *env, jclass thisClass, jboolean on)
{
	Kaffe_JavaVMArgs[0].enableVerboseJIT = on;
}

void
Java_kaffe_management_Debug_setTracing
	(JNIEnv *env, jclass thisClass, jint level)
{
	Kaffe_JavaVMArgs[0].enableVerboseCall = level;
}

void
Java_kaffe_management_Debug_enableStags
	(JNIEnv *env, jclass thisClass, jobject list)
{
	const char *real_list = (*env)->GetStringUTFChars(env, list, 0);
	/* do we always have alloca? */
	char *copy = alloca(strlen(real_list) + 1);

	strcpy(copy, real_list);
	statsSetMaskStr(copy);
	(*env)->ReleaseStringUTFChars(env, list, real_list);
}

