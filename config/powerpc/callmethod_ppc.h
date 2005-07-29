/*
 * sysdepCallMethod_ppc(CALL) implementation for linuxppc
 *
 * Copyright (c) 1998
 *	Kevin B. Hendricks  All rights reserved.
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * distributed under the GPL license
 */

/*
   This function translates the "Kaffe" calling convention into the "C"
   calling convention used in native methods. See the files
   common.h, and support.h for information as to the input parameters
*/

 

// #define dbg_mode 1

#if 0
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jni.h"
#include <stdarg.h>
#include "classMethod.h"
#include "jtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "baseClasses.h"
#include "lookup.h"
#include "errors.h"
#include "exception.h"
#include "slots.h"
#include "machine.h"
#include "support.h"
#include "md.h"
#include "itypes.h"
#include "external.h"
#include "thread.h"
#include "locks.h"
#endif

void sysdepCallMethod_ppc(
     callMethodInfo* CALL)  /* pointer to Kaffe callMethodInfo */
{

  /* Because of the Power PC calling conventions we could be passing
     parameters in both register types and on the stack. To create the
     stack parameter area we need we now simply allocate local
     variable storage param[ssiz] that is the size of the java stack 
     (more than enough space) which we can overwrite the parameters into.

     Note: This keeps us from having problems with later local variables.

     Note: could require up to  2*argsize words of parameter stack area 
     if the call has many float parameters (i.e. floats take up only 1 
     word on the kaffe args but take 2 words in parameter area in the 
     stack frame .

     The basic idea here is to use gpr[8] as a storage area for
     the future values of registers r3 to r10 needed for the call, 
     and similarly fpr[8] as a storage area for the future values
     of floating point registers f1 to f8, and param to set aside
     possible stack area needed by the call
  */

     unsigned long gpr[8];    /* storage for gpregisters, map to r3-r10 */
     double fpr[8];           /* storage for fpregisters, map to f1-f8 */
     int n;                   /* number of gprs mapped so far */
     int f;                   /* number of fprs mapped so far */
     long *p;                 /* pointer to parameter overflow area */
     int c;                   /* calltype being processes */
     int argidx;              /* pointer into kaffe args array */
     double dret;             /* temporary function return values*/
     long iret, iret2;
     unsigned long param[(2*((CALL)->nrargs)+1)];

     /* now begin to load the C function arguments into storage */
     n = 0;
     f = 0;
     argidx = 0;
     
     /* set up a pointer to the stack parameter area */
     __asm__ __volatile__ ( "addi %0,r1,8" : "=r" (p) : /* no inputs */ );

     /* now loop through each argument and assign it properly to
        either gpr, fpr, or the param area */

#if defined(dbg_mode)
     fprintf(stdout,"called with %d args\n",(CALL)->nrargs);
     fprintf(stdout, "return type %c size %d\n",(CALL)->rettype,
                 (CALL)->retsize);
     while (argidx < (CALL)->nrargs) {
         fprintf(stdout, "arg %d: value: %x size %d type %c\n",
                argidx, (CALL)->args[argidx].l,(CALL)->callsize[argidx],
                (CALL)->calltype[argidx]);
         argidx++;
     }
     fflush(stdout);
     argidx = 0;
#endif

      while (argidx < (CALL)->nrargs) {
          c = (int)((CALL)->calltype[argidx]);

       switch (c) {

       case 'D':                   /* type is double */
            if (f < 8) {
               fpr[f++] = (CALL)->args[argidx].d;
	    } else {
	       if (((long) p) & 4)          
	          p++;
              /* into parameter stack */
               *((double *)p) = (CALL)->args[argidx].d; 
               p+=2;
	    }
            break;

       case 'F':                   /* type is float */
	   /* floats are store as 32 bit word on Kaffe and passed as
	      float in parameter stack to C as native method have prototype.
	      Without prototype, float will be cast to double but this is
	      not the case here.  */
	   if (f < 8) {
	       fpr[f++] = (double) (CALL)->args[argidx].f;
	   } else {
	       /* into parameter stack */
	       *((float *)p) = (CALL)->args[argidx].f;
	       p++;
	   }
	   break;

       case 'J':                /* type is long long */
            if (n & 1) n++; 	/* note even elements gpr[] will map to odd registers*/
            if (n <= 6) {
               *((long long *) (&gpr[n])) = (CALL)->args[argidx].j;
               n+=2;
	    } else {
	       if (((long) p) & 4)          
	          p++;
               *((long long *)p) = (CALL)->args[argidx].j; 
               p+=2;
	    }
            break;

       case 'L':		/* type is jref (void*)   */
     	                /* fall through and handle in gpr*/
       
       default:
            if (n < 8) {
               gpr[n++] = (unsigned long) (CALL)->args[argidx].i;
	    } else {
	      /* place on the stack */
               *((unsigned long *)p) = (CALL)->args[argidx].i; 
               p++; 
	    }
            break;
       }

       if (((int)((CALL)->callsize[argidx])) == 2) argidx++;
       argidx++;
 }						


/* Set up the machine registers and invoke the function */

    __asm__ __volatile__ (
	"
		lwz	r3,	0(%3)
		lwz	r4,	4(%3)
		lwz	r5,	8(%3)
		lwz	r6,	12(%3)
		lwz	r7,	16(%3)
		lwz	r8,	20(%3)
		lwz	r9,	24(%3)
		lwz	r10,	28(%3)
		lfd	f1,	0(%4)
		lfd	f2,	8(%4)
		lfd	f3,	16(%4)
		lfd	f4,	24(%4)
		lfd	f5,	32(%4)
		lfd	f6,	40(%4)
		lfd	f7,	48(%4)
		lfd	f8,	56(%4)
		mtctr	%5
		bctrl
		mr	%1,	r3
		mr	%2,	r4
		fmr	%0,	f1
	" : "=f" (dret), "=r" (iret), "=r" (iret2)
	  : "r" (gpr), "r" (fpr), "r" (CALL->function)
	  : "0", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"
    );


    /* Put the return value in the right place. */
    c = (int) (CALL)->rettype;

    switch (c) {
        case 0 :
	  /* fall through */
	case 'V':
	    break;
	case 'D':
	    * (double *) (CALL)->ret = dret;
	    break;
	case 'F':
	    * (float *) (CALL)->ret = (float) dret;
	    break;
	case 'J':
	    * (unsigned long *) (CALL)->ret = iret;
            * (((unsigned long *) (CALL)->ret) + 1) = iret2;
	    break;
	default:
	    * (unsigned long *)(CALL)->ret = iret;
	    break;
    }
}

