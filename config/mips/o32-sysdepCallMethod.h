/*
 * mips/o32-sysdepCallMethod.h
 * Dynamically build function calls using the O32 ABI.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

/* This file is supposed to be included twice, once with LONG_SYSDEP
   defined, once without it.  The first version cannot be inlined by
   gcc/egcs at least up to 1.2, because it contains variable-sized
   arrays, but it is only used for functions with many arguments.  For
   functions with few arguments, the second, inlinable version is
   used.  */

/* egcs 1.1.* fail to compile the sysdepCallMethod, but older releases
   of gcc, as well as egcs 1.0 and pre-1.2 snapshots, compile it
   successfully.  So, with egcs 1.1.*, we have to fallback to a
   single, non-inlinable version of sysdepCallMethod.  :-( */
#ifndef SYSDEP_ONE_VERSION
#if __GNUC__ == 2 && __GNUC_MINOR__ == 91
# define SYSDEP_ONE_VERSION 1
# undef LONG_SYSDEP
#endif

/* This causes all 64bit arguments to be aligned at even arguments.
   If a jlong or jdouble appears as on odd argument, a place-holder
   will be introduced.  */
#ifndef ALIGN_AT_64bits
# define ALIGN_AT_64bits 1
#endif

  /* ARG_TYPE is the type of a register used for passing arguments.  */
#define ARG_TYPE int
  /* ARG_TYPES is a parameter list declaration for a function type
     that takes all possible arguments in registers.  */
#define ARG_TYPES ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE
  /* ARG_LIST is the argument list for such a function.  */
#define ARG_LIST a0, a1, a2, a3
  /* ARG_COUNT is the number of registers for passing arguments.  */
#define ARG_COUNT 4
  /* AG_DISPLACEMENT is the offset between the beginning of a
     variable-sized array, allocated in the stack, and the position of
     the first argument that can't be passed in a register.  */
#define ARG_DISPLACEMENT 0
 
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
static inline void sysdepCallMethod(callMethodInfo *call) {
  unsigned args = call->nrargs;
  void *func = call->function;
  jvalue *callargs;
  char *calltype;
  char D = 'D', F = 'F';
#ifdef SYSDEP_ONE_VERSION
  ARG_TYPE extraargs[((args>ARG_COUNT)?(args-ARG_COUNT-ARG_DISPLACEMENT):0)];
#endif

  if (args == 0)
    goto noargs;
  
  callargs = call->args;
  calltype = call->calltype;

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
		     unsigned args,
		     void *func,
		     jvalue *callargs,
		     char *calltype) {
  ARG_TYPE extraargs[args-ARG_COUNT-ARG_DISPLACEMENT];
  char D = 'D', F = 'F';

  goto manyargs;
#endif

  switch(args) {
    register ARG_TYPE a0 asm("$4");
    register ARG_TYPE a1 asm("$5");
    register ARG_TYPE a2 asm("$6");
    register ARG_TYPE a3 asm("$7");

    register double d0 asm("$f12");
    register double d2 asm("$f14");

    register float f0 asm("$f12");
    register float f2 asm("$f14");

  default:
#if ! defined(LONG_SYSDEP) && ! defined(SYSDEP_ONE_VERSION)
    return longSysdepCallMethod(call, args, func, callargs, calltype);
#else
  manyargs:
    {
      jvalue *last = &callargs[args];
      ARG_TYPE *xargs = extraargs+ARG_DISPLACEMENT + args-ARG_COUNT;
      while (last != &callargs[ARG_COUNT])
	*--xargs = (--last)->i;
    }
#endif

  case 4:
      if (calltype[0] != D) {
	  a3 = callargs[3].i;
	  goto allint_3;
      }
      
      if (calltype[2] == D) {
	  d2 = callargs[2].d;
	  goto alldouble_2;
      }

      a3 = callargs[3].i;
      goto int_double;

  case 3:
      if (calltype[0] != D) {
      allint_3:
	  a2 = callargs[2].i;
	  goto testfloat_2;
      }
      
      if (calltype[2] == F)
	  f2 = callargs[2].f;
      else {
      int_double:
	  a2 = callargs[2].i;
      }
      goto alldouble_2;

  case 2:
      if (calltype[0] == D) {
      alldouble_2:
	  d0=callargs[0].d;
      } else {
      testfloat_2:
	  if (calltype[0] != F) {
	      a1 = callargs[1].i;
	      goto allint_1;
	  }

	  if (calltype[1] == F)
	      f2=callargs[1].f;
	  else
	      a1=callargs[1].i;
	  goto allfloat_1;
      }

  case 1:
      if (calltype[0] == F) {
      allfloat_1:
	  f0=callargs[0].f;
      } else {
      allint_1:
	  a0=callargs[0].i;
      }

  case 0:
#ifndef LONG_SYSDEP
  noargs:
#endif
    /* Ensure that the assignments to f* registers won't be optimized away. */
    asm ("" :: "f" (f0), "f" (f2), "f" (d0), "f" (d2));

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
#undef ARG_COUNT
#undef ARG_DISPLACEMENT

#endif /* SYSDEP_ONE_VERSION */
