/*
 * icode.h
 * Define the instruction codes macros.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __icode_h
#define	__icode_h

#define	move_long_const(t, c)			(t)[0].v.tlong = (c)
#define	add_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong + (f2)[0].v.tlong
#define	sub_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong - (f2)[0].v.tlong
#define	mul_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong * (f2)[0].v.tlong
#define	div_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong / (f2)[0].v.tlong
#define	rem_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong % (f2)[0].v.tlong
#define	neg_long(t, f)				(t)[0].v.tlong = -(f)[0].v.tlong

#define	and_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong & (f2)[0].v.tlong
#define	or_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong | (f2)[0].v.tlong
#define	xor_long(t, f1, f2)			(t)[0].v.tlong = (f1)[0].v.tlong ^ (f2)[0].v.tlong

#define	lshl_long(t, f1, f2)			(t)[0].v.tlong = ((f1)[0].v.tlong) << ((f2)[0].v.tint & 63)
#define	ashr_long(t, f1, f2)			(t)[0].v.tlong = ((int64)(f1)[0].v.tlong) >> ((f2)[0].v.tint & 63)
#define	lshr_long(t, f1, f2)			(t)[0].v.tlong = ((uint64)(f1)[0].v.tlong) >> ((f2)[0].v.tint & 63)

#define	lcmp(t, f1, f2)				lcc = ((f2)[0].v.tlong) - ((f1)[0].v.tlong); \
						(t)[0].v.tint = (lcc < 0 ? -1 : lcc > 0 ? 1 : 0)

#define	cvt_int_long(t, f)			(t)[0].v.tlong = (f)[0].v.tint
#define	cvt_long_int(t, f)			(t)[0].v.tint = (f)[0].v.tlong
#define	cvt_long_float(t, f)			(t)[0].v.tfloat = (f)[0].v.tlong
#define	cvt_long_double(t, f)			(t)[0].v.tdouble = (f)[0].v.tlong
#define	cvt_float_long(t, f)			(t)[0].v.tlong = floor((f)[0].v.tfloat)
#define	cvt_double_long(t, f)			(t)[0].v.tlong = floor((f)[0].v.tdouble)

#define	move_int_const(t, c)			(t)[0].v.tint = (c)
#define	move_ref_const(t, c)			(t)[0].v.taddr = (void*)(c)
#define	move_label_const(t, c)			move_ref_const(t, c)
#define	move_string_const(t, c)			move_ref_const(t, c)

#define	move_int(t, f)				(t)[0].v.tint = (f)[0].v.tint
#define	move_ref(t, f)				(t)[0].v.taddr = (f)[0].v.taddr
#define	move_any(t, f)				move_long(t, f)

#define	swap_any(t1, t2)			{			\
						  tmp[0] = (t1)[0];	\
						  (t1)[0] = (t2)[0];	\
						  (t2)[0] = tmp[0];	\
						}

#define	load_int(t, f)				(t)[0].v.tint = *(jint*)((f)[0].v.taddr)
#define	load_ref(t, f)				(t)[0].v.taddr = *(void**)((f)[0].v.taddr)
#define	load_byte(t, f)				(t)[0].v.tint = *(jbyte*)((f)[0].v.taddr)
#define	load_char(t, f)				(t)[0].v.tint = *(jchar*)((f)[0].v.taddr)

#define	store_int(t, f)				*(jint*)((t)[0].v.taddr) = ((f)[0].v.tint)
#define	store_ref(t, f)				*(void**)((t)[0].v.taddr) = ((f)[0].v.taddr)
#define	store_byte(t, f)			*(jbyte*)((t)[0].v.taddr) = ((f)[0].v.tint)
#define	store_char(t, f)			*(jchar*)((t)[0].v.taddr) = ((f)[0].v.tint)

#define	load_any(t, f)				load_int(t, f)
#define load_offset_any(t, f, o)		load_offset_int(t, f, o)
#define	store_any(t, f)				store_int(t, f)
#define store_offset_any(t, f, o)		store_offset_int(t, f, o)

#define	add_int_const(t, f, c)			(t)[0].v.tint = ((f)[0].v.tint) + (c)
#define	add_ref_const(t, f, c)			(t)[0].v.taddr = (void*)((uint8*)((f)[0].v.taddr) + (c))
#define	sub_int_const(t, f, c)			(t)[0].v.tint = ((f)[0].v.tint) - (c)
#define	mul_int_const(t, f, c)			(t)[0].v.tint = ((f)[0].v.tint) * (c)

#define	add_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) + ((f2)[0].v.tint)
#define	add_ref(t, f1, f2)			(t)[0].v.taddr = (void*)((uint8*)((f1)[0].v.taddr) + ((f2)[0].v.tint))
#define	sub_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) - ((f2)[0].v.tint)
#define	mul_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) * ((f2)[0].v.tint)
#define	div_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) / ((f2)[0].v.tint)
#define	rem_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) % ((f2)[0].v.tint)
#define	neg_int(t, f)				(t)[0].v.tint = -((f)[0].v.tint)
#define	lshl_int_const(t, f, c)			(t)[0].v.tint = ((f)[0].v.tint) << (c & 31)
#define	lshl_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) << ((f2)[0].v.tint & 31)
#define	ashr_int(t, f1, f2)			(t)[0].v.tint = ((int32)(f1)[0].v.tint) >> ((f2)[0].v.tint & 31)
#define	lshr_int(t, f1, f2)			(t)[0].v.tint = ((uint32)(f1)[0].v.tint) >> ((f2)[0].v.tint & 31)
#define	and_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) & ((f2)[0].v.tint)
#define	or_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) | ((f2)[0].v.tint)
#define	xor_int(t, f1, f2)			(t)[0].v.tint = ((f1)[0].v.tint) ^ ((f2)[0].v.tint)

#define	cvt_int_byte(t, f)			(t)[0].v.tint = (((f)[0].v.tint) << 24) >> 24
#define	cvt_int_char(t, f)			(t)[0].v.tint = ((f)[0].v.tint) & 0xFFFF
#define	cvt_int_short(t, f)			(t)[0].v.tint = (((f)[0].v.tint) << 16) >> 16

#define	branch_indirect(w)			w
#define	branch_a(w)				w

#define	cbranch_int_eq(s1, s2, w)		if ((s1)[0].v.tint == (s2)[0].v.tint) w
#define	cbranch_int_ne(s1, s2, w)		if ((s1)[0].v.tint != (s2)[0].v.tint) w
#define	cbranch_int_lt(s1, s2, w)		if ((s1)[0].v.tint < (s2)[0].v.tint) w
#define	cbranch_int_le(s1, s2, w)		if ((s1)[0].v.tint <= (s2)[0].v.tint) w
#define	cbranch_int_gt(s1, s2, w)		if ((s1)[0].v.tint > (s2)[0].v.tint) w
#define	cbranch_int_ge(s1, s2, w)		if ((s1)[0].v.tint >= (s2)[0].v.tint) w
#define	cbranch_int_ult(s1, s2, w)		if ((unsigned int)(s1)[0].v.tint < (unsigned int)(s2)[0].v.tint) w

#define	cbranch_int_const_eq(s1, s2, w)		if ((s1)[0].v.tint == (s2)) w
#define	cbranch_int_const_ne(s1, s2, w)		if ((s1)[0].v.tint != (s2)) w
#define	cbranch_int_const_lt(s1, s2, w)		if ((s1)[0].v.tint < (s2)) w
#define	cbranch_int_const_le(s1, s2, w)		if ((s1)[0].v.tint <= (s2)) w
#define	cbranch_int_const_gt(s1, s2, w)		if ((s1)[0].v.tint > (s2)) w
#define	cbranch_int_const_ge(s1, s2, w)		if ((s1)[0].v.tint >= (s2)) w
#define	cbranch_int_const_ult(s1, s2, w)	if ((unsigned int)(s1)[0].v.tint < (unsigned int)(s2)) w

#define cbranch_ref_eq(s1, s2, w)		if ((s1)[0].v.taddr == (s2)[0].v.taddr) w
#define cbranch_ref_ne(s1, s2, w)		if ((s1)[0].v.taddr != (s2)[0].v.taddr) w
#define cbranch_ref_const_eq(s1, s2, w)		if ((s1)[0].v.taddr == (void*)(s2)) w
#define cbranch_ref_const_ne(s1, s2, w)		if ((s1)[0].v.taddr != (void*)(s2)) w

#define	call(m)					softcall_initialise_class(method_class()); \
						virtualMachine((methods*)(m)[0].v.taddr, sp+1, retval, tid)
#define	call_indirect_method(m)			softcall_initialise_class(method_class()); \
						virtualMachine(m, sp+1, retval, tid)

#define	ret()					goto end

#define	returnarg_int(s)			retval[0].v.tint = (s)[0].v.tint
#define	returnarg_ref(s)			retval[0].v.taddr = (s)[0].v.taddr
#define	returnarg_long(s)			retval[0].v.tlong = (s)[0].v.tlong
#define	returnarg_float(s)			retval[0].v.tfloat = (s)[0].v.tfloat
#define	returnarg_double(s)			retval[0].v.tdouble = (s)[0].v.tdouble

#define	pusharg_int_const(c, i)			/* Not needed for interpreter */
#define	pusharg_int(f, i)			/* Not needed for interpreter */
#define	pusharg_ref(f, i)			/* Not needed for interpreter */
#define	pusharg_ref_const(f, i)			/* Not needed for interpreter */
#define	pusharg_any(f, i)			/* Not needed for interpreter */
#define	pusharg_anylong(f, i)			/* Not needed for interpreter */
#define	popargs()				/* Not needed for interpreter */

#define	return_int(s)				(s)[0].v.tint = retval[0].v.tint
#define	return_long(s)				(s)[0].v.tlong = retval[0].v.tlong
#define	return_float(s)				(s)[0].v.tfloat = retval[0].v.tfloat
#define	return_double(s)			(s)[0].v.tdouble = retval[0].v.tdouble
#define	return_ref(s)				(s)[0].v.taddr = retval[0].v.taddr

#define	monitor_enter()				/* Not needed for interpreter */
#define	monitor_exit()				/* Not needed for interpreter */

#define	start_function()			/* Not needed for interpreter */
#define	start_basic_block()			/* Not needed for interpreter */
#define	end_basic_block()			/* Not needed for interpreter */
#define	end_function()				/* Not needed for interpreter */
#define	start_sub_block()			/* Not needed for interpreter */
#define	end_sub_block()				/* Not needed for interpreter */

#define	set_label(i, l)				label_##i##l:
#define	reference_code_label(l)			npc = (l)
#define	reference_label(i, l)			goto label_##i##l
#define	reference_table_label(l)		&code[npc]
#define	stored_code_label(l)			npc = (l)[0].v.tint
#define	table_code_label(l)			npc = (l)[0].v.tint + pc

#define	load_key(t, f)				(t)[0].v.tint = \
						 ((((uint8*)(f)->v.taddr)[0] << 24) | \
						 (((uint8*)(f)->v.taddr)[1] << 16) | \
						 (((uint8*)(f)->v.taddr)[2] << 8) | \
						 ((uint8*)(f)->v.taddr)[3])
#define	load_code_ref				load_key

#define	breakpoint()				ABORT()

#define	move_double(t, f)			(t)[0].v.tdouble = (f)[0].v.tdouble
#define	move_double_const(t, c)			(t)[0].v.tdouble = (c)
#define	move_float(t, f)			(t)[0].v.tfloat = (f)[0].v.tfloat
#define	move_float_const(t, c)			(t)[0].v.tfloat = (c)
#define	move_long(t, f)				(t)[0].v.tlong = (f)[0].v.tlong
#define	move_anylong(t, f)			move_long(t, f)

#define	load_short(t, f)			(t)[0].v.tint = *(jshort*)((f)[0].v.taddr)
#define	load_long(t, f)				(t)[0].v.tlong = *(jlong*)((f)[0].v.taddr)
#define	load_anylong(t, f)			load_long(t, f)
#define	load_float(t, f)			(t)[0].v.tfloat = *(jfloat*)((f)[0].v.taddr)
#define	load_double(t, f)			(t)[0].v.tdouble = *(jdouble*)((f)[0].v.taddr)

#define	store_short(t, f)			*(jshort*)((t)[0].v.taddr) = ((f)[0].v.tint)
#define	store_long(t, f)			*(jlong*)((t)[0].v.taddr) = ((f)[0].v.tlong)
#define	store_anylong(t, f)			store_long(t, f)
#define	store_float(t, f)			*(jfloat*)((t)[0].v.taddr) = ((f)[0].v.tfloat)
#define	store_double(t, f)			*(jdouble*)((t)[0].v.taddr) = ((f)[0].v.tdouble)

#define	load_offset_int(t, f, o)		(t)[0].v.tint = *(jint*)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_byte(t, f, o)		(t)[0].v.tint = *(jbyte*)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_char(t, f, o)		(t)[0].v.tint = *(jchar*)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_short(t, f, o)		(t)[0].v.tint = *(jshort*)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_ref(t, f, o)		(t)[0].v.taddr = *(void**)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_long(t, f, o)		(t)[0].v.tlong = *(jlong*)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_float(t, f, o)		(t)[0].v.tfloat = *(jfloat*)((uint8*)(f)[0].v.taddr + (o))
#define	load_offset_double(t, f, o)		(t)[0].v.tdouble = *(jdouble*)((uint8*)(f)[0].v.taddr + (o))

#define	store_offset_int(t, o, f)		*(jint*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tint)
#define	store_offset_byte(t, o, f)		*(jbyte*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tint)
#define	store_offset_char(t, o, f)		*(jchar*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tint)
#define	store_offset_short(t, o, f)		*(jshort*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tint)
#define	store_offset_ref(t, o, f)		*(void**)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.taddr)
#define	store_offset_long(t, o, f)		*(jlong*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tlong)
#define	store_offset_float(t, o, f)		*(jfloat*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tfloat)
#define	store_offset_double(t, o, f)		*(jdouble*)((uint8*)(t)[0].v.taddr + (o)) = ((f)[0].v.tdouble)

#define	load_offset_scaled_int(t, b, i, o)	(t)[0].v.tint = ((jint*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_byte(t, b, i, o)	(t)[0].v.tint = ((jbyte*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_char(t, b, i, o)	(t)[0].v.tint = ((jchar*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_short(t, b, i, o)	(t)[0].v.tint = ((jshort*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_ref(t, b, i, o)	(t)[0].v.taddr = ((void**)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_long(t, b, i, o)	(t)[0].v.tlong = ((jlong*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_float(t, b, i, o)	(t)[0].v.tfloat = ((jfloat*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]
#define	load_offset_scaled_double(t, b, i, o)	(t)[0].v.tdouble = ((jdouble*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint]

#define	store_offset_scaled_int(b, i, o, f)	((jint*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tint)
#define	store_offset_scaled_byte(b, i, o, f)	((jbyte*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tint)
#define	store_offset_scaled_char(b, i, o, f)	((jchar*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tint)
#define	store_offset_scaled_short(b, i, o, f)	((jshort*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tint)
#define	store_offset_scaled_ref(b, i, o, f)	((void**)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.taddr)
#define	store_offset_scaled_long(b, i, o, f)	((jlong*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tlong)
#define	store_offset_scaled_float(b, i, o, f)	((jfloat*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tfloat)
#define	store_offset_scaled_double(b, i, o, f)	((jdouble*)((uint8*)(b)[0].v.taddr + (o)))[(i)[0].v.tint] = ((f)[0].v.tdouble)

#define	load_addr_int(t, a)	(t)[0].v.tint = *(jint*)(a)
#define	load_addr_byte(t, a)	(t)[0].v.tint = *(jbyte*)(a)
#define	load_addr_char(t, a)	(t)[0].v.tint = *(jchar*)(a)
#define	load_addr_short(t, a)	(t)[0].v.tint = *(jshort*)(a)
#define	load_addr_ref(t, a)	(t)[0].v.taddr = *(void**)(a)
#define	load_addr_long(t, a)	(t)[0].v.tlong = *(jlong*)(a)
#define	load_addr_float(t, a)	(t)[0].v.tfloat = *(jfloat*)(a)
#define	load_addr_double(t, a)	(t)[0].v.tdouble = *(jdouble*)(a)

#define	store_addr_int(a, f)	*(jint*)(a) = (f)[0].v.tint
#define	store_addr_byte(a, f)	*(jbyte*)(a) = (f)[0].v.tint
#define	store_addr_char(a, f)	*(jchar*)(a) = (f)[0].v.tint
#define	store_addr_short(a, f)	*(jshort*)(a) = (f)[0].v.tint
#define	store_addr_ref(a, f)	*(void**)(a) = (f)[0].v.taddr
#define	store_addr_long(a, f)	*(jlong*)(a) = (f)[0].v.tlong
#define	store_addr_float(a, f)	*(jfloat*)(a) = (f)[0].v.tfloat
#define	store_addr_double(a, f)	*(jdouble*)(a) = (f)[0].v.tdouble

#define	add_float(t, f1, f2)			(t)[0].v.tfloat = (f1)[0].v.tfloat + (f2)[0].v.tfloat
#define	add_double(t, f1, f2)			(t)[0].v.tdouble = (f1)[0].v.tdouble + (f2)[0].v.tdouble
#define	sub_float(t, f1, f2)			(t)[0].v.tfloat = (f1)[0].v.tfloat - (f2)[0].v.tfloat
#define	sub_double(t, f1, f2)			(t)[0].v.tdouble = (f1)[0].v.tdouble - (f2)[0].v.tdouble
#define	mul_float(t, f1, f2)			(t)[0].v.tfloat = (f1)[0].v.tfloat * (f2)[0].v.tfloat
#define	mul_double(t, f1, f2)			(t)[0].v.tdouble = (f1)[0].v.tdouble * (f2)[0].v.tdouble
#define	div_float(t, f1, f2)			(t)[0].v.tfloat = (f1)[0].v.tfloat / (f2)[0].v.tfloat
#define	div_double(t, f1, f2)			(t)[0].v.tdouble = (f1)[0].v.tdouble / (f2)[0].v.tdouble
#define	rem_float(t, f1, f2)			(t)[0].v.tfloat = javaRemainderf((f1)[0].v.tfloat, (f2)[0].v.tfloat)
#define	rem_double(t, f1, f2)			(t)[0].v.tdouble = javaRemainder((f1)[0].v.tdouble, (f2)[0].v.tdouble)
#define	neg_float(t, f)				(t)[0].v.tfloat = -(f)[0].v.tfloat
#define	neg_double(t, f)			(t)[0].v.tdouble = -(f)[0].v.tdouble

#define	cmpg_float(t, f1, f2)			(t)[0].v.tint = soft_fcmpg((f1)[0].v.tfloat, (f2)[0].v.tfloat)
#define	cmpg_double(t, f1, f2)			(t)[0].v.tint = soft_dcmpg((f1)[0].v.tdouble, (f2)[0].v.tdouble)
#define	cmpl_float(t, f1, f2)			(t)[0].v.tint = soft_fcmpl((f1)[0].v.tfloat, (f2)[0].v.tfloat)
#define	cmpl_double(t, f1, f2)			(t)[0].v.tint = soft_dcmpl((f1)[0].v.tdouble, (f2)[0].v.tdouble)

#define	cvt_int_float(t, f)			(t)[0].v.tfloat = (f)[0].v.tint
#define	cvt_int_double(t, f)			(t)[0].v.tdouble = (f)[0].v.tint
#define	cvt_float_int(t, f)			(t)[0].v.tint = floor((f)[0].v.tfloat)
#define	cvt_float_double(t, f)			(t)[0].v.tdouble = (f)[0].v.tfloat
#define	cvt_double_int(t, f)			(t)[0].v.tint = floor((f)[0].v.tdouble)
#define	cvt_double_float(t, f)			(t)[0].v.tfloat = (f)[0].v.tdouble

#define	softcall_lookupmethod(r, n, t)		(r)[0].v.taddr = soft_lookupmethod((t)[0].v.taddr, (n)->name, (n)->signature)
#define	softcall_new(r, t)			(r)->v.taddr = soft_new(t)
#define	softcall_newarray(r, s, t)		(r)->v.taddr = soft_newarray(t, (s)->v.tint)
#define	softcall_anewarray(r, s, t)		(r)->v.taddr = soft_anewarray(t, (s)->v.tint)
#define	softcall_athrow(s)			soft_athrow((s)[0].v.taddr)
#define	softcall_checkcast(n, o, t)		soft_checkcast(t, (o)->v.taddr)
#define softcall_initialise_class(cl)		soft_initialise_class(cl)
#define	softcall_instanceof(r, o, t)		(r)->v.tint = soft_instanceof(t, (o)->v.taddr)

#define	softcall_monitorenter(o)		lockMutex((o)[0].v.taddr)
#define	softcall_monitorexit(o)			unlockMutex((o)[0].v.taddr)

#define	softcall_multianewarray(r, z, s, t)	(r)->v.taddr = soft_multianewarray(t, z, s)

#define	softcall_badarrayindex()		soft_badarrayindex()
#define	softcall_nullpointer()			soft_nullpointer()
#define	softcall_nosuchmethod(c,n,s)		soft_nosuchmethod(c,n,s)

#define	softcall_checkarraystore(a, o)		soft_checkarraystore((a)[0].v.taddr, (o)[0].v.taddr)
#define	softcall_addreference(f, t)		soft_addreference((f)[0].v.taddr, (t)[0].v.taddr)
#define	softcall_addreference_static(f, t)	soft_addreference((f), (t)[0].v.taddr)

#define	adjustpc(a)				/* Not needed for interpreter */

#define	check_array_index(O, I)			if ((I)[0].v.tint >= ((struct Array*)((O)[0].v.taddr))->length) { \
							soft_badarrayindex(); \
						}
#define	build_call_frame(SIG, OBJ, NRARGS)	/* Not needed for interpreter */


#endif
