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

#define getJavaArg(CALL, n)						      \
(   /*									      \
     *  Sign-extend short arguments:					      \
     *									      \
     *  This macro sign extends short (i.e, 32-bit or less) arguments to a    \
     *  64-bit "long" that can be inserted directly into a 64-bit register.   \
     */									      \
									      \
    ((CALL)->callsize[n] == 2) ? (CALL)->args[n].j : (long)((CALL)->args[n].i)\
)

#define	sysdepCallMethod(CALL)						      \
{   /*									      \
     *  Call a Java Method:						      \
     *									      \
     *  This macro generates an in-line call to the Java method described by  \
     *  the given method info structure ("CALL" argument) and stores the      \
     *  return value in the info struct as well.			      \
     */									      \
									      \
    int x, rags = 0;		    					      \
    long lags[MAXMARGS-6];					      	      \
									      \
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
    switch (rags)							      \
    {                                                                         \
	/* The parameter registers ... 					    */\
        register long r16 asm("$16");					      \
        register long r17 asm("$17");					      \
        register long r18 asm("$18");					      \
        register long r19 asm("$19");					      \
        register long r20 asm("$20");					      \
        register long r21 asm("$21");					      \
									      \
        default:							      \
        {   /*								      \
             *  If we have more than 6 arguments, we need to push them onto   \
             *  the stack.  We've already made room for them by allocating    \
             *  the "lags" array, so all we do here is copy them into it.     \
             */								      \
									      \
            register long *lap;						      \
            asm("bis $30,$30,%0" : "=r" (lap));                               \
            for (x = 5; ++x <= rags; *lap++ = getJavaArg(CALL, x));           \
        }								      \
									      \
        /* The first 6 arguments are loaded into the parameter registers.   */\
        case 6: r21 = getJavaArg(CALL, 5);				      \
        case 5: r20 = getJavaArg(CALL, 4);				      \
        case 4: r19 = getJavaArg(CALL, 3);				      \
        case 3: r18 = getJavaArg(CALL, 2);				      \
        case 2: r17 = getJavaArg(CALL, 1);				      \
        case 1: r16 = getJavaArg(CALL, 0);				      \
    }									      \
									      \
    asm("ldq $27,%0" :: "m" ((CALL)->function));			      \
    asm("jsr $26,($27),0");						      \
    asm("ldgp $29,0($26)");						      \
    asm("stq $0,%0" : "=m" ((CALL)->ret->j));				      \
									      \
    /* Store floating point return value over (default) integer value ...   */\
    if ((CALL)->rettype == 'D') asm("stt $f0,%0" : "=m" ((CALL)->ret->d));    \
    if ((CALL)->rettype == 'F') asm("sts $f0,%0" : "=m" ((CALL)->ret->f));    \
}
#endif