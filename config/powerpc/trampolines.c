/*
 * powerpc/trampolines.c
 *
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
#define	START_ASM_FUNC() ".text\n\t.align 2\n\t.globl "
#endif
#if !defined(END_ASM_FUNC)
#define	END_ASM_FUNC() ""
#endif
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define	C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define	C_FUNC_NAME(FUNC) #FUNC
#endif

asm(
	""
	START_ASM_FUNC() C_FUNC_NAME(powerpc_do_fixup_trampoline)      "\n"
	"								\n"
C_FUNC_NAME(powerpc_do_fixup_trampoline) ":				\n"
	"								\n"
	"# Function prologue: establish a minimum stack frame. We don't	\n"
	"# save lr - it was already saved at 8(r1) by the trampoline	\n"
	"# code. However, we must save r3 and r4, as we will use them	\n"
	"# to call soft_fixup_trampoline().				\n"
	"stw  3, -4(1)							\n"
	"stw  4, -8(1)							\n"
	"stw  5, -12(1)							\n"
	"stw  6, -16(1)							\n"
	"stw  7, -20(1)							\n"
	"stw  8, -24(1)							\n"
	"stw  9, -28(1)							\n"
	"stw  10, -32(1)						\n"
	"								\n"
	"stfd  1, -40(1)						\n"
	"stfd  2, -48(1)						\n"
	"stfd  3, -56(1)						\n"
	"stfd  4, -64(1)						\n"
	"stfd  5, -72(1)						\n"
	"stfd  6, -80(1)						\n"
	"stfd  7, -88(1)						\n"
	"stfd  8, -96(1)						\n"
	"stfd  9, -104(1)						\n"
	"stfd  10, -112(1)						\n"
	"stfd  11, -120(1)						\n"
	"stfd  12, -128(1)						\n"
	"stfd  13, -136(1)						\n"
	"stwu 1, -192(1)						\n"
	"								\n"
	"# Build function call to soft_fixup_trampoline(). This function\n"
	"# requires two arguments: a 'Method *m' and a 'void **where'.	\n"
	"# They can be found immediately after the trampoline code which\n"
	"# took us here - that is, at (lr) and 4(lr)			\n"
	"mflr 4								\n"
	"lwz  3, 0(4)							\n"
	"lwz  4, 4(4)							\n"
	"								\n"
	"# Call soft_fixup_trampoline(). Return value will be in r3, 	\n"
	"# which is the address of the translated native code.		\n"
	"								\n"
	"bl " C_FUNC_NAME(soft_fixup_trampoline) "			\n"
	"								\n"
	"# Save return value in ctr, so that we can jump there at the	\n"
	"# end of this function						\n"
	"mtctr 3							\n"
	"								\n"
	"# Restore previous stack pointer				\n"
	"addi 1, 1, 192							\n"
	"								\n"
	"# Restore r3 and r4 (the original arguments to the native	\n"
	"# method that we overwrote to call soft_fixup_trampoline)	\n"
	"lwz  3, -4(1)							\n"
	"lwz  4, -8(1)							\n"
	"lwz  5, -12(1)							\n"
	"lwz  6, -16(1)							\n"
	"lwz  7, -20(1)							\n"
	"lwz  8, -24(1)							\n"
	"lwz  9, -28(1)							\n"
	"lwz  10, -32(1)						\n"
	"								\n"
	"lfd  1, -40(1)							\n"
	"lfd  2, -48(1)							\n"
	"lfd  3, -56(1)							\n"
	"lfd  4, -64(1)							\n"
	"lfd  5, -72(1)							\n"
	"lfd  6, -80(1)							\n"
	"lfd  7, -88(1)							\n"
	"lfd  8, -96(1)							\n"
	"lfd  9, -104(1)						\n"
	"lfd  10, -112(1)						\n"
	"lfd  11, -120(1)						\n"
	"lfd  12, -128(1)						\n"
	"lfd  13, -136(1)						\n"
	"								\n"
	"# Restore Link Register before invoking native method, so that	\n"
	"# it returns to the right place.				\n"
#if defined(__APPLE__)
	"lwz  0, 8(1)					\n"
#else
	"lwz  0, 4(1)					\n"
#endif
	"mtlr 0								\n"
	"								\n"
	"# Jump to the native method address that we obtained as a	\n"
	"# return value of soft_fixup_trampoline()			\n"
	"bctr"
);

#endif
