/*
 * kaffe.io.CharToByteIconv
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

#include "config.h"
#include "config-mem.h"
#include "kaffe_io_CharToByteIconv.h"

#if defined(HAVE_ICONV_H)
#include <iconv.h>
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

static jfieldID cd_id;
static jmethodID carry_id;

void
Java_kaffe_io_CharToByteIconv_initialize0(JNIEnv* env, jclass cls)
{
	cd_id = (*env)->GetFieldID(env, cls, "cd", "kaffe.util.Ptr");
	carry_id = (*env)->GetMethodID(env, cls, "carry", "([CII)V");
}

jboolean
Java_kaffe_io_CharToByteIconv_open0 (JNIEnv* env, jobject _this, jstring enc)
{
#if defined(HAVE_ICONV)
    const jbyte* str;
    iconv_t cd;

    str = (*env)->GetStringUTFChars(env, enc, NULL);
#ifdef WORDS_BIGENDIAN
    cd = iconv_open (str, "UCS-2BE");
#else
    cd = iconv_open (str, "UCS-2LE");
#endif
    (*env)->ReleaseStringUTFChars(env, enc, str);
    if (cd != (iconv_t)-1) {
	(*env)->SetObjectField(env, _this, cd_id, (jobject)cd);
	return JNI_TRUE;
    }
#endif
    return JNI_FALSE;
}

void
Java_kaffe_io_CharToByteIconv_close0 (JNIEnv* env UNUSED, jobject r UNUSED, jobject cd)
{
#if defined(HAVE_ICONV)
    iconv_close ((iconv_t)cd);
#endif
}

jint
Java_kaffe_io_CharToByteIconv_convert (JNIEnv* env, jobject _this, 
				       jcharArray fromChars, jint fromPos, jint fromLen,
				       jbyteArray toBytes, jint toPos, jint toLen )
{
#if defined(HAVE_ICONV)
    jboolean	isCopy;
    jchar	*jc  = (*env)->GetCharArrayElements(env, fromChars, &isCopy);
    char	*icv_in = (char*) (jc + fromPos);
    size_t	icv_inlen = fromLen * 2;
    jbyte	*jb = (*env)->GetByteArrayElements(env, toBytes, &isCopy);
    char	*icv_out = (char *) (jb + toPos);
    size_t	icv_outlen = toLen;
    iconv_t	cd = (iconv_t) (*env)->GetObjectField(env, _this, cd_id);
    int		ret;

    ret = iconv (cd, (ICONV_CONST char **) &icv_in, &icv_inlen, &icv_out, &icv_outlen);
    if (icv_inlen > 0) {
	/* In case we have some bytes left, save them */
	(*env)->CallVoidMethod(env, _this, carry_id,
			       fromChars, fromPos + fromLen - icv_inlen / 2, icv_inlen / 2);
    }
    (*env)->ReleaseCharArrayElements(env, fromChars, jc, JNI_ABORT);
    (*env)->ReleaseByteArrayElements(env, toBytes, jb, 0);
    return toLen - icv_outlen;
#else
    jclass sd = (*env)->FindClass(env, "kaffe.util.SupportDisabled");
    (*env)->ThrowNew(env, sd, "iconv() was not found by Kaffe configure script");
    return 0;
#endif
}
