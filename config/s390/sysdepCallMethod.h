/*
 * s390/sysdepCallMethod.h (cloned from s390/common.h by ROSSP)
 * Common s390 configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 * Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 * Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __s390_sysdepCallMethod_h
#define __s390_sysdepCallMethod_h

#include "debug.h"
#include "support.h"

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
#if defined(NEED_sysdepCallMethod)
/*
 * The calling convention is as follows:
 *
 * Up to the first five up-to-32-bit integer values are passed in general
 * purpose registers 2 through 6, and the remainder go on the stack.  
 * Types shorter than 32 bits (i.e. byte, boolean, char and short) are 
 * promoted to 32 bits (i.e. to int).
 *
 * 64-bit integer values are intermingled with 32-bit values in the
 * order they appear in the argument list, and are passed in pairs of
 * registers (NOT even/odd pairs, just pairs) or on the stack.  If there 
 * is only one register available and the next integer argument is a
 * 64-bit value, that register is unused and all the remaining integer
 * values are passed on the stack.
 *
 * Floating point values are passed in floating point registers 0 and 2,
 * and then on the stack.
 * 
 * Function results are returned in GPR 2 (up-to-32-bit integer), GPRS 2
 * and 3 (64-bit integer) or FPR 2 (floating point).
 * 
 * Values on the stack are intermingled in the order they appear.
 *
 * Based on implementation in config/arm/common.h, with guidance from
 * <http://ww2.biglobe.ne.jp/~inaba/sysdepCallMethod.html>, 
 * /usr/src/linux/Documentation/Debugging390.txt, and analysis of
 * assorted test programs compiled with gcc 2.95.2+IBM S/390 patches and
 * decompiled with objdump.
 *
 * Ross Patterson <Ross.Patterson@CA.Com>
 * Computer Associates International, Inc.
 */
static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call)
{
 /*
  * argl is placed here to exploit a gcc artifact: the first
  * automatic variable defined is placed at the lowest address on the
  * stack frame.  Thus by judicious fiddling of the stack pointer,
  * argl becomes the argument list to the callee function.  This
  * is explained nicely at the URL above.
  */
  int argl[(call)->nrargs];                /* outgoing args on stack */
  union {                                             /* fp reg args */
      float f;
      double d;
        } fpr_args[2];
  int gpr_args[5] = {0, 0, 0, 0, 0};                  /* gp reg args */
    {
      int *out_args = argl;                /* outgoing args on stack */
      int argidx = {0};                           /* input arg index */
      int fpr_argc = {0};                        /* fp reg arg count */
      int gpr_argc = {0};                        /* gp reg arg count */
      for(; argidx < (call)->nrargs; ++argidx) {
         DBG(SYSDEPCALLMETHOD,
             printf("sysdepCallMethod: arg[%2d]=%c/%d  V=%8x %8x\n",
                    argidx, (call)->calltype[argidx],
                    (call)->callsize[argidx],
                    (&(call)->args[argidx].i)[0],
                    (&(call)->args[argidx].i)[1]);
            );
         if ((call)->callsize[argidx] != 0) { /* if non-empty, copy: */
           switch ((call)->calltype[argidx]) {
           case 'B':                          /* 8-bit byte          */
           case 'Z':                          /* 8-bit boolean       */
           case 'C':                          /* 16-bit char         */
           case 'S':                          /* 16-bit int          */
           case 'I':                          /* 32-bit int          */
           case 'L':                          /* 32-bit objectref    */
             if (gpr_argc < 5)       /* if any gp regs left, use one */
                gpr_args[gpr_argc++] = (call)->args[argidx].i;
             else          /* otherwise, put it on the outgoing list */
               *out_args++ = (call)->args[argidx].i;
             break;
           case 'J':                          /* 64-bit int          */
             if (gpr_argc < 4) {    /* if two gp regs left, use them */
                gpr_args[gpr_argc++] = (&(call)->args[argidx].i)[0];
                gpr_args[gpr_argc++] = (&(call)->args[argidx].i)[1];
             }
             else {        /* otherwise, put it on the outgoing list */
               *out_args++ = (&(call)->args[argidx].i)[0];
               *out_args++ = (&(call)->args[argidx].i)[1];
               if (gpr_argc == 4)
                  gpr_argc++;        /* do not use last gpr for args */
             }
             break;
           case 'F':                          /* 32-bit float        */
             if (fpr_argc < 2)       /* if any fp regs left, use one */
                fpr_args[fpr_argc++].f = (call)->args[argidx].f;
             else          /* otherwise, put it on the outgoing list */
               *out_args++ = (call)->args[argidx].i;
             break;
           case 'D':                          /* 64-bit float        */
             if (fpr_argc < 2)       /* if any fp regs left, use one */
                fpr_args[fpr_argc++].d = (call)->args[argidx].d;
             else {        /* otherwise, put it on the outgoing list */
               *out_args++ = (&(call)->args[argidx].i)[0];
               *out_args++ = (&(call)->args[argidx].i)[1];
             }
             break;
           default:
             printf("sysdepCallMethod: unknown arg[%d] type %c\n",
                argidx, ((call)->calltype[argidx]));
           } /* switch ((call)->calltype[argidx]) */
         } /* if ((call)->callsize[argidx] != 0) */
      } /* for(; argidx < (call)->nrargs; ++argidx) */
    }
    asm (" \n"
"     ld    0,0(,%2)                        # Load fpr args         \n"
"     ld    2,8(,%2)                        #    into regs 0-2.     \n"
"     lm    2,6,0(%1)                       # Load gpr args into 2-6\n"
"     basr 14,%0                            # Call the routine.     \n"
"     stm   2,3,0(%1)                       # Save int result.      \n"
"     std   0,0(,%2)                        # Save float result.    \n"
"     "
        :                                     /* sets these          */
        : "ra" ((call)->function),            /* uses these          */
          "ra" (gpr_args),
          "ra" (fpr_args)
        : "cc",                               /* clobbers these      */
          "0", "1", "2", "3", "4", "5", "6", "7",
                                        "14",
          "16", "17", "18", "19",       "21",       "23",
          "24", "25", "26", "27", "28", "29", "30", "31",
          "memory"              /* just gpr_args[0..1] & fpr_args[0] */
   );
        DBG(SYSDEPCALLMETHOD,
            printf("sysdepCallMethod: rettype=%c/%d R2=%8x R3=%8x F0=%8x %8x\n",
                   (call)->rettype, (call)->retsize, gpr_args[0],
                   gpr_args[1], ((int *)&fpr_args)[0],
                   ((int *)&fpr_args)[1]);
        );
    switch ((call)->rettype) {                /* what kind of retval?*/
    case 'D':                                 /* 64-bit float        */
       (call)->ret->d = fpr_args[0].d;
       break;
    case 'F':                                 /* 32-bit float        */
       (call)->ret->f = fpr_args[0].f;
       break;
    case 'J':                                 /* 64-bit int          */
       (&(call)->ret->i)[1] = gpr_args[1];
    default:                                  /* all shorter types   */
       (call)->ret->i = gpr_args[0];
    } /* switch ((call)->rettype) */
}
#endif /* defined(NEED_sysdepCallMethod) */

#endif /* __s390_sysdepCallMethod_h */
