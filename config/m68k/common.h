/*
 * m68k/common.h
 * Common M68K configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_common_h
#define __m68k_common_h

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
               move.l  4(%1,%0.l*8),-(%%sp)                          \n\
       2:                                                            \n\
               move.l  (%1,%0.l*8),-(%%sp)                           \n\
               jmp     1b                                            \n\
       3:                                                            \n\
               jsr     (%3)                                          \n\
               cmp.b   #0x46,%4                                      \n\
               bne     4f                                            \n\
               fmove.s %%fp0,(%5)                                    \n\
               jmp     6f                                            \n\
       4:                                                            \n\
               cmp.b   #0x44,%4                                      \n\
               bne     5f                                            \n\
               fmove.d %%fp0,(%5)                                    \n\
               jmp     6f                                            \n\
       5:                                                            \n\
               move.l  %%d0,(%5)                                     \n\
               cmp.b   #0x4a,%4                                      \n\
               bne     6f                                            \n\
               move.l  %%d1,4(%5)                                    \n\
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
               add.w %0,%%sp                                         \n\
        " : : "r" ((CALL)->argsize * sizeof(jint)) : "cc")

#endif
