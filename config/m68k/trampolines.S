/*
 * m68k/trampolines.S
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

#if defined(__ELF__)

     .text
     .even
     .globl C_FUNC_NAME(m68k_do_fixup_trampoline)

C_FUNC_NAME(m68k_do_fixup_trampoline) :
     jbsr   C_FUNC_NAME(soft_fixup_trampoline)
     addqw	#4,%sp
     movel	%d0,%a0
     jmp     %a0@

#else

     .text
     .even
     .globl C_FUNC_NAME(m68k_do_fixup_trampoline)

C_FUNC_NAME(m68k_do_fixup_trampoline) :

     jbsr    C_FUNC_NAME(soft_fixup_trampoline)
     addqw   #4,sp
     movel   d0,a0
     jmp     a0@

#endif

#endif
