/*
 * java.io.File.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003, 2006
 *      Kaffe's team.
 * Copyright (c) 2006
 *      Free Software Foundation, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <native.h>
#include <jsyscall.h>
#ifdef HAVE_UTIME_H
#include <utime.h>
#endif
#include "defs.h"
#include "files.h"
#include "../../../include/system.h"
#include "jni.h"
#include "java_io_VMFile.h"
#include "support.h"
#include "stringSupport.h"

/*
 * Is named item a file?
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_isFile (JNIEnv * env,
			    jobject obj __attribute__ ((__unused__)),
			    jstring name)
{
  const char *filename;
  int r;
  struct stat buf;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }

  r = KSTAT(filename, &buf);
  (*env)->ReleaseStringUTFChars (env, name, filename);

  if (r == 0 && S_ISREG(buf.st_mode)) {
    return (1);
  }
  else {
    return (0);
  }
}

/*
 * Is named item a directory?
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_isDirectory (JNIEnv * env,
				 jobject obj __attribute__ ((__unused__)),
				 jstring name)
{
  const char * filename;
  int r;
  struct stat buf;

  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }
  
  r = KSTAT(filename, &buf);
  (*env)->ReleaseStringUTFChars (env, name, filename);

  if (r == 0 && S_ISDIR(buf.st_mode)) {
    return (1);
  }
  else {
    return (0);
  }
}

/*
 * Does named file exist?
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_exists (JNIEnv * env,
			    jobject obj __attribute__ ((__unused__)),
			    jstring name)
{
  const char *filename;
  int result;
  struct stat buf;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }
  
  /* A file exists if I can stat it */
  result = KSTAT(filename, &buf);
  (*env)->ReleaseStringUTFChars (env, name, filename);

  return result == 0;
}

/*
 * Last modified time on file.
 */
JNIEXPORT jlong JNICALL
Java_java_io_VMFile_lastModified (JNIEnv * env,
				  jobject obj __attribute__ ((__unused__)),
				  jstring name)
{
  const char *filename;
  struct stat buf;
  int r;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }

  
  r = KSTAT(filename, &buf);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  if (r != 0) {
    return ((jlong)0);
  }
  return ((jlong)buf.st_mtime * (jlong)1000);
}

/*
 * Can I write to this file?
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_canWrite (JNIEnv * env,
			      jobject obj __attribute__ ((__unused__)),
			      jstring name)
{
  const char *filename;
  int r;

  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }

  /* XXX make part of jsyscall interface !? */
  r = access(filename, W_OK);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  return (r < 0 ? 0 : 1);
}

/*
 * Can I read from this file.
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_canRead (JNIEnv * env,
			     jobject obj __attribute__ ((__unused__)),
			     jstring name)
{
  const char *filename;
  int r;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }

  /* XXX make part of jsyscall interface !? */
  r = access(filename, R_OK);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  return (r < 0 ? 0 : 1);
}

/*
 * Return length of file.
 */
JNIEXPORT jlong JNICALL
Java_java_io_VMFile_length (JNIEnv * env,
			    jobject obj __attribute__ ((__unused__)),
			    jstring name)
{
  const char *filename;
  struct stat buf;
  int r;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }
  
  r = KSTAT(filename, &buf);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  if (r != 0) {
    return ((jlong)0);
  }
  return ((jlong)buf.st_size);
}

/*
 * Create a directory.
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_mkdir (JNIEnv * env,
			   jobject obj __attribute__ ((__unused__)),
			   jstring name)
{
  const char *filename;
  int r;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }
 
  r = KMKDIR(filename, 0777);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  return (r != 0 ? 0 : 1);
}

/*
 * Rename a file.
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_renameTo (JNIEnv * env,
			      jobject obj __attribute__ ((__unused__)),
			      jstring t, jstring d)
{
  const char *old_filename, *new_filename;
  int r;

  old_filename = (*env)->GetStringUTFChars (env, t, 0);
  if (old_filename == NULL)
    {
      return (0);
    }

  new_filename = (*env)->GetStringUTFChars (env, d, 0);
  if (new_filename == NULL)
    {
      (*env)->ReleaseStringUTFChars (env, t, old_filename);
      return (0);
    }

  r = KRENAME(old_filename, new_filename);

  (*env)->ReleaseStringUTFChars (env, t, old_filename);
  (*env)->ReleaseStringUTFChars (env, t, new_filename);

  return (r != 0 ? 0 : 1);
}

/*
 * Delete a file.
 */
JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_delete (JNIEnv * env,
			    jobject obj __attribute__ ((__unused__)),
			    jstring name)
{
  const char *filename;
  int r;
	
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }

  r = KREMOVE(filename);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  return(r != 0 ? 0 : 1);
}

/*
 * Get a directory listing.
 */
JNIEXPORT jobjectArray JNICALL
Java_java_io_VMFile_list (JNIEnv * env, jobject obj
			  __attribute__ ((__unused__)), jstring name)
{
#if defined(HAVE_DIRENT_H)
  const char *dirname;
  DIR* dir;
  struct dirent* entry;
  struct dentry {
    struct dentry* next;
    char name[1];
  };
  struct dentry* dirlist;
  struct dentry* mentry;
  struct dentry* lastentry;
  HArrayOfObject* array;
  int count;
  int i;
  int oom = 0;

  dirname = (*env)->GetStringUTFChars (env, name, 0);
  if (dirname == NULL)
    {
      return (0);
    }
  
  /* XXX make part of jsyscall interface !? */
  dir = opendir(dirname);

  (*env)->ReleaseStringUTFChars (env, name, dirname);

  if (dir == 0) {
    return (NULL);
  }
  
  dirlist = NULL;
  lastentry = NULL;
  count = 0;
  /* XXX make part of jsyscall interface !? */
  while ((entry = readdir(dir)) != NULL) {
    /* We skip '.' and '..' */
    if (strcmp(".", entry->d_name) == 0 ||
	strcmp("..", entry->d_name) == 0) {
      continue;
    }
    mentry = KMALLOC(sizeof(struct dentry) + NAMLEN(entry));
    if (!mentry) {
      errorInfo info;
      
      while (dirlist) {
	mentry = dirlist;
	dirlist = dirlist->next;
	KFREE(mentry);
      }
      postOutOfMemory(&info);
      throwError(&info);
    }
    strcpy(mentry->name, entry->d_name);
    mentry->next = NULL;
    if (count == 0) {
        dirlist = mentry;
    }
    else {
        lastentry->next = mentry;
    }
    lastentry = mentry;
    count++;
  }
  /* XXX make part of jsyscall interface !? */
  closedir(dir);
  
  array = (HArrayOfObject*)AllocObjectArray(count,
					    "Ljava/lang/String;", NULL);
  /* XXX: This assert is a noop.  If AllocObjectArray throws an
     exception, we leak. */
  assert(array != NULL);
  for (i = 0; i < count; i++) {
    mentry = dirlist;
    dirlist = mentry->next;
    unhand_array(array)->body[i] =
      (Hjava_lang_Object*)checkPtr(stringC2Java(mentry->name));
    /* if allocation fails, continue freeing mentries in
       this loop. */
    oom |= !unhand_array(array)->body[i];
    KFREE(mentry);
  }
  if (oom) {
    errorInfo info;
    postOutOfMemory(&info);
    throwError(&info);
  }
  
  return (array);
#else
  return (0);
#endif
}

JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_create (JNIEnv * env,
			    jclass clazz __attribute__ ((__unused__)),
			    jstring name)
{
  const char *filename;
  int fd;
  int rc;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }
 
  rc = KOPEN(filename, O_EXCL|O_WRONLY|O_BINARY|O_CREAT, 0600, &fd);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  switch (rc) {
  case 0:
    break;
  case EEXIST:
    return 0;
  default:
    SignalError("java.io.IOException", SYS_ERROR(rc));
  }
  rc = KCLOSE(fd);
  if (rc != 0)
    SignalError("java.io.IOException", SYS_ERROR(rc));
  return 1;
}

JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_setLastModified (JNIEnv * env,
				     jobject obj __attribute__ ((__unused__)),
				     jstring name, jlong newtime)
{
#ifdef HAVE_UTIME_H
  const char *filename;
  int result;
  struct utimbuf ub;
#endif
  
  if (newtime < 0)
    SignalError("java.lang.IllegalArgumentException", "time < 0");
#ifdef HAVE_UTIME_H

  ub.actime = (time_t)(newtime / 1000);
  ub.modtime = ub.actime;

  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }

  result = utime(filename, &ub);

  (*env)->ReleaseStringUTFChars (env, name, filename);

  return result >= 0;
#else
  return 0;
#endif
}

JNIEXPORT jboolean JNICALL
Java_java_io_VMFile_setReadOnly (JNIEnv * env,
				 jobject obj __attribute__ ((__unused__)),
				 jstring name)
{
  const char *filename;
  struct stat buf;
  int r;
  
  filename = (*env)->GetStringUTFChars (env, name, 0);
  if (filename == NULL)
    {
      return (0);
    }
  
  r = KSTAT(filename, &buf);

  if (r == 0)
    r = chmod(filename, buf.st_mode & ~(S_IWOTH|S_IWGRP|S_IWUSR));

  (*env)->ReleaseStringUTFChars (env, name, filename);

  return (r == 0);
}

