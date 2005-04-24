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

#define	check_local_int(l)	CHDBG (dprintf ("- local %d: int %d\n", (l), (int) local(l)->v.tint))
#define	check_local_long(l)	CHDBG (dprintf ("- local %d: long %jd\n", (l), (jlong) local_long(l)->v.tlong))
#define	check_local_float(l)	CHDBG (dprintf ("- local %d: float %g\n", (l), (double) local_float(l)->v.tfloat))
#define	check_local_double(l)	CHDBG (dprintf ("- local %d: double %g\n", (l), (double) local_double(l)->v.tdouble))
#define	check_local_ref(l)	CHDBG (dprintf ("- local %d: ref %p\n", (l), (void *) local(l)->v.taddr))

#define	check_stack_int(l)	CHDBG (dprintf ("- stack %d: int %d\n", (l), (int) stack(l)->v.tint))
#define	check_stack_long(l)	CHDBG (dprintf ("- stack %d: long %jd\n", (l), (jlong) stack_long(l)->v.tlong))
#define	check_stack_float(l)	CHDBG (dprintf ("- stack %d: float %g\n", (l), (double) stack(l)->v.tfloat))
#define	check_stack_double(l)	CHDBG (dprintf ("- stack %d: double %g\n", (l), (double) stack_double(l)->v.tdouble))
#define	check_stack_ref(l)	CHDBG (dprintf ("- stack %d: ref %p\n", (l), (void *) stack(l)->v.taddr))

#define	check_stack_array(l)		CHDBG (dprintf ("- stack %d: array * %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_intarray(l)		CHDBG (dprintf ("- stack %d: array int %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_longarray(l)	CHDBG (dprintf ("- stack %d: array long %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_floatarray(l)	CHDBG (dprintf ("- stack %d: array float %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_doublearray(l)	CHDBG (dprintf ("- stack %d: array double %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_refarray(l)		CHDBG (dprintf ("- stack %d: array ref %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_bytearray(l)	CHDBG (dprintf ("- stack %d: array byte %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_chararray(l)	CHDBG (dprintf ("- stack %d: array char %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))
#define	check_stack_shortarray(l)	CHDBG (dprintf ("- stack %d: array short %p[%d]\n", (l), (void *) stack(l)->v.taddr, (int) *(uint32*)(object_array_length+(char*)stack(l)->v.taddr)))

#define check_pc(l)		CHDBG (dprintf ("  ARG %d: %d\n", l, getpc(l)))
#define check_pc_wide(l)	CHDBG (dprintf ("  ARG %d: %d = %d + %d[WIDE]\n", l, getpc(l)+wide, getpc(l), wide))
#define check_pcidx(l)	CHDBG (dprintf ("  ARG %d-%d: %d\n", l, l+1, (int16)((getpc(l) << 8) | getpc(l+1))))
#define check_pcwidx(l)	CHDBG (dprintf ("  ARG %d-%d: %d\n", l, l+3, (int32)((getpc(l) << 24) | (getpc(l+1) << 16) | (getpc(l+2) << 8) | getpc(l+3))))

#endif
