/*
 * mips/ultrix4/md.c
 * Ultrix4.2 MIPS specific functions.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "kaffe/jtypes.h"
#include "object.h"
#include "support.h"

#define	JB_PC		2
#define JB_REGS		3
#define	JB_S0		(JB_REGS+16)
#define	JB_S1		(JB_REGS+17)
#define	JB_S2		(JB_REGS+18)
#define	JB_S3		(JB_REGS+19)
#define	JB_S4		(JB_REGS+20)
#define	JB_S5		(JB_REGS+21)
#define	JB_S6		(JB_REGS+22)
#define	JB_S7		(JB_REGS+23)
#define JB_GP		(JB_REGS+28)
#define JB_SP		(JB_REGS+29)
#define JB_S8		(JB_REGS+30)
#define JB_FREGS	38
#define	JB_F20		(JB_FREGS+20)
#define	JB_F22		(JB_FREGS+22)
#define	JB_F24		(JB_FREGS+24)
#define	JB_F26		(JB_FREGS+26)
#define	JB_F28		(JB_FREGS+28)
#define	JB_F30		(JB_FREGS+30)
#define	JB_FPC_CSR	(JB_FREGS+32)

void
ultrixlongjmp(int *env, int val)
{
        /* Pull back the floating point callee-saved registers.  */
        asm volatile ("l.d $f20, %0" : : "m" (env[JB_F20]));
        asm volatile ("l.d $f22, %0" : : "m" (env[JB_F22]));
        asm volatile ("l.d $f24, %0" : : "m" (env[JB_F24]));
        asm volatile ("l.d $f26, %0" : : "m" (env[JB_F26]));
        asm volatile ("l.d $f28, %0" : : "m" (env[JB_F28]));
        asm volatile ("l.d $f30, %0" : : "m" (env[JB_F30]));

        /* Restore the stack pointer.  */
        asm volatile ("lw $29, %0" : : "m" (env[JB_SP]));

        /* Get and reconstruct the floating point csr.  */
        asm volatile ("lw $2, %0" : : "m" (env[JB_FPC_CSR]));
        asm volatile ("ctc1 $2, $31");

        /* Get the FP.  */
        asm volatile ("lw $30, %0" : : "m" (env[JB_S8]));

        /* Get the GP. */
        asm volatile ("lw $gp, %0" : : "m" (env[JB_GP]));

        /* Get the callee-saved registers.  */
        asm volatile ("lw $16, %0" : : "m" (env[JB_S0]));
        asm volatile ("lw $17, %0" : : "m" (env[JB_S1]));
        asm volatile ("lw $18, %0" : : "m" (env[JB_S2]));
        asm volatile ("lw $19, %0" : : "m" (env[JB_S3]));
        asm volatile ("lw $20, %0" : : "m" (env[JB_S4]));
        asm volatile ("lw $21, %0" : : "m" (env[JB_S5]));
        asm volatile ("lw $22, %0" : : "m" (env[JB_S6]));
        asm volatile ("lw $23, %0" : : "m" (env[JB_S7]));

        /* Give setjmp 1 if given a 0, or what they gave us if non-zero.  */
        if (val == 0)
                asm volatile ("li $2, 1");
        else
                asm volatile ("move $2, %0" : : "r" (val));

        /* Get the PC.  */
        asm volatile ("lw $31, %0" : : "m" (env[JB_PC]));
        asm volatile ("j $31");
}
