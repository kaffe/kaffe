/*
 * DirectByteBufferImpl.c
 *
 * Copyright (c) 2003, 2004 The Kaffe.org's team.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 
#include "config.h"
#include "config-std.h"
#include "kaffe/jmalloc.h"
#include "java_nio_DirectByteBufferImpl.h"

jobject JNICALL
Java_java_nio_DirectByteBufferImpl_allocateImpl(JNIEnv *env UNUSED, jclass clazz UNUSED, jint bytes)
{
  return (jobject)jmalloc((size_t)bytes);
}

void JNICALL
Java_java_nio_DirectByteBufferImpl_freeImpl(JNIEnv *env UNUSED, jclass clazz UNUSED, jobject data)
{
  jfree(data);
}

jbyte JNICALL
Java_java_nio_DirectByteBufferImpl_getImpl__Lgnu_classpath_RawData_2I(JNIEnv *env UNUSED, jclass clazz UNUSED, 
								      jobject address, jint offset)
{
  jbyte *nativeBytes = (jbyte *)address;
  return nativeBytes[offset];
}

void JNICALL
Java_java_nio_DirectByteBufferImpl_putImpl(JNIEnv *env UNUSED, jclass clazz UNUSED, 
					   jobject address, jint offset, jbyte b)
{
  jbyte *nativeBytes = (jbyte *)address;
  nativeBytes[offset] = b;
}

void JNICALL
Java_java_nio_DirectByteBufferImpl_getImpl__Lgnu_classpath_RawData_2I_3BII(JNIEnv *env, jclass clazz UNUSED, jobject address, jint nativeOffset, jbyteArray bytes, jint offset, jint length)
{
  jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytes, NULL);

  memcpy(&nativeBytes[offset], &((jbyte *)address)[nativeOffset], (size_t)length);
  (*env)->ReleaseByteArrayElements(env, bytes, nativeBytes, 0);
}

void JNICALL
Java_java_nio_DirectByteBufferImpl_shiftDown(JNIEnv *env UNUSED, jclass clazz UNUSED, jobject address, jint dst_offset, jint src_offset, jint length)
{
  memcpy(&((jbyte *)address)[dst_offset], &((jbyte *)address)[src_offset], (size_t)length);
}

jobject JNICALL 
Java_java_nio_DirectByteBufferImpl_adjustAddress(JNIEnv *env UNUSED, jclass clazz UNUSED, jobject address, jint offset)
{
  return (jobject) ((jbyte *)address + offset);
}
