/*
 * powerpc/callKaffeException.h
 * Common PowerPC JIT exception calling.
 *
 * Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_JANOSVM_LICENSE@
 */

#ifndef __powerpc_callKaffeException_h
#define __powerpc_callKaffeException_h

/*
 * Call a jitted java exception handler.
 */
#if defined(__APPLE__)
#define CALL_KAFFE_EXCEPTION(frame, handler, object) \
	asm volatile ("mr r1, %0\n" \
		      "\tlwz r10, 0(r1)\n" \
		      "\tlmw r14, -72(r10)\n" \
		      "\tmr r3, %1\n" \
		      "\tmtctr %2\n" \
		      "\tbctr\n" \
		      : : "r" (frame), "r" (object), "r" (handler) : "r3")
#else
#define CALL_KAFFE_EXCEPTION(frame, handler, object) \
	asm volatile ("mr 1, %0\n" \
		      "\tlwz 10, 0(1)\n" \
		      "\tlmw 14, -72(10)\n" \
		      "\tmr 3, %1\n" \
		      "\tmtctr %2\n" \
		      "\tbctr\n" \
		      : : "r" (frame), "r" (object), "r" (handler) : "r3")
#endif

#endif
