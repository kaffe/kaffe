/*
 * mips/trampolines.S
 * MIPS trampolines codes for various occasions.
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
	.align	4
	sw	$31, 0($sp)
l1:	.word	C_FUNC_NAME(soft_fixup_trampoline)
	.globl 	mips_do_fixup_trampoline
	.ent	mips_do_fixup_trampoline
mips_do_fixup_trampoline:
	addiu	$sp, $sp, -48
	# needed by the backtracer
	sw	$31, 0($sp)
	sw	$2, 0($sp)
	sw	$4, 8($sp)
	sw	$5, 12($sp)
	sw	$6, 16($sp)
	sw	$7, 20($sp)
	swc1	$f13, 24($sp)
	swc1	$f12, 28($sp)
	swc1	$f15, 32($sp)
	swc1	$f14, 36($sp)
	sw	$30, 40($sp)
        sw      $gp, 4($sp)
        # $fp = $sp
	move	$30, $sp
	lw	$4, 0($31)
	lw	$5, 4($31)
	lw	$25, -4($25)
	jalr	$25
	nop
	lw	$30, 40($sp)
	lwc1	$f14, 36($sp)
	lwc1	$f15, 32($sp)
	lwc1	$f12, 28($sp)
	lwc1	$f13, 24($sp)
	lw	$7, 20($sp)
	lw	$6, 16($sp)
	lw	$5, 12($sp)
	lw	$4, 8($sp)
	lw	$gp, 4($sp)
	lw	$31, 0($sp)
	addiu	$sp, $sp, 48
	move	$25, $2
	jr	$2
	.end	mips_do_fixup_trampoline

#endif
