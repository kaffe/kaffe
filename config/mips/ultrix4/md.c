/*
 * mips/ultrix4/md.c
 * Ultrix4.2 MIPS specific functions.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jtypes.h"
#include "object.h"
#include "support.h"

int
sysdepCalcArgSize(callMethodInfo *call)
{
	int i, argsize;

	for (i=0, argsize=0; i<call->nrargs; i++) {
		argsize += call->callsize[i];
		if ((call->callsize[i] == 2) && ((argsize % 2) == 1))
			argsize += 1;
	}

	return(argsize);
}

/*
 * The calling convention is such that the first four 32 bit values are
 * passed in $4-$7, and the remainder goes on the stack.
 * If first one or two values are floating points, they will be in f12, f14.
 * When 8 bytes values are stored into register, they are 8 bytes alligned,
 * and $5 or $7 are not used to store first half of them.
 *
 * Return values are stored in $2, $2 and $3 combination, or f0.
 */

void
sysdepCallMethod(callMethodInfo *CALL)
{
  int stacksize;
  stacksize = sysdepCalcArgSize(CALL)-4;
  if (stacksize < 0) stacksize = 0;
  do {
    int extraargs[stacksize];
    register int r2 asm("$2");
    register int r3 asm("$3");
    register int r4 asm("$4");
    register int r5 asm("$5");
    register int r6 asm("$6");
    register int r7 asm("$7");
    register double d0 asm("$f0");
    register float f0 asm("$f0");
    register double d12 asm("$f12");
    register float f12 asm("$f12");
    register double d14 asm("$f14");
    register float f14 asm("$f14");
    int *res;
    int i, argidx = -4;

    for (i=0; i<(CALL)->nrargs; i++) {
      if (((CALL)->callsize[i] == 2) && ((argidx % 2) == 1))
          argidx += 1;
      switch (argidx) {
      case -4:
        r4 = (CALL)->args[i].i;
	if ((CALL)->callsize[i] == 2)
          r5 = (CALL)->args[i].j;
        break;
      case -3:
        r5 = (CALL)->args[i].i;
        break;
      case -2:
        r6 = (CALL)->args[i].i;
	if ((CALL)->callsize[i] == 2)
          r7 = (CALL)->args[i].j;
        break;
      case -1:
        r7 = (CALL)->args[i].i;
        break;
      default:
        extraargs[argidx] = (CALL)->args[i].i;
	if ((CALL)->callsize[i] == 2)
          extraargs[argidx+1] = (CALL)->args[i].j;
        break;
      }
      argidx += (CALL)->callsize[i];
    }
    if (((CALL)->nrargs >= 1) && ((CALL)->calltype[0] == 'F')) {
      f12 = (CALL)->args[0].f;
      if (((CALL)->nrargs >= 2) && ((CALL)->calltype[1] == 'F'))
        f14 = (CALL)->args[1].f;
    } else if (((CALL)->nrargs >= 2) && ((CALL)->calltype[0] == 'D')) {
      d12 = (CALL)->args[0].d;
      if (((CALL)->nrargs >= 3) && ((CALL)->calltype[2] == 'F'))
        f14 = (CALL)->args[2].f;
      else if (((CALL)->nrargs >= 4) && ((CALL)->calltype[2] == 'D'))
        d14 = (CALL)->args[2].d;
    }
    asm ("move $25, %2\njal $31, $25\n"
          : "=r" (r2), "=r" (r3)
          : "r" ((CALL)->function),
            "r" (r4), "r" (r5), "r" (r6), "r" (r7)
          : "cc"
          );
    if ((CALL)->retsize != 0) {
      res = (int *)(CALL)->ret;
      res[1] = r3;
      res[0] = r2;
      switch((CALL)->rettype) {
      case 'D':
        *(double*)res = d0;
        break;
      case 'F':
        *(float*)res = f0;
        break;
      }
    }
  } while (0);
}

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
