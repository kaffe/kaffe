/*
 * slots.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __slots_h
#define	__slots_h

#define	push(N)			sp += (N)
#define	pop(N)			sp -= (N)

#define	stack(N)		(&sp[-(N)])
#define	stack_long(N)		(&sp[-(N)-1])
#define	stack_ref		stack
#define	stack_float		stack
#define	stack_double		stack_long

#define	rstack			stack
#define	wstack			stack
#define	rstack_ref		stack_ref
#define	wstack_ref		stack_ref
#define	rstack_long		stack_long
#define	wstack_long		stack_long
#define	rstack_float		stack_float
#define	wstack_float		stack_float
#define	rstack_double		stack_double
#define	wstack_double		stack_double

#define	local(N)		(&lcl[(N)])
#define local_ref		local
#define local_long		local
#define local_float		local
#define local_double		local_long

#define	slot_alloctmp(s)	/* Does nothing for interpreter */
#define	slot_freetmp(s)		/* Does nothing for interpreter */
#define	slot_nowriteback(s)	/* Does nothing for interpreter */
#define	slot_nowriteback2(s)	/* Does nothing for interpreter */

/* NB. This should match jvalue and eventually be replaced by it */
typedef struct _slots {
        union {
                jint            tint;
                jword           tword;
                jlong           tlong;
                jfloat          tfloat;
                jdouble         tdouble;
                void*           taddr;
                char*           tstr;
        } v;
} slots;

#endif
