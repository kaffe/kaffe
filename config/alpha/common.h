/*
 * alpha/common.h
 * Common Alpha configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 * and Edouard G. Parmelan <egp@free.fr>
 */

#ifndef __alpha_common_h
#define __alpha_common_h

#if NEED_sysdepCallMethod

#define LONG_SYSDEP 1
#include "sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "sysdepCallMethod.h"

#endif /* NEED_sysdepCallMethod */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)			\
({							\
	unsigned long tmp;				\
	int ret;					\
							\
	asm volatile(					\
	"1:	ldq_l %0,%5\n"				\
	"	cmpeq %0,%3,%1\n"			\
	"	cmovne %1,%4,%0\n"			\
	"	stq_c %0,%2\n"				\
	"	beq %0,2f\n"				\
	"	mb\n"					\
	"	br 3f\n"				\
	"2:	br 1b\n"				\
	"3:\n"						\
	: "=&r"(tmp), "=&r"(ret), "=m"(*(A))		\
	: "r"(O), "r"(N), "m"(*(A)) : "memory");	\
							\
	ret;						\
})

#endif
