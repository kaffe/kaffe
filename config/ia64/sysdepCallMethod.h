/*
 * ia64/sysdepCallMethod.h
 * Dynamically build function calls using IA-64 SVR4 ABI.
 *
 * Copyright (c) 2001-2003
 *	MandrakeSoft.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#if !defined(IA64_SYSDEP_CALL_METHOD)
#define IA64_SYSDEP_CALL_METHOD

/* Needed for callMethodInfo declaration */
#include "support.h"

/* This define will cause callMethodV and callMethodA to avoid
   introducing unused slots after jlongs and jdoubles.  */
#ifndef NO_HOLES
# define NO_HOLES 1
#endif

/* This define will cause callMethodV and callMethodA to promote every
   integer type to a 64bit word, and every float to double, so that
   every value can be loaded as a single 64bit word.  It also causes
   float arguments to be marked as 'D'.  */
#ifndef PROMOTE_TO_64bits
# define PROMOTE_TO_64bits 1
#endif

#ifndef PROMOTE_jfloat2jdouble
# define PROMOTE_jfloat2jdouble 0
#endif

/* ARG_TYPE is the type of a register used for passing arguments.  */
#define ARG_TYPE	long

/* ARG_TYPES is a parameter list declaration for a function type
   that takes all possible arguments in registers.  */
#define ARG_TYPES	ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE

/* ARG_LIST is the argument list for such a function.  */
#define ARG_LIST	a0, a1, a2, a3, a4, a5, a6, a7

/* GPR_ARGS is the number of GPR (integer) registers for passing
   arguments.  */
#define GPR_ARGS	8

/* FPR_ARGS is the number of FPR (float) registers for passing
   arguments.  */
#define FPR_ARGS	8

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
      if (nr_gpr < GPR_ARGS) {
	nr_fpr++;
	nr_gpr++;
      }
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
    case 'F':
      /* Parameters allocated beyond the eighth parameter slot are
	 never passed in registers, even if floating-point parameter
	 registers remain unused. (IA-64 Software Conventions and
	 Runtime Architecture Guide - 8.5.2) */
       if (nr_gpr < GPR_ARGS) {
	fpr[nr_fpr++] = args->f;
	/* Also pass the FP slot in GPR so that callee knows
	   about it in case it was not prototyped beforehand. */
	gpr[nr_gpr++] = args->j;
      }
      else {
	*(float *)stack = args->f;
	stack++;
      }
      break;

    case 'D':
      /* Parameters allocated beyond the eighth parameter slot are
	 never passed in registers, even if floating-point parameter
	 registers remain unused. (IA-64 Software Conventions and
	 Runtime Architecture Guide - 8.5.2) */
      if (nr_gpr < GPR_ARGS) {
	fpr[nr_fpr++] = args->d;
	/* Also pass the FP slot in GPR so that callee knows
	   about it in case it was not prototyped beforehand. */
	gpr[nr_gpr++] = args->j;
      }
      else {
	*(double *)stack = args->d;
	stack++;
      }
      break;

    default:
      if (nr_gpr < GPR_ARGS)
	gpr[nr_gpr++] = args->j;
      else
	*stack++ = args->j;
      break;
    }
    args++;
  }

  {
    register ARG_TYPE a0 asm("out0");
    register ARG_TYPE a1 asm("out1");
    register ARG_TYPE a2 asm("out2");
    register ARG_TYPE a3 asm("out3");
    register ARG_TYPE a4 asm("out4");
    register ARG_TYPE a5 asm("out5");
    register ARG_TYPE a6 asm("out6");
    register ARG_TYPE a7 asm("out7");

    register double d0 asm("f8");
    register double d1 asm("f9");
    register double d2 asm("f10");
    register double d3 asm("f11");
    register double d4 asm("f12");
    register double d5 asm("f13");
    register double d6 asm("f14");
    register double d7 asm("f15");

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
      ARG_GPR(7);
      ARG_GPR(6);
      ARG_GPR(5);
      ARG_GPR(4);
      ARG_GPR(3);
      ARG_GPR(2);
      ARG_GPR(1);
      ARG_GPR(0);
    case 0:;
    }

    /* Ensure that the assignments to f* registers won't be optimized away. */
    asm ("" ::
	 "f" (d0), "f" (d1), "f" (d2), "f" (d3),
	 "f" (d4), "f" (d5), "f" (d6), "f" (d7));

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
#undef ARG_DISPLACEMENT
#undef ARG_GPR
#undef ARG_FPR

#endif
