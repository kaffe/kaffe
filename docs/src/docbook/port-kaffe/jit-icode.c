/* jit-icode.c
 * $Id: jit-icode.c,v 1.2 2005/07/04 00:03:57 robilad Exp $
 *
 * Copyright (c) 2001
 *     Edouard G. Parmelan.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

typedef long jint;
typedef unsigned long ujint;
typedef long long jlong;
typedef unsigned long long ujlong;
typedef float jfloat;
typedef double jdouble;
typedef signed char jbyte;
typedef short jshort;
typedef unsigned short jchar;

typedef unsigned char* uintp;


/* Register spills and reloads */

/* spill_TYPE() see store_offset_TYPE() */
/* reload_TYPE() see load_offset_TYPE() */

/* Prologues and epilogues */

extern int do_some_job(int a0, int a1, int a2, int a3);

extern int do_other_job(int a0, int a1, int a2, int a3,
			int a4, int a5, int a6, int a7,
			int a8, int a9, int a10, int a11,
			int a12, int a13, int a14, int a15);

int prologue_epilogue_4(int a0, int a1, int a2, int a3)
{
    int v0;

    v0 = do_some_job (a0, a1, a2, a3);
    return v0;
}

int prologue_epilogue_8(int a0, int a1, int a2, int a3,
			int a4, int a5, int a6, int a7)
{
    int v0, v1;

    v0 = do_some_job (a0, a1, a2, a3);
    v1 = do_some_job (a4, a5, a6, a7);
    return v0 + v1;
}

int prologue_epilogue_12(int a0, int a1, int a2, int a3,
			 int a4, int a5, int a6, int a7,
			 int a8, int a9, int a10, int a11)
{
    int v0, v1, v2;

    v0 = do_some_job (a0, a1, a2, a3);
    v1 = do_some_job (a4, a5, a6, a7);
    v2 = do_some_job (a8, a9, a10, a11);
    return v0 + v1 + v2;
}

int prologue_epilogue_16(int a0, int a1, int a2, int a3,
			 int a4, int a5, int a6, int a7,
			 int a8, int a9, int a10, int a11,
			 int a12, int a13, int a14, int a15)
{
    int v0, v1, v2, v3;

    v0 = do_some_job (a0, a1, a2, a3);
    v1 = do_some_job (a4, a5, a6, a7);
    v2 = do_some_job (a8, a9, a10, a11);
    v3 = do_some_job (a12, a13, a14, a15);
    return v0 + v1 + v2 + v3;
}

int prologue_epilogue_16_long(int a0, int a1, int a2, int a3,
			      int a4, int a5, int a6, int a7,
			      int a8, int a9, int a10, int a11,
			      int a12, int a13, int a14, int a15)
{
    int v0, v1, v2, v3, v4;

    v0 = do_some_job (a0, a1, a2, a3);
    v1 = do_some_job (a4, a5, a6, a7);
    v2 = do_some_job (a8, a9, a10, a11);
    v3 = do_some_job (a12, a13, a14, a15);
    v4 = do_other_job (a0, a1, a2, a3,
		       a4, a5, a6, a7,
		       a8, a9, a10, a11,
		       a12, a13, a14, a15);
    return do_some_job (v0, v1, v2, v3) + v4;
}


/* Moves */

#define move_int_const(NAME, val) \
	jint move_int_const_##NAME(void) { return val; }

move_int_const(0, 0);
move_int_const(32767, 32767)
move_int_const(m32768, -32768)
move_int_const(0x7fff, 0x7fff)
move_int_const(m0x8000, -0x8000)
move_int_const(0x8000, 0x8000)
move_int_const(65535, 65535)
move_int_const(65536, 65536)


/* Arithmetic operators */

#define add_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a + b; }

add_TYPE(add_int, jint);
// add_TYPE(add_ref, void*);
add_TYPE(add_long, jlong);
add_TYPE(add_float, jfloat);
add_TYPE(add_double, jdouble);


#define sub_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a - b; }

sub_TYPE(sub_int, jint);
// sub_TYPE(sub_ref, void*);
sub_TYPE(sub_long, jlong);
sub_TYPE(sub_float, jfloat);
sub_TYPE(sub_double, jdouble);


#define mul_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a * b; }

mul_TYPE(mul_int, jint);
mul_TYPE(mul_long, jlong);
mul_TYPE(mul_float, jfloat);
mul_TYPE(mul_double, jdouble);


#define div_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a / b; }

div_TYPE(div_int, jint);
div_TYPE(div_long, jlong);
div_TYPE(div_float, jfloat);
div_TYPE(div_double, jdouble);


#define rem_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a % b; }

rem_TYPE(rem_int, jint);
rem_TYPE(rem_long, jlong);


#define neg_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a) { return -a; }

neg_TYPE(neg_int, jint);
neg_TYPE(neg_long, jlong);
neg_TYPE(neg_float, jfloat);
neg_TYPE(neg_double, jdouble);


/* Logical operators */

#define and_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a &amp; b; }

and_TYPE(and_int, jint);
and_TYPE(and_long, jlong);


#define or_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a | b; }

or_TYPE(or_int, jint);
or_TYPE(or_long, jlong);


#define xor_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a ^ b; }

xor_TYPE(xor_int, jint);
xor_TYPE(xor_long, jlong);


#define lshl_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a &lt;&lt; b; }

lshl_TYPE(lshl_int, jint);
lshl_TYPE(lshl_long, jlong);

#define lshl_int_const(NAME, VAL) \
	jint lshl_int_const_##NAME(jint a) { return a &lt;&lt; VAL; }

lshl_int_const(1, 1)
lshl_int_const(2, 2)
lshl_int_const(3, 3)
lshl_int_const(4, 4)
lshl_int_const(5, 5)
lshl_int_const(6, 6)
lshl_int_const(7, 7)
lshl_int_const(8, 8)
lshl_int_const(9, 9)
lshl_int_const(10, 10)
lshl_int_const(11, 11)
lshl_int_const(12, 12)
lshl_int_const(13, 13)
lshl_int_const(14, 14)
lshl_int_const(15, 15)
lshl_int_const(16, 16)
lshl_int_const(17, 17)
lshl_int_const(18, 18)
lshl_int_const(19, 19)
lshl_int_const(20, 20)
lshl_int_const(21, 21)

#define ashr_TYPE(NAME, TYPE) \
	TYPE NAME (TYPE a, TYPE b) { return a >> b; }

ashr_TYPE(ashr_int, jint);
ashr_TYPE(ashr_long, jlong);


#define lshr_TYPE(NAME, TYPE) \
	TYPE NAME (u##TYPE a, TYPE b) { return a >> b; }

lshr_TYPE(lshr_int, jint);
lshr_TYPE(lshr_long, jlong);


/* Load and store */

#define load_TYPE(NAME, TYPE) \
	TYPE NAME (uintp r) { return *(TYPE *)r; }

load_TYPE(load_int, jint);
load_TYPE(load_ref, void*);
load_TYPE(load_long, jlong);
load_TYPE(load_float, jfloat);
load_TYPE(load_double, jdouble);
load_TYPE(load_byte, jbyte);
load_TYPE(load_char, jchar);
load_TYPE(load_short, jshort);

// load_constpool_TYPE

#define load_offset_TYPE(NAME, TYPE) \
	TYPE NAME (uintp r, int o) { return *(TYPE *)(r + o); }

load_offset_TYPE(load_offset_int, jint);
load_offset_TYPE(load_offset_ref, void*);
load_offset_TYPE(load_offset_long, jlong);
load_offset_TYPE(load_offset_float, jfloat);
load_offset_TYPE(load_offset_double, jdouble);
load_offset_TYPE(load_offset_byte, jbyte);
load_offset_TYPE(load_offset_char, jchar);
load_offset_TYPE(load_offset_short, jshort);

#define load_addr_TYPE(NAME, TYPE) \
	TYPE NAME (void) { return *(TYPE *)(0x87654321); }

load_addr_TYPE(load_addr_int, jint);
load_addr_TYPE(load_addr_ref, void*);
load_addr_TYPE(load_addr_long, jlong);
load_addr_TYPE(load_addr_float, jfloat);
load_addr_TYPE(load_addr_double, jdouble);
load_addr_TYPE(load_addr_byte, jbyte);
load_addr_TYPE(load_addr_char, jchar);
load_addr_TYPE(load_addr_short, jshort);


#define load_offset_scaled_TYPE(NAME, TYPE) \
	TYPE NAME (uintp r, int idx, int o) { return ((TYPE *)(r + o))[idx]; }

load_offset_scaled_TYPE(load_offset_scaled_int, jint);
load_offset_scaled_TYPE(load_offset_scaled_ref, void*);
load_offset_scaled_TYPE(load_offset_scaled_long, jlong);
load_offset_scaled_TYPE(load_offset_scaled_float, jfloat);
load_offset_scaled_TYPE(load_offset_scaled_double, jdouble);
load_offset_scaled_TYPE(load_offset_scaled_byte, jbyte);
load_offset_scaled_TYPE(load_offset_scaled_char, jchar);
load_offset_scaled_TYPE(load_offset_scaled_short, jshort);


#define store_TYPE(NAME, TYPE) \
	void NAME (TYPE r, uintp w) { *(TYPE *)w = r; }

store_TYPE(store_int, jint);
store_TYPE(store_ref, void*);
store_TYPE(store_long, jlong);
store_TYPE(store_float, jfloat);
store_TYPE(store_double, jdouble);
store_TYPE(store_byte, jbyte);
store_TYPE(store_char, jchar);
store_TYPE(store_short, jshort);


#define store_offset_TYPE(NAME, TYPE) \
	void NAME (TYPE r, uintp w, int o) { *(TYPE *)(w + o) = r; }

store_offset_TYPE(store_offset_int, jint);
store_offset_TYPE(store_offset_ref, void*);
store_offset_TYPE(store_offset_long, jlong);
store_offset_TYPE(store_offset_float, jfloat);
store_offset_TYPE(store_offset_double, jdouble);
store_offset_TYPE(store_offset_byte, jbyte);
store_offset_TYPE(store_offset_char, jchar);
store_offset_TYPE(store_offset_short, jshort);

// store_addr_TYPE

#define store_offset_scaled_TYPE(NAME, TYPE) \
	void NAME (TYPE r, uintp w, int idx, int o) { ((TYPE *)(w + o))[idx] = r; }

store_offset_scaled_TYPE(store_offset_scaled_int, jint);
store_offset_scaled_TYPE(store_offset_scaled_ref, void*);
store_offset_scaled_TYPE(store_offset_scaled_long, jlong);
store_offset_scaled_TYPE(store_offset_scaled_float, jfloat);
store_offset_scaled_TYPE(store_offset_scaled_double, jdouble);
store_offset_scaled_TYPE(store_offset_scaled_byte, jbyte);
store_offset_scaled_TYPE(store_offset_scaled_char, jchar);
store_offset_scaled_TYPE(store_offset_scaled_short, jshort);



/* Function argument management */

/* Control flow changes */

/* Labels */

/* Comparisons */

/* Conversions */

#define cvt_FROM_TO(NAME, FROM, TO) \
	TO NAME(FROM a) { return (TO)a; }

cvt_FROM_TO(cvt_int_long, jint, jlong);
cvt_FROM_TO(cvt_int_float, jint, jfloat);
cvt_FROM_TO(cvt_int_double, jint, jdouble);
cvt_FROM_TO(cvt_long_float, jlong, jfloat);
cvt_FROM_TO(cvt_long_double, jlong, jdouble);
cvt_FROM_TO(cvt_float_int, jfloat, jint);
cvt_FROM_TO(cvt_float_long, jfloat, jlong);
cvt_FROM_TO(cvt_float_double, jfloat, jdouble);
cvt_FROM_TO(cvt_double_int, jdouble, jint);
cvt_FROM_TO(cvt_double_long, jdouble, jlong);
cvt_FROM_TO(cvt_double_float, jdouble, jfloat);
cvt_FROM_TO(cvt_int_byte, jint, jbyte);
cvt_FROM_TO(cvt_int_short, jint, jshort);



/*
 * Local variables:
 * compile-command: "gcc -O2 -fverbose-asm -S jit-icode.c"
 * End:
 */
