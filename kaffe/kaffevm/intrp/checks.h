/*
 * checks.h
 * Validate the given item is of the correct type.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __checks_h
#define	__checks_h

#define	check_local_int(l)	CHDBG (printf ("- local %d: int %d\n", (l), local(l)->v.tint))
#define	check_local_long(l)	CHDBG (printf ("- local %d: long %ld\n", (l), local_long(l)->v.tlong))
#define	check_local_float(l)	CHDBG (printf ("- local %d: float %g\n", (l), local_float(l)->v.tfloat))
#define	check_local_double(l)	CHDBG (printf ("- local %d: double %lg\n", (l), local_double(l)->v.tdouble))
#define	check_local_ref(l)	CHDBG (printf ("- local %d: ref %p\n", (l), local(l)->v.taddr))

#define	check_stack_int(l)	CHDBG (printf ("- stack %d: int %d\n", (l), stack(l)->v.tint))
#define	check_stack_long(l)	CHDBG (printf ("- stack %d: long %ld\n", (l), stack(l)->v.tlong))
#define	check_stack_float(l)	CHDBG (printf ("- stack %d: float %g\n", (l), stack(l)->v.tfloat))
#define	check_stack_double(l)	CHDBG (printf ("- stack %d: double %lg\n", (l), stack(l)->v.tdouble))
#define	check_stack_ref(l)	CHDBG (printf ("- stack %d: ref %p\n", (l), stack(l)->v.taddr))

#define	check_stack_array(l)		CHDBG (printf ("- stack %d: array * %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_intarray(l)		CHDBG (printf ("- stack %d: array int %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_longarray(l)	CHDBG (printf ("- stack %d: array long %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_floatarray(l)	CHDBG (printf ("- stack %d: array float %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_doublearray(l)	CHDBG (printf ("- stack %d: array double %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_refarray(l)		CHDBG (printf ("- stack %d: array ref %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_bytearray(l)	CHDBG (printf ("- stack %d: array byte %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_chararray(l)	CHDBG (printf ("- stack %d: array char %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_shortarray(l)	CHDBG (printf ("- stack %d: array short %p[%d]\n", (l), stack(l)->v.taddr, *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))

#define check_pc(l)		CHDBG (printf ("  ARG %d: %d\n", l, getpc(l)))
#define check_pc_wide(l)	CHDBG (printf ("  ARG %d: %d = %d + %d[WIDE]\n", l, getpc(l)+wide, getpc(l), wide))
#define check_pcidx(l)	CHDBG (printf ("  ARG %d-%d: %d\n", l, l+1, (int16)((getpc(l) << 8) | getpc(l+1))))
#define check_pcwidx(l)	CHDBG (printf ("  ARG %d-%d: %d\n", l, l+3, (int32)((getpc(l) << 24) | (getpc(l+1) << 16) | (getpc(l+2) << 8) | getpc(l+3))))

#endif
