/*
 * i386/callKaffeException.h
 * Common i386 JIT configuration information.
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

#ifndef __i386_callKaffeException_h
#define __i386_callKaffeException_h

struct Hjava_lang_Throwable;

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(FramePointer, Handler, EObj)  callKaffeException(FramePointer, Handler, EObj)

static inline void callKaffeException(uintp fp, 
				      uintp handler, 
				      struct Hjava_lang_Throwable* eobj) {
	asm("   movl %2,%%eax \n"
	    "	movl %0,%%ebp \n"
	    "	jmp *%1	\n"
	    : : "g" (fp), "r" (handler), "g" (eobj) : "eax");
}

#endif /* __i386_callKaffeException_h */
