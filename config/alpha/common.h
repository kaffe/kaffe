/*
 * alpha/common.h
 * Common Alpha configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

#ifndef __alpha_common_h
#define __alpha_common_h

#if NEED_sysdepCallMethod
/* This define will cause callMethodV and callMethodA to promote every
   integer type to a 64bit word, and every float to double, so that
   every value can be loaded as a single 64bit word.  It also causes
   float arguments to be marked as 'D'.  */
#define PROMOTE_TO_64bits 1

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
static inline void sysdepCallMethod(callMethodInfo *call) {
  unsigned args = call->nrargs;

  /* ARG_TYPE is the type of a register used for passing arguments.  */
#define ARG_TYPE long
  /* ARG_TYPES is a parameter list declaration for a function type
     that takes all possible arguments in registers.  */
#define ARG_TYPES ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE, ARG_TYPE
  /* ARG_LIST is the argument list for such a function.  */
#define ARG_LIST a0, a1, a2, a3, a4, a5
  /* ARG_COUNT is the number of registers for passing arguments.  */
#define ARG_COUNT 6
  /* AG_DISPLACEMENT is the offset between the beginning of a
     variable-sized array, allocated in the stack, and the position of
     the first argument that can't be passed in a register.  */
#define ARG_DISPLACEMENT 0
  /* ARG_REG a case label and a statement that arranges for one
     argument to be passed. */
#define ARG_REG(n) \
case n+1: (calltype[n]=='D') ? (f##n = callargs[n].d) : (a##n = callargs[n].j)

  ARG_TYPE extraargs[((args>ARG_COUNT+ARG_DISPLACEMENT)
		      ?(args-ARG_COUNT-ARG_DISPLACEMENT):0)];
  void *func = call->function;
  jvalue *callargs;
  char *calltype;

  if (args == 0)
    goto noargs;
  
  callargs = call->args;
  calltype = call->calltype;

  switch(args) {
    register ARG_TYPE a0 asm("$16");
    register ARG_TYPE a1 asm("$17");
    register ARG_TYPE a2 asm("$18");
    register ARG_TYPE a3 asm("$19");
    register ARG_TYPE a4 asm("$20");
    register ARG_TYPE a5 asm("$21");

    register double f0 asm("$f16");
    register double f1 asm("$f17");
    register double f2 asm("$f18");
    register double f3 asm("$f19");
    register double f4 asm("$f20");
    register double f5 asm("$f21");

  default:
    memcpy(extraargs+ARG_DISPLACEMENT,
	   &callargs[ARG_COUNT],
	   (args-ARG_COUNT)*sizeof(ARG_TYPE));

    ARG_REG(5);
    ARG_REG(4);
    ARG_REG(3);
    ARG_REG(2);
    ARG_REG(1);
    ARG_REG(0);

  case 0:
    /* Ensure that the assignments to f* registers won't be optimized away. */
    asm ("" :: "f" (f0), "f" (f1), "f" (f2), "f" (f3), "f" (f4), "f" (f5) );

  noargs:
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

#undef ARG_TYPE
#undef ARG_TYPES
#undef ARG_LIST
#undef ARG_COUNT
#undef ARG_DISPLACEMENT
#undef ARG_REG
}
#endif /* NEED_sysdepCallMethod */

#endif
