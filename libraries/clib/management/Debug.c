#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "kaffe_management_Debug.h"
#include "debug.h"

void
Java_kaffe_management_Debug_enable
	(JNIEnv *env, jclass thisClass UNUSED, jobject list)
{
	const char *real_list = (*env)->GetStringUTFChars(env, list, NULL);
	/* do we always have alloca? */
	char *copy = alloca(strlen(real_list) + 1);

	strcpy(copy, real_list);
	/* the argument really isn't const, since dbgSetMaskStr uses strtok*/
	dbgSetMaskStr(copy);
	(*env)->ReleaseStringUTFChars(env, list, real_list);
}

void
Java_kaffe_management_Debug_setVerboseGC
	(JNIEnv *env UNUSED, jclass thisClass UNUSED, jint level)
{
	Kaffe_JavaVMArgs.enableVerboseGC = level;
}

void
Java_kaffe_management_Debug_setVerboseJIT
	(JNIEnv *env UNUSED, jclass thisClass UNUSED, jboolean on)
{
	Kaffe_JavaVMArgs.enableVerboseJIT = on;
}

void
Java_kaffe_management_Debug_setTracing
	(JNIEnv *env UNUSED, jclass thisClass UNUSED, jint level)
{
	Kaffe_JavaVMArgs.enableVerboseCall = level;
}

#ifdef KAFFE_STATS
void
Java_kaffe_management_Debug_enableStats
	(JNIEnv *env, jclass thisClass UNUSED, jobject list)
{
	static char *curStats;	/* kaffe expects this string to be
				 * around at exit.
				 */
	const char *real_list = (*env)->GetStringUTFChars(env, list, NULL);

	if (curStats) jfree(curStats);
	curStats = jmalloc(strlen(real_list) + 1);

	strcpy(curStats, real_list);
	statsSetMaskStr(curStats);
	(*env)->ReleaseStringUTFChars(env, list, real_list);
}
#else
void
Java_kaffe_management_Debug_enableStats
	(JNIEnv *env UNUSED, jclass thisClass UNUSED, jobject list UNUSED)
{
	fputs("Kaffe is not configured for stats\n", stderr);
}
#endif

