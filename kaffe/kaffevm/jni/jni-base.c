/*
 * jni-base.c
 * Java Native Interface - Basic exported JNI functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004, 2005
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "debug.h"
#include "jni.h"
#include "jnirefs.h"
#include "jni_i.h"
#include "threadData.h"
#include "thread.h"
#include "baseClasses.h"
#include "kaffe_jni.h"
#include "support.h"
#include "classMethod.h"
#include "jvmpi_kaffe.h"
#include "external.h"

/*
 * Keep track of how many VM's are active. Right now
 * we only support one at a time.
 */
static int Kaffe_NumVM = 0;
static jthread_t startingThread;

extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern KaffeVM_Arguments Kaffe_JavaVMInitArgs;

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
    case JNI_VERSION_1_4:
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
	default:
		break;
	}

	return (sz);
}

/**
 * Parse a property set by the user. 
 * Users can set properties with the -D switch. 
 * 
 * This function modifies the global variable userProperties.
 *
 * @param opt the option string to parse
 */ 
static 
void
KaffeJNI_ParseUserProperty(char * opt)
{
  userProperty *prop; 
  unsigned int sz;
  char *internalOpt;

  /* Allocate a new property. */
  prop = (userProperty *)malloc(sizeof(userProperty)); 
  assert (prop != NULL);

  /* Skip '-D' in the option string. */
  internalOpt = strdup(&opt[2]);

  /* Insert new property at head of the user properties list. */
  prop->next = userProperties;
  userProperties = prop;

  /* Search for '=' to find out whether the property is
   * assigned a value.
   */
  for (sz = 0; internalOpt[sz] != 0; sz++)
    {
      if (internalOpt[sz] == '=')
	{
	  /* Split the property string into property 
	   * name and value.
	   */
	  internalOpt[sz] = '\0';
	  sz++;
	  break;
	}
    }

  /* Initialize new property with name and value. */
  prop->key = internalOpt;
  prop->value = &internalOpt[sz];
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
	args->jni_vfprintf = (jint (*)(FILE*,const char *,va_list))options[i].extraInfo;
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
	  KaffeJNI_ParseUserProperty(opt);
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
	  DBG(INIT,
	    dprintf("Setup stack size to %zd\n", sz);
	  );
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
  jnirefs *reftable;

  switch (vm_args->version)
    {
    case JNI_VERSION_1_1:
      memcpy(&Kaffe_JavaVMArgs, args, sizeof(Kaffe_JavaVMArgs));
      break;
    case JNI_VERSION_1_4:
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
  reftable =
    (jnirefs *)gc_malloc(sizeof(jnirefs) + sizeof(jref) * DEFAULT_JNIREFS_NUMBER,
			 KGC_ALLOC_STATIC_THREADDATA);
  reftable->frameSize = DEFAULT_JNIREFS_NUMBER;
  reftable->localFrames = 1;
  THREAD_DATA()->jnireferences = reftable; 

  /* Return the VM and JNI we're using */
  *vm = KaffeJNI_GetKaffeVM();
  *env = THREAD_JNIENV();
  startingThread = KTHREAD(current)();
  Kaffe_NumVM++;

#if defined(ENABLE_JVMPI)
  if (Kaffe_JavaVMArgs.profilerLibname != NULL)
    {
      char errbuf[256];
      jint (*onloadProfiler)(JavaVM *jvm, const char *options, void *reserved);

      if (loadNativeLibrary(Kaffe_JavaVMArgs.profilerLibname, NULL, errbuf, sizeof(errbuf)) < 0)
	{
	  fprintf(stderr,
		  "Unable to load %s: %s\n",
		  Kaffe_JavaVMArgs.profilerLibname,
		  errbuf);
	  exit(1);
	}
      
      onloadProfiler = (jint (*)(JavaVM *jvm, const char *options, void *reserved))loadNativeLibrarySym("JVM_OnLoad");
      if (onloadProfiler != NULL && onloadProfiler (*vm, Kaffe_JavaVMArgs.profilerArguments, NULL) < 0)
	exit(1);
    }
  
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
KaffeJNI_DestroyJavaVM(JavaVM* vm UNUSED)
{
  /* The destroy function must be called by the same thread that has
   * built the VM object
   */
  if (KTHREAD(current)() != startingThread)
    return -1;

  exitThread();
  return 0;
}

jint
JNI_GetCreatedJavaVMs(JavaVM** vm, jsize buflen UNUSED, jsize* nvm)
{
  vm[0] = KaffeJNI_GetKaffeVM();
  *nvm = Kaffe_NumVM;

  return (0);
}
