/*
 * kaffe.io.CharToByteDefault
 *
 * This class goes native because its main task is to copy chars to bytes
 * (i.e. System.arraycopy - incompatible arrays). It should
 * avoid buffer-to-buffer copying
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <jni.h>

jint
Java_kaffe_io_CharToByteDefault_convert( JNIEnv* env, jobject _this, 
										  jarray fromChars, jint fromPos, jint fromLen,
										  jarray toBytes, jint toPos, jint toLen )
{
  register jint i, j;
  jboolean  isCopy;
  jbyte     *jb  = (*env)->GetByteArrayElements( env, toBytes, &isCopy);
  int       blen = (*env)->GetArrayLength( env, toBytes);
  jchar     *jc  = (*env)->GetCharArrayElements( env, fromChars, &isCopy);
  int       clen = (*env)->GetArrayLength( env, fromChars);
  int       n = fromPos + fromLen;
  int       m = toPos + toLen;

  if ( n > clen ) n = clen - fromPos;    /* be paranoid, we are native */
  if ( m > blen ) m = blen - toPos;

  for ( i=fromPos, j=toPos; (i<n) && (j<m); i++, j++ )
	jb[j] = jc[i];

  /*
   * in case we have something left we cannot convert (because of missing
   * 'to' buffer space), we have to store it here (via a method call to save
   * the field access costs). This is very inefficient and should be avoided by
   * setting the right parameter values in Java
   */
  if ( i < n ) {
	jclass     clazz  = (*env)->GetObjectClass( env, _this);
	jmethodID  mId    = (*env)->GetMethodID( env, clazz, "carry", "([CII)V");
	
	(*env)->CallVoidMethod( env, _this, mId, fromChars, i, (n-i));
  }

  (*env)->ReleaseByteArrayElements( env, toBytes, jb, JNI_ABORT);
  (*env)->ReleaseCharArrayElements( env, fromChars, jc, 0);

  return (j - toPos);
}
