/*
 * powerpc/trampolines.S
 *
 */

#define L(x) x
#include "kaffe/trampoline.h"

/* include common ppc asm syntax header */
#if defined(HAVE_ASM_PPC_ASM_H)
#include <asm/ppc_asm.h>
#endif /* defined(HAVE_ASM_PPC_ASM_H) */

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
.align 2
.globl C_FUNC_NAME(powerpc_do_fixup_trampoline)


     C_FUNC_NAME(powerpc_do_fixup_trampoline) :

/* Function prologue: establish a minimum stack frame. We don't
 * save lr - it was already saved at 8(r1) by the trampoline
 * code. However, we must save r3 and r4, as we will use them
 * to call soft_fixup_trampoline().
 */
     stw  r3, -4(r1)
     stw  r4, -8(r1)
     stw  r5, -12(r1)
     stw  r6, -16(r1)
     stw  r7, -20(r1)
     stw  r8, -24(r1)
     stw  r9, -28(r1)
     stw  r10, -32(r1)

     stfd  f1, -40(r1)
     stfd  f2, -48(r1)
     stfd  f3, -56(r1)
     stfd  f4, -64(r1)
     stfd  f5, -72(r1)
     stfd  f6, -80(r1)
     stfd  f7, -88(r1)
     stfd  f8, -96(r1)
     stfd  f9, -104(r1)
     stfd  f10, -112(r1)
     stfd  f11, -120(r1)
     stfd  f12, -128(r1)
     stfd  f13, -136(r1)
     stwu r1, -192(r1)

# Build function call to soft_fixup_trampoline(). This function
# requires two arguments: a 'Method *m' and a 'void **where'.
# They can be found immediately after the trampoline code which
# took us here - that is, at (lr) and 4(lr)
     mflr r4
     lwz  r3, 0(r4)
     lwz  r4, 4(r4)

# Call soft_fixup_trampoline(). Return value will be in r3,
# which is the address of the translated native code.

#if defined(__APPLE__)
     bl Lsoft_fixup_trampoline$stub
#else
     bl soft_fixup_trampoline
#endif

# Save return value in ctr, so that we can jump there at the
# end of this function
     mtctr r3

# Restore previous stack pointer
     addi r1, r1, 192

# Restore r3 and r4 (the original arguments to the native
# method that we overwrote to call soft_fixup_trampoline)
     lwz  r3, -4(r1)
     lwz  r4, -8(r1)
     lwz  r5, -12(r1)
     lwz  r6, -16(r1)
     lwz  r7, -20(r1)
     lwz  r8, -24(r1)
     lwz  r9, -28(r1)
     lwz  r10, -32(r1)

     lfd  f1, -40(r1)
     lfd  f2, -48(r1)
     lfd  f3, -56(r1)
     lfd  f4, -64(r1)
     lfd  f5, -72(r1)
     lfd  f6, -80(r1)
     lfd  f7, -88(r1)
     lfd  f8, -96(r1)
     lfd  f9, -104(r1)
     lfd  f10, -112(r1)
     lfd  f11, -120(r1)
     lfd  f12, -128(r1)
     lfd  f13, -136(r1)

# Restore Link Register before invoking native method, so that
# it returns to the right place.
#if defined(__APPLE__)
     lwz  r0, 8(r1)
#else
     lwz  r0, 4(r1)
#endif
     mtlr r0

# Jump to the native method address that we obtained as a
# return value of soft_fixup_trampoline()
     bctr

#endif
	
#if defined(__APPLE__)
.data
	.align 2
LDFCM0:
	.section __TEXT,__picsymbolstub1,symbol_stubs,pure_instructions,32
	.align	2
Lsoft_fixup_trampoline$stub:
	.indirect_symbol _soft_fixup_trampoline
	mflr	r0
	bcl	20,31,LO$soft_fixup_trampoline
LO$soft_fixup_trampoline:
	mflr	r11
	addis	r11,r11,ha16(L_soft_fixup_trampoline$lazy_ptr - LO$soft_fixup_trampoline)
	mtlr	r0
	lwzu	r12,lo16(L_soft_fixup_trampoline$lazy_ptr - LO$soft_fixup_trampoline)(r11)
	mtctr	r12
	bctr
.data
.lazy_symbol_pointer
L_soft_fixup_trampoline$lazy_ptr:
	.indirect_symbol _soft_fixup_trampoline
	.long   dyld_stub_binding_helper
#endif
