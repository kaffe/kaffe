/*
 * x86_64/sysdepCallMethod.h
 * Dynamically build function calls using X86-64 SVR4 ABI.
 *
 * Copyright (c) 2002
 *	MandrakeSoft.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

/* Needed for callMethodInfo declaration */
#include "support.h"

/* ARG_TYPE is the type of a register used for passing arguments.  */
#define ARG_TYPE        long

/* ARG_TYPES is a parameter list declaration for a function type
   that takes all possible arguments in registers.  */
#define ARG_TYPES       ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE

/* ARG_LIST is the argument list for such a function.  */
#define ARG_LIST        a0, a1, a2, a3, a4, a5

/* GPR_ARGS is the number of GPR (integer) registers for passing
   arguments.  */
#define GPR_ARGS        6

/* FPR_ARGS is the number of FPR (float) registers for passing
   arguments.  */
#define FPR_ARGS        8

/* ARG_GPR a case label and a statement that arranges for one integer
   argument to be passed. */
#define ARG_GPR(N) \
    case N+1: a##N = gpr[N];

/* ARG_FPR a case label and a statement that arranges for one float
   argument to be passed. */
#define ARG_FPR(N) \
    case N+1: d##N = fpr[N];


/* Make a call to a native or Java (JIT) method.  This assembly code should
   build a standard C call using the passed call information.  By its
   nature this is highly processor specific.  This function is mandatory
   for both JIT and Interpreter (since stubs have now been deprecated).  */
static inline void sysdepCallMethod(callMethodInfo* call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo* call)
{
    void *func = call->function;
    jvalue *callargs = call->args;
    char *calltype = call->calltype;
    jvalue *args = callargs;
    jvalue *last = &callargs[call->nrargs];
    int nr_gpr = 0;
    int nr_fpr = 0;
    int nr_stack = 0;

    unsigned long *stack;
    unsigned long gpr[GPR_ARGS];
    double fpr[FPR_ARGS];

    /* Compute gpr[], fpr[] and stack[] arrays' size */
    while (args != last) {
      switch (calltype[(args++) - callargs]) {
      case 'F':
      case 'D':
        if (nr_fpr < FPR_ARGS)
          nr_fpr++;
        else
          nr_stack++;
        break;
        
      default:
        if (nr_gpr < GPR_ARGS)
          nr_gpr++;
        else
          nr_stack++;
        break;
      }
    }

    /* Allocate all arrays with one big alloca() */
    {
      int nr = nr_stack;
      
      /* stack, if used, must be 16 bytes aligned */
      if (nr_stack)
        nr = (nr + 1) & ~1;

      /* stack[] is in called parameters area.  */
      stack = __builtin_alloca (8 * nr);
    }

    /* build gpr[], fpr[] and stack[] arrays */
    nr_gpr = 0;
    nr_fpr = 0;
    nr_stack = 0;
    
    args = callargs;
    while (args != last) {
      switch (calltype[args - callargs]) {
      case 'F': {
        if (nr_fpr < FPR_ARGS)
          /* The value in %xmm register is already prepared to be
             retrieved as a float. Therefore, we pass the float value
             verbatim, as a double without any conversion.  */
          fpr[nr_fpr++] = args->d;
        else {
          *(float *)stack = args->f;
          stack++;
        }
        break;
      }
      
      case 'D': {
        if (nr_fpr < FPR_ARGS)
          fpr[nr_fpr++] = args->d;
        else {
          *(double *)stack = args->d;
          stack++;
        }
        break;
      }
      
      default:
        if (nr_gpr < GPR_ARGS)
          gpr[nr_gpr++] = args->j;
        else
          *stack++ = args->j;
      }
      args++;
    }

    {
      register ARG_TYPE a0 asm("rdi");
      register ARG_TYPE a1 asm("rsi");
      register ARG_TYPE a2 asm("rdx");
      register ARG_TYPE a3 asm("rcx");
      register ARG_TYPE a4 asm("r8");
      register ARG_TYPE a5 asm("r9");
      
      register double d0 asm("xmm0");
      register double d1 asm("xmm1");
      register double d2 asm("xmm2");
      register double d3 asm("xmm3");
      register double d4 asm("xmm4");
      register double d5 asm("xmm5");
      register double d6 asm("xmm6");
      register double d7 asm("xmm7");
      
      /* load FPR registers from fpr[] */
      switch (nr_fpr) {
        ARG_FPR(7);
        ARG_FPR(6);
        ARG_FPR(5);
        ARG_FPR(4);
        ARG_FPR(3);
        ARG_FPR(2);
        ARG_FPR(1);
        ARG_FPR(0);
      case 0:;
      }

      /* load GPR registers from gpr[] */
      switch (nr_gpr) {
        ARG_GPR(5);
        ARG_GPR(4);
        ARG_GPR(3);
        ARG_GPR(2);
        ARG_GPR(1);
        ARG_GPR(0);
      case 0:;
      }
      
      /* Ensure that the assignments to SSE registers won't be optimized away. */
      asm ("" ::
           "x" (d0), "x" (d1), "x" (d2), "x" (d3),
           "x" (d4), "x" (d5), "x" (d6), "x" (d7));

      switch(call->retsize) {
      case 0:
        /* Must be void.  */
        ((void (*)(ARG_TYPES))(func))(ARG_LIST);
        break;
        
      case 1:
        if (call->rettype == 'F')
          call->ret->f = ((jfloat (*)(ARG_TYPES))(func))(ARG_LIST);
        else /* Must be 32-bit or smaller int.  */
          call->ret->i = ((jint (*)(ARG_TYPES))(func))(ARG_LIST);
        break;

      default:
        /* It could've been `case 2;', but then we'd get an additional cmp
         * that we don't really need.  */
        if (call->rettype == 'D')
          call->ret->d = ((jdouble (*)(ARG_TYPES))(func))(ARG_LIST);
        else /* Must be jlong.  */
          call->ret->j = ((jlong (*)(ARG_TYPES))(func))(ARG_LIST);
        break;
      }
    }
}

#undef ARG_TYPE
#undef ARG_TYPES
#undef ARG_LIST
#undef GPR_ARGS
#undef FPR_ARGS
#undef ARG_GPR
#undef ARG_FPR
