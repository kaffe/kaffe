/*
 * mips/trampolines.c
 * MIPS trampolines codes for various occasions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if defined(TRAMPOLINE_FUNCTION)
/*
 * If we have an explit function defined then use that.
 */
TRAMPOLINE_FUNCTION()
 
#else
/*
 * Otherwise we'll try to construct one.
 */
#if !defined(START_ASM_FUNC)
#define START_ASM_FUNC() ".text\n\t.align 4\n\t.globl "
#endif
#if !defined(END_ASM_FUNC)
#define END_ASM_FUNC() ""
#endif
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define C_FUNC_NAME(FUNC) #FUNC
#endif

asm(" \n"
"	.text \n"						\
"	.align	4 \n"						\
"	sw	$31, 0($sp) \n"					\
"l1:	.word	" C_FUNC_NAME(soft_fixup_trampoline) " \n"	\
"	.globl 	mips_do_fixup_trampoline \n"			\
"	.ent	mips_do_fixup_trampoline \n"			\
"mips_do_fixup_trampoline: \n"					\
"	addi	$sp, $sp, -48 \n"				\
"	# needed by the backtracer \n"				\
"	sw	$31, 0($sp) \n"					\
"	sw	$2, 0($sp) \n"					\
"	sw	$4, 8($sp) \n"					\
"	sw	$5, 12($sp) \n"					\
"	sw	$6, 16($sp) \n"					\
"	sw	$7, 20($sp) \n"					\
"	swc1	$f13, 24($sp) \n"				\
"	swc1	$f12, 28($sp) \n"				\
"	swc1	$f15, 32($sp) \n"				\
"	swc1	$f14, 36($sp) \n"				\
"	sw	$30, 40($sp) \n"				\
"       sw      $gp, 4($sp) \n"					\
"        # $fp = $sp \n"					\
"	move	$30, $sp \n"					\
"	lw	$4, 0($31) \n"					\
"	lw	$5, 4($31) \n"					\
"	lw	$25, -4($25) \n"				\
"	jalr	$25 \n"						\
"	nop \n"							\
"	lw	$30, 40($sp) \n"				\
"	lwc1	$f14, 36($sp) \n"				\
"	lwc1	$f15, 32($sp) \n"				\
"	lwc1	$f12, 28($sp) \n"				\
"	lwc1	$f13, 24($sp) \n"				\
"	lw	$7, 20($sp) \n"					\
"	lw	$6, 16($sp) \n"					\
"	lw	$5, 12($sp) \n"					\
"	lw	$4, 8($sp) \n"					\
"	lw	$gp, 4($sp) \n"					\
"	lw	$31, 0($sp) \n"					\
"	addi	$sp, $sp, 48 \n"				\
"	move	$25, $2	\n"					\
"	jr	$2 \n"						\
"	.end	mips_do_fixup_trampoline"
);

#endif
