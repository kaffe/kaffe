/*
 * jniTestWeak.c
 *
 * Copyright (c) 2005
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
  JavaVMOption myoptions[3];
  jclass cls, scls;
  jarray args;
  jmethodID mainid;

  /* set up libtool/libltdl dlopen emulation */
  LTDL_SET_PRELOADED_SYMBOLS();

  /* set up libtool/libltdl dlopen emulation */
  myoptions[0].optionString = concatString("-Xbootclasspath:", getenv("BOOTCLASSPATH"));
  myoptions[1].optionString = concatString("-Xclasspath:", CLASSPATH_SOURCE_DIR);  
  myoptions[2].optionString = "-Xvmdebug:NATIVELIB";  

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
  
  cls = (*jni_env)->FindClass(jni_env, "JNIWeakTest");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
	    (*jni_env)->ExceptionDescribe(jni_env);
      fprintf(stderr, "FindClass has failed\n");
      return 1;
    }

  mainid = (*jni_env)->GetStaticMethodID(jni_env, cls, "main", "([Ljava/lang/String;)V");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "GetStaticMethodID has failed\n");
      return 1;
    }

  scls = (*jni_env)->FindClass(jni_env, "java/lang/String");
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "FindClass(java/lang/String) has failed\n");
      return 1;
    }

  args = (*jni_env)->NewObjectArray(jni_env, 0, scls, 0);
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "NewObjectArray has failed\n");
      return 1;
    }

  (*jni_env)->CallStaticVoidMethod(jni_env, cls, mainid, args);
  if ((*jni_env)->ExceptionOccurred(jni_env))
    {
      fprintf(stderr, "CallStaticMethod has failed\n");
      return 1;
    }

  (*vm)->DestroyJavaVM(vm);

  return 0;
}
