/*
 * sparc/callKaffeException.h
 * Common SPARC JIT exception calling.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_callKaffeException_h
#define __sparc_callKaffeException_h

struct Hjava_lang_Throwable;

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(FramePointer, Handler, EObj)  callKaffeException(FramePointer, Handler, EObj)

static inline void callKaffeException(uintp fp, 
				      uintp handler, 
				      struct Hjava_lang_Throwable* eobj) {
	register int o1 asm("o1"), o2 asm("o2"), o3 asm("o3");
	asm volatile(
		     "		ta 3 \n"
		     "		sub %%sp,64,%%sp \n"
		     "		mov %2,%%fp \n"
		     "		jmpl %1,%%g0 \n"
		     "		restore	%0,0,%%o0 \n"
		     : : "r" (o1=(int)(eobj)), "r" (o2=(int)(handler)), "r" (o3=(int)(fp)));
        asm volatile("" : : "r"(o1), "r"(o2), "r"(o3));
}

#endif /* __sparc_callKaffeException_h */
