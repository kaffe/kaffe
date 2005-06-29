/*
 * alpha/trampolines.S
 * Alpha trampolines codes for various occasions.
 *
 * Copyright (c) 1999, 2000, 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 1999, 2000, 2001
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
     .align 3
     .globl C_FUNC_NAME(alpha_do_fixup_trampoline)
     .ent C_FUNC_NAME(alpha_do_fixup_trampoline)

     C_FUNC_NAME(alpha_do_fixup_trampoline) :


     .frame	$30,14*8,$26,0
     .mask	0x04000000,-14*8
     ldgp	$29,0($27)
     lda        $30,-14*8($30)		# reserve 14 on stack

# Save frame registers
     stq	$26,0*8($30)		# ra
     .prologue 1

# Save register arguments as local variables
     stq	$16,1*8($30)		# a0
     stq	$17,2*8($30)		# a1
     stq	$18,3*8($30)		# a2
     stq	$19,4*8($30)		# a3
     stq	$20,5*8($30)		# a4
     stq	$21,6*8($30)		# a5
     stt	$f16,7*8($30)
     stt	$f17,8*8($30)
     stt	$f18,9*8($30)
     stt	$f19,10*8($30)
     stt	$f20,11*8($30)
     stt	$f21,12*8($30)

# Compute the address of methodTrampoline.method
     lda	$16,8($1)
# Call C to do the fixup
     jsr	$26, C_FUNC_NAME(soft_fixup_trampoline)
     ldgp	$29,0($26)
     mov	$0,$27

# Reload register arguments
     ldq	$16,1*8($30)		# a0
     ldq	$17,2*8($30)		# a1
     ldq	$18,3*8($30)		# a2
     ldq	$19,4*8($30)		# a3
     ldq	$20,5*8($30)		# a4
     ldq	$21,6*8($30)		# a5
     ldt	$f16,7*8($30)
     ldt	$f17,8*8($30)
     ldt	$f18,9*8($30)
     ldt	$f19,10*8($30)
     ldt	$f20,11*8($30)
     ldt	$f21,12*8($30)

     ldq	$26,0*8($30)		# ra
     lda    $30,14*8($30)		# release stack

# Jump to translated method
     jmp	$31,($27),0

# for __alpha_nextFrame()
     ret	$31,($26),1

     .end C_FUNC_NAME(alpha_do_fixup_trampoline)

#endif
