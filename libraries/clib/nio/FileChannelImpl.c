/*
 * java.nio.channels.FileChannelImpl.c
 *
 * Copyright (c) 2004 The Kaffe.org's team.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "files.h"
#include "defs.h"
#include "jsyscall.h"
#include "jni.h"
#include "classMethod.h"
#include "utf8const.h"
#include "support.h"

#include "gnu_java_nio_channels_FileChannelImpl.h"
#include "java_nio_MappedByteBufferImpl.h"

static inline void
throwIOException(JNIEnv *env, int rc)
{
  jclass clazz = (*env)->FindClass(env, "java.io.IOException");

  assert(clazz != NULL);
  (*env)->ThrowNew(env, clazz, SYS_ERROR(rc));
}

void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_init(JNIEnv *env, jclass clazz)
{  
  const char *field_names[3] = { "in", "out", "err" };
  const int field_modes[3] = { 
    gnu_java_nio_channels_FileChannelImpl_READ,
    gnu_java_nio_channels_FileChannelImpl_WRITE, 
    gnu_java_nio_channels_FileChannelImpl_WRITE };
  jfieldID field;
  jmethodID mid = (*env)->GetMethodID(env, clazz, "<init>", "(II)V");
  int i;

  if (mid == NULL)
    return;

  /* Initialize the static fields */
  for (i = 0; i < 3; i++)
  {
    jobject channel;

    field = (*env)->GetStaticFieldID(env, clazz, field_names[i], "gnu.java.nio.channels.FileChannelImpl");
    if (field == NULL)
      return;

    channel = (*env)->NewObject(env, clazz, mid, i, field_modes[i]);
    (*env)->SetStaticObjectField(env, clazz, field, channel); 
  }
}

jint JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_open(JNIEnv *env, jobject filechannel UNUSED,
						jstring fileName, jint mode)
{
  const char *str;
  int fd;
  int rc;
  int open_options = 0;
  jclass clazz;
  
  str = (*env)->GetStringUTFChars(env, fileName, NULL);
    
  if ((mode & gnu_java_nio_channels_FileChannelImpl_READ) && (mode & gnu_java_nio_channels_FileChannelImpl_WRITE))
    open_options = O_RDWR|O_CREAT;
  else if (mode & gnu_java_nio_channels_FileChannelImpl_READ)
    open_options = O_RDONLY;
  else {
    open_options = O_WRONLY|O_CREAT;
    if (mode & gnu_java_nio_channels_FileChannelImpl_APPEND)
      open_options |= O_APPEND;
    else
      open_options |= O_TRUNC;
  }

#if defined(O_SYNC)
  if (mode & gnu_java_nio_channels_FileChannelImpl_SYNC)
    open_options |= O_SYNC;
#endif
#if defined(O_DSYNC)
  if (mode & gnu_java_nio_channels_FileChannelImpl_DSYNC)
    open_options |= O_DSYNC;
#endif
#if defined(O_EXCL)
  if (mode & gnu_java_nio_channels_FileChannelImpl_EXCL)
    open_options |= O_EXCL;
#endif

  /* By default we put the files in all rw access. 
   * It will be modified by process umask.
   */
  rc = KOPEN(str, open_options|O_BINARY, 0666, &fd);

  (*env)->ReleaseStringUTFChars(env, fileName, str);

  switch (rc) {
  case 0:
    break;
  default:
    clazz = (*env)->FindClass(env, "java.io.FileNotFoundException");
    (*env)->ThrowNew(env, clazz, SYS_ERROR(rc));
    return -1;
  }
  return fd;
}

static
jint getFD(JNIEnv *env, jobject filechannel)
{
  jclass filechannel_class = (*env)->GetObjectClass(env, filechannel);
  jfieldID fid = (*env)->GetFieldID(env, filechannel_class, "fd", "I");

  return (*env)->GetIntField(env, filechannel, fid);
}

jint JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_available(JNIEnv *env, jobject filechannel)
{
  int r, nr, rc;
  int nativeFd = getFD(env, filechannel);
  off_t cur = 0;
  
  r = KLSEEK(nativeFd, cur, SEEK_CUR, &cur);
  if (r == 0) {
    struct stat statbuf;

    if ((KFSTAT(nativeFd, &statbuf) == 0) && S_ISREG(statbuf.st_mode)) {
      return (statbuf.st_size - cur);
    }
  }
  
  /* If lseek or fstat fail, try another mechanism... */
  
#if defined(HAVE_IOCTL) && defined(FIONREAD)
  /* XXX make part of jsyscall interface */
  r = ioctl(nativeFd, FIONREAD, &nr);
  if (r >= 0 && nr != 0) {
    return nr;
  } else
    /* FIONREAD may report 0 for files for which data is
       available; maybe select will do... */
#endif
    {
      /* This uses select() to work out if we can read - but
       * what happens at the end of file?  */
      static struct timeval tm = { 0, 0 };
      fd_set rd;
      
      FD_ZERO(&rd);
      FD_SET(nativeFd, &rd);
      rc = KSELECT(nativeFd+1, &rd, NULL, NULL, &tm, &r);
      if (rc != 0)
	{
	  throwIOException(env, rc);
	  return 0;
	}
      if (r == 1) {
	nr = 1;
      }
      else {
	nr = 0;
      }
      
      return (nr);
    }
}

jlong JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_implPosition(JNIEnv *env, jobject filechannel)
{  
  int nativeFd = getFD(env, filechannel);
  off_t cur;
  int rc;

  rc = KLSEEK(nativeFd, 0, SEEK_CUR, &cur);
  if (rc != 0)
    {
      throwIOException(env, rc);
      return 0;
    }
  return (jlong)cur;
}

void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_seek(JNIEnv *env, jobject filechannel,
						jlong offset)
{
  int nativeFd = (int)getFD(env, filechannel);
  int sysWhence = SEEK_SET;
  int rc;
  off_t sysOffset = (off_t)offset;
  off_t cur;

  rc = KLSEEK(nativeFd, sysOffset, sysWhence, &cur);
  if (rc != 0)
    throwIOException(env, rc);
}

static
int getFileSize(int fd, off_t *fileSize)
{
  struct stat statbuf;

  int rc = KFSTAT(fd, &statbuf);

  if (rc == 0 && S_ISREG(statbuf.st_mode)) {
    *fileSize = statbuf.st_size;
  } else
    if (rc == 0)
      {
	rc = EINVAL;
      }
  return rc;
}

void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_implTruncate(JNIEnv *env, jobject filechannel,
							jlong newSize)
{
  int nativeFd = (int) getFD(env, filechannel);
  off_t fileSize;
  off_t new_length = (off_t)newSize;
  off_t cur, oldPosition;
  int rc;

  rc = getFileSize(nativeFd, &fileSize);
  if (rc != 0)
    {
      throwIOException(env, rc);
      return;
    }
  
  if (new_length < fileSize)
    {
      /* Get the old file position */
      rc = KLSEEK(nativeFd, 0, SEEK_CUR, &oldPosition);
      if (rc != 0)
        {
          throwIOException(env, rc);
          return;
        }

      if (oldPosition > new_length)
	{
	  /* Go to eof */
	  rc = KLSEEK(nativeFd, new_length, SEEK_SET, &cur);
	  if (rc != 0)
	  {
	    throwIOException(env, rc);
	    return;
	  }
        }

      rc = KFTRUNCATE(nativeFd, new_length);
      if (rc != 0)
	{
	  throwIOException(env, rc);
	  return;
	}
    }
  else
    {
      char data = 0;
      ssize_t ret;
      
      /* Save the old file position */
      rc = KLSEEK(nativeFd, 0, SEEK_CUR, &oldPosition);
      if (rc != 0)
	{
	  throwIOException(env, rc);
	  return;
	}
      
      /* Go to new_length-1 */
      rc = KLSEEK(nativeFd, new_length-1, SEEK_SET, &cur);
      if (rc != 0)
	{
	  throwIOException(env, rc);
	  return;
	}

      /* Write a null byte */
      rc = KWRITE(nativeFd, &data, 1, &ret); 
      if (rc != 0)
	{
	  throwIOException(env, rc);
	  return;
	}
    
      /* Go back to the old position */
      rc = KLSEEK(nativeFd, oldPosition, SEEK_SET, &cur);
      if (rc != 0)
	{
	  throwIOException(env, rc);
	  return;
	}
    }
}

jobject JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_mapImpl(JNIEnv *env, jobject filechannel,
						   jchar mode, jlong pos, jint size)
{
  /* Kaffe's mmapping mode corresponds exactly to java.nio.channels.FileChannel.MapMode numbers. */
  void *memory;
  int rc;
  int nativeFd = (int)getFD(env, filechannel);
  off_t nativePos = (off_t)pos;
  size_t nativeSize = (size_t)size;
  jclass bytebuffer_class = (*env)->FindClass(env, "java.nio.MappedByteBufferImpl");
  jobject bytebuffer;
  jmethodID bytebuffer_init = (*env)->GetMethodID(env, bytebuffer_class, "<init>", "(Lgnu/classpath/RawData;IZ)V");

  int nativeMode;

  assert(bytebuffer_class != NULL);
  assert(bytebuffer_init != NULL);

  switch (mode) 
    {
    case 'r':
      nativeMode = KAFFE_MMAP_READ;
      break;
    case 'c':
      nativeMode = KAFFE_MMAP_PRIVATE;
      break;
    default:
      nativeMode = KAFFE_MMAP_WRITE;
    }

  rc = KMMAP(&memory, &nativeSize, nativeMode, nativeFd, &nativePos);
  if (rc != 0)
    {
      throwIOException(env, rc);
      return NULL;
    }

  bytebuffer = (*env)->NewObject(env, bytebuffer_class, bytebuffer_init,
				 memory, nativeSize, (nativeMode == KAFFE_MMAP_READ));

  return bytebuffer;
}

jlong JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_size(JNIEnv *env, jobject filechannel)
{
  int fd = (int)getFD(env, filechannel);
  int rc;
  off_t fileSize;

  rc = getFileSize(fd, &fileSize);
  if (rc != 0)
    {
      throwIOException(env, rc);
      return 0;
    }
  return (jlong)fileSize;
}

jint JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_read___3BII(JNIEnv *env, jobject filechannel,
						       jbyteArray bytes, jint offset, jint len)
{
  int rc;
  int nativeFd = (int)getFD(env, filechannel);
  ssize_t nativeLen = (ssize_t)len;
  ssize_t nativeRead = 0;
  off_t off = (off_t)offset;
  ssize_t ret;
  jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytes, NULL);

  while (nativeRead < nativeLen)
    {
      rc = KREAD(nativeFd, &nativeBytes[off], (size_t)(nativeLen-nativeRead), &ret);
      if (rc == 0)
	{
	  (*env)->ReleaseByteArrayElements(env, bytes, nativeBytes, 0);
	  if (ret == 0)
	    return -1;
	  else
	    return (jint)ret;
	}
      if (rc != 0 && rc != EINTR)
	{
	  (*env)->ReleaseByteArrayElements(env, bytes, nativeBytes, 0);
	  throwIOException(env, rc);
	  return 0;
	}
      nativeRead += ret;
      off += ret;
    }
  (*env)->ReleaseByteArrayElements(env, bytes, nativeBytes, 0);
  
  return (jint)nativeRead;
}

jint JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_read__(JNIEnv *env, jobject filechannel)
{
  int rc;
  uint8 one_byte;
  int nativeFd = (int)getFD(env, filechannel);
  int ret;

  do 
    {
      rc = KREAD(nativeFd, &one_byte, 1, &ret);
      if (rc != 0 && rc != EINTR)
	{
	  throwIOException(env, rc);
	  return 0;
	}
    }
  while (rc == EINTR);
 
  if (ret == 0)
    return -1;

  return one_byte;
}


void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_write___3BII(JNIEnv *env, jobject filechannel,
							jbyteArray bytes, jint offset, jint len)
{
  int rc;
  int nativeFd = (int)getFD(env, filechannel);
  ssize_t nativeLen = (ssize_t)len;
  ssize_t nativeWritten = 0;
  off_t off = (off_t)offset;
  ssize_t ret;
  jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytes, NULL);

  while (nativeWritten < nativeLen)
    {
      rc = KWRITE(nativeFd, &nativeBytes[off], (size_t)(nativeLen-nativeWritten), &ret);
      if (rc != 0 && rc != EINTR)
	{
	  (*env)->ReleaseByteArrayElements(env, bytes, nativeBytes, 0);
	  throwIOException(env, rc);
	  return;
	}
      nativeWritten += ret;
      off += ret;
    }

  (*env)->ReleaseByteArrayElements(env, bytes, nativeBytes, 0);
}

void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_write__I(JNIEnv *env, jobject filechannel, jint byte)						  
{
  int rc;
  int nativeFd = (int)getFD(env, filechannel);
  uint8 real_byte = byte;
  int ret;

  do {
    rc = KWRITE(nativeFd, &real_byte, 1, &ret);
    if (rc != 0 && rc != EINTR)
      {
	throwIOException(env, rc);
	return;
      }
  } while (rc == EINTR);
}

void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_implCloseChannel(JNIEnv *env, jobject filechannel)
{
  int fd = (int)getFD(env, filechannel);
  int rc = KCLOSE(fd);

  if (rc != 0)
    throwIOException(env, rc);
}

jboolean JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_lock(JNIEnv *env UNUSED, jobject filechannel UNUSED, jlong position UNUSED,
						jlong size UNUSED, jboolean shared UNUSED, 
						jboolean wait_lock UNUSED)
{
  (*env)->ThrowNew(env, (*env)->FindClass(env, "java.io.IOException"), "java.nio.FileChannelImpl.lock(): not implemented");
  return false;
}

void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_unlock(JNIEnv *env UNUSED, jobject filechannel UNUSED, jlong position UNUSED,
						  jlong size UNUSED)
{
  (*env)->ThrowNew(env, (*env)->FindClass(env, "java.io.IOException"), "java.nio.FileChannelImpl.unlock(): not implemented");
}

/*
 * MappedByteBufferImpl part
 */

void JNICALL
Java_java_nio_MappedByteBufferImpl_unmapImpl(JNIEnv *env, jobject mapbuffer)
{
  jclass clazz = (*env)->GetObjectClass(env, mapbuffer);
  jfieldID address_id = (*env)->GetFieldID(env, clazz, "implPtr", "gnu.classpath.RawData");
  jfieldID length_id = (*env)->GetFieldID(env, clazz, "implLen", "J");
  jobject address = (*env)->GetObjectField(env, mapbuffer, address_id);
  jlong length = (*env)->GetLongField(env, mapbuffer, length_id);
  size_t size = (size_t)length;

  int rc = KMUNMAP(address, size);
  if (rc != 0)
    throwIOException(env, rc);
}

jboolean JNICALL
Java_java_nio_MappedByteBufferImpl_isLoadedImpl(JNIEnv *env UNUSED, jobject mapbuffer UNUSED)
{
  return true;
}

void JNICALL
Java_java_nio_MappedByteBufferImpl_loadImpl(JNIEnv *env UNUSED, jobject mapbuffer UNUSED)
{
}

void JNICALL
Java_java_nio_MappedByteBufferImpl_forceImpl(JNIEnv *env, jobject mapbuffer)
{
  jclass clazz = (*env)->GetObjectClass(env, mapbuffer);
  jfieldID address_id = (*env)->GetFieldID(env, clazz, "implPtr", "gnu.classpath.RawData");
  jfieldID length_id = (*env)->GetFieldID(env, clazz, "implLen", "J");
  jobject address = (*env)->GetObjectField(env, mapbuffer, address_id);
  jlong length = (*env)->GetLongField(env, mapbuffer, length_id);
  size_t size = (size_t)length;

  int rc = KMSYNC(address, size);
  if (rc != 0)
    throwIOException(env, rc);
}
