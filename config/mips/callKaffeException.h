/*
 * mips/callKaffeException.h
 * Common MIPS JIT exception calling.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Christian Krusel <chrkr@uni-paderborn.de>, 1997.
 * Modified by Michael Chen <mikey@hydra.stanford.edu>, 1998.
 * last changed 2.15.98
 */

#ifndef __mips_callKaffeException_h
#define __mips_callKaffeException_h

struct Hjava_lang_Throwable;

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(FramePointer, Handler, EObj)  callKaffeException(FramePointer, Handler, EObj)

static inline void callKaffeException(uintp fp, 
				      uintp handler, 
				      struct Hjava_lang_Throwable* eobj) {
	asm volatile("\n"
		"	.set push	\n"
		"	.set noreorder	\n"
		"	move $2,%2	\n"
		"	jr %1		\n"
		"	 move $fp,%0	\n"
		"	.set pop	\n"
		: : "r" (fp), "r" (handler), "r" (eobj));
}

#endif /* __mips_callKaffeException_h */
