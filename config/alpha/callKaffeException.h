/*
 * alpha/callKaffeException.h
 * Common Alpha JIT configuration information.
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

#ifndef __alpha_callKaffeException_h
#define __alpha_callKaffeException_h

struct Hjava_lang_Throwable;

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(FramePointer, Handler, EObj)  callKaffeException(FramePointer, Handler, EObj)

static inline void callKaffeException(uintp fp, 
				      uintp handler, 
				      struct Hjava_lang_Throwable* eobj) {
	__asm__ __volatile__(
		"mov %0,$15\n\t"
		"mov %1,$0\n\t"
		"mov %2,$27\n\t"
		"jmp (%2)"
		: : "r" (fp), "r" (eobj), "r" (handler)
		: "$15", "$0", "$27");
}

#endif /* __alpha_callKaffeException_h */
