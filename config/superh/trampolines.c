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
#define START_ASM_FUNC() ".text\n\t.align 4\n\t.global "
#endif
#if !defined(END_ASM_FUNC)
#define END_ASM_FUNC() ""
#endif
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define C_FUNC_NAME(FUNC) #FUNC
#endif

asm(
	START_ASM_FUNC() C_FUNC_NAME(superh_do_fixup_trampoline) " \n"
C_FUNC_NAME(superh_do_fixup_trampoline) ": \n"
"	mov.l	r4,@-r15 \n"
"	mov.l	r5,@-r15 \n"
"	mov.l	r6,@-r15 \n"
"	mov.l	r7,@-r15 \n"
"	mov.l	r1,@-r15 \n"
"	sts	pr,r4 \n"
"	mov.l	.L99999,r0 \n"
"	jsr	@r0 \n"
"	nop \n"
"	lds.l	@r15+,pr \n"
"	mov.l	@r15+,r7 \n"
"	mov.l	@r15+,r6 \n"
"	mov.l	@r15+,r5 \n"
"	mov.l	@r15+,r4 \n"
"	jmp	@r0 \n"
"	nop \n"
"	.align 2 \n"
".L99999: \n"
"	.long	" C_FUNC_NAME(soft_fixup_trampoline) " \n"
" \n"
	END_ASM_FUNC()
);

#endif
