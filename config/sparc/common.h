/*
 * sparc/common.h
 * Common SPARC configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

#ifndef __sparc_common_h
#define __sparc_common_h

#if NEED_sysdepCallMethod
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
#define ARG_TYPE int
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
#define ARG_DISPLACEMENT -1
  /* ARG_REG a case label and a statement that arranges for one
     argument to be passed. */
#define ARG_REG(n) case n+1: a##n = callargs[n].i

  ARG_TYPE extraargs[((args>ARG_COUNT+ARG_DISPLACEMENT)
		      ?(args-ARG_COUNT-ARG_DISPLACEMENT):0)];
  void *func = call->function;
  jvalue *callargs;

  if (args == 0)
    goto noargs;

  callargs = call->args;

  switch(args) {
    register ARG_TYPE a0 asm ("o0");
    register ARG_TYPE a1 asm ("o1");
    register ARG_TYPE a2 asm ("o2");
    register ARG_TYPE a3 asm ("o3");
    register ARG_TYPE a4 asm ("o4");
    register ARG_TYPE a5 asm ("o5");
    
  default:
    {
      jvalue *last = &callargs[args];
      ARG_TYPE *xargs = extraargs+ARG_DISPLACEMENT + args-ARG_COUNT;
      while (last != &callargs[ARG_COUNT])
	*--xargs = (--last)->i;
    }

    ARG_REG(5);
    ARG_REG(4);
    ARG_REG(3);
    ARG_REG(2);
    ARG_REG(1);
    ARG_REG(0);

  case 0:
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
