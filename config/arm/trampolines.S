/*
 * arm/trampolines.S
 * ARM trampolines codes for various occasions.
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

#if defined(NO_SHARED_VMLIBRARY)

#ifdef __riscos__

    ALIGN
    EXPORT C_FUNC_NAME(arm_do_fixup_trampoline)
#else

    .text
    .align 4
    .globl C_FUNC_NAME(arm_do_fixup_trampoline)
#endif

#ifdef __riscos__
| C_FUNC_NAME(arm_do_fixup_trampoline) |
#else
C_FUNC_NAME(arm_do_fixup_trampoline) :
#endif

    stmdb	sp!,{r0,r1,r2,r3,fp,ip,lr}
    add	fp, sp, #28
    mov	r0,ip
    bl	C_FUNC_NAME(soft_fixup_trampoline)
    str	r0, [sp, #-4]
    ldmia	sp!,{r0,r1,r2,r3,fp,ip,lr}
    ldr	pc, [sp, #-32]

#else
/*
 * We need to create a seperate stack frame for the trampoline
 * so that we consider it as a seperate function when creating
 * a backtrace. Therefore, we have to push the fp and adjust it
 * according to the sp afterwards (we can't use the ip for this,
 * since that holds the method that is to be translated). Since
 * kaffe expects the stackframe to also contain the ip (since it's
 * normally used to save the sp), we have to push that one too,
 * although this trampoline doesn't use it itself. Removing the
 * stackframe is a little bit tricky, since we have to restore
 * all registers, but need to get the tranlated code into pc.
 * Therefore, the address of the translated code is pushed on
 * the stack, then the registers are restored and the saved
 * address is loaded into the pc afterwards.
 */
#ifdef __riscos__

    ALIGN
    EXPORT C_FUNC_NAME(arm_do_fixup_trampoline)
#else

    .text
    .align 4
    .globl C_FUNC_NAME(arm_do_fixup_trampoline)
#endif

#ifdef __riscos__
| C_FUNC_NAME(arm_do_fixup_trampoline) |
#else
C_FUNC_NAME(arm_do_fixup_trampoline) :
#endif

     stmdb	sp!,{r0,r1,r2,r3,fp,ip,lr}
     add	fp, sp, #28
     mov	r0,ip
     bl	C_FUNC_NAME(soft_fixup_trampoline) (PLT)
     str	r0, [sp, #-4]
     ldmia	sp!,{r0,r1,r2,r3,fp,ip,lr}
     ldr	pc, [sp, #-32]
#endif

#endif
