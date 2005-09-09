/*
 * jniBase.c -- Test the basic Invocation API.
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
  JavaVMOption myoptions[1];

  /* set up libtool/libltdl dlopen emulation */
  LTDL_SET_PRELOADED_SYMBOLS();
  
  myoptions[0].optionString = concatString("-Xbootclasspath:", getenv("BOOTCLASSPATH"));

  vmargs.version = JNI_VERSION_1_2;
  
  if (JNI_GetDefaultJavaVMInitArgs (&vmargs) < 0)
    {
      fprintf(stderr, " Cannot retrieve default arguments\n");
      return 1;
    }

  vmargs.nOptions = 1;
  vmargs.options = myoptions;

  if (JNI_CreateJavaVM (&vm, &env, &vmargs) < 0)
    {
      fprintf(stderr, " Cannot create the Java VM\n");
      return 1;
    }

  (*vm)->DestroyJavaVM(vm);

  return 0;
}
