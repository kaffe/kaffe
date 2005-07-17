/* gnu_java_nio_channels_FileChannelImpl.c -
   Copyright (C) 2003, 2004, 2005  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

/* do not move; needed here because of some macro definitions */
#include <config.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <jni.h>
#include <jcl.h>

#include "gnu_java_nio_channels_FileChannelImpl.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#define IO_EXCEPTION "java/io/IOException"

extern jint get_native_fd(JNIEnv *env, jobject filechannel);

JNIEXPORT jboolean JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_lock (JNIEnv *env, jobject obj,
                                                 jlong position, jlong size,
                                                 jboolean shared, jboolean wait)
{
#ifdef HAVE_FCNTL
  int fd = get_native_fd (env, obj);
  int cmd = wait ? F_SETLKW : F_SETLK;
  struct flock flock;
  int ret;

  flock.l_type = shared ? F_RDLCK : F_WRLCK;
  flock.l_whence = SEEK_SET;
  flock.l_start = (off_t) position;
  flock.l_len = (off_t) size;

  ret = fcntl (fd, cmd, &flock);
  if (ret)
    {
      /* Linux man pages for fcntl state that errno might be either
         EACCES or EAGAIN if we try F_SETLK, and another process has
         an overlapping lock. */
      if (errno != EACCES && errno != EAGAIN)
        {
          JCL_ThrowException (env, IO_EXCEPTION, strerror (errno));
        }
      return JNI_FALSE;
    }
  return JNI_TRUE;
#else
  (void) obj;
  (void) position;
  (void) size;
  (void) shared;
  (void) wait;
  JCL_ThrowException (env, "java/lang/UnsupportedOperationException",
                      "file locks not implemented on this platform");
  return JNI_FALSE;
#endif /* HAVE_FCNTL */
}

JNIEXPORT void JNICALL
Java_gnu_java_nio_channels_FileChannelImpl_unlock (JNIEnv *env,
                                                   jobject obj,
                                                   jlong position,
                                                   jlong length)
{
#ifdef HAVE_FCNTL
  int fd = get_native_fd (env, obj);
  struct flock flock;
  int ret;

  flock.l_type = F_UNLCK;
  flock.l_whence = SEEK_SET;
  flock.l_start = (off_t) position;
  flock.l_len = (off_t) length;

  ret = fcntl (fd, F_SETLK, &flock);
  if (ret)
    {
      JCL_ThrowException (env, IO_EXCEPTION, strerror (errno));
    }
#else
  (void) obj;
  (void) position;
  (void) length;
  JCL_ThrowException (env, "java/lang/UnsupportedOperationException",
                      "file locks not implemented on this platform");
#endif /* HAVE_FCNTL */
}
