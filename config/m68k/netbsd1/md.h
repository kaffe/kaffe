/*
 * m68k/netbsd/md.h
 * netbsd/m68k specific functions.
 *
 * Copyright (c) 1996 Matthias Hopf <mshopf@informatik.uni-erlangen.de>
 * netbsd/m68k version  Erik Johannessen <erjohann@sn.no>
 *
 * Permission granted for Tim Wilkinson to include this source in his
 * Kaffe system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * SP_OFFSET, ALIGNMENT_OF_SIZE, and sysdepCallMethod macro were
 * written by Kiyo Inaba <inaba@src.ricoh.co.jp>, 1998;
 */

#ifndef __m68k_netbsd_md_h
#define __m68k_netbsd_md_h

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

#define sysdepCallMethod(CALL) do {                                     \
        int extraargs[(CALL)->nrargs];                                  \
        register int d0 asm ("d0");                                     \
        register int d1 asm ("d1");                                     \
        int *res;                                                       \
        int *args = extraargs;                                          \
        int argidx;                                                     \
        for(argidx = 0; argidx < (CALL)->nrargs; ++argidx) {            \
            if ((CALL)->callsize[argidx])                               \
                *args++ = (CALL)->args[argidx].i;                       \
            else                                                        \
                *args++ = (CALL)->args[argidx-1].j;                     \
        }                                                               \
        asm volatile ("jsr     %2@\naddw    %3,sp\n"                    \
         : "=r" (d0), "=r" (d1)                                         \
         : "a" ((CALL)->function),                                      \
           "r" ((CALL)->nrargs * sizeof(int))                           \
         : "cc", "memory");                                             \
        if ((CALL)->retsize != 0) {                                     \
                res = (int *)(CALL)->ret;                               \
                res[1] = d1;                                            \
                res[0] = d0;                                            \
        }                                                               \
} while (0)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
