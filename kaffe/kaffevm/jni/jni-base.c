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
#include "support.h"

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
      vm_args->options = NULL;
      vm_args->nOptions = 0;
      break;
    default:
      return -1;
  }
  
  return 0;
}

static char *
concatString(const char *s1, const char *s2)
{
  char *s;

  assert(s1 != NULL || s2 != NULL);

  if (s1 == NULL)
    s1 = "";
  if (s2 == NULL)
    s2 = "";
  
  s = (char *)malloc(strlen(s1) + strlen(s2) + 1);

  return strcat(strcpy(s, s1), s2);
}

static
size_t
parseSize(char* arg)
{
	size_t sz;
	char* narg;

	sz = strtol(arg, &narg, 0);
	switch (narg[0]) {
	case 'b': case 'B':
		break;

	case 'k': case 'K':
		sz *= 1024;
		break;

	case 'm': case 'M':
		sz *= 1024 * 1024;
		break;
	}

	return (sz);
}

static jint
KaffeJNI_ParseArgs(KaffeVM_Arguments *args, JavaVMOption *options, jint nOptions)
{
  int i;
  char *classpath = NULL;
  char *bootClasspath = NULL;  
  size_t sz;

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
	  char *internalOpt = strdup(&opt[2]);

	  assert (prop != 0);

	  prop->next = userProperties;
	  userProperties = prop;

	  for (sz = 0; internalOpt[sz] != 0; sz++)
	    {
	      if (internalOpt[sz] == '=')
		{
		  internalOpt[sz] = 0;
		  sz++;
		  break;
		}
	    }
	  prop->key = internalOpt;
	  prop->value = &internalOpt[sz];
	}
      else if (!strncmp(opt, "-Xbootclasspath:", 16))
        {
	  bootClasspath = strdup(opt + 16);
        }
      else if (!strncmp(opt, "-Xbootclasspath/a:", 18))
	{
	  char *newBootPath = concatString(bootClasspath, 
					   concatString(":", opt + 18));
	  
	  if (bootClasspath != NULL)
	    free(bootClasspath);

	  bootClasspath = newBootPath;
	}
      else if (!strncmp(opt, "-Xbootclasspath/p:", 18))
	{
	  char *newBootPath = concatString(opt + 18, 
					   concatString(":", bootClasspath));
	  
	  if (bootClasspath != NULL)
	    free(bootClasspath);

	  bootClasspath = newBootPath;
	}
      else if (!strncmp(opt, "-Xclasspath:", 12))
        {
	  classpath = strdup(opt + 12);
        }
      else if (!strncmp(opt, "-Xclasspath/a:", 14))
	{
	  char *newPath = concatString(classpath, 
				       concatString(":", opt + 14));
	  
	  if (classpath != NULL)
	    free(classpath);

	  classpath = newPath;
	}
      else if (!strncmp(opt, "-Xclasspath/p:", 14))
	{
	  char *newPath = concatString(opt + 14, 
				       concatString(":", classpath));
	  
	  if (classpath != NULL)
	    free(classpath);

	  classpath = newPath;
	}
      else if (!strncmp(opt, "-Xvmdebug:", 10))
	{
	  if (!dbgSetMaskStr(&opt[10]))
	    return 0;
	}
      else if (!strncmp(opt, "-Xss:", 5))
        {
	  sz = parseSize(&opt[5]);
	  if (sz < THREADSTACKSIZE)
	    {
	      fprintf(stderr,  "Warning: Attempt to set stack size smaller than %d - ignored.\n", THREADSTACKSIZE);
	    }
	  else
	    {
	      args->nativeStackSize = sz;
	    }
	  dprintf("Setup stack size to %d\n", sz);
	}
    }
  args->bootClasspath = bootClasspath;
  args->classpath = classpath;

  return 1;
}

jint
JNI_CreateJavaVM(JavaVM** vm, void** penv, void* args)
{
  JavaVMInitArgs *vm_args = (JavaVMInitArgs *)args;
  JNIEnv **env = (JNIEnv **)penv;

  switch (vm_args->version)
    {
    case JNI_VERSION_1_1:
      memcpy(&Kaffe_JavaVMArgs, args, sizeof(Kaffe_JavaVMArgs));
      break;
    case JNI_VERSION_1_2:
      memcpy(&Kaffe_JavaVMArgs, &Kaffe_JavaVMInitArgs, sizeof(Kaffe_JavaVMArgs));
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
