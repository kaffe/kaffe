/*
 * java.math.BigInteger.c
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"

#if defined(HAVE_GMP_H)

#include <jni.h>
#include <gmp.h>

#undef	malloc
#undef	free

static jfieldID number;

void
Java_java_math_BigInteger_initialize0(JNIEnv* env, jclass cls)
{
	number = (*env)->GetFieldID(env, cls, "number", "kaffe.util.Ptr");
}

void
Java_java_math_BigInteger_init0(JNIEnv* env, jobject r)
{
	mpz_ptr res;

	res = (mpz_ptr)malloc(sizeof(mpz_t));
	mpz_init(res);

	(*env)->SetObjectField(env, r, number, (jobject)res);
}

void
Java_java_math_BigInteger_finalize0(JNIEnv* env, jobject r)
{
	mpz_ptr res;

	res = (*env)->GetObjectField(env, r, number);

	mpz_clear(res);
	free(res);
}

void
Java_java_math_BigInteger_assignBytes0(JNIEnv* env, jobject r, jint sign, jarray magnitude)
{
	mpz_ptr res;
	jbyte* data;
	int i;
	int len;

	res = (*env)->GetObjectField(env, r, number);

	len = (*env)->GetArrayLength(env, magnitude);
	data = (*env)->GetByteArrayElements(env, magnitude, 0);

	mpz_clear(res);
	for (i = 0; i < len; i++) {
		mpz_mul_ui(res, res, (unsigned long)256);
		mpz_add_ui(res, res, (unsigned long)(data[i] & 0xFF));
	}

	(*env)->ReleaseByteArrayElements(env, magnitude, data, JNI_ABORT);

	if (sign == -1) {
		mpz_neg(res, res);
	}
}

void
Java_java_math_BigInteger_assignString0(JNIEnv* env, jobject r, jstring val, jint radix)
{
	mpz_ptr res;
	const jbyte* str;

	res = (*env)->GetObjectField(env, r, number);
	str = (*env)->GetStringUTFChars(env, val, 0);

	mpz_set_str(res, (char*)str, (int)radix);

	(*env)->ReleaseStringUTFChars(env, val, str);
}

void
Java_java_math_BigInteger_add0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_add(res, src1, src2);
}

void
Java_java_math_BigInteger_sub0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_sub(res, src1, src2);
}

void
Java_java_math_BigInteger_mul0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_mul(res, src1, src2);
}

void
Java_java_math_BigInteger_div0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_tdiv_q(res, src1, src2);
}

void
Java_java_math_BigInteger_rem0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_tdiv_r(res, src1, src2);
}

void
Java_java_math_BigInteger_divrem0(JNIEnv* env, jobject r1, jobject r2, jobject s1, jobject s2)
{
	mpz_ptr res1;
	mpz_ptr res2;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res1 = (mpz_ptr)(*env)->GetObjectField(env, r1, number);
	res2 = (mpz_ptr)(*env)->GetObjectField(env, r2, number);

	mpz_tdiv_qr(res1, res2, src1, src2);
}

void
Java_java_math_BigInteger_abs0(JNIEnv* env, jobject r, jobject s)
{
	mpz_ptr res;
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_abs(res, src);
}

void
Java_java_math_BigInteger_neg0(JNIEnv* env, jobject r, jobject s)
{
	mpz_ptr res;
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_neg(res, src);
}

void
Java_java_math_BigInteger_pow0(JNIEnv* env, jobject r, jobject s, jint power)
{
	mpz_ptr res;
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_pow_ui(res, src, (unsigned long)power);
}

void
Java_java_math_BigInteger_gcd0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_gcd(res, src1, src2);
}

void
Java_java_math_BigInteger_mod0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_mod(res, src1, src2);
}

void
Java_java_math_BigInteger_modpow0(JNIEnv* env, jobject r, jobject s1, jobject s2, jobject s3)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;
	mpz_srcptr src3;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	src3 = (mpz_srcptr)(*env)->GetObjectField(env, s3, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_powm(res, src1, src2, src3);
}

void
Java_java_math_BigInteger_and0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_and(res, src1, src2);
}

void
Java_java_math_BigInteger_or0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_ior(res, src1, src2);
}

void
Java_java_math_BigInteger_xor0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;
	mpz_t tmp;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_init(tmp);
	mpz_and(res, src1, src2);
	mpz_com(res, res);
	mpz_ior(tmp, src1, src2);
	mpz_and(res, res, tmp);

	mpz_clear(tmp);
}

void
Java_java_math_BigInteger_not0(JNIEnv* env, jobject r, jobject s)
{
	mpz_ptr res;
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_com(res, src);
}

void
Java_java_math_BigInteger_setbit0(JNIEnv* env, jobject r, jobject s, jint n)
{
	mpz_ptr res;
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_set(res, src);
	mpz_setbit(res, (unsigned long)n);
}

void
Java_java_math_BigInteger_clrbit0(JNIEnv* env, jobject r, jobject s, jint n)
{
	mpz_ptr res;
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	mpz_set(res, src);
	mpz_clrbit(res, (unsigned long)n);
}

jint
Java_java_math_BigInteger_scansetbit0(JNIEnv* env, jobject s)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return ((jint)mpz_scan1(src, 0));
}


jint
Java_java_math_BigInteger_cmp0(JNIEnv* env, jclass cls, jobject s1, jobject s2)
{
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);

	return (mpz_cmp(src1, src2));
}

jstring
Java_java_math_BigInteger_toString0(JNIEnv* env, jobject* s, jint base)
{
	char* res;
	mpz_srcptr src;
	jstring str;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	res = mpz_get_str(0, (int)base, src);
	str = (*env)->NewStringUTF(env, res);
	free(res);

	return (str);
}

jdouble
Java_java_math_BigInteger_toDouble0(JNIEnv* env, jobject* s)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return ((jdouble)mpz_get_d(src));
}


jint
Java_java_math_BigInteger_toInt0(JNIEnv* env, jobject* s)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return ((jint)mpz_get_si(src));
}

jint
Java_java_math_BigInteger_probablyPrime0(JNIEnv* env, jobject* s, jint prop)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return (mpz_probab_prime_p(src, (int)prop));
}

#else

/* We should put some dummies in here */

#endif
