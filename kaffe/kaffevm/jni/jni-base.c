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
#include "kaffe_jni.h"

/*
 * Keep track of how many VM's are active. Right now
 * we only support one at a time.
 */
static int Kaffe_NumVM = 0;

extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern KaffeVM_Arguments Kaffe_JavaVMInitArgs;
extern JavaVM Kaffe_JavaVM;

jint
JNI_GetDefaultJavaVMInitArgs(void* args)
{
  JavaVMInitArgs *vm_args = (JavaVMInitArgs *)args;
  switch (vm_args->version)
    {
    case JNI_VERSION_1_1:
      memcpy(args, &Kaffe_JavaVMInitArgs, sizeof(Kaffe_JavaVMInitArgs));
      vm_args->version = JNI_VERSION_1_1;
      break;
    case JNI_VERSION_1_2:
      vm_args->ignoreUnrecognized = JNI_FALSE;
      vm_args->options = JNI_FALSE;
      vm_args->nOptions = 0;
      break;
    default:
      return -1;
  }
  
  return 0;
}

static jint
KaffeJNI_ParseArgs(KaffeVM_Arguments *args, JavaVMOption *options, jint nOptions)
{
  int i;
  
  for (i = 0; i < nOptions; i++)
    {
      char *opt = options[i].optionString;

      if (!strcmp(opt, "vfprintf"))
	args->vfprintf = (jint (*)(FILE*,const char *,va_list))options[i].extraInfo;
      else if (!strcmp(opt, "exit"))
	args->exit = (void (*)(jint))options[i].extraInfo;
      else if (!strcmp(opt, "abort"))
	args->abort = (void (*)(void))options[i].extraInfo;
      else if (!strncmp(opt, "-verbose:", 9))
	{
	  opt += 9;
	  if (!strcmp(opt, "gc"))
	    args->enableVerboseGC = 1;
	  else if (!strcmp(opt, "class"))
	    args->enableVerboseClassloading = 1;
	  else if (!strcmp(opt, "jit"))
	    args->enableVerboseJIT = 1;
	  else if (!strcmp(opt, "call"))
	    args->enableVerboseCall = 1;
	}
      else if (!strcmp(opt, "-verify"))
	args->verifyMode = 3;
      else if (!strcmp(opt, "-verifyremote"))
	args->verifyMode = 2;
      else if (!strcmp(opt, "-noverify"))
	args->verifyMode = 0;
      else if (!strncmp(opt, "-D", 2))
	{
	  userProperty *prop = (userProperty *)malloc(sizeof(userProperty)); 
	  int sz;
	  assert (prop != 0);

	  prop->next = userProperties;
	  userProperties = prop;

	  for (sz = 2; opt[sz] != 0; sz++)
	    {
	      opt[sz] = 0;
	      sz++;
	      break;
	    }
	  prop->key = &opt[2];
	  prop->value = &opt[sz];
	}
    }

  return 1;
}

jint
JNI_CreateJavaVM(JavaVM** vm, JNIEnv** env, void* args)
{
  JavaVMInitArgs *vm_args = (JavaVMInitArgs *)args;

  switch (vm_args->version)
    {
    case JNI_VERSION_1_1:
      memcpy(&Kaffe_JavaVMArgs, args, sizeof(Kaffe_JavaVMArgs));
      break;
    case JNI_VERSION_1_2:
      if (!KaffeJNI_ParseArgs(&Kaffe_JavaVMArgs, vm_args->options, vm_args->nOptions))
	return -1;
      break;
    default:
      return -1;
    }

  /* We can only init. one KVM */
  if (Kaffe_NumVM != 0) {
    return -1;
  }

  /* Setup the machine */
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
	
  return 0;
}

jint
JNI_GetCreatedJavaVMs(JavaVM** vm, jsize buflen UNUSED, jsize* nvm)
{
  vm[0] = &Kaffe_JavaVM;
  *nvm = Kaffe_NumVM;

  return (0);
}
