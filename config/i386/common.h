/*
 * i386/common.h
 * Common i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_h
#define __i386_h

/* The 386 never aligns to more than a 4 byte boundary. */
#define	ALIGNMENT_OF_SIZE(S)	((S) < 4 ? (S) : 4)

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
/* Jason <jbaker@cs.utah.edu> notes that we must not use %ebx because it's
 * used for shared lib support on ELF systems (Linux, FreeBSD3.0) and gcc
 * will not save it before it uses it again, despite what we say in asm().
 */
#if NEED_sysdepCallMethod
extern inline void sysdepCallMethod(callMethodInfo *call) {
  int args = call->nrargs;
  unsigned int retsize = call->retsize;
  char rettype = call->rettype;

  while (args > 0)
    asm volatile ("pushl %0" : : "m" (call->args[--args].i) : "sp");

  switch(retsize) {
  case 0:
    ((void (*)(void))(call->function))();
    break;

  case 1:
    if (rettype == 'F')
      call->ret->f = ((jfloat (*)(void))(call->function))();
    else
      call->ret->i = ((jint (*)(void))(call->function))();
    break;

  default:
    if (rettype == 'D')
      call->ret->d = ((jdouble (*)(void))(call->function))();
    else
      call->ret->j = ((jlong (*)(void))(call->function))();
    break;
  }

  asm volatile ("addl %0,%%esp" : :
		"r" (call->argsize * sizeof(jint)) : "cc", "sp");
}
#endif /* NEED_sysdepCallMethod */

#endif
