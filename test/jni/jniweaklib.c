#include <jni.h>

JNIEXPORT jobject JNICALL
Java_JNIWeakTest_testWeak(JNIEnv *env, jobject obj)
{
	return (*env)->NewWeakGlobalRef(env, obj);
}	
