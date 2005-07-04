/*
 * powerpc/netbsd1/md.c
 * NetBSD PowerPC specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#if (__GNUC__ == 2 && __GNUC_MINOR__ == 91)
/* egcs-1.1.2 aka gcc-2.91 can't compile locks.c with "cc" constraint */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
int __egcs_cmpxchg(void **A, void *O, void *N) {
	int tmp, ret;

	asm volatile(
	"	li	%1,0\n"
	"1:	lwarx	%0,0,%3\n"
	"	cmpw	0,%0,%4\n"
	"	bne	2f\n"
	"	stwcx.	%5,0,%3\n"
	"	bne-	1b\n"
	"	sync\n"
	"	li	%1,1\n"
	"2:\n"
	: "=&r"(tmp), "=&r"(ret), "=m"(*(A))
	: "r"(A), "r"(O), "r"(N), "m"(*(A))
	: "cc", "memory");

	return ret;
}
#endif


#if defined(JTHREAD_JBLEN) && (JTHREAD_JBLEN > 0)
/*-
   From glibc-2.2.2 sysdeps/powerpc/__longjmp.S and sysdeps/powerpc/setjmp.S

   longjmp for PowerPC.
   setjmp for PowerPC.
   Copyright (C) 1995, 1996, 1997, 1999, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#define SP_OFFSET 0

#if 0
# define JB_GPR1   0  /* Also known as the stack pointer */
# define JB_GPR2   1
# define JB_LR     2  /* The address we will return to */
# define JB_GPRS   3  /* GPRs 14 through 31 are saved, 18 in total */
# define JB_CR     21 /* Condition code registers. */
# define JB_FPRS   22 /* FPRs 14 through 31 are saved, 18*2 words total */
# define JB_SIZE   (58*4)
#endif


asm(
"	.text \n"
"	.globl kaffe_longjmp \n"
"kaffe_longjmp: \n"
" \n"
"	lwz 1,(0*4)(3) \n"
"	lwz 2,(1*4)(3) \n"
"	lwz 0,(2*4)(3) \n"
"	lwz 14,((3+0)*4)(3) \n"
"	lfd 14,((22+0*2)*4)(3) \n"
"	lwz 15,((3+1)*4)(3) \n"
"	lfd 15,((22+1*2)*4)(3) \n"
"	lwz 16,((3+2)*4)(3) \n"
"	lfd 16,((22+2*2)*4)(3) \n"
"	lwz 17,((3+3)*4)(3) \n"
"	lfd 17,((22+3*2)*4)(3) \n"
"	lwz 18,((3+4)*4)(3) \n"
"	lfd 18,((22+4*2)*4)(3) \n"
"	lwz 19,((3+5)*4)(3) \n"
"	lfd 19,((22+5*2)*4)(3) \n"
"	lwz 20,((3+6)*4)(3) \n"
"	lfd 20,((22+6*2)*4)(3) \n"
"	mtlr 0 \n"
"	lwz 21,((3+7)*4)(3) \n"
"	lfd 21,((22+7*2)*4)(3) \n"
"	lwz 22,((3+8)*4)(3) \n"
"	lfd 22,((22+8*2)*4)(3) \n"
"	lwz 0,(21*4)(3) \n"
"	lwz 23,((3+9)*4)(3) \n"
"	lfd 23,((22+9*2)*4)(3) \n"
"	lwz 24,((3+10)*4)(3) \n"
"	lfd 24,((22+10*2)*4)(3) \n"
"	lwz 25,((3+11)*4)(3) \n"
"	lfd 25,((22+11*2)*4)(3) \n"
"	mtcrf 0xFF,0 \n"
"	lwz 26,((3+12)*4)(3) \n"
"	lfd 26,((22+12*2)*4)(3) \n"
"	lwz 27,((3+13)*4)(3) \n"
"	lfd 27,((22+13*2)*4)(3) \n"
"	lwz 28,((3+14)*4)(3) \n"
"	lfd 28,((22+14*2)*4)(3) \n"
"	lwz 29,((3+15)*4)(3) \n"
"	lfd 29,((22+15*2)*4)(3) \n"
"	lwz 30,((3+16)*4)(3) \n"
"	lfd 30,((22+16*2)*4)(3) \n"
"	lwz 31,((3+17)*4)(3) \n"
"	lfd 31,((22+17*2)*4)(3) \n"
"	mr  3,4 \n"
"	blr \n"
);

asm(
"	.text \n"
"	.globl kaffe_setjmp \n"
"kaffe_setjmp: \n"
" \n"
"	stw  1,(0*4)(3) \n"
"	mflr 0 \n"
"	stw  2,(1*4)(3) \n"
"	stw  14,((3+0)*4)(3) \n"
"	stfd 14,((22+0*2)*4)(3) \n"
"	stw  0,(2*4)(3) \n"
"	stw  15,((3+1)*4)(3) \n"
"	stfd 15,((22+1*2)*4)(3) \n"
"	mfcr 0 \n"
"	stw  16,((3+2)*4)(3) \n"
"	stfd 16,((22+2*2)*4)(3) \n"
"	stw  0,(21*4)(3) \n"
"	stw  17,((3+3)*4)(3) \n"
"	stfd 17,((22+3*2)*4)(3) \n"
"	stw  18,((3+4)*4)(3) \n"
"	stfd 18,((22+4*2)*4)(3) \n"
"	stw  19,((3+5)*4)(3) \n"
"	stfd 19,((22+5*2)*4)(3) \n"
"	stw  20,((3+6)*4)(3) \n"
"	stfd 20,((22+6*2)*4)(3) \n"
"	stw  21,((3+7)*4)(3) \n"
"	stfd 21,((22+7*2)*4)(3) \n"
"	stw  22,((3+8)*4)(3) \n"
"	stfd 22,((22+8*2)*4)(3) \n"
"	stw  23,((3+9)*4)(3) \n"
"	stfd 23,((22+9*2)*4)(3) \n"
"	stw  24,((3+10)*4)(3) \n"
"	stfd 24,((22+10*2)*4)(3) \n"
"	stw  25,((3+11)*4)(3) \n"
"	stfd 25,((22+11*2)*4)(3) \n"
"	stw  26,((3+12)*4)(3) \n"
"	stfd 26,((22+12*2)*4)(3) \n"
"	stw  27,((3+13)*4)(3) \n"
"	stfd 27,((22+13*2)*4)(3) \n"
"	stw  28,((3+14)*4)(3) \n"
"	stfd 28,((22+14*2)*4)(3) \n"
"	stw  29,((3+15)*4)(3) \n"
"	stfd 29,((22+15*2)*4)(3) \n"
"	stw  30,((3+16)*4)(3) \n"
"	stfd 30,((22+16*2)*4)(3) \n"
"	stw  31,((3+17)*4)(3) \n"
"	stfd 31,((22+17*2)*4)(3) \n"
"	li   3,0 \n"
"	blr \n"
);
#endif

