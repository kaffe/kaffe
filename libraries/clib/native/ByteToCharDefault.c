/*
 * kaffe.io.ByteToCharDefault
 *
 * This class goes native because its main task is to copy bytes to chars
 * (i.e. System.arraycopy - incompatible arrays). It should avoid buffer-to-buffer
 * copying
 *
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include <jni.h>


jint
Java_kaffe_io_ByteToCharDefault_convert( JNIEnv* env, jobject _this, 
										  jarray fromBytes, jint fromPos, jint fromLen,
										  jarray toChars, jint toPos, jint toLen )
{
  register jint i, j;
  jboolean  isCopy;
  jbyte     *jb  = (*env)->GetByteArrayElements( env, fromBytes, &isCopy);
  int       blen = (*env)->GetArrayLength( env, fromBytes);
  jchar     *jc  = (*env)->GetCharArrayElements( env, toChars, &isCopy);
  int       clen = (*env)->GetArrayLength( env, toChars);
  int       n = fromPos + fromLen;
  int       m = toPos + toLen;

  if ( n > blen ) n = blen - fromPos;    /* be paranoid, we are native */
  if ( m > clen ) m = clen - toPos;

  for ( i=fromPos, j=toPos; (i<n) && (j<clen); i++, j++ )
	jc[j] = jb[i];

  /*
   * in case we have some bytes left we cannot convert (because of exhausted 'to' buffer
   * space, or because there are some bytes missing we haven't read in yet), we have to
   * store it here (via a method call to save the field access costs). This is very
   * inefficient and should be avoided by setting the right parameter values in Java
   */
  if ( i < n ) {
	jclass     clazz  = (*env)->GetObjectClass( env, _this);
	jmethodID  mId    = (*env)->GetMethodID( env, clazz, "carry", "([BII)V");
	
	(*env)->CallVoidMethod( env, _this, mId, fromBytes, i, (n-i));
  }

  (*env)->ReleaseByteArrayElements( env, fromBytes, jb, JNI_ABORT);
  (*env)->ReleaseCharArrayElements( env, toChars, jc, 0);

  return (j - toPos);
}
