/*
 * sparc/trampolines.S
 * Sparc trampolines codes for various occasions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "kaffe/trampoline.h"

#if defined(TRAMPOLINE_FUNCTION)
/*
 * If we have an explit function defined then use that.
 */
TRAMPOLINE_FUNCTION()
 
#else
/*
 * Otherwise we'll try to construct one.
 */

    .text
    .align 8
    .global C_FUNC_NAME(sparc_do_fixup_trampoline)

C_FUNC_NAME(sparc_do_fixup_trampoline) :
	save	%sp,-64,%sp
	ld	[%i7+8],%o0
	ld	[%i7+12],%o1
	call	C_FUNC_NAME(soft_fixup_trampoline)
	mov	%g1,%i7
	jmp	%o0
	restore

#endif
