/*
 * powerpc/jit-labels.h
 *
 * Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_JANOSVM_LICENSE@
 */

#ifndef __powerpc_jit_labels_h
#define __powerpc_jit_labels_h

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
	int frame_size = 0, saved_registers; \
\
	saved_registers = maxLocal - PPC_ARG_REGISTER_COUNT; \
	if( saved_registers < 0 ) \
		saved_registers = 0; \
	saved_registers = maxStack + maxTemp + 18; \
	if( (KaffeJIT3_getFirstConst() != KaffeJIT3_getCurrConst()) || maxLocal || maxTemp ) \
	{ \
		saved_registers += 1; /* r31 */ \
	} \
	frame_size += -SLOTSIZE * saved_registers; \
	if( ((L)->type & Lfrommask) == Lrelative ) \
	{ \
		frame_size += (L)->from; \
	} \
	FILL_LABEL_Llong16noalign(P, frame_size, L); \
}

#define FILL_LABEL_Lframe(P, V, L, neg) \
{ \
	int frame_size = sizeof(ppc_stack_frame_t), saved_registers; \
\
	saved_registers = maxLocal - PPC_ARG_REGISTER_COUNT; \
	if( saved_registers < 0 ) \
		saved_registers = 0; \
	saved_registers += maxStack + maxTemp; \
	if( (KaffeJIT3_getFirstConst() != KaffeJIT3_getCurrConst()) || maxLocal || maxTemp ) \
	{ \
		saved_registers += 1; /* r31 */ \
	} \
	saved_registers += (saved_registers * 2) + 18; \
	frame_size += SLOTSIZE * saved_registers; \
	if( ((L)->type & Lfrommask) == Lrelative ) \
	{ \
		frame_size += (L)->from; \
	} \
	if( neg ) \
		frame_size = -frame_size; \
	FILL_LABEL_Llong16noalign(P, frame_size, L); \
}

#define FILL_LABEL_Lreg_s(P, V, L) \
{ \
	ppc_code_t *_code = (ppc_code_t *)(P); \
	int saved_registers; \
\
	saved_registers = maxLocal - PPC_ARG_REGISTER_COUNT; \
	if( saved_registers < 0 ) \
		saved_registers = 0; \
	saved_registers = maxStack + maxTemp; \
	if( (KaffeJIT3_getFirstConst() != KaffeJIT3_getCurrConst()) || maxLocal || maxTemp ) \
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

#endif
