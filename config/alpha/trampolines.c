/*
 * alpha/trampolines.c
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

#if defined(TRAMPOLINE_FUNCTION)
/*
 * If we have an explit function defined then use that.
 */
TRAMPOLINE_FUNCTION()

#else
/*
 * Otherwise we'll try to construct one.
 */
#if !defined(C_FUNC_NAME)
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define C_FUNC_NAME(FUNC) #FUNC
#endif
#endif

#if !defined(START_ASM_FUNC)
#define START_ASM_FUNC(FUNC)		\
    	".text				\n"\
	".align 3			\n"\
	".globl " C_FUNC_NAME(FUNC) "	\n"\
	".ent " C_FUNC_NAME(FUNC) "	\n"\
  C_FUNC_NAME(FUNC) ":"
#endif
#if !defined(END_ASM_FUNC)
#define END_ASM_FUNC(FUNC)		\
	".end " C_FUNC_NAME(FUNC)
#endif

asm(
	START_ASM_FUNC(alpha_do_fixup_trampoline) "		\n"
"	.frame	$30,14*8,$26,0					\n"
"	.mask	0x04000000,-14*8				\n"
"	ldgp	$29,0($27)					\n"
"	lda    $30,-14*8($30)		# reserve 14 on stack	\n"
"								\n"
"	# Save frame registers					\n"
"	stq	$26,0*8($30)		# ra			\n"
"	.prologue 1						\n"
"								\n"
"	# Save register arguments as local variables		\n"
"	stq	$16,1*8($30)		# a0			\n"
"	stq	$17,2*8($30)		# a1			\n"
"	stq	$18,3*8($30)		# a2			\n"
"	stq	$19,4*8($30)		# a3			\n"
"	stq	$20,5*8($30)		# a4			\n"
"	stq	$21,6*8($30)		# a5			\n"
"	stt	$f16,7*8($30)					\n"
"	stt	$f17,8*8($30)					\n"
"	stt	$f18,9*8($30)					\n"
"	stt	$f19,10*8($30)					\n"
"	stt	$f20,11*8($30)					\n"
"	stt	$f21,12*8($30)					\n"
"								\n"
"	# Compute the address of methodTrampoline.method	\n"
"	lda	$16,8($1)					\n"
"	# Call C to do the fixup				\n"
"	jsr	$26," C_FUNC_NAME(soft_fixup_trampoline) "	\n"
"	ldgp	$29,0($26)					\n"
"	mov	$0,$27						\n"
"								\n"
"	# Reload register arguments				\n"
"	ldq	$16,1*8($30)		# a0			\n"
"	ldq	$17,2*8($30)		# a1			\n"
"	ldq	$18,3*8($30)		# a2			\n"
"	ldq	$19,4*8($30)		# a3			\n"
"	ldq	$20,5*8($30)		# a4			\n"
"	ldq	$21,6*8($30)		# a5			\n"
"	ldt	$f16,7*8($30)					\n"
"	ldt	$f17,8*8($30)					\n"
"	ldt	$f18,9*8($30)					\n"
"	ldt	$f19,10*8($30)					\n"
"	ldt	$f20,11*8($30)					\n"
"	ldt	$f21,12*8($30)					\n"
"								\n"
"	ldq	$26,0*8($30)		# ra			\n"
"	lda    $30,14*8($30)		# release stack		\n"
"								\n"
"	# Jump to translated method				\n"
"	jmp	$31,($27),0					\n"
"								\n"
"	# for __alpha_nextFrame()				\n"
"	ret	$31,($26),1					\n"
    	END_ASM_FUNC(alpha_do_fixup_trampoline)
);

#endif
