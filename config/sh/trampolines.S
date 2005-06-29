/*
 * sh/trampolines.S
 * Super-H trampolines codes for various occasions.
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
    .align 4
    .global C_FUNC_NAME(sh_do_fixup_trampoline)

C_FUNC_NAME(sh_do_fixup_trampoline) :
	mov.l	r4,@-r15
	mov.l	r5,@-r15
	mov.l	r6,@-r15
	mov.l	r7,@-r15
	mov.l	r1,@-r15
	sts	pr,r4
	mov.l	.L99999,r0
	jsr	@r0
	nop
	lds.l	@r15+,pr
	mov.l	@r15+,r7
	mov.l	@r15+,r6
	mov.l	@r15+,r5
	mov.l	@r15+,r4
	jmp	@r0
	nop
	.align 2 
.L99999:
	.long   C_FUNC_NAME(soft_fixup_trampoline)

#endif
