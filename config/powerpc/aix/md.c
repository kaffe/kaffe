/*
 * powerpc/aix/md.c
 * AIX powerpc specific functions.
 *
 * Copyright (c) 1996 Sandpiper Software Consulting, LLC
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Rick Greer <rick@sandpiper.com>, 1996.
 * Used by Brad Harvey <b_harvey@hitachi.com>, 1997 for AIX port.
 */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
int __aix_cmpxchg(void **A, void *O, void *N) {
	int tmp, ret;

	asm volatile(
	"	li	%1,0\n"
	"L..cax1:	lwarx	%0,0,%3\n"
	"	cmpw	0,%0,%4\n"
	"	bne	L..cax2\n"
	"	stwcx.	%5,0,%3\n"
	"	bne-	L..cax1\n"
	"	sync\n"
	"	li	%1,1\n"
	"L..cax2:\n"
	: "=&r"(tmp), "=&r"(ret), "=m"(*(A))
	: "r"(A), "r"(O), "r"(N), "m"(*(A))
	: "cc", "memory");

	return ret;
}
