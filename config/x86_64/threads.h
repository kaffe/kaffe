/*
 * x86_64/threads.h
 * X86-64 threading information.
 *
 * Copyright (c) 2002
 *	MandrakeSoft.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __x86_64_threads_h
#define __x86_64_threads_h

/*
 * Set a default size for the stack.
 */
#if defined(INTERPRETER)
#define THREADSTACKSIZE         (128 * 1024)
#else
#define THREADSTACKSIZE         (64 * 1024)
#endif

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#define	SP_OFFSET	6
#define FP_OFFSET	1

#endif
