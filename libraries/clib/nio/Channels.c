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
#include "java_nio_channels_Channels.h"

jobject JNICALL
Java_java_nio_channels_Channels_newInputStream(JNIEnv *env, jclass clazz UNUSED, jobject channelimpl)
{
  jclass fileinputstream_class;
  jmethodID file_constructor;

  fileinputstream_class = (*env)->FindClass(env, "java.io.FileInputStream");  
  if (fileinputstream_class == NULL)
    return NULL;
  
  file_constructor = (*env)->GetMethodID(env, fileinputstream_class, "<init>", "(Lgnu/java/nio/channels/FileChannelImpl;)V");
  if (file_constructor == NULL)
    return NULL;

  return (*env)->NewObject(env, fileinputstream_class, file_constructor, channelimpl);
}

jobject JNICALL
Java_java_nio_channels_Channels_newOutputStream(JNIEnv *env, jclass clazz UNUSED, jobject channelimpl)
{
  jclass fileoutputstream_class;
  jmethodID file_constructor;

  fileoutputstream_class = (*env)->FindClass(env, "java.io.FileOutputStream");  
  if (fileoutputstream_class == NULL)
    return NULL;
  
  file_constructor = (*env)->GetMethodID(env, fileoutputstream_class, "<init>", "(Lgnu/java/nio/channels/FileChannelImpl;)V");
  if (file_constructor == NULL)
    return NULL;

  return (*env)->NewObject(env, fileoutputstream_class, file_constructor, channelimpl);
}
