/*
 * s390/common.h (cloned from s390/common.h by ROSSP)
 * Common s390 configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 * Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __s390_h
#define __s390_h

/* The s390 gcc port aligns to the appropriate boundary: 1->1, 2->2, 4->4,
   and 8->8.  So alignment is pretty simple: */
#define  ALIGNMENT_OF_SIZE(S) (S)

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
#if NEED_sysdepCallMethod
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
#define  sysdepCallMethod(CALL) do {                                    \
 /*                                                                     \
  * argl is placed here to exploit a gcc artifact: the first            \
  * automatic variable defined is placed at the lowest address on the   \
  * stack frame.  Thus by judicious fiddling of the stack pointer,      \
  * argl becomes the argument list to the callee function.  This        \
  * is explained nicely at the URL above.                               \
  */                                                                    \
  int argl[(CALL)->nrargs];                /* outgoing args on stack */ \
  union {                                             /* fp reg args */ \
      float f;                                                          \
      double d;                                                         \
        } fpr_args[2];                                                  \
  int gpr_args[5] = {0, 0, 0, 0, 0};                  /* gp reg args */ \
    {                                                                   \
      int *out_args = argl;                /* outgoing args on stack */ \
      int argidx = {0};                           /* input arg index */ \
      int fpr_argc = {0};                        /* fp reg arg count */ \
      int gpr_argc = {0};                        /* gp reg arg count */ \
      for(; argidx < (CALL)->nrargs; ++argidx) {                        \
         DBG(SYSDEPCALLMETHOD,                                          \
             printf("sysdepCallMethod: arg[%2d]=%c/%d  V=%8x %8x\n",    \
                    argidx, (CALL)->calltype[argidx],                   \
                    (CALL)->callsize[argidx],                           \
                    (&(CALL)->args[argidx].i)[0],                       \
                    (&(CALL)->args[argidx].i)[1]);                      \
            )                                                           \
         if ((CALL)->callsize[argidx] != 0) { /* if non-empty, copy: */ \
           switch ((CALL)->calltype[argidx]) {                          \
           case 'B':                          /* 8-bit byte          */ \
           case 'Z':                          /* 8-bit boolean       */ \
           case 'C':                          /* 16-bit char         */ \
           case 'S':                          /* 16-bit int          */ \
           case 'I':                          /* 32-bit int          */ \
           case 'L':                          /* 32-bit objectref    */ \
             if (gpr_argc < 5)       /* if any gp regs left, use one */ \
                gpr_args[gpr_argc++] = (CALL)->args[argidx].i;          \
             else          /* otherwise, put it on the outgoing list */ \
               *out_args++ = (CALL)->args[argidx].i;                    \
             break;                                                     \
           case 'J':                          /* 64-bit int          */ \
             if (gpr_argc < 4) {    /* if two gp regs left, use them */ \
                gpr_args[gpr_argc++] = (&(CALL)->args[argidx].i)[0];    \
                gpr_args[gpr_argc++] = (&(CALL)->args[argidx].i)[1];    \
             }                                                          \
             else {        /* otherwise, put it on the outgoing list */ \
               *out_args++ = (&(CALL)->args[argidx].i)[0];              \
               *out_args++ = (&(CALL)->args[argidx].i)[1];              \
               if (gpr_argc == 4)                                       \
                  gpr_argc++;        /* do not use last gpr for args */ \
             }                                                          \
             break;                                                     \
           case 'F':                          /* 32-bit float        */ \
             if (fpr_argc < 2)       /* if any fp regs left, use one */ \
                fpr_args[fpr_argc++].f = (CALL)->args[argidx].f;        \
             else          /* otherwise, put it on the outgoing list */ \
               *out_args++ = (CALL)->args[argidx].i;                    \
             break;                                                     \
           case 'D':                          /* 64-bit float        */ \
             if (fpr_argc < 2)       /* if any fp regs left, use one */ \
                fpr_args[fpr_argc++].d = (CALL)->args[argidx].d;        \
             else {        /* otherwise, put it on the outgoing list */ \
               *out_args++ = (&(CALL)->args[argidx].i)[0];              \
               *out_args++ = (&(CALL)->args[argidx].i)[1];              \
             }                                                          \
             break;                                                     \
           default:                                                     \
             printf("sysdepCallMethod: unknown arg[%d] type %c\n",      \
                argidx, ((CALL)->calltype[argidx]));                    \
           } /* switch ((CALL)->calltype[argidx]) */                    \
         } /* if ((CALL)->callsize[argidx] != 0) */                     \
      } /* for(; argidx < (CALL)->nrargs; ++argidx) */                  \
    }                                                                   \
    asm ("                                                            \n\
     ld    0,0(,%2)                           # Load fpr args         \n\
     ld    2,8(,%2)                           #    into regs 0-2.     \n\
     lm    2,6,0(%1)                          # Load gpr args into 2-6\n\
     basr 14,%0                               # Call the routine.     \n\
     stm   2,3,0(%1)                          # Save int result.      \n\
     std   0,0(,%2)                           # Save float result.    \n\
     "                                                                  \
        :                                     /* sets these          */ \
        : "ra" ((CALL)->function),            /* uses these          */ \
          "ra" (gpr_args),                                              \
          "ra" (fpr_args)                                               \
        : "cc",                               /* clobbers these      */ \
          "0", "1", "2", "3", "4", "5", "6", "7",                       \
                                        "14",                           \
          "16", "17", "18", "19",       "21",       "23",               \
          "24", "25", "26", "27", "28", "29", "30", "31",               \
          "memory"              /* just gpr_args[0..1] & fpr_args[0] */ \
   );                                                                   \
        DBG(SYSDEPCALLMETHOD,                                           \
            printf("sysdepCallMethod: rettype=%c/%d R2=%8x R3=%8x F0=%8x %8x\n",\
                   (CALL)->rettype, (CALL)->retsize, gpr_args[0],       \
                   gpr_args[1], ((int *)&fpr_args)[0],                  \
                   ((int *)&fpr_args)[1]);                              \
        )                                                               \
    switch ((CALL)->rettype) {                /* what kind of retval?*/ \
    case 'D':                                 /* 64-bit float        */ \
       (CALL)->ret->d = fpr_args[0].d;                                  \
       break;                                                           \
    case 'F':                                 /* 32-bit float        */ \
       (CALL)->ret->f = fpr_args[0].f;                                  \
       break;                                                           \
    case 'J':                                 /* 64-bit int          */ \
       (&(CALL)->ret->i)[1] = gpr_args[1];                              \
    default:                                  /* all shorter types   */ \
       (CALL)->ret->i = gpr_args[0];                                    \
    } /* switch ((CALL)->rettype) */                                    \
} while (0)
#endif /* NEED_sysdepCallMethod */


#if defined(__GNUC__)
#define KAFFE_PROFILER 1
#endif

#if defined(KAFFE_PROFILER)

/* profiler clicks counter type.  */
typedef int64  profiler_click_t;

/* ReaD the processor Time Stamp Counter.
 * This is a macro to help GCC optimization. 
 * The rdtsc instruction load TSC to edx:eax aka A register.  */
#if 1 /* Not ready yet */
#define profiler_get_clicks(COUNTER)    \
   The s390 port does not have a profiler_get_clicks macro yet!
#else
#define profiler_get_clicks(COUNTER)    \
   asm volatile (".byte 0xf; .byte 0x31" /* "rdtsc" */ : "=A" (COUNTER))
#endif /* if 1 */

#endif

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against  
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)                                     \
   ({                                                                   \
      char ret;                                                         \
      asm volatile("                                                  \n\
         # gcc will invoke us with *A in %1, O in %3 and N in %2      \n\
         cs %3,%2,0(%1) # compare O to A and exchange A and N if      \n\
                        # equal, else load O from A                   \n\
         je 1f          # branch if equal                             \n\
         la %0,0        # compare did not match                       \n\
         j  2f          # skip                                        \n\
      1: la %0,1        # compare matched                             \n\
      2:                                                              \n\
         "                                                              \
        : "=r&" (ret),                                 /* sets these */ \
          "+r" (A)                                                      \
        : "r" (N),                                     /* uses these */ \
          "r" (O)                                                       \
        : "cc" );                                  /* clobbers these */ \
      (ret);                                                            \
   })

#endif
