/*
 * m68k/callKaffeException.h
 * Common M68000 JIT exception calling.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Modified by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1999;
 *	to support the trampoline mechanism.
 */

#ifndef __m68k_callKaffeException_h
#define __m68k_callKaffeException_h

struct Hjava_lang_Throwable;

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(FramePointer, Handler, EObj)  callKaffeException(FramePointer, Handler, EObj)

static inline void callKaffeException(uintp fp, 
				      uintp handler, 
				      struct Hjava_lang_Throwable* eobj) {
	__asm__ __volatile__("move%.l %1,%/d0\n\t"
			     "move%.l %0,%/a6\n\t"
			     "jmp %2@"
			     : : "g"(fp), "g"(eobj), "a"(handler)
			     : "d0", "cc", "memory");
}



#endif /* __m68k_callKaffeException_h */
