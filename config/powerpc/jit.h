/*
 * powerpc/jit.h
 * Contains the powerpc trampoline and dcache flush macros.
 *
 * Copyright (c) 2002 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_JANOSVM_LICENSE@
 */

#ifndef __powerpc_jit_h
#define __powerpc_jit_h

#include "ppc_isa.h"
#include "ppc_macros.h"
#include "ppc_stack_frame.h"

extern void powerpc_do_fixup_trampoline(void);

/*
 * The PowerPC trampoline structure.
 *
 * code - The trampoline code.
 * meth - The method the trampoline interposes on.
 * where - A pointer to the pointer that refers to this trampoline.
 */
typedef struct _methodTrampoline {
	ppc_code_t code[6];
	struct _methods *meth;
	void **where;
} methodTrampoline;

/*
 * The trampoline comprises six instructions. The first two of them load into
 * r0 (the function prologue scratch register) the address of our fixup
 * routine.  We do it in two steps: first we load the high nibble into r0 and
 * then or in the lower nibble.
 *
 *	code[0]		lis   r0, hi(powerpc_do_fixup_trampoline)
 *	code[1]		ori   r0, r0, lo(powerpc_do_fixup_trampoline)
 *
 * Afterwards we copy that address from r0 into ctr, so that later on
 * we can jump to it:
 *
 *	code[2]		mtctr r0
 *
 * Next, we save the current lr value since we are going to use it to tell the
 * powerpc_do_fixup_trampoline() function about this trampoline.
 *
 *	code[3]		mflr r0
 *	code[4]		stw  r0, <lr offset in frame>(sp)
 *
 * And finally we update the link register and branch to the fixup function.
 * The fixup function can then use the link register value to find the meth
 * and where values in this trampoline.
 *
 *	code[5]		bctrl
 *
 * This is the simplest way of jumping to a generic 32-bit address
 * that I can think of.  Simplifications are welcome :-)
 *
 *
 * NOTES:
 *
 *   1. I think we need to avoid any registers except r0 since they might
 *   contain arguments/saved state.
 */
#define FILL_IN_TRAMPOLINE(t, m, w) \
do { \
	(t)->code[0] = ppc_op_lis(PPC_R0, \
				  ppc_hi16(powerpc_do_fixup_trampoline)); \
	(t)->code[1] = ppc_op_ori(PPC_R0, \
				  PPC_R0, \
				  ppc_lo16(powerpc_do_fixup_trampoline)); \
	(t)->code[2] = ppc_op_mtctr(PPC_R0); \
	(t)->code[3] = ppc_op_mflr(PPC_R0); \
	(t)->code[4] = ppc_op_stw(PPC_R0, \
				  PPC_RSP, \
				  PPC_FRAME_LR); \
	(t)->code[5] = ppc_op_bctrl(); \
	(t)->meth = (m); \
	(t)->where = (w); \
} while(0)

/*
 * PowerPC stores return address in register LR, not in the stack. Therefore,
 * we use 'Method *_meth' and 'meth = _meth' instead of 'Method **_pmeth' and
 * 'meth = *_pmeth'
 */
#define FIXUP_TRAMPOLINE_DECL Method *_meth, void *_where
#define FIXUP_TRAMPOLINE_INIT meth = _meth; where = _where;

/*
 * Call a jitted java exception handler.
 */
#define CALL_KAFFE_EXCEPTION(frame, handler, object) \
	asm volatile ("mr r1, %0\n" \
		      "\tmr r3, %1\n" \
		      "\tmtctr %2\n" \
		      "\tbctr\n" \
		      : : "r" (frame), "r" (object), "r" (handler) : "r3")

/*
 * Call a JanosVM catastrophe handler.
 * XXX implement me.
 */
#define CALL_KAFFE_C_EXCEPTION(F, P, H, O)

/* Extra label types. */

/* 26 bit displacement */
#define Llong26  (Larchdepend + 0)
/* 16 bit displacement */
#define Llong16  (Larchdepend + 1)
/* 16 bit displacement, no alignment restrictions.  */
#define Llong16noalign  (Larchdepend + 2)
/* 32 bit displacement broken into two instructions. */
#define Llong16x16 (Larchdepend + 3)
/* XXX not very different from Lnegframe... */
#define Lsavedregs (Larchdepend + 4)
/* Update the source register in a stmw. */
#define Lreg_s (Larchdepend + 5)

#define FILL_LABEL_Llong16x16(P, V, L) \
{ \
	ppc_code_t *_code = (ppc_code_t *)(P); \
	ppc_code_t _disp = (V); \
\
	_code[0] |= ppc_lo16(_disp); \
	_code[1] |= ppc_ha16(_disp); \
	/* \
	 *                 ^^^^^^^^^^^^ Need to clear out the high order bits \
	 * if the offset is negative. \
	 */ \
}

#define FILL_LABEL_Llong26(P, V, L) \
{ \
	ppc_code_t *_code = (ppc_code_t *)(P); \
	ppc_code_t _disp = (V); \
\
	assert(((_disp & ~PPC_LI_MASK) == 0x00000000) || \
	       ((_disp & ~PPC_LI_MASK) == 0xFC000000)); \
\
	_code[0] |= (_disp & PPC_LI_MASK); \
	/* \
	 *                 ^^^^^^^^^^^^ Need to clear out the high order bits \
	 * if the offset is negative. \
	 */ \
}

#define FILL_LABEL_Llong16(P, V, L) \
{ \
	ppc_code_t *_code = (ppc_code_t *)(P); \
	ppc_code_t _disp = (V); \
\
	assert(((_disp & ~PPC_BD_MASK) == 0x00000000) || \
	       ((_disp & ~PPC_BD_MASK) == 0xFFFF0000)); \
\
	_code[0] |= (_disp & PPC_BD_MASK); \
}

#define FILL_LABEL_Llong16noalign(P, V, L) \
{ \
	ppc_code_t *_code = (ppc_code_t *)(P); \
	int _disp = (V); \
\
	assert(((_disp & ~PPC_D_MASK) == 0x00000000) || \
	       ((_disp & ~PPC_D_MASK) == ~PPC_D_MASK)); \
\
	_code[0] |= (_disp & PPC_D_MASK); \
}

#define FILL_LABEL_Lsavedregs(P, V, L) \
{ \
	int frame_size, saved_registers; \
\
	saved_registers = (maxLocal + maxStack + maxTemp) - \
		PPC_ARG_REGISTER_COUNT; \
	if( saved_registers < 0 ) \
		saved_registers = 0; \
	if( (firstConst != currConst) || maxLocal || maxTemp ) \
	{ \
		saved_registers += 1; /* r31 */ \
	} \
	frame_size = -SLOTSIZE * saved_registers; \
	if( ((L)->type & Lfrommask) == Lrelative ) \
	{ \
		frame_size += (L)->from; \
	} \
	FILL_LABEL_Llong16noalign(P, frame_size, L); \
}

#define FILL_LABEL_Lframe(P, V, L, neg) \
{ \
	int frame_size = 0; \
\
	if( (firstConst != currConst) || maxLocal || maxTemp ) \
	{ \
		frame_size += SLOTSIZE; /* r31 */ \
	} \
	if( ((L)->type & Lfrommask) == Lrelative ) \
		frame_size += (L)->from; \
	if( neg ) \
		frame_size = -frame_size; \
	FILL_LABEL_Llong16noalign(P, frame_size, L); \
}

#define FILL_LABEL_Lreg_s(P, V, L) \
{ \
	ppc_code_t *_code = (ppc_code_t *)(P); \
	int saved_registers; \
\
	saved_registers = (maxLocal + maxStack + maxTemp) - \
		PPC_ARG_REGISTER_COUNT; \
	if( saved_registers < 0 ) \
		saved_registers = 0; \
	if( (firstConst != currConst) || maxLocal || maxTemp ) \
	{ \
		saved_registers += 1; /* r31 */ \
	} \
	_code[0] |= PPC_SET_RS((32 - saved_registers)); \
}

#define EXTRA_LABELS(P, V, L) \
	case Lframe: \
		FILL_LABEL_Lframe(P, V, L, 0); \
		break; \
	case Lnegframe: \
		FILL_LABEL_Lframe(P, V, L, 1); \
		break; \
	case Llong26: \
		FILL_LABEL_Llong26(P, V, L); \
		break; \
	case Llong16: \
		FILL_LABEL_Llong16(P, V, L); \
		break; \
	case Llong16noalign: \
		FILL_LABEL_Llong16noalign(P, V, L); \
		break; \
	case Llong16x16: \
		FILL_LABEL_Llong16x16(P, V, L); \
		break; \
	case Lsavedregs: \
		FILL_LABEL_Lsavedregs(P, V, L); \
		break; \
	case Lreg_s: \
		FILL_LABEL_Lreg_s(P, V, L); \
		break;

#define SLOTSIZE 4

/* XXX */
#define NR_ARGUMENTS 8

#define REGISTERS_SAVED 21

#define SLOT2LOCALOFFSET(N) \
	(-(SLOTSIZE * ((N) + 2)))

#define SLOT2ARGOFFSET(N) \
	(unsigned long)(&((ppc_stack_frame_t *)0)->args[(N)])

#define KAFFEJIT_TO_NATIVE(M)

#define PUSHARG_FORWARDS

#endif
