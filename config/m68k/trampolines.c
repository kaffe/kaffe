/*
 * m68k/trampolines.c
 * m68k trampolines codes for for various occasions.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1998;
 *	based on i386/trampolines.c
 * Modified by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1999;
 *	to use linux's assembler syntax to support both linux/netbsd.
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
#define START_ASM_FUNC() ".text\n\t.even\n\t.globl "
#endif
#if !defined(END_ASM_FUNC)
#define END_ASM_FUNC() ""
#endif
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define C_FUNC_NAME(FUNC) #FUNC
#endif

#if defined(__ELF__)

asm (
        START_ASM_FUNC() C_FUNC_NAME(m68k_do_fixup_trampoline) "\n"
C_FUNC_NAME(m68k_do_fixup_trampoline) ": \n"
"       jbsr    " C_FUNC_NAME(soft_fixup_trampoline) " \n"
"	addqw	#4,%sp \n"
"	movel	%d0,%a0 \n"
"       jmp     %a0@"
        END_ASM_FUNC()
);

#else

asm (
        START_ASM_FUNC() C_FUNC_NAME(m68k_do_fixup_trampoline) "\n"
C_FUNC_NAME(m68k_do_fixup_trampoline) ": \n"
"       jbsr    " C_FUNC_NAME(soft_fixup_trampoline) " \n"
"       addqw   #4,sp \n"
"       movel   d0,a0 \n"
"       jmp     a0@"
        END_ASM_FUNC()
);

#endif

#endif
