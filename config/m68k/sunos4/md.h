/*
 * m68k/sunos4/md.h
 * SunOS4 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <inaba@src.ricoh.co.jp>, 1998;
 * Based on the ports
 *	by Remi Perrot <r_perrot@mail.club-internet.fr> to m68k/linux
 * and
 *	by Alexandre Oliva <oliva@dcc.unicamp.br> to sparc
 *
 */

#ifndef __m68k_sunos4_md_h
#define __m68k_sunos4_md_h

#include "m68k/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET       2

/*
 * Alignment in structure is 2 bytes packed.
 */
#define ALIGNMENT_OF_SIZE(S)    (((S>1)?2:1))

#define	sysdepCallMethod(CALL) do {					\
	int extraargs[(CALL)->nrargs];					\
	register int d0 asm ("d0");					\
	register int d1 asm ("d1");					\
	int *res;							\
	int *args = extraargs;						\
	int argidx;							\
	for(argidx = 0; argidx < (CALL)->nrargs; ++argidx) {		\
	    if ((CALL)->callsize[argidx])				\
		*args++ = (CALL)->args[argidx].i;			\
	    else							\
		*args++ = (CALL)->args[argidx-1].j;			\
	}								\
	asm volatile ("jsr     %2@\naddw    %3,sp\n"			\
	 : "=r" (d0), "=r" (d1)						\
	 : "a" ((CALL)->function),					\
	   "r" ((CALL)->nrargs * sizeof(int))				\
	 : "cc", "memory");						\
	if ((CALL)->retsize != 0) {					\
		res = (int *)(CALL)->ret;				\
		res[1] = d1;						\
		res[0] = d0;						\
	}								\
} while (0)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

struct sockaddr;
extern int rename(const char*, const char*);
extern int remove(const char*);
extern int socket(int, int, int);
extern ssize_t sendto(int, const void*, size_t, int, const struct sockaddr*, int);
extern int setsockopt(int, int, int, const void*, int);
extern int getsockopt(int, int, int, void*, int*);
extern int getsockname(int, struct sockaddr*, int*);
extern int getpeername(int, struct sockaddr*, int*);
extern int select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
extern int vfprintf(FILE *, char *, va_list);

#endif
