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


#undef getSingleJavaArg
#define getSingleJavaArg(CALL, n) (CALL) -> args[n].j

/*									      \
*  Sign-extend short arguments:					      \
*									      \
*  This macro sign extends short (i.e, 32-bit or less) arguments to a    \
*  64-bit "long" that can be inserted directly into a 64-bit register.   \
*/									      \
#define getJavaArg(CALL, n, iarg, farg) \
{ \
    if ( (CALL) -> callsize[n] == 1 ) { \
      iarg = (long)   ((CALL) -> args[n].i); \
      farg = (double) ((CALL) ->  args[n].f); \
    } else { \
      iarg = (long)   ((CALL) -> args[n].j); \
      farg = (double) ((CALL) -> args[n].d); \
    } \
}

#define javaArgCase(CALL,_x_,_y_) case _x_: getJavaArg(CALL, (_x_-1), r ## _y_, f ## _y_)

#define	sysdepCallMethod(CALL)						      \
{   /*									      \
     *  Call a Java Method:						      \
     *									      \
     *  This macro generates an in-line call to the Java method described by  \
     *  the given method info structure ("CALL" argument) and stores the      \
     *  return value in the info struct as well.			      \
     */									      \
									      \
    asm ("" ::: "$9", "$10", "$11"); \
    asm ("" ::: "$16", "$17", "$18", "$19", "$20", "$21", "$9", "$27", "$26"); \
    {                                                                         \
    int x, rags = 0;		    					      \
    /* The parameter registers ... 					    */\
    register long r16 asm("$16");					      \
    register long r17 asm("$17");					      \
    register long r18 asm("$18");					      \
    register long r19 asm("$19");					      \
    register long r20 asm("$20");					      \
    register long r21 asm("$21");					      \
									      \
    register double f16 asm("$f16");					      \
    register double f17 asm("$f17");					      \
    register double f18 asm("$f18");					      \
    register double f19 asm("$f19");					      \
    register double f20 asm("$f20");					      \
    register double f21 asm("$f21");					      \
									      \
    register unsigned long *osp asm("$9"); \
    register unsigned long *ogp asm("$10"); \
    /* any temporary register would do.. */  \
    register unsigned long *nsp asm("$11"); \
    register unsigned long* pv asm("$27"); \
    asm ("mov	$30,%0" : "=r" (osp)); \
    asm ("mov	$29,%0" : "=r" (ogp)); \
    nsp = osp;						      \
    pv = (CALL) -> function;				      \
    for (x = 0; x < (CALL)->nrargs; x++)				      \
    {   /*								      \
         *  The alpha has 64-bit registers, so we don't really need the ex-   \
         *  tra "callsize = 0" entries in the arg list.  This loop removes    \
         *  them by sliding any subsequent entries to the left.		      \
         */								      \
									      \
        if ((CALL)->callsize[x] > 0)					      \
        {								      \
            if (x > rags) (CALL)->args[rags] = (CALL)->args[x];		      \
            rags += 1;							      \
        }								      \
    }									      \
									      \
    {                                                                         \
									      \
    switch (rags)							      \
    {                                                                         \
        default:							      \
	  {									\
        /*		                                                              \
         * We're doing to hack the frame buffer the difficult way..               \
         */                                                                       \
        int margs = rags - 6;        \
        nsp = osp - margs;  \
	/* Copy arguments to the stack we allocated */			      \
	  for (x = 0; x < margs; x++) {						\
	    nsp[x] = getSingleJavaArg(CALL, x+6);				\
	  }									\
        }								      \
									      \
        /* The first 6 arguments are loaded into the parameter registers.   */\
        javaArgCase(CALL,6,21); \
        javaArgCase(CALL,5,20); \
        javaArgCase(CALL,4,19); \
        javaArgCase(CALL,3,18); \
        javaArgCase(CALL,2,17); \
        javaArgCase(CALL,1,16); \
        case 0:  /* nothing */ \
    }									      \
    asm volatile ("mov %0,$30" :: "r" (nsp), \
	/* include all the argument registers least they get DCE'd away */   \
	"r" (pv), \
	"r" (r16), "r" (r17), "r" (r18), "r" (r19), "r" (r20), "r" (r21) ); \
    asm volatile ("\
         jsr $26,($27),0 \n\
         ldgp $29,0($26)\n\
	 mov %0, $30" \
	 :: "r" (osp), \
	"f" (f16), "f" (f17), "f" (f18), "f" (f19), "f" (f20), "f" (f21) \
	 /* and mention that we kill these.. */   \
	 : \
		 "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", \
		 "$22", "$23", "$24", "$25", "$27", "$26", \
		 "$f0", "$f1", \
	          /*  we don't modify the FP callee-saves registers */ \
	 	  /* "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", */ \
		 "$f10", "$f11", "$f12", "$f13", "$f14", "$f15", \
                 "$f16", "$f17", "$f18", "$f19",  "$f20", "$f21", \
                 "$f22", "$f23", "$f24", "$f25", "$f26", "$f27", "$f28", "$f29", "$f30", \
		 "memory"); \
    }									      \
    asm ("stq $0,%0" : "=m" ((CALL)->ret->j));				      \
									      \
    /* Store floating point return value over (default) integer value ...   */\
    {									      \
      char rt = (CALL) -> rettype;									      \
      if (rt == 'D') asm volatile ("stt $f0,%0" : "=m" ((CALL)->ret->d));    \
      if (rt == 'F') asm volatile ("sts $f0,%0" : "=m" ((CALL)->ret->f));    \
    }									      \
    }									      \
}
#endif
