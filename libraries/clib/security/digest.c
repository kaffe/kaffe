/*
 * digest.c
 *
 * kaffe.security.provider.MD2
 * kaffe.security.provider.MD4
 * kaffe.security.provider.MD5
 * kaffe.security.provider.SHA
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "gtypes.h"
#include "locks.h"
#include "object.h"
#include "itypes.h"
#include "exception.h"
#include "stringSupport.h"
#include "fp.h"
#include "sha-1.h"
#include <native.h>
#include "kaffe_security_provider_MD2.h"
#include "kaffe_security_provider_MD4.h"
#include "kaffe_security_provider_MD5.h"
#include "kaffe_security_provider_SHA.h"

#if (!defined(HAVE_MD2INIT) || !defined(HAVE_MD4INIT)) && !defined(HAVE_LIBMD)
static void 
supportDisabled (JNIEnv* env)
{
	jclass sd = (*env)->FindClass(env, "kaffe.util.SupportDisabled");
	(*env)->ThrowNew(env, sd,
			 "libmd was not found by Kaffe configure script");
}
#endif

/**************************** MD2 ***********************************/

#if defined(HAVE_MD2INIT) || defined(HAVE_LIBMD)

#include <md2.h>

void JNICALL
Java_kaffe_security_provider_MD2_Init(JNIEnv *env, jobject this)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jfieldID	contextField = (*env)->GetFieldID(env,
					  class, "context", "[B");
	jbyteArray	ary;
	MD2_CTX		ctx;

	/* Initialize MD2 context */
	MD2Init(&ctx);

	/* Copy initialized context into byte array */
	ary = (*env)->NewByteArray(env, sizeof(ctx));
	(*env)->SetByteArrayRegion(env, ary, 0, sizeof(ctx), (jbyte *) &ctx);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Assign byte array to instance variable */
	(*env)->SetObjectField(env, this, contextField, (jobject) ary);
}

void JNICALL
Java_kaffe_security_provider_MD2_Update(JNIEnv *env, jobject this,
	jbyteArray buf, jint off, jint len)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || off + len > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Update with new data and release array data */
	MD2Update((MD2_CTX *) ctxBytes, bufBytes + off, len);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

void JNICALL
Java_kaffe_security_provider_MD2_Final(JNIEnv *env, jobject this,
	jbyteArray buf, jint off)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	const jfieldID	diglenField = (*env)->GetStaticFieldID(env,
				class, "DIGEST_LENGTH", "I");
	jint		digestLen = (*env)->GetStaticIntField(env,
				this, diglenField);
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || off + digestLen > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Finalize and release byte arrays */
	MD2Final(bufBytes + off, (MD2_CTX *) ctxBytes);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

#else

void JNICALL
Java_kaffe_security_provider_MD2_Init(JNIEnv *env, jobject this UNUSED)
{
	supportDisabled(env);
}

void JNICALL
Java_kaffe_security_provider_MD2_Update(JNIEnv *env, jobject this UNUSED,
	jbyteArray buf UNUSED, jint off UNUSED, jint len UNUSED)
{
	supportDisabled(env);
}

void JNICALL
Java_kaffe_security_provider_MD2_Final(JNIEnv *env, jobject this UNUSED,
	jbyteArray buf UNUSED, jint off UNUSED)
{
	supportDisabled(env);
}

#endif	/* defined(HAVE_MD2INIT) || defined(HAVE_LIBMD) */


/**************************** MD4 ***********************************/

#if defined(HAVE_MD4INIT) || defined(HAVE_LIBMD)

#include <md4.h>

void JNICALL
Java_kaffe_security_provider_MD4_Init(JNIEnv *env, jobject this)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jfieldID	contextField = (*env)->GetFieldID(env,
					  class, "context", "[B");
	jbyteArray	ary;
	MD4_CTX		ctx;

	/* Initialize MD4 context */
	MD4Init(&ctx);

	/* Copy initialized context into byte array */
	ary = (*env)->NewByteArray(env, sizeof(ctx));
	(*env)->SetByteArrayRegion(env, ary, 0, sizeof(ctx), (jbyte *) &ctx);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Assign byte array to instance variable */
	(*env)->SetObjectField(env, this, contextField, (jobject) ary);
}

void JNICALL
Java_kaffe_security_provider_MD4_Update(JNIEnv *env, jobject this,
	jbyteArray buf, jint off, jint len)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || off + len > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Update with new data and release array data */
	MD4Update((MD4_CTX *) ctxBytes, bufBytes + off, len);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

void JNICALL
Java_kaffe_security_provider_MD4_Final(JNIEnv *env, jobject this,
	jbyteArray buf, jint off)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	const jfieldID	diglenField = (*env)->GetStaticFieldID(env,
				class, "DIGEST_LENGTH", "I");
	jint		digestLen = (*env)->GetStaticIntField(env,
				this, diglenField);
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || off + digestLen > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Finalize and release byte arrays */
	MD4Final(bufBytes + off, (MD4_CTX *) ctxBytes);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

#else

void JNICALL
Java_kaffe_security_provider_MD4_Init(JNIEnv *env, jobject this UNUSED)
{
	supportDisabled(env);
}

void JNICALL
Java_kaffe_security_provider_MD4_Update(JNIEnv *env, jobject this UNUSED,
	jbyteArray buf UNUSED, jint off UNUSED, jint len UNUSED)
{
	supportDisabled(env);
}

void JNICALL
Java_kaffe_security_provider_MD4_Final(JNIEnv *env, jobject this UNUSED,
	jbyteArray buf UNUSED, jint off UNUSED)
{
	supportDisabled(env);
}

#endif	/* defined(HAVE_MD4INIT) || defined(HAVE_LIBMD) */


/**************************** MD5 ***********************************/

#if defined(HAVE_MD5INIT) || defined(HAVE_LIBMD)
#include <md5.h>
#else
/*
 * If we don't have an external MD5 library, we use an internal version
 * instead.
 */
#include "kaffe_md5.h"
#define	MD5_CTX			struct md5_ctx
#define	MD5Init(C)		md5_init_ctx(C)
#define	MD5Update(C,B,L)	md5_process_bytes(B,L,C)
#define	MD5Final(B,C)		md5_finish_ctx(C,B)
#endif	/* defined(HAVE_MD5INIT) || defined(HAVE_LIBMD) */

void JNICALL
Java_kaffe_security_provider_MD5_Init(JNIEnv *env, jobject this)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jfieldID	contextField = (*env)->GetFieldID(env,
					  class, "context", "[B");
	jbyteArray	ary;
	MD5_CTX		ctx;

	/* Initialize MD5 context */
	MD5Init(&ctx);

	/* Copy initialized context into byte array */
	ary = (*env)->NewByteArray(env, sizeof(ctx));
	(*env)->SetByteArrayRegion(env, ary, 0, sizeof(ctx), (jbyte *) &ctx);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Assign byte array to instance variable */
	(*env)->SetObjectField(env, this, contextField, (jobject) ary);
}

void JNICALL
Java_kaffe_security_provider_MD5_Update(JNIEnv *env, jobject this,
	jbyteArray buf, jint off, jint len)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || len < 0 || off + len > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Update with new data and release array data */
	MD5Update((MD5_CTX *) ctxBytes, bufBytes + off, (unsigned)len);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

void JNICALL
Java_kaffe_security_provider_MD5_Final(JNIEnv *env, jobject this,
	jbyteArray buf, jint off)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	const jfieldID	diglenField = (*env)->GetStaticFieldID(env,
				class, "DIGEST_LENGTH", "I");
	jint		digestLen = (*env)->GetStaticIntField(env,
				this, diglenField);
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || off + digestLen > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Finalize and release byte arrays */
	MD5Final(bufBytes + off, (MD5_CTX *) ctxBytes);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

/**************************** SHA ***********************************/

void JNICALL
Java_kaffe_security_provider_SHA_Init(JNIEnv *env, jobject this)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jfieldID	contextField = (*env)->GetFieldID(env,
					  class, "context", "[B");
	jbyteArray	ary;
	SHA1_CTX	ctx;

	/* Initialize SHA context */
	SHA1Init(&ctx);

	/* Copy initialized context into byte array */
	ary = (*env)->NewByteArray(env, sizeof(ctx));
	(*env)->SetByteArrayRegion(env, ary, 0, sizeof(ctx), (jbyte *) &ctx);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Assign byte array to instance variable */
	(*env)->SetObjectField(env, this, contextField, (jobject) ary);
}

void JNICALL
Java_kaffe_security_provider_SHA_Update(JNIEnv *env, jobject this,
	jbyteArray buf, jint off, jint len)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || len < 0 || off + len > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Update with new data and release array data */
	SHA1Update((SHA1_CTX *) ctxBytes, bufBytes + off, (unsigned)len);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

void JNICALL
Java_kaffe_security_provider_SHA_Final(JNIEnv *env, jobject this,
	jbyteArray buf, jint off)
{
	const jclass	class = (*env)->GetObjectClass(env, this);
	const jclass	aiobClass = (*env)->FindClass(env,
				"java/lang/ArrayIndexOutOfBoundsException");
	const jfieldID	contextField = (*env)->GetFieldID(env,
				class, "context", "[B");
	const jfieldID	diglenField = (*env)->GetStaticFieldID(env,
				class, "DIGEST_LENGTH", "I");
	jint		digestLen = (*env)->GetStaticIntField(env,
				this, diglenField);
	jbyte		*ctxBytes, *bufBytes;
	jbyteArray	ctxArray;

	/* Get context byte array data */
	ctxArray = (*env)->GetObjectField(env, this, contextField);
	ctxBytes = (*env)->GetByteArrayElements(env, ctxArray, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Get data byte array data */
	if (off < 0 || off + digestLen > (*env)->GetArrayLength(env, buf)) {
		(*env)->ThrowNew(env, aiobClass, "out of range");
		return;
	}
	bufBytes = (*env)->GetByteArrayElements(env, buf, NULL);
      	if ((*env)->ExceptionOccurred(env)) {
		return;
	}

	/* Finalize and release byte arrays */
	SHA1Final(bufBytes + off, (SHA1_CTX *) ctxBytes);
	(*env)->ReleaseByteArrayElements(env, ctxArray, ctxBytes, 0);
	(*env)->ReleaseByteArrayElements(env, buf, bufBytes, JNI_ABORT);
}

