/*
 * kaffe.lang.UNIXProcess.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developpers.
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

/* Note: this file uses JNI throughout. */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "kaffe/jmalloc.h"
#include <jni.h>
#include <jsyscall.h>
#include <native.h>
#include <files.h>
#include "gtypes.h"
#include "system.h"
#include "kaffe_lang_UNIXProcess.h"
#include "gnu_java_nio_channels_FileChannelImpl.h"

static void
freevec(char **v)
{
  char **p = v;
  if (v)
    {
      while (*p) KFREE(*p++);
      KFREE(v);
    }
}

static inline jboolean
checkPointer(JNIEnv *env, jref pointer, const char *message)
{
  jclass error_class;

  if (pointer != NULL)
    return true;

  error_class = (*env)->FindClass(env, "java.lang.InternalError");

  if (error_class == NULL)
    return false;

  (*env)->ThrowNew(env, error_class, message);
  return false;
}

jint JNICALL
Java_kaffe_lang_UNIXProcess_forkAndExec
(JNIEnv* env,
 jobject proc, jobjectArray args, jobjectArray envs, jstring dir)
{
  jint pid;
  char** argv;
  char** arge;
  const jbyte *dirchars;
  char *dirbuf;
  jint ioes[4];
  int arglen;
  int envlen;
  int i;
  int rc;
  jclass ioexc_class;
  jclass filechannelimpl_class;
  jclass filedescriptor_class;
  jmethodID init_filechan;
  jmethodID init_filedesc;
  jclass proc_class;
  jfieldID pidField;
  /* the names given to the stream in Java */
  const char *fd_names[] = { "stdin_fd", 
			     "stdout_fd", 
			     "stderr_fd", 
			     "sync_fd"};
  const jint iomodes[] = { 
    gnu_java_nio_channels_FileChannelImpl_WRITE,
    gnu_java_nio_channels_FileChannelImpl_READ,
    gnu_java_nio_channels_FileChannelImpl_READ,
    gnu_java_nio_channels_FileChannelImpl_WRITE
  };
 
  ioexc_class = (*env)->FindClass(env, "java.io.IOException");
  if (!checkPointer(env, ioexc_class, "Cannot find java.io.IOException"))
    return -1;

  filechannelimpl_class = (*env)->FindClass(env, "gnu.java.nio.channels.FileChannelImpl");
  if (!checkPointer(env, filechannelimpl_class, "Cannot find gnu.java.nio.channels.FileChannelImpl"))
    return -1;

  filedescriptor_class = (*env)->FindClass(env, "java.io.FileDescriptor");
  if (!checkPointer(env, filedescriptor_class, "Cannot find java.io.FileDescriptor"))
    return -1;

  init_filechan = (*env)->GetMethodID(env, filechannelimpl_class, "<init>", "(II)V");
  if (!checkPointer(env, init_filechan, "Cannot find gnu.java.nio.channels.FileChannelImpl's constructor"))
    return -1;

  init_filedesc = (*env)->GetMethodID(env, filedescriptor_class, "<init>", "(Ljava/nio/channels/ByteChannel;)V");
  if (!checkPointer(env, init_filedesc, "Cannont find java.io.FileDescriptor's constructor"))
    return -1;

  arglen = (args ? (*env)->GetArrayLength(env, args) : 0);
  envlen = (envs ? (*env)->GetArrayLength(env, envs) : 0);

  if (arglen < 1) {
    (*env)->ThrowNew(env, ioexc_class, "No such file");
    return -1;
  }

  /* Build arguments and environment */
  argv = KCALLOC(arglen + 1, sizeof(jbyte*));
  if (!argv)
    {
      errorInfo info;
      postOutOfMemory(&info);
      throwError(&info);
    }

  for (i = 0; i < arglen; i++)
    {
      jstring argi;
      const jbyte *argichars;

      argi = (jstring)(*env)->GetObjectArrayElement(env, args, i);
      argichars = (*env)->GetStringUTFChars(env, argi, NULL);
      argv[i] = KMALLOC(strlen(argichars) + 1);
      if (argv[i])
	{
	  strcpy(argv[i], argichars);
	}
      (*env)->ReleaseStringUTFChars(env, argi, argichars);
      (*env)->DeleteLocalRef(env, argi);

      if (!argv[i])
	{
	  errorInfo info;

	  freevec(argv);
	  postOutOfMemory(&info);
	  throwError(&info);
	}
    }

  if (envlen > 0)
    {
      arge = KCALLOC(envlen + 1, sizeof(jbyte*));
      if (!arge) 
	{
	  errorInfo info;

	  freevec(argv);
	  postOutOfMemory(&info);
	  throwError(&info);
	}
    } else
      arge = NULL;

  for (i = 0; i < envlen; i++) 
    {
      jstring envi;
      const jbyte *envichars;

      envi = (jstring)(*env)->GetObjectArrayElement(env, envs, i);
      envichars = (*env)->GetStringUTFChars(env, envi, NULL);
      arge[i] = KMALLOC(strlen(envichars) + 1);
      if (arge[i])
	{
	  strcpy(arge[i], envichars);
	}
      (*env)->ReleaseStringUTFChars(env, envi, envichars);
      (*env)->DeleteLocalRef(env, envi);
      if (!arge[i])
	{
	  errorInfo info;

	  freevec(argv);
	  freevec(arge);
	  postOutOfMemory(&info);
	  throwError(&info);
	}
    }

  /* Search program in path */
  rc = ENOENT;
  if (strstr (argv[0], file_separator) == NULL)
    {
      char **var;
      char *ptr, *nptr;

      /* Search PATH variable */
      ptr = NULL;
      for (var = arge; var != NULL && *var != NULL; var++)
	{
	  if (strncmp (*var, "PATH=", 5) == 0)
	    {
	      ptr = *var + 5;
	      break;
	    }
	}
      if (ptr == NULL)
	{
	  ptr = getenv ("PATH");
	}

      /* Search program in path */
      for (; ptr != NULL; ptr = nptr)
	{
	  char *file;

	  nptr = strstr (ptr, path_separator);
	  if (nptr == NULL)
	    {
	      file = KMALLOC (strlen (ptr)
			      + strlen (file_separator)
			      + strlen (argv[0]) + 1);
	      if (!file) {
		errorInfo info;

		freevec(argv);
		freevec(arge);
		postOutOfMemory(&info);
		throwError(&info);
	      }
	      strcpy (file, ptr);
	      strcat (file, file_separator);
	    }
	  else if (nptr == ptr)
	    {
	      /* Assume empty allow current dir */
	      file = KMALLOC (strlen (argv[0]) + 1);
	      nptr += strlen (path_separator);
	    }
	  else
	    {
	      file = KMALLOC (nptr - ptr
			      + strlen (file_separator)
			      + strlen (argv[0]) + 1);
	      if (!file)
		{
		  errorInfo info;

		  freevec(argv);
		  freevec(arge);
		  postOutOfMemory(&info);
		  throwError(&info);
		}
	      strncpy (file, ptr, (size_t)(nptr - ptr));
	      file[nptr - ptr] = 0;
	      strcat (file, file_separator);
	      nptr += strlen (path_separator);
	    }
	  strcat (file, argv[0]);
	  if (access (file, X_OK) != -1)
	    {
	      rc = 0;
	      nptr = NULL;
	      KFREE (argv[0]);
	      argv[0] = file;
	    }
	  else
	    {
	      KFREE (file);
	    }
	}
    }
  else if (access (argv[0], X_OK) != -1)
    {
      rc = 0;
    }

  /* Get working directory for new process */
  dirchars = (*env)->GetStringUTFChars(env, dir, NULL);
  dirbuf = KMALLOC(strlen(dirchars) + 1);
  if (dirbuf != 0)
    {
      strcpy(dirbuf, dirchars);
    }
  (*env)->ReleaseStringUTFChars(env, dir, dirchars);
  if (dirbuf == 0)
    {
      errorInfo info;

      freevec(argv);
      freevec(arge);
      postOutOfMemory(&info);
      throwError(&info);
    }

  /* Do the fork & exec */
  if (!rc)
    {
      rc = KFORKEXEC(argv, arge, ioes, &pid, dirbuf);
    }

  freevec(argv);
  freevec(arge);
  KFREE(dirbuf);

  if (rc)
    {
      (*env)->ThrowNew(env, ioexc_class, SYS_ERROR(rc));
      return (-1);
    }

  /* get kaffe.lang.UNIXProcess class */
  proc_class = (*env)->GetObjectClass(env, proc);

  /* Save PID */
  pidField = (*env)->GetFieldID(env, proc_class, "pid", "I");
  (*env)->SetIntField(env, proc, pidField, pid);


  /*
   * Here we initialize the FileDescriptor object using FileChannelImpl.
   *
   * Note: even though it is likely that `pfd_field' and `fd_field'
   * will be identical throughout the loop, this code does not assume
   * that.  Hence, it will work even when used with reloaded classes.
   */
  for (i = 0; i < 4; i++)
    {
      jobject filechannel;
      jobject filedesc;
      jfieldID pfd_field = (*env)->GetFieldID(env, proc_class, 
					      fd_names[i], "java.io.FileDescriptor");
      
      if (!checkPointer(env, ioexc_class, "Cannot find a descriptor field in gnu.java.nio.channels.FileChannelImpl"))
	return -1;
      
      filechannel = (*env)->NewObject(env, filechannelimpl_class, init_filechan, ioes[i], iomodes[i]);
      if (filechannel == NULL)
	return -1;

      filedesc = (*env)->NewObject(env, filedescriptor_class, init_filedesc, filechannel);
      if (filechannel == NULL)
	return -1;

      (*env)->SetObjectField(env, proc, pfd_field, filedesc);
    }

  return 0;
}

/*
 * Wait for a process to exit.
 */
jint
Java_kaffe_lang_UNIXProcess_execWait(JNIEnv* env, jobject process)
{
  int npid;
  int status = -1;
  jfieldID pid = (*env)->GetFieldID(env, 
				    (*env)->GetObjectClass(env, process), 
				    "pid", "I");
  KWAITPID((*env)->GetIntField(env, process, pid), &status, 0, &npid);
  return (status);
}

/*
 * Send a process a signal
 */
void
Java_kaffe_lang_UNIXProcess_sendSignal0(JNIEnv *e UNUSED, jclass c UNUSED, jint pid, jint sig)
{
#if defined(HAVE_KILL)
  KKILL(pid, sig);
#else
  unimp("kill() not provided");
#endif
}

/*
 * Return the signal that kills a process
 */
jint 
Java_kaffe_lang_UNIXProcess_getKillSignal(JNIEnv* env UNUSED, jclass c UNUSED)
{
#if defined(HAVE_KILL)
  return SIGTERM;
#else
  return -1;
#endif
}

