/*
 * mips/threads.h
 * Mips threading information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_threads_h
#define __mips_threads_h

#if defined(HAVE_SGIDEFS_H)
#include <sgidefs.h>
#endif /* defined(HAVE_SGIDEFS_H) */

#if defined(INTERPRETER)
#if !defined(_MIPS_SIM) || (_MIPS_SIM == _MIPS_SIM_ABI32)
#define	THREADSTACKSIZE		(64 * 1024)
#else
#define	THREADSTACKSIZE		(128 * 1024)
#endif /* _MIPS_SIM == _MIPS_SIM_ABI32 */
#else
#define	THREADSTACKSIZE		(64 * 1024)
#endif

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#if !defined(SP_OFFSET)

#if !defined(_MIPS_SIM) || (_MIPS_SIM == _MIPS_SIM_ABI32)
#define SP_OFFSET               1
#define FP_OFFSET		10
#endif

#if (_MIPS_SIM == _MIPS_SIM_ABI64 || _MIPS_SIM == _MIPS_SIM_NABI32)
#define SP_OFFSET               1
#define FP_OFFSET               23
#endif

#endif /* SP_OFFSET */
#endif
