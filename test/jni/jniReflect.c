/*
 * jniExecClass.c
 *
 * Copyright (c) 2004
 *    The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ltdl.h>

static char *concatString(const char *s1, const char *s2)
{
	char *s;

	if (s1 == NULL)
		s1 = "";
	if (s2 == NULL)
		s2 = "";
	s = (char *) malloc(strlen(s1) + strlen(s2) + 1);
	return strcat(strcpy(s, s1), s2);
}

int main(void)
{
  JavaVMInitArgs vmargs;
  JavaVM *vm;
  void *env;
  JNIEnv *jni_env;
  JavaVMOption myoptions[2];
  jclass cls, refcls;
  jmethodID mainid, nameid;
  jobject reflect;
  const char *name_utf;
  jstring name;
  jboolean isCopy;

  /* set up libtool/libltdl dlopen emulation */
  LTDL_SET_PRELOADED_SYMBOLS();
  
  myoptions[0].optionString = concatString("-Xbootclasspath:", getenv("BOOTCLASSPATH"));
  myoptions[1].optionString = concatString("-Xclasspath:", CLASSPATH_SOURCE_DIR);  

  vmargs.version = JNI_VERSION_1_2;

  if (JNI_GetDefaultJavaVMInitArgs (&vmargs) < 0)
    {
      fprintf(stderr, " Cannot retrieve default arguments\n");
      return 1;
    }

  vmargs.nOptions = 2;
  vmargs.options = myoptions;

  if (JNI_CreateJavaVM (&vm, (void **)&env, &vmargs) < 0)
    {
      fprintf(stderr, " Cannot create the Java VM\n");
      return 1;
    }

  jni_env = env;
  
  cls = (*jni_env)->FindClass(jni_env, "HelloWorldApp");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "FindClass has failed\n");
      return 1;
    }

  mainid = (*jni_env)->GetStaticMethodID(jni_env, cls, "main", "([Ljava/lang/String;)V");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "GetStaticMethodID has failed\n");
      return 1;
    }

  reflect = (*jni_env)->ToReflectedMethod(jni_env, cls, mainid, 1);
  if (reflect == NULL || (*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "ToReflectedMethod has failed\n");
      return 1;
    }

  refcls = (*jni_env)->FindClass(jni_env, "java/lang/reflect/Method");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "FindClass(java/lang/reflect/Method has failed\n");
      return 1;
    }

  nameid = (*jni_env)->GetMethodID(jni_env, refcls, "getName", "()Ljava/lang/String;");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "GetMethodID has failed\n");
      return 1;
    }

  name = (*jni_env)->CallObjectMethod(jni_env, reflect, nameid);
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "getName() has failed\n");
      return 1;
    }
  
  isCopy = 0;
  name_utf = (*jni_env)->GetStringUTFChars(jni_env, name, &isCopy);  

  (*jni_env)->ReleaseStringChars(jni_env, name, name_utf);

  (*vm)->DestroyJavaVM(vm);

  return 0;
}
