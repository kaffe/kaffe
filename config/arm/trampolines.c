/*
 * arm/trampolines.c
 * ARM trampolines codes for various occasions.
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
#define	START_ASM_FUNC() ".text\n\t.align 4\n\t.globl "
#endif
#if !defined(END_ASM_FUNC)
#define	END_ASM_FUNC() ""
#endif
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define	C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define	C_FUNC_NAME(FUNC) #FUNC
#endif

#if 1 /* defined(NO_SHARED_VMLIBRARY) */

asm(
	START_ASM_FUNC() C_FUNC_NAME(arm_do_fixup_trampoline) "\n"
C_FUNC_NAME(arm_do_fixup_trampoline) ":				\n
	stmdb	sp!,{r0,r1,r2,r3,lr}\n
	mov	r0,ip\n
	bl	" C_FUNC_NAME(soft_fixup_trampoline) "		\n
	mov	ip, r0\n
	ldmia	sp!,{r0,r1,r2,r3,lr}\n
	mov	pc, ip\n
"
	END_ASM_FUNC()
);

#else

asm(
	START_ASM_FUNC() C_FUNC_NAME(arm_do_fixup_trampoline) "\n"
C_FUNC_NAME(arm_do_fixup_trampoline) ":				\n
	stmdb	sp!,{r0,r1,r2,r3,lr}\n
	mov	r0,ip\n
	bl	" C_FUNC_NAME(soft_fixup_trampoline) " (PLT)	\n
	mov	ip, r0\n
	ldmia	sp!,{r0,r1,r2,r3,lr}\n
	mov	pc, ip\n
"
	END_ASM_FUNC()
);

#endif

#endif
