/*
 * java.io.FileDescriptor.c
 *
 * Copyright (c) 2003 Kaffe's team.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "files.h"
#include "defs.h"
#include "java_io_FileDescriptor.h"
#include "jsyscall.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/utf8const.h"
#include "../../../kaffe/kaffevm/support.h"

/* Mask for open Javacall */
#define JAVA_READ 1
#define JAVA_WRITE 2
#define JAVA_APPEND 4
#define JAVA_EXCL 8
#define JAVA_SYNC 16 
#define JAVA_DSYNC 32

/* Seek mode for Javacall */
#define JAVA_SET 0
#define JAVA_CUR 1
#define JAVA_END 2

static Hjava_lang_Class* FileDescriptorClass = NULL;

void java_io_FileDescriptor_nativeInit()
{
  errorInfo einfo;
  struct Hjava_io_FileDescriptor** fd;
  Field* field;
  char *field_names[3] = { "in", "out", "err" };
  int i;

  FileDescriptorClass = lookupClass("java/io/FileDescriptor", NULL, &einfo);
  if (FileDescriptorClass == NULL)
  {
    throwError(&einfo);
  }

  /* Initialize the static fields */
  for (i=0;i<3;i++)
  {
    Utf8Const* utf8;
    utf8 = checkPtr(utf8ConstNew(field_names[i], -1));
    field = lookupClassField(FileDescriptorClass, utf8, true, &einfo);  
    utf8ConstRelease(utf8);

    if (field == NULL)
    {
      throwError(&einfo);
    }
    fd = (struct Hjava_io_FileDescriptor **) FIELD_ADDRESS(field);
    (*fd)->nativeFd = i;
  }
}

jlong java_io_FileDescriptor_nativeOpen(struct Hjava_io_FileDescriptor* this, struct Hjava_lang_String* name, jint mode)
{
  char str[MAXPATHLEN];
  int fd;
  int rc;
  int open_options = 0;
  
  stringJava2CBuf(name, str, sizeof(str));
  
  if ((mode & JAVA_READ) && (mode & JAVA_WRITE))
    open_options = O_RDWR|O_CREAT;
  else if (mode & JAVA_READ)
    open_options = O_RDONLY;
  else {
    open_options = O_WRONLY|O_CREAT;
    if (mode & JAVA_APPEND)
      open_options |= O_APPEND;
    else
      open_options |= O_TRUNC;
  }

#if defined(O_SYNC)
  if (mode & JAVA_SYNC)
    open_options |= O_SYNC;
#endif
#if defined(O_DSYNC)
  if (mode & JAVA_DSYNC)
    open_options |= O_DSYNC;
#endif

  // By default we put the files in all rw access. It will be modified by process umask.
  rc = KOPEN(str, open_options|O_BINARY, 0666, &fd);
  switch (rc) {
  case 0:
    break;
  default:
    SignalError("java.io.FileNotFoundException", SYS_ERROR(rc));
    return -1;
  }
  return fd;
}

jlong java_io_FileDescriptor_nativeClose(struct Hjava_io_FileDescriptor* this, jlong fd)
{
  int rc;

  rc = KCLOSE(fd);
  if (rc != 0)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
      return -1;
    }
  return 0;
}

jlong java_io_FileDescriptor_nativeWriteByte(struct Hjava_io_FileDescriptor* this, jlong fd,
					     jint b)
{
  int rc;
  int nativeFd = (int)fd;
  char native_data = b & 0xFF;
  ssize_t ret;

  do
  {
    rc = KWRITE(nativeFd, &native_data, 1, &ret);
    if (rc != 0 && rc != EINTR)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
      return -1;
    }
  }
  while (rc != 0);
  return 0;
}

jlong java_io_FileDescriptor_nativeWriteBuf(struct Hjava_io_FileDescriptor* this, jlong fd,
					    HArrayOfByte* bytes, jint offset, jint len)
{
  int rc;
  int nativeFd = (int)fd;
  ssize_t nativeLen = (ssize_t)len;
  ssize_t nativeWritten = 0;
  off_t off = (off_t)offset;
  ssize_t ret;

  while (nativeWritten < nativeLen)
  {
    rc = KWRITE(nativeFd, &unhand_array(bytes)->body[off], nativeLen-nativeWritten, &ret);
    if (rc != 0 && rc != EINTR)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
      return -1;
    }
    nativeWritten += ret;
  }
  
  return 0;
}

jint java_io_FileDescriptor_nativeReadByte(struct Hjava_io_FileDescriptor* this,
					   jlong fd)
{
  int rc;
  int nativeFd = (int)fd;
  char native_data;
  ssize_t ret;

  do
  {
    rc = KREAD(nativeFd, &native_data, 1, &ret);
    if (rc != 0 && rc != EINTR)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
      return -1;
    }
  }
  while (ret != 1);
  
  return (jint)(native_data & 0xFF);
}

extern jint java_io_FileDescriptor_nativeReadBuf(struct Hjava_io_FileDescriptor* this, jlong fd,
						 HArrayOfByte* bytes, jint offset, jint len)
{
  int rc;
  int nativeFd = (int)fd;
  ssize_t nativeLen = (ssize_t)len;
  ssize_t nativeRead = 0;
  off_t off = (off_t)offset;
  ssize_t ret;

  while (nativeRead < nativeLen)
  {
    rc = KREAD(nativeFd, &unhand_array(bytes)->body[off], nativeLen-nativeRead, &ret);
    if (rc == 0)
    {
      if (ret == 0)
	return -1;
      else
	return (jint)ret;
    }
    if (rc != 0 && rc != EINTR)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
      return -1;
    }
    nativeRead += ret;
  }
  
  return (jint)nativeRead;
}

jint java_io_FileDescriptor_nativeAvailable(struct Hjava_io_FileDescriptor* this, jlong fd)
{
  int r, nr, rc;
  int nativeFd = (int)fd;
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
    return (nr);
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
	SignalError("java.io.IOException", SYS_ERROR(rc));
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

static int getFileSize(int fd, off_t *fileSize)
{
  struct stat statbuf;

  int rc = KFSTAT(fd, &statbuf);

  if (rc == 0 && S_ISREG(statbuf.st_mode)) {
    *fileSize = statbuf.st_size;
  }
  if (rc == 0)
  {
    rc = EINVAL;
  }
  return rc;
}

jlong java_io_FileDescriptor_nativeSeek(struct Hjava_io_FileDescriptor* this,
					jlong fd, jlong offset, jint whence,
					jboolean stop_at_eof)
{
  int nativeFd = (int)fd;
  int sysWhence;
  int rc;
  off_t sysOffset = (off_t)offset;
  off_t fileSize;
  off_t cur;

  switch (whence)
  {
  case JAVA_SET:
    sysWhence = SEEK_SET;
    break;
  case JAVA_CUR:
    sysWhence = SEEK_CUR;
    break;
  case JAVA_END:
  default:
    sysWhence = SEEK_END;
    break;
  }

  if (stop_at_eof)
  {
    rc = getFileSize(nativeFd, &fileSize);
    if (rc != 0)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
    }
    switch (sysWhence)
    {
    case SEEK_SET:
      if (sysOffset > fileSize)
	sysOffset = fileSize;
      break;
    case SEEK_CUR:
      rc = KLSEEK(nativeFd, 0, SEEK_CUR, &cur);
      if (rc != 0)
      {
	SignalError("java.io.IOException", SYS_ERROR(rc));
      }
      if (cur+sysOffset > fileSize)
      {
	sysOffset = fileSize;
	sysWhence = SEEK_SET;
      }
      break;
    case SEEK_END:
      if (sysOffset > 0)
	sysOffset = 0;
      break;
    default:
      // Not reachable.
      sysOffset = 0;
      break;
    }
  }

  rc = KLSEEK(nativeFd, sysOffset, sysWhence, &cur);
  if (rc != 0)
  {
    SignalError("java.io.IOException", SYS_ERROR(rc));
  }
  return (jlong)cur;
}

jlong java_io_FileDescriptor_nativeGetFilePointer(struct Hjava_io_FileDescriptor* this, jlong fd)
{
  off_t cur;
  int rc;

  rc = KLSEEK((int)fd, 0, SEEK_CUR, &cur);
  if (rc != 0)
  {
    SignalError("java.io.IOException", SYS_ERROR(rc));
  }
  return (jlong)cur;
}

jlong java_io_FileDescriptor_nativeGetLength(struct Hjava_io_FileDescriptor* this, jlong fd)
{
  int rc;
  off_t fileSize;

  rc = getFileSize((int)fd, &fileSize);
  if (rc != 0)
  {
    SignalError("java.io.IOException", SYS_ERROR(rc));
  }
  return (jlong)fileSize;
}

void java_io_FileDescriptor_nativeSetLength(struct Hjava_io_FileDescriptor* this, jlong fd,
					    jlong new_length)
{
  int nativeFd = (int) fd;
  off_t fileSize;
  int rc;

  rc = getFileSize(nativeFd, &fileSize);
  if (rc != 0)
  {
    SignalError("java.io.IOException", SYS_ERROR(rc));
  }
  
  if (new_length < fileSize)
  {
    rc = KFTRUNCATE(nativeFd, new_length);
    if (rc != 0)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
    }
  }
  else
  {
    off_t cur;
    off_t oldPosition;
    char data = 0;
    int ret;

    /* Save the old file position */
    rc = KLSEEK(nativeFd, SEEK_CUR, 0, &oldPosition);
    if (rc != 0)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
    }

    /* Go to new_length-1 */
    rc = KLSEEK(nativeFd, SEEK_SET, new_length-1, &cur);
    if (rc != 0)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
    }

    /* Write a null byte */
    rc = KWRITE(nativeFd, &data, 1, &ret); 
    if (rc != 0)
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
    }
    
    /* Go back to the old position */
    rc = KLSEEK(nativeFd, SEEK_SET, oldPosition, &cur);
    {
      SignalError("java.io.IOException", SYS_ERROR(rc));
    }
  }
}

jboolean java_io_FileDescriptor_nativeValid(struct Hjava_io_FileDescriptor* this, jlong fd)
{
  return 1;
}

void java_io_FileDescriptor_nativeSync(struct Hjava_io_FileDescriptor* this, jlong fd)
{
  int nativeFd = (int)fd;

  if (nativeFd >= 0)
  {
    int r = KFSYNC(nativeFd);
    if (r != 0) {
      SignalError("java.io.SyncFailedException", SYS_ERROR(r));
    }
  }
}
