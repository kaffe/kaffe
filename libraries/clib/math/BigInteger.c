/*
 * java.math.BigInteger.c
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "BigInteger.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include <native.h>

void
java_math_BigInteger_plumbInit(void)
{
	/* No init needed */
}

#if defined(HAVE_LIBGMP)

#include <gmp.h>

static
void
bytes2mpz(MP_INT* out, HArrayOfByte* val)
{
	int l;
	int i;

	l = obj_length(val);
	mpz_set_ui(out, (uint8)unhand(val)->body[0]);
	for (i = 1; i < l; i++) {
		mpz_mul_ui(out, out, 256);
		mpz_add_ui(out, out, (uint8)unhand(val)->body[i]);
	}
}

static
void
mpz2bytes(HArrayOfByte** outp, MP_INT* val)
{
	int l;
	HArrayOfByte* out;
	int i;
	MP_INT rem;

	mpz_init(&rem);

	l = (mpz_sizeinbase(val, 2) + 7) / 8;
	out = (HArrayOfByte*)AllocArray(l, TYPE_Byte);

	for (i = l - 1; i >= 0; i--) {
		mpz_divmod_ui(val, &rem, val, 256);
		unhand(out)->body[i] = mpz_get_ui(&rem);
	}

	mpz_clear(&rem);

	(*outp) = out;
}

HArrayOfByte*
java_math_BigInteger_plumbAdd(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT d;
	MP_INT s1;
	MP_INT s2;
	HArrayOfByte* result;

	mpz_init(&d);
	mpz_init(&s1);
	mpz_init(&s2);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_add(&d, &s1, &s2);

	mpz2bytes(&result, &d);

	mpz_clear(&d);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

struct Hjava_math_BigInteger*
java_math_BigInteger_plumbSubtract(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT r;
	MP_INT s1;
	MP_INT s2;
	Hjava_math_BigInteger* result;
	HArrayOfByte* array;

	mpz_init(&r);
	mpz_init(&s1);
	mpz_init(&s2);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_sub(&r, &s1, &s2);

	result = (Hjava_math_BigInteger*)AllocObject("java/math/BigInteger");

	if (mpz_cmp_ui(&r, 0) < 0) {
		/* Result if negative */
		mpz_abs(&r, &r);
		unhand(result)->signum = -1;
	}
	else {
		/* Result if positive */
		unhand(result)->signum = 1;
	}

	mpz2bytes(&array, &r);
	unhand(result)->magnitude = array;
	unhand(result)->bitCount = 0;
	unhand(result)->bitLength = 0;
	unhand(result)->firstNonzeroByteNum = 0;
	unhand(result)->lowestSetBit = 0;

	mpz_clear(&r);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

HArrayOfByte*
java_math_BigInteger_plumbMultiply(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT d;
	MP_INT s1;
	MP_INT s2;
	HArrayOfByte* result;

	mpz_init(&d);
	mpz_init(&s1);
	mpz_init(&s2);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_mul(&d, &s1, &s2);

	mpz2bytes(&result, &d);

	mpz_clear(&d);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

HArrayOfByte*
java_math_BigInteger_plumbDivide(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT q;
	MP_INT r;
	MP_INT s1;
	MP_INT s2;
	HArrayOfByte* result;

	mpz_init(&q);
	mpz_init(&r);
	mpz_init(&s1);
	mpz_init(&s2);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_divmod(&q, &r, &s1, &s2);

	mpz2bytes(&result, &q);

	mpz_clear(&q);
	mpz_clear(&r);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

HArrayOfByte*
java_math_BigInteger_plumbRemainder(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT q;
	MP_INT r;
	MP_INT s1;
	MP_INT s2;
	HArrayOfByte* result;

	mpz_init(&q);
	mpz_init(&r);
	mpz_init(&s1);
	mpz_init(&s2);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_divmod(&q, &r, &s1, &s2);

	mpz2bytes(&result, &r);

	mpz_clear(&q);
	mpz_clear(&r);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

HArrayOfArray*
java_math_BigInteger_plumbDivideAndRemainder(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT q;
	MP_INT r;
	MP_INT s1;
	MP_INT s2;
	HArrayOfArray* result;

	mpz_init(&r);
	mpz_init(&r);
	mpz_init(&s1);
	mpz_init(&s2);

	result = (HArrayOfArray*)AllocObjectArray(2, "[B");

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_divmod(&q, &r, &s1, &s2);

	mpz2bytes((HArrayOfByte**)&unhand(result)->body[0], &q);
	mpz2bytes((HArrayOfByte**)&unhand(result)->body[1], &r);

	mpz_clear(&q);
	mpz_clear(&r);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

HArrayOfByte*
java_math_BigInteger_plumbGcd(HArrayOfByte* src1, HArrayOfByte* src2)
{
	MP_INT r;
	MP_INT s1;
	MP_INT s2;
	HArrayOfByte* result;

	mpz_init(&r);
	mpz_init(&s1);
	mpz_init(&s2);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);

	mpz_gcd(&r, &s1, &s2);

	mpz2bytes(&result, &r);

	mpz_clear(&r);
	mpz_clear(&s1);
	mpz_clear(&s2);

	return (result);
}

HArrayOfByte*
java_math_BigInteger_plumbModPow(HArrayOfByte* src1, HArrayOfByte* src2, HArrayOfByte* src3)
{
	MP_INT r;
	MP_INT s1;
	MP_INT s2;
	MP_INT s3;
	HArrayOfByte* result;

	mpz_init(&r);
	mpz_init(&s1);
	mpz_init(&s2);
	mpz_init(&s3);

	bytes2mpz(&s1, src1);
	bytes2mpz(&s2, src2);
	bytes2mpz(&s3, src3);

	mpz_powm(&r, &s1, &s2, &s3);

	mpz2bytes(&result, &r);

	mpz_clear(&r);
	mpz_clear(&s1);
	mpz_clear(&s2);
	mpz_clear(&s3);

	return (result);
}

#else

HArrayOfByte*
java_math_BigInteger_plumbAdd(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

struct Hjava_math_BigInteger*
java_math_BigInteger_plumbSubtract(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbMultiply(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbDivide(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbRemainder(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfArray*
java_math_BigInteger_plumbDivideAndRemainder(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbGcd(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbModPow(HArrayOfByte* src1, HArrayOfByte* src2, HArrayOfByte* src3)
{
	unimp("bigint");
}

#endif

HArrayOfByte*
java_math_BigInteger_plumbModInverse(HArrayOfByte* src1, HArrayOfByte* src2)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbSquare(HArrayOfByte* src)
{
	unimp("bigint");
}

HArrayOfByte*
java_math_BigInteger_plumbGeneratePrime(HArrayOfByte* src)
{
	unimp("bigint");
}
