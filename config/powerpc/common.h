/*
 * powerpc/common.h
 * Common PowerPC configuration information.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __powerpc_common_h
#define __powerpc_common_h

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

#if defined(NEED_sysdepCallMethod)
#include "sysdepCallMethod.h"
#endif /* defined(NEED_sysdepCallMethod) */


/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 *
 * If you change this, don't forget to change not inlined version in
 * aix/md.c and netbsd1/md.c
 */
#define COMPARE_AND_EXCHANGE(A,O,N)		\
({						\
	int tmp, ret;				\
						\
	asm volatile(				\
	"	li	%1,0\n"			\
	"1:	lwarx	%0,0,%3\n"		\
	"	cmpw	0,%0,%4\n"		\
	"	bne	2f\n"			\
	"	stwcx.	%5,0,%3\n"		\
	"	bne-	1b\n"			\
	"	sync\n"				\
	"	li	%1,1\n"			\
	"2:\n"					\
	: "=&r"(tmp), "=&r"(ret), "=m"(*(A))	\
	: "r"(A), "r"(O), "r"(N), "m"(*(A))	\
	: "cc", "memory");			\
						\
	ret;					\
})

#define   ALIGNMENT_OF_SIZE(S)    ((S) < 4 ? (S) : 4)

#endif
