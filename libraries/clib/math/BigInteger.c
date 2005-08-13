/*
 * java.math.BigInteger.c
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <stdio.h>

#include "config.h"
#include "java_math_BigInteger.h"

#if defined(HAVE_GMP_H) && defined(USE_GMP)

#include <gmp.h>
#include <stdlib.h>

#include "kaffe/jmalloc.h"

static jfieldID number;

static void *
bi_alloc(size_t size)
{
	void *p = KMALLOC(size);
	return (p);
}

static void *
/* ARGUSED */
bi_realloc(void *ptr, size_t UNUSED old_size, size_t new_size)
{
	void *p = KREALLOC(ptr, new_size);
	return (p);
}

static void
/* ARGUSED */
bi_free(void *ptr, size_t UNUSED size) 
{
	KFREE(ptr);
}

void
Java_java_math_BigInteger_initialize0(JNIEnv* env, jclass cls)
{
	number = (*env)->GetFieldID(env, cls, "number", "kaffe.util.Ptr");
	mp_set_memory_functions (bi_alloc, bi_realloc, bi_free);
}

void
Java_java_math_BigInteger_init0(JNIEnv* env, jobject r)
{
	mpz_ptr res;

	res = (mpz_ptr)bi_alloc(sizeof(mpz_t));
	mpz_init(res);

	(*env)->SetObjectField(env, r, number, (jobject)res);
}

void
Java_java_math_BigInteger_finalize0(JNIEnv* env, jobject r)
{
	mpz_ptr res;

	res = (*env)->GetObjectField(env, r, number);

	mpz_clear(res);
	bi_free(res, sizeof(mpz_t));
}

void
Java_java_math_BigInteger_assignLong0(JNIEnv* env, jobject r, jlong v)
{
	mpz_ptr res;
	int negative = v < 0 ? -1 : 0;

	res = (*env)->GetObjectField(env, r, number);

	if (negative)
		v = -v;
	/* Note that v will remain negative if it's LONG_LONG_MIN.
	   This is not a problem because any sign copying in the right
	   shift will be stripped with the cast to jint, and the
	   number will be considered positive.  Furthermore, in this
	   case, (jint)v will be zero, so the addition will be a
	   do-nothing operation.  At last, the number will be made
	   negative, as appropriate.  */
	mpz_set_ui(res, (unsigned long)(jint)(v >> 32));
	mpz_mul_2exp(res, res, 32);
	mpz_add_ui(res, res, (unsigned long)(jint)v);
	if (negative)
		mpz_neg(res, res);
}

void
Java_java_math_BigInteger_assignBytes0(JNIEnv* env, jobject r, jint sign, jbyteArray magnitude)
{
	mpz_ptr res;
	jbyte* data;
	int i;
	int len;

	res = (*env)->GetObjectField(env, r, number);

	len = (*env)->GetArrayLength(env, magnitude);
	data = (*env)->GetByteArrayElements(env, magnitude, NULL);

	/* clear mpz by setting it to zero; do not use mpz_clear here 
	 * cause that would free its storage which is wrong.
	 */
	mpz_set_ui(res, 0);
	for (i = 0; i < len; i++) {
		mpz_mul_ui(res, res, (unsigned long)256);
		mpz_add_ui(res, res, (unsigned long)(data[i] & 0xFF));
	}

	(*env)->ReleaseByteArrayElements(env, magnitude, data, JNI_ABORT);

	if (sign == -1) {
		mpz_neg(res, res);
	}
}

/** 
 * Parse a string with given radix and assign its value to a BigInteger object.
 * 
 * @param env JNI environment
 * @param r this BigInteger object
 * @param val Java string value to assign
 * @param radix radix of string value
 * 
 * @return 0 if the string can be parsed and assigned. If anything fails, return -1.
 */
jint
Java_java_math_BigInteger_assignString0(JNIEnv* env, jobject r, jstring val, jint radix)
{
	mpz_ptr res;
	const jbyte* str;
	int rc;

	res = (*env)->GetObjectField(env, r, number);
	str = (*env)->GetStringUTFChars(env, val, NULL);

	rc = mpz_set_str(res, (char*)str, (int)radix);

	(*env)->ReleaseStringUTFChars(env, val, str);

	return rc;
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
Java_java_math_BigInteger_divrem0(JNIEnv* env, jclass cls UNUSED, jobject r1,
				  jobject r2, jobject s1, jobject s2)
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

/*
 * r = s1^s2 % s3
 */
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

	if (mpz_sgn (src2) == -1) {
		mpz_t tmp;
		int rc;

		mpz_init (tmp);
		mpz_neg (tmp, src2);
		mpz_powm (tmp, src1, tmp, src3);
		rc = mpz_invert (res, tmp, src3);
		mpz_clear (tmp);
		if (rc == 0) {
			jclass arexc = (*env)->FindClass(env, 
				"java.lang.ArithmeticException");
			(*env)->ThrowNew(env, arexc, "Inverse does not exist");
		}
		if (mpz_sgn (res) == -1)
			mpz_add (res, src3, res);
	}
	else {
		mpz_powm (res, src1, src2, src3);
	}
	
}

/*
 * r = s1^(-1) mod s2
 */
void
Java_java_math_BigInteger_modinv0(JNIEnv* env, jobject r, jobject s1, jobject s2)
{
	mpz_ptr res;
	mpz_srcptr src1;
	mpz_srcptr src2;
	int rc;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);
	res = (mpz_ptr)(*env)->GetObjectField(env, r, number);

	/*
	 * mpz_invert (rop, op1, op2)
	 * Compute the inverse of op1 modulo op2 and put the result 
	 * in rop. Return non-zero if an inverse exist, zero otherwise. 
	 * When the function returns zero, do not assume anything about 
	 * the value in rop. 
	 */
	rc = mpz_invert (res, src1, src2);
	if (rc == 0) {
		jclass arexc = (*env)->FindClass(env, 
			"java.lang.ArithmeticException");
		(*env)->ThrowNew(env, arexc, "Inverse does not exist");
	}
	/* java doesn't allow negative results */
	if (mpz_sgn(res) == -1)
		mpz_add (res, src2, res);
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
Java_java_math_BigInteger_cmp0(JNIEnv* env, jclass cls UNUSED, jobject s1,
			       jobject s2)
{
	mpz_srcptr src1;
	mpz_srcptr src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);

	return (mpz_cmp(src1, src2));
}

jstring
Java_java_math_BigInteger_toString0(JNIEnv* env, jobject s, jint base)
{
	char* res;
	mpz_srcptr src;
	jstring str;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	res = mpz_get_str(NULL, (int)base, src);
	str = (*env)->NewStringUTF(env, res);
	bi_free(res, 0 /* is ignored */);

	return (str);
}

jdouble
Java_java_math_BigInteger_toDouble0(JNIEnv* env, jobject s)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return ((jdouble)mpz_get_d(src));
}


jint
Java_java_math_BigInteger_toInt0(JNIEnv* env, jobject s)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return ((jint)mpz_get_ui(src));
}

jint
Java_java_math_BigInteger_probablyPrime0(JNIEnv* env, jobject s, jint prop)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return (mpz_probab_prime_p(src, (int)prop));
}

jint
Java_java_math_BigInteger_bitLength0(JNIEnv* env, jobject s)
{
	mpz_srcptr src;

	src = (mpz_srcptr)(*env)->GetObjectField(env, s, number);

	return (mpz_sizeinbase(src, 2));
}

jint
Java_java_math_BigInteger_hamDist0(JNIEnv* env, jobject s1, jobject s2)
{
	mpz_srcptr src1, src2;

	src1 = (mpz_srcptr)(*env)->GetObjectField(env, s1, number);
	src2 = (mpz_srcptr)(*env)->GetObjectField(env, s2, number);

	return (mpz_hamdist(src1, src2));
}

#else /* !defined(HAVE_GMP_H) || !defined(USE_GMP) */

/* throw a kaffe.util.SupportDisabled exception */
static void
gmp_not_found(JNIEnv* env) 
{
	jclass sd = (*env)->FindClass(env, "kaffe.util.SupportDisabled");
	(*env)->ThrowNew(env, sd, "GNU gmp was not found by Kaffe configure script");
}

/* We should put some dummies in here */
void
Java_java_math_BigInteger_initialize0(JNIEnv* env, jclass cls UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_init0(JNIEnv* env, jobject r UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_finalize0(JNIEnv* env, jobject r UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_assignLong0(JNIEnv* env, jobject r UNUSED,
				      jlong v UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_assignBytes0(JNIEnv* env, jobject r UNUSED,
				       jint sign UNUSED,
				       jbyteArray magnitude UNUSED)
{
	gmp_not_found(env);
}

jint
Java_java_math_BigInteger_assignString0(JNIEnv* env, jobject r UNUSED,
					jstring val UNUSED, jint radix UNUSED)
{
	gmp_not_found(env);

	return 0;
}

void
Java_java_math_BigInteger_add0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_sub0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_mul0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_div0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_rem0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_divrem0(JNIEnv* env, jclass cls UNUSED,
				  jobject r1 UNUSED, jobject r2 UNUSED,
				  jobject s1 UNUSED, jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_abs0(JNIEnv* env, jobject r UNUSED, jobject s UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_neg0(JNIEnv* env, jobject r UNUSED, jobject s UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_pow0(JNIEnv* env, jobject r UNUSED, jobject s UNUSED,
			       jint power UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_gcd0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_mod0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_modpow0(JNIEnv* env, jobject r UNUSED,
				  jobject s1 UNUSED, jobject s2 UNUSED,
				  jobject s3 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_modinv0(JNIEnv* env, jobject r UNUSED,
				  jobject s1 UNUSED, jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_and0(JNIEnv* env, jobject r UNUSED,
			       jobject s1 UNUSED, jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_or0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			      jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_xor0(JNIEnv* env, jobject r UNUSED, jobject s1 UNUSED,
			       jobject s2 UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_not0(JNIEnv* env, jobject r UNUSED,  jobject s UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_setbit0(JNIEnv* env, jobject r UNUSED,
				  jobject s UNUSED,
				  jint n UNUSED)
{
	gmp_not_found(env);
}

void
Java_java_math_BigInteger_clrbit0(JNIEnv* env, jobject r UNUSED,
				  jobject s UNUSED, jint n UNUSED)
{
	gmp_not_found(env);
}

jint
Java_java_math_BigInteger_scansetbit0(JNIEnv* env, jobject s UNUSED)
{
	gmp_not_found(env);

	return 0;
}

jint
Java_java_math_BigInteger_cmp0(JNIEnv* env, jclass cls UNUSED,
			       jobject s1 UNUSED, jobject s2 UNUSED)
{
	gmp_not_found(env);

	return 0;
}

jstring
Java_java_math_BigInteger_toString0(JNIEnv* env, jobject s UNUSED,
				    jint base UNUSED)
{
	gmp_not_found(env);

	return NULL;
}

jdouble
Java_java_math_BigInteger_toDouble0(JNIEnv* env, jobject s UNUSED)
{
	gmp_not_found(env);

	return 0.0;
}

jint
Java_java_math_BigInteger_toInt0(JNIEnv* env, jobject s UNUSED)
{
	gmp_not_found(env);

	return 0;
}

jint
Java_java_math_BigInteger_probablyPrime0(JNIEnv* env, jobject s UNUSED,
					 jint prop UNUSED)
{
	gmp_not_found(env);

	return 0;
}

jint
Java_java_math_BigInteger_bitLength0(JNIEnv* env, jobject s UNUSED)
{
	gmp_not_found(env);

	return 0;
}

jint
Java_java_math_BigInteger_hamDist0(JNIEnv* env, jobject s1 UNUSED,
				   jobject s2 UNUSED)
{
	gmp_not_found(env);

	return 0;
}

#endif /* defined(HAVE_GMP_H) && defined(USE_GMP) */
