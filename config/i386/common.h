/*
 * i386/common.h
 * Common i386 configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
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
#if NEED_sysdepCallMethod
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


#if defined(__GNUC__)
#define KAFFE_PROFILER 1
#endif

#if defined(KAFFE_PROFILER)

/* profiler clicks counter type.  */
typedef int64	profiler_click_t;

/* ReaD the processor Time Stamp Counter.
 * This is a macro to help GCC optimization.
 * The rdtsc instruction load TSC to edx:eax aka A register.  */
#define profiler_get_clicks(COUNTER)	\
	asm volatile (".byte 0xf; .byte 0x31" /* "rdtsc" */ : "=A" (COUNTER))

#endif

#ifndef KAFFE_PLUS_M_ASM_CONSTRAINT
# if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#  define KAFFE_PLUS_M_ASM_CONSTRAINT "+m"
# else
#  define KAFFE_PLUS_M_ASM_CONSTRAINT "=m"
/* Note: it should really be `+m', to indicate both input and output,
   but gcc 2.7 didn't accept it yet.  As long as we don't assign to
   *(A) ``just before'' COMPARE_AND_EXCHANGE, so that gcc could
   consider the assignment dead, it should be fine.  */
# endif
#endif

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against  
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N) \
	({ \
		char ret; \
		asm volatile(" \
			lock \n\
			cmpxchgl %2,%1 \n\
			sete %0" \
		  : "=q" (ret), KAFFE_PLUS_M_ASM_CONSTRAINT (*(A)) \
		  : "r" (N), "a" (O) : "cc" ); \
		(ret); \
	})

#endif
