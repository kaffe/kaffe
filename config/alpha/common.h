/*
 * alpha/common.h
 * Common Alpha configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * 15 July, 1998:
 *      Modified by Rick Greer (rick@sandpiper.com) for use with Alpha Linux
 */

#ifndef __alpha_common_h
#define __alpha_common_h

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for the JIT, but can be avoided for
 *  the interpreter if you don't define NO_KAFFE_STUBS.  However, the
 *  Kaffe stubs will be deprecated at some time in the future and
 *  this function will be needed for JNI support when it comes.
 */
#define	NO_KAFFE_STUBS

/*
 * For each argument, callsize[n] is either '1' or '2', saying
 * how many words are being used.
 */

#define getSingleJavaArg(CALL, n)			      \
(   /*									      \
     *  Sign-extend short arguments:					      \
     *									      \
     *  This macro sign extends short (i.e, 32-bit or less) arguments to a    \
     *  64-bit "long" that can be inserted directly into a 64-bit register.   \
     */									      \
    ((CALL)->callsize[n] == 2) ? (CALL)->args[n].j : (long)((CALL)->args[n].i)\
)

/*									      \
*  Sign-extend short arguments:					      \
*									      \
*  This macro sign extends short (i.e, 32-bit or less) arguments to a    \
*  64-bit "long" that can be inserted directly into a 64-bit register.   \
*/									      \
#define getJavaArg(CALL, n, iarg, farg) do { \
    if ( (CALL) -> callsize[n] == 1 ) { \
      iarg = (long)   ((CALL) -> args[n].i); \
      farg = (double) ((CALL) ->  args[n].f); \
    } else { \
      iarg = (long)   ((CALL) -> args[n].j); \
      farg = (double) ((CALL) -> args[n].d); \
    } } while (0)

#define javaArgCase(CALL,_x_,_y_) case _x_: getJavaArg(CALL, (_x_-1), r ## _y_, f ## _y_)

#define sysdepCallMethod(CALL) { \
  /* \
   *  Call a Java Method: \
   * \
   *  This macro generates an in-line call to the Java method described by \
   *  the given method info structure ("CALL" argument) and stores the \
   *  return value in the info struct as well. \
   */ \
\
  int x, rags = 0; \
  /* The parameter registers ... */ \
  register long r16 asm("$16"); \
  register long r17 asm("$17"); \
  register long r18 asm("$18"); \
  register long r19 asm("$19"); \
  register long r20 asm("$20"); \
  register long r21 asm("$21"); \
\
  register double f16 asm("$f16"); \
  register double f17 asm("$f17"); \
  register double f18 asm("$f18"); \
  register double f19 asm("$f19"); \
  register double f20 asm("$f20"); \
  register double f21 asm("$f21"); \
\
  /* return values */ \
  register double f0d asm("$f0"); \
  register float f0f asm("$f0"); \
  register long v0 asm("$0"); \
\
  for (x = 0; x < (CALL)->nrargs; x++) { \
    /* \
     *  The alpha has 64-bit registers, so we don't really need the ex- \
     *  tra "callsize = 0" entries in the arg list.  This loop removes \
     *  them by sliding any subsequent entries to the left. \
     */ \
\
    if ((CALL)->callsize[x] > 0) \
      { \
	if (x > rags) \
	  (CALL)->args[rags] = (CALL)->args[x]; \
	rags++; \
      } \
  } \
  { \
    int margs = rags - 6; \
    unsigned long xargs[MAX(margs,0)]; \
    switch (rags)	{ \
    default: \
      { \
	/* \
	 * We're doing to hack the frame buffer the difficult way.. \
	 */ \
	/* Copy arguments to the stack we allocated */ \
	for (x=0; rags > 6; --rags) { \
	  xargs[x++] = getSingleJavaArg(CALL, rags); \
	} \
      } \
\
      /* The first 6 arguments are loaded into the parameter registers.   */ \
      javaArgCase(CALL,6,21); \
      javaArgCase(CALL,5,20); \
      javaArgCase(CALL,4,19); \
      javaArgCase(CALL,3,18); \
      javaArgCase(CALL,2,17); \
      javaArgCase(CALL,1,16); \
    case 0:  /* nothing */ \
    } \
\
    asm volatile \
      ("" :: \
       /* include all the argument registers least they get DCE'd away */ \
       "r" (r16), "r" (r17), "r" (r18), "r" (r19), "r" (r20), "r" (r21), \
       "m" (xargs) ); \
    asm volatile \
      ("" :: \
       /* include all the argument registers least they get DCE'd away */ \
       "f" (f16), "f" (f17), "f" (f18), "f" (f19), "f" (f20), "f" (f21) ); \
    /* call the function as if it had no arguments */ \
    ((void(*)())((CALL)->function))(); \
  } \
  /* choose return value */ \
  switch ((CALL)->rettype) { \
  case 'D': (CALL)->ret->d = f0d; break; \
  case 'F': (CALL)->ret->f = f0f; break; \
  default:  (CALL)->ret->j = v0;  break; \
  } \
}
#endif
