/*
 * m68k/common.h
 * Common M68K configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1998;
 * Based on the ports
 *      by Remi Perrot <r_perrot@mail.club-internet.fr> to m68k/linux
 * and
 *      by Alexandre Oliva <oliva@dcc.unicamp.br> to sparc
 *
 * Modified by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1999 to support
 *	both linux and netbsd1.
 */

#ifndef __m68k_common_h
#define __m68k_common_h

/*
 * Alignment in structure is 2 bytes packed.
 */
#define ALIGNMENT_OF_SIZE(S)    (((S>1)?2:1))

/*
 * sysdepCallMethod supports:
 *
 *	Calling sequences for linux and netbsd1 are same, except for
 *	the place of return values. The float/double values are in
 *	fp0 (linux) or d0/d1 (netbsd1).
 *
 *	Still I do not understand 'asm' statement well, and the
 *	statement below is a 'because it works' version.
 */
#if defined(__linux__)
#ifdef	USE_ASM		/* Old and obsolete... */
#define	sysdepCallMethod(CALL)					       \
               asm volatile ("                                       \n\
       1:                                                            \n\
               cmp.l   #0,%0                                         \n\
               beq     3f                                            \n\
               subq.l  #1,%0                                         \n\
               cmp.b   #0,(%2,%0.l)                                  \n\
               beq     1b                                            \n\
               cmp.b   #1,(%2,%0.l)                                  \n\
               beq     2f                                            \n\
               move.l  4(%1,%0.l*8),-(%/sp)                          \n\
       2:                                                            \n\
               move.l  (%1,%0.l*8),-(%/sp)                           \n\
               jmp     1b                                            \n\
       3:                                                            \n\
               jsr     (%3)                                          \n\
               cmp.b   #0x46,%4                                      \n\
               bne     4f                                            \n\
               fmove.s %/fp0,(%5)                                    \n\
               jmp     6f                                            \n\
       4:                                                            \n\
               cmp.b   #0x44,%4                                      \n\
               bne     5f                                            \n\
               fmove.d %/fp0,(%5)                                    \n\
               jmp     6f                                            \n\
       5:                                                            \n\
               move.l  %/d0,(%5)                                     \n\
               cmp.b   #0x4a,%4                                      \n\
               bne     6f                                            \n\
               move.l  %/d1,4(%5)                                    \n\
       6:                                                              \
       " :                                                             \
         : "r" ((CALL)->nrargs),                                       \
           "a" ((CALL)->args),                                         \
           "a" ((CALL)->callsize),                                     \
           "a" ((CALL)->function),                                     \
           "m" ((CALL)->rettype),                                      \
           "a" ((CALL)->ret)                                           \
          : "d0", "d1", "fp0", "cc", "memory");                        \
               asm volatile ("                                       \n\
               add.w %0,%/sp                                         \n\
        " : : "r" ((CALL)->argsize * sizeof(jint)) : "cc")
#else	/* USE_ASM */
#define sysdepCallMethod(CALL) do {				\
	int extraargs[(CALL)->nrargs];				\
	register int d0 asm ("d0");				\
	register int d1 asm ("d1");				\
	register double f0d asm ("fp0");			\
	register float f0f asm ("fp0");				\
	int *res;						\
	int *args = extraargs;					\
	int argidx;						\
	for(argidx = 0; argidx < (CALL)->nrargs; ++argidx) {	\
		if ((CALL)->callsize[argidx])			\
			*args++ = (CALL)->args[argidx].i;	\
		else						\
			*args++ = (CALL)->args[argidx-1].j;	\
	}							\
	asm volatile ("jsr	%2@\n"				\
	 : "=r" (d0), "=r" (d1)					\
	 : "a" ((CALL)->function)				\
	 : "cc", "memory");					\
	if ((CALL)->retsize != 0) {				\
		res = (int *)(CALL)->ret;			\
	switch((CALL)->retsize) {				\
	case 2:							\
	  if ((CALL)->rettype == 'D')				\
	    *(double*)res = f0d;				\
	  else {						\
		res[1] = d1;					\
		res[0] = d0;					\
	  }							\
	  break;						\
	case 1:							\
	  if ((CALL)->rettype == 'F')				\
	    *(double*)res = f0f;				\
	  else							\
		res[0] = d0;					\
	  break;						\
	}							\
	}							\
} while (0)
#endif	/* USE_ASM */
#else	/* defined(__linux__) */

#define sysdepCallMethod(CALL) do {				\
	int extraargs[(CALL)->nrargs];				\
	register int d0 asm ("d0");				\
	register int d1 asm ("d1");				\
	int *res;						\
	int *args = extraargs;					\
	int argidx;						\
	for(argidx = 0; argidx < (CALL)->nrargs; ++argidx) {	\
		if ((CALL)->callsize[argidx])			\
			*args++ = (CALL)->args[argidx].i;	\
		else						\
			*args++ = (CALL)->args[argidx-1].j;	\
	}							\
	asm volatile ("jsr	%2@\n"				\
	 : "=r" (d0), "=r" (d1)					\
	 : "a" ((CALL)->function)				\
	 : "cc", "memory");					\
	if ((CALL)->retsize != 0) {				\
		res = (int *)(CALL)->ret;			\
		res[1] = d1;					\
		res[0] = d0;					\
	}							\
} while (0)

#endif	/* defined(__linux__) */


/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)		\
({						\
	unsigned int tmp, ret = 0;		\
						\
	asm volatile(				\
	"1:	movel	%2, %0\n"		\
	"	cmpl	%4, %0\n"		\
	"	bne	2f\n"			\
	"	casl	%0, %5, %2\n"		\
	"	bne	1b\n"			\
	"	movl	#1, %1\n"		\
	"2:\n"					\
	: "=&r" (tmp), "=&r" (ret), "=m" (*(A))	\
	: "m" (*(A)), "r" (O), "r" (N)		\
	: "memory");				\
						\
	ret;					\
})


#endif
