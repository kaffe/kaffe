/*
 * powerpc/common.h
 * Common PowerPC configuration information.
 *
 * by Kevin B. Hendricks <khendricks@ivey.uwo.ca>
 * and Edouard G. Parmelan <egp@free.fr>
 */

#ifndef __powerpc_common_h
#define __powerpc_common_h

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

#if defined(EGP)
#if NEED_sysdepCallMethod
#include "callmethod_v4.h"
#endif
#else
#if defined(__linux__) || defined(__NetBSD__)
#if NEED_sysdepCallMethod
#include "callmethod_ppc.h"
#define	sysdepCallMethod(CALL) sysdepCallMethod_ppc(CALL)
#endif
#endif
#endif /* defined(EGP) */


/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)		\
({						\
	int tmp, ret = 0;			\
						\
	asm volatile(				\
	"	eieio\n"			\
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

#endif
