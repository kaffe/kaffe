/*
 * NIOServerSocket.c
 *
 * Copyright (c) 2003, 2004 The Kaffe.org's team.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "jni.h"
#include "gnu_java_nio_NIOServerSocket.h"

jobject JNICALL
Java_gnu_java_nio_NIOServerSocket_getPlainSocketImpl(JNIEnv* env, jobject thiz)
{
  jmethodID mid = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, thiz), "getImpl", "()Ljava/net/SocketImpl;");

  if (mid == NULL)
    return NULL;

  return (*env)->CallObjectMethod(env, thiz, mid);
}
