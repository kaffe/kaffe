/*
 * i386/sysdepCallMethod.h
 * Common i386 sysdepCallMethod.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_sysdepCallMethod_h
#define __i386_sysdepCallMethod_h

/* Needed for callMethodInfo declaration */
#include "support.h"

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
#if defined(NEED_sysdepCallMethod)
/* ALWAYS_INLINE attribute can only be added to the declaration, not the 
 * definition of a function, so we need to declare it separately.
 */
static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call) {
  unsigned args = call->nrargs;

  /* Push all arguments into the stack, in last-to-first order.  This
     assumes that the second 32bit-word of longs and doubles is
     available as an additional int argument, as callMethod[AV]() do.  */
  while (args > 0)
    asm volatile ("pushl %0" : : "m" (call->args[--args].i) : "sp");

  switch(call->retsize) {
  case 0:
    /* Must be void.  */
    ((void (*)(void))(call->function))();
    break;

  case 1:
    if (call->rettype == 'F')
      call->ret->f = ((jfloat (*)(void))(call->function))();
    else /* Must be 32-bit or smaller int.  */
      call->ret->i = ((jint (*)(void))(call->function))();
    break;

  default:
    /* It could've been `case 2;', but then we'd get an additional cmp
     * that we don't really need.  */
    if (call->rettype == 'D')
      call->ret->d = ((jdouble (*)(void))(call->function))();
    else /* Must be jlong.  */
      call->ret->j = ((jlong (*)(void))(call->function))();
    break;
  }

  /* Adjust the stack pointer.  */
  asm volatile ("addl %0,%%esp" : :
		"r" (call->argsize * sizeof(jint)) : "cc", "sp");
}
#endif /* NEED_sysdepCallMethod */

#endif /* __i386_sysdepCallMethod_h */
