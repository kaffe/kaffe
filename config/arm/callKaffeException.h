/*
 * arm/callKaffeException.h
 * Common ARM JIT exception calling.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm_callKaffeException_h
#define __arm_callKaffeException_h

struct Hjava_lang_Throwable;

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(FramePointer, Handler, EObj)  callKaffeException(FramePointer, Handler, EObj)

static inline void callKaffeException(uintp fp, 
				      uintp handler, 
				      struct Hjava_lang_Throwable* eobj) {
	asm volatile ("	\n"
		      "	mov r0, %0 \n"
		      "	mov fp, %2 \n"
		      "	mov pc, %1 \n"
		      : : "r" (eobj), "r" (handler), "r" (fp) : "r0");
}

#endif /* __arm_callKaffeException_h */
