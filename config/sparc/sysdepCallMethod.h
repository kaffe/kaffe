/*
 * sparc/sysdepCallMethod.h
 * Dynamically build function calls.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

/* Needed for callMethodInfo declaration */
#include "support.h"

/* This file is supposed to be included twice, once with LONG_SYSDEP
   defined, once without it.  The first version cannot be inlined by
   gcc/egcs at least up to 1.2, because it contains variable-sized
   arrays, but it is only used for functions with many arguments.  For
   functions with few arguments, the second, inlinable version is
   used.  */

/* On other platforms, egcs 1.1.* fail to compile the
   sysdepCallMethod, but older releases of gcc, as well as egcs 1.0
   and pre-1.2 snapshots, compile it successfully.  So, with egcs
   1.1.*, we have to fallback to a single, non-inlinable version of
   sysdepCallMethod.  :-( */
#ifndef SYSDEP_ONE_VERSION
#if __GNUC__ == 2 && __GNUC_MINOR__ == 91 && 0 /* Not needed on sparc */
# define SYSDEP_ONE_VERSION 1
# undef LONG_SYSDEP
#endif

/* ARG_TYPE is the type of a register used for passing arguments.  */
#define ARG_TYPE int
/* ARG_TYPES is a parameter list declaration for a function type that
   takes all possible arguments in registers.  */
#define ARG_TYPES ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE
/* ARG_LIST is the argument list for such a function.  */
#define ARG_LIST a0, a1, a2, a3, a4, a5
/* ARG_COUNT is the number of registers for passing arguments.  */
#define ARG_COUNT 6
/* AG_DISPLACEMENT is the offset between the beginning of a
   variable-sized array, allocated in the stack, and the position of
   the first argument that can't be passed in a register.  */
#define ARG_DISPLACEMENT 1
/* ARG_REG a case label and a statement that arranges for one argument
   to be passed. */
#define ARG_REG(n) case n+1: a##n = callargs[n].i

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
#ifndef LONG_SYSDEP
/* In a perfect world, this function would always be inlined, and we
   could declare it as extern inline.  However, since inlining isn't
   performed without optimization, we'd be unable to compile it
   without optimization, for debugging.  */
static inline void
sysdepCallMethod(callMethodInfo *call) {
  unsigned args = call->nrargs;
  void *func = call->function;
  jvalue *callargs;
#ifdef SYSDEP_ONE_VERSION
  ARG_TYPE extraargs[((args>ARG_COUNT)?(args-ARG_COUNT+ARG_DISPLACEMENT):0)];
#endif

  if (args == 0)
    goto noargs;

  callargs = call->args;
#else
/* This version cannot be inlined, because it contains variable-sized
   arrays.  It should only be called by the first version, when it
   finds there are too many arguments to be passed, so that some must
   be passed on the stack.  Although it cannot be inlined, it is
   declared as such, because some day gcc may be able to inline such
   functions.  Furthermore, declaring it inline means it will only be
   emitted if it is actually used.  */
static inline void
longSysdepCallMethod(callMethodInfo *call,
		     unsigned args, void *func, jvalue *callargs) {
  ARG_TYPE extraargs[args-ARG_COUNT+ARG_DISPLACEMENT];

  goto manyargs;
#endif

  switch(args) {
    register ARG_TYPE a0 asm ("o0");
    register ARG_TYPE a1 asm ("o1");
    register ARG_TYPE a2 asm ("o2");
    register ARG_TYPE a3 asm ("o3");
    register ARG_TYPE a4 asm ("o4");
    register ARG_TYPE a5 asm ("o5");
    
  default:
#if ! defined(LONG_SYSDEP) && ! defined(SYSDEP_ONE_VERSION)
    return longSysdepCallMethod(call, args, func, callargs);
#else
  manyargs:
    {
      jvalue *last = &callargs[args];
      ARG_TYPE *xargs = extraargs-ARG_DISPLACEMENT + args-ARG_COUNT;
      while (last != &callargs[ARG_COUNT])
	*--xargs = (--last)->i;
    }
#endif

    ARG_REG(5);
    ARG_REG(4);
    ARG_REG(3);
    ARG_REG(2);
    ARG_REG(1);
    ARG_REG(0);

  case 0:
#ifndef LONG_SYSDEP
  noargs:
#endif
    switch(call->retsize) {
    case 0:
      /* Must be void.  */
      ((void (*)(ARG_TYPES))(func))(ARG_LIST);
      break;

    case 1:
      if (call->rettype == 'F')
	call->ret->f = ((jfloat (*)(ARG_TYPES))(func))(ARG_LIST);
      else
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
#undef ARG_COUNT
#undef ARG_DISPLACEMENT
#undef ARG_REG

#endif /* SYSDEP_ONE_VERSION */
