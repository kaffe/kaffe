/*
 * arm32/threads.h
 * arm32 threading information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm32_threads_h
#define __arm32_threads_h

#define	THREADSTACKSIZE		(64 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#if defined(__XSCALE__) || defined(HAVE_XSCALE)
#define	SP_OFFSET		20
#else
#define	SP_OFFSET		23
#endif /* defined(__XSCALE__) || defined(HAVE_XSCALE) */

#endif
