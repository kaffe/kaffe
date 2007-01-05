/* gnu_java_nio_channel_KqueueSelectorImpl.c -- 
   Copyright (C) 2006  Free Software Foundation, Inc.

This file is a part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

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
exception statement from your version.  */


#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#if HAVE_SYS_EVENT_H
#include <sys/event.h>
#endif /* HAVE_SYS_EVENT_H */
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <jni.h>
#include <gnu_java_nio_KqueueSelectorImpl.h>

#include <jcl.h>

#define KEY_OP_ACCEPT  16
#define KEY_OP_CONNECT  8
#define KEY_OP_READ     1
#define KEY_OP_WRITE    4

/* XXX this requires -std=gnu99 or c99 */
/* #ifdef TRACE_KQUEUE */
/* #define TRACE(fmt, ...) fprintf (stderr, "%s: " fmt "\n", __FUNCTION__, __VA_ARGS__); */
/* #else */
/* #define TRACE(fmt, ...) */
/* #endif */


#define throw_not_supported(env) JCL_ThrowException (env, "java/lang/UnsupportedOperationException", "kqueue/kevent support not available")


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    kqueue_supported
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_kqueue_1supported (JNIEnv *env __attribute__((unused)),
                                                        jclass clazz __attribute__((unused)))
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  return JNI_TRUE;
#else
  return JNI_FALSE;
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    sizeof_struct_kevent
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_sizeof_1struct_1kevent
(JNIEnv *env __attribute__((unused)), jclass clazz __attribute__((unused)))
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
/*   TRACE("return sizeof %lu", sizeof (struct kevent)); */
  return sizeof (struct kevent);
#else
  throw_not_supported (env);
  return -1;
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    implOpen
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_implOpen
(JNIEnv *env, jclass clazz __attribute__((unused)))
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  int kq = kqueue ();
/*   TRACE("kqueue returns %d", kq); */
  if (kq == -1)
    JCL_ThrowException (env, "java/io/IOException", strerror (errno));
  return kq;
#else
  throw_not_supported (env);
  return -1;
#endif
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    implClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_implClose (JNIEnv *env,
                                                jclass clazz __attribute__((unused)),
                                                jint kq)
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
/*   TRACE("closing %d", kq); */
  if (close (kq) != 0)
    JCL_ThrowException (env, "java/io/IOException", strerror (errno));
#else
  (void) kq;
  throw_not_supported (env);
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    kevent_set
 * Signature: (Ljava/nio/ByteBuffer;IIIZ)V
 */
JNIEXPORT void JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_kevent_1set (JNIEnv *env,
                                                  jclass clazz __attribute__((unused)),
                                                  jobject nstate, jint fd,
                                                  jint ops, jint key, jboolean delete)
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  struct kevent *kev = (struct kevent *) (*env)->GetDirectBufferAddress (env, nstate);
  short ident = kev->ident;

  if (fd != 0)
    ident = fd;

  if ((ops & KEY_OP_READ) || (ops & KEY_OP_ACCEPT))
    {
      /* Add the event if we never added it before. */
      if (kev[0].flags == 0 && JNI_FALSE == delete)
        EV_SET(&kev[0], ident, EVFILT_READ, EV_ADD, 0, 0, (void *) key);
      /* Otherwise, delete or reenable it. */
      else if (kev[0].flags != 0)
        EV_SET(&kev[0], ident, EVFILT_READ, (JNI_TRUE == delete) ? EV_DELETE : EV_ENABLE,
               0, 0, (void *) key);
    }
  else
    {
      /* Means we've added this event before, and need to disable it. */
      if (kev[0].flags != 0)
        EV_SET(&kev[0], ident, EVFILT_READ, EV_DISABLE, 0, 0, (void *) key);
    }

  /* Do the same thing for the write filter. */
  if ((ops & KEY_OP_WRITE) || (ops & KEY_OP_CONNECT))
    {
      if (kev[1].flags == 0 && JNI_FALSE == delete)
        EV_SET(&kev[1], ident, EVFILT_WRITE, EV_ADD, 0, 0, (void *) key);
      else if (kev[1].flags != 0)
        EV_SET(&kev[1], ident, EVFILT_WRITE, (JNI_TRUE == delete) ? EV_DELETE : EV_ENABLE,
               0, 0, (void *) key);
    }
  else
    {
      if (kev[1].flags != 0)
        EV_SET(&kev[1], ident, EVFILT_WRITE, EV_DISABLE, 0, 0, (void *) key);
    }
#else
  (void) nstate;
  (void) fd;
  (void) ops;
  (void) key;
  (void) delete;
  throw_not_supported (env);
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    kevent
 * Signature: (ILjava/nio/ByteBuffer;IJ)I
 */
JNIEXPORT jint JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_kevent (JNIEnv *env,
                                             jobject this __attribute__((unused)),
                                             jint kq, jobject nstate, jint nevents,
                                             jlong timeout)
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  struct timespec tv;
  struct timespec *t = NULL;
  struct kevent *kev = (struct kevent *) (*env)->GetDirectBufferAddress (env, nstate);
  int ret;

#ifdef TRACE_KQUEUE
  int i;
  for (i = 0; i < nevents; i++)
    {
      printf ("kevent [%d]: ident:%u filter:%x flags:%o fflags:%o data:%p udata:%p\n",
              i, (unsigned) kev[i].ident, kev[i].filter, kev[i].flags, kev[i].fflags,
              (void *) kev[i].data, kev[i].udata);
    }
#endif

/*   TRACE("events: %p; nevents: %d; timeout: %lld", (void *) kev, nevents, timeout); */

  if (timeout != -1)
    {
      tv.tv_sec = timeout / 1000;
      tv.tv_nsec = (timeout % 1000) * 1000;
      t = &tv;
    }

  ret = kevent (kq, (const struct kevent *) kev, nevents, kev, nevents, t);

  if (ret == -1)
    {
      if (errno == EINTR)
        ret = 0;
      else
        JCL_ThrowException (env, "java/io/IOException", strerror (errno));
    }

#ifdef TRACE_KQUEUE
  for (i = 0; i < ret; i++)
    {
      printf ("kevent [%d]: ident:%u filter:%x flags:%o fflags:%o data:%p udata:%p\n",
              i, (unsigned) kev[i].ident, kev[i].filter, kev[i].flags, kev[i].fflags,
              (void *) kev[i].data, kev[i].udata);
    }
#endif

  return ret;
#else
  (void) kq;
  (void) nstate;
  (void) nevents;
  (void) timeout;
  throw_not_supported (env);
  return -1;
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    fetch_key
 * Signature: (Ljava/nio/ByteBuffer;)I;
 */
JNIEXPORT jint JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_fetch_1key (JNIEnv *env,
                                                 jclass clazz __attribute__((unused)),
                                                 jobject nstate)
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  struct kevent *kev = (struct kevent *) (*env)->GetDirectBufferAddress (env, nstate);
/*   TRACE("return key %p\n", kev->udata); */
  return (jint) kev->udata;
#else
  (void) nstate;
  throw_not_supported (env);
  return -1;
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    ready_ops
 * Signature: (Ljava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_ready_1ops (JNIEnv *env,
                                                 jclass clazz __attribute__((unused)),
                                                 jobject nstate, jint interest)
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  struct kevent *kev = (struct kevent *) (*env)->GetDirectBufferAddress (env, nstate);
  jint ready = 0;

  /* We poll for READ for OP_READ and OP_ACCEPT. */
  if (kev->filter == EVFILT_READ)
    {
      ready = (interest & KEY_OP_READ) | (interest & KEY_OP_ACCEPT);
/*       TRACE("filter EVFILT_READ. Ready ops set to %x", ready); */
    }

  /* Poll for WRITE for OP_WRITE and OP_CONNECT; I guess we *should*
     get a WRITE event if we are connected, but I don't know if we do
     for real. FIXME */
  if (kev->filter == EVFILT_WRITE)
    {
      ready = (interest & KEY_OP_WRITE) | (interest & KEY_OP_CONNECT);
/*       TRACE("filter EVFILT_WRITE. Ready ops set to %x", ready); */
    }

  return ready;
#else
  (void) nstate;
  (void) interest;
  throw_not_supported (env);
  return -1;
#endif /* HAVE_KQUEUE && HAVE_KEVENT */
}


/*
 * Class:     gnu_java_nio_KqueueSelectorImpl
 * Method:    check_eof
 * Signature: (Ljava/nio/ByteBuffer;)Z
 */
JNIEXPORT jboolean JNICALL
Java_gnu_java_nio_KqueueSelectorImpl_check_1eof (JNIEnv *env,
                                                 jclass clazz __attribute__((unused)),
                                                 jobject nstate)
{
#if defined(HAVE_KQUEUE) && defined(HAVE_KEVENT)
  struct kevent *kev = (struct kevent *) (*env)->GetDirectBufferAddress (env, nstate);
  if ((kev->flags & EV_EOF) == EV_EOF)
    return JNI_TRUE;
  return JNI_FALSE;
#else
  (void) nstate;
  throw_not_supported (env);
  return JNI_FALSE;
#endif
}
