/*
 * mips/jit.h
 * Common MIPS JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Christian Krusel <chrkr@uni-paderborn.de>, 1997.
 * Modified by Michael Chen <mikey@hydra.stanford.edu>, 1998.
 * last changed 2.15.98
 */

#ifndef __mips_jit_h
#define __mips_jit_h

#if defined(HAVE_SGIDEFS_H)
#include <sgidefs.h>
#endif /* defined(HAVE_SGIDEFS_H) */

#if defined (_MIPS_SIM) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#error "Calling convention other than o32 not supported"
#endif

/**/
/* Exception handling information. */
/**/

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
	char*	return_frame;
	char*	return_pc;
	char*	return_sp;
} exceptionFrame;

extern void __mipsGetNextFrame(struct _exceptionFrame*);

#define	STACK_NEXT_FRAME(F)	__mipsGetNextFrame(F)
#define	FIRSTFRAME(F,O)		(F).return_frame = (F).return_sp = 0; \
				__mipsGetNextFrame(&F)
#define	NEXTFRAME(F)		((F)->return_frame)
#define	PCFRAME(F)		((F)->return_pc)
#define	FPFRAME(F)		((F)->return_frame)

/* Extract the object argument from given frame */
#define FRAMEOBJECT(obj, f, einfo)					\
	(obj) = (*(Hjava_lang_Object**)(f))

/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

/* The layout of this struct is know by inline assembly.  */

typedef struct _methodTrampoline {
	unsigned code[4];
	struct _jmethodID *meth;
	void** where;
	unsigned pad[2];
} methodTrampoline;

extern void mips_do_fixup_trampoline(void);

/* We must construct jump address since jal can not hold absolute address */
#define FILL_IN_TRAMPOLINE(t,m,w)					\
	do {								\
		uint32 pc = (unsigned int)mips_do_fixup_trampoline;	\
		(t)->code[0] = 0x3c190000 | (pc >> 16);	/* lui $25,addr(high) */ \
		(t)->code[1] = 0x37390000 | (pc & 0xffff);/* ori $25,$25,addr(low) */ \
		(t)->code[2] = 0x0320f809;	/* jalr $31,$25 */		\
		(t)->code[3] = 0x001f1021;	/* addu $2,$31,$0 */	\
		(t)->meth = (m);					\
		(t)->where = (w);					\
	} while (0)

#define FIXUP_TRAMPOLINE_DECL	Method *_meth, void* _where
#define FIXUP_TRAMPOLINE_INIT	meth = _meth; where = _where;

/**/
/* Register management information. */
/**/

#define	RIL	(Rint|Rref)
#define	RFD	(Rfloat|Rdouble)
#define	RG	(Rglobal|Rnosaveoncall)

/* Define the register set, already adapted for MIPS. */
#define	REGISTER_SET							\
	{ /* i0 */	0, 0, Reserved,		0, 0, 0   },		\
	{ /* i1 */	0, 0, Reserved,		0, 0, 1   },		\
	{ /* i2 */	0, 0, RIL|Reserved,	0, 0, 2   },		\
	{ /* i3 */	0, 0, RIL|Reserved,	0, 0, 3   },		\
	{ /* i4 */	0, 0, RIL,		0, 0, 4   },		\
	{ /* i5 */	0, 0, RIL,		0, 0, 5   },		\
	{ /* i6 */	0, 0, RIL,		0, 0, 6   },		\
	{ /* i7 */	0, 0, RIL,		0, 0, 7   },		\
	{ /* i8 */	0, 0, RIL,		0, 0, 8   },		\
	{ /* i9 */	0, 0, RIL,		0, 0, 9   },		\
	{ /* i10 */	0, 0, RIL,		0, 0, 10   },		\
	{ /* i11 */	0, 0, RIL,		0, 0, 11   },		\
	{ /* i12 */	0, 0, RIL,		0, 0, 12   },		\
	{ /* i13 */	0, 0, RIL,		0, 0, 13   },		\
	{ /* i14 */	0, 0, RIL,		0, 0, 14   },		\
	{ /* i15 */	0, 0, RIL,		0, 0, 15   },		\
	{ /* i16 */	0, 0, RIL,		RG, 0, 16   },\
	{ /* i17 */	0, 0, RIL,		RG, 0, 17   },\
	{ /* i18 */	0, 0, RIL,		RG, 0, 18   },\
	{ /* i19 */	0, 0, RIL,		RG, 0, 19   },\
	{ /* i20 */	0, 0, RIL,		RG, 0, 20   },\
	{ /* i21 */	0, 0, RIL,		RG, 0, 21   },\
	{ /* i22 */	0, 0, RIL,		RG, 0, 22   },\
	{ /* i23 */	0, 0, RIL,		RG, 0, 23   },\
	{ /* i24 */	0, 0, RIL,		0, 0, 24   },		\
	{ /* i25 */	0, 0, RIL,		0, 0, 25   },		\
	{ /* i26 */	0, 0, Reserved,		0, 0, 26   },		\
	{ /* i27 */	0, 0, Reserved,		0, 0, 27   },		\
	{ /* i28/gp */	0, 0, Reserved,		0, 0, 28   },\
	{ /* i29 */	0, 0, Reserved, 	Rnosaveoncall, 0, 29   },\
	{ /* i30 */	0, 0, Reserved,		Rnosaveoncall, 0, 30   },\
	{ /* i31 */	0, 0, Reserved,		Rnosaveoncall, 0, 31   },\
	{ /* f0 */	0, 0, RFD|Reserved,	0, 0, 0   },		\
	{ /* f1 */	0, 0, Reserved,		0, 0, 1   },		\
	{ /* f2 */	0, 0, Reserved,		0, 0, 2   },		\
	{ /* f3 */	0, 0, Reserved,		0, 0, 3   },		\
	{ /* f4 */	0, 0, RFD,		0, 0, 4   },		\
	{ /* f5 */	0, 0, Reserved,		0, 0, 5   },		\
	{ /* f6 */	0, 0, RFD,		0, 0, 6   },		\
	{ /* f7 */	0, 0, Reserved,		0, 0, 7   },		\
	{ /* f8 */	0, 0, RFD,		0, 0, 8   },		\
	{ /* f9 */	0, 0, Reserved,		0, 0, 9   },		\
	{ /* f10 */	0, 0, RFD,		0, 0, 10   },		\
	{ /* f11 */	0, 0, Reserved,		0, 0, 11   },		\
	{ /* f12 */	0, 0, RFD,		0, 0, 12   },		\
	{ /* f13 */	0, 0, Reserved,		0, 0, 13   },		\
	{ /* f14 */	0, 0, RFD,		0, 0, 14   },		\
	{ /* f15 */	0, 0, Reserved,		0, 0, 15   },		\
	{ /* f16 */	0, 0, RFD,		0, 0, 16   },		\
	{ /* f17 */	0, 0, Reserved,		0, 0, 17   },		\
	{ /* f18 */	0, 0, RFD,		0, 0, 18   },		\
	{ /* f19 */	0, 0, Reserved,		0, 0, 19   },		\
	{ /* f20 */	0, 0, RFD,		0, 0, 20   },		\
	{ /* f21 */	0, 0, Reserved,		0, 0, 21   },		\
	{ /* f22 */	0, 0, RFD,		0, 0, 22   },		\
	{ /* f23 */	0, 0, Reserved,		0, 0, 23   },		\
	{ /* f24 */	0, 0, RFD,		0, 0, 24   },		\
	{ /* f25 */	0, 0, Reserved,		0, 0, 25   },		\
	{ /* f26 */	0, 0, RFD,		0, 0, 26   },		\
	{ /* f27 */	0, 0, Reserved,		0, 0, 27   },		\
	{ /* f28 */	0, 0, RFD,		0, 0, 28   },		\
	{ /* f29 */	0, 0, Reserved,		0, 0, 29   },		\
	{ /* f30 */	0, 0, RFD,		0, 0, 30   },		\
	{ /* f31 */	0, 0, Reserved,		0, 0, 31   },		\

/* Number of registers in the register set */
#define	NR_REGISTERS	64

/**/
/* We must setup the incoming argument state */
/**/
#define NR_ARGUMENTS	4

/**/
/* Number of global registers */
/**/
#define	NR_GLOBALS	8

/**/
/* Define to use the constant pool. */
/**/
#define	USE_JIT_CONSTANT_POOL	1

/**/
/* Define to use stack limit checking - give it the place to find the limit */
/**/
#define	STACK_LIMIT()	SLOT2ARGOFFSET(maxArgs)

/**/
/* Number of registers saved in the prologue. */
/**/
#define	REGISTERS_SAVED	12

/**/
/* Opcode generation. */
/**/

/* Extra label types */
#define Llong16b	(Larchdepend+0)
#define Llong16x16	(Larchdepend+1)
#define Llong26		(Larchdepend+2)
#define Lexception	(Larchdepend+3)
#define Llong16		(Larchdepend+4)

/* HACK XXX - We add in a little extra space to stop the JIT from breaking */
#define	EXTRASPACE	2

#define	FRAMESIZE \
	STACKALIGN(((maxLocal-maxArgs) + maxStack + maxTemp + \
		REGISTERS_SAVED + EXTRASPACE + max_args) * SLOTSIZE)

#define	LABEL_Lframe(P,V,L) \
	{ \
		int framesize = FRAMESIZE; \
		assert((framesize & 0xFFFF0000) == 0); \
		*(P) = (*(P) & 0xFFFF0000) | ((-framesize) & 0xFFFF); \
	}
#define	LABEL_Lnegframe(P,V,L) \
	{ \
		int framesize = FRAMESIZE; \
		assert((framesize & 0xFFFF0000) == 0); \
		*(P) = (*(P) & 0xFFFF0000) | ((framesize) & 0xFFFF); \
	}
#define	LABEL_Llong16b(P,V,L)	(P)[0]=((P)[0]&0xFFFF0000)|((((V)-4)>>2)&0xFFFF)
#define	LABEL_Llong16(P,V,L)	(P)[0]=((P)[0]&0xFFFF0000)|(((V))&0xFFFF)
#define LABEL_Llong16x16(P,V,L) (P)[1]=((P)[1]&0xFFFF0000)|((V) & 0xFFFF); \
                                (P)[0]=((P)[0]&0xFFFF0000)|(((V)>>16)&0xFFFF)
#if defined(USE_JIT_CONSTANT_POOL)
#define	LABEL_Lexception(P,V,L)	LABEL_Lframe(P,V,L); \
				LABEL_Llong16x16((P)+1,codebase,L)
#else
#define	LABEL_Lexception(P,V,L)	LABEL_Lframe(P,V,L)
#endif

#define	EXTRA_LABELS(P,D,L)						\
	case Lframe:		LABEL_Lframe(P,D,L);		break;	\
	case Lnegframe:		LABEL_Lnegframe(P,D,L);		break;	\
	case Llong16b:		LABEL_Llong16b(P,D,L);		break;	\
	case Llong16x16:	LABEL_Llong16x16(P,D,L);	break;	\
	case Lexception:	LABEL_Lexception(P,D,L);	break;	\
	case Llong16:		LABEL_Llong16(P,D,L);		break;


/**/
/* Slot management information. */
/**/

/* Size of each slot */
#define	SLOTSIZE		4

#define	FRAMEALIGN		16
#define	STACKALIGN(v)		(((v) + FRAMEALIGN - 1) & -FRAMEALIGN)

/* We push arguments low to high. */
#define	PUSHARG_FORWARDS	1

/*
 * A mips stack frame looks like:
 *
 *
 *      |  caller's frame               |
 *      |-------------------------------|
 *      |  incoming                     |
 *      |  arguments                    |
 * $fp->|-------------------------------|
 *      |  locals and                   |
 *      |  temporaries                  |
 *      |-------------------------------|
 *      |  callee-saved registers       |
 *      |                               |
 *      |-------------------------------|  -
 *      |  outgoing                     |  | max
 *      |  arguments                    |  | Push
 * $sp->|-------------------------------|  -
 *      |  callee's frame               |
 *
 */

/* We need to do some special initialization for the MIPS JIT */
struct _jmethodID;
extern void __mipsInitJit(struct _jmethodID*);
#define	INIT_JIT_MD(M)		__mipsInitJit(M)

/* Generate slot offset for an argument relative to the frame pointer */
#define SLOT2ARGOFFSET(N)	(argMap[N] * SLOTSIZE)

/* Generate slot offset for a local (non-argument) */
#define SLOT2LOCALOFFSET(N) \
	(-SLOTSIZE * (REGISTERS_SAVED + (N) - maxArgs + EXTRASPACE))

/* Generate slot offset for an argument push (relative to $sp) */
#define SLOT2PUSHOFFSET(N)	((N) * SLOTSIZE)

/* Wrap up a native call for the JIT */
#define KAFFEJIT_TO_NATIVE(_m)

/* On the mips we need to flush the data cache after generating new code */
#if defined(HAVE_MIPS_CACHECTL_H)
#include <mips/cachectl.h>
#endif /* defined(HAVE_MIPS_CACHECTL_H) */

#if defined(HAVE_SYS_CACHECTL_H)
#include <sys/cachectl.h>
#endif /* defined(HAVE_SYS_CACHECTL_H) */

#if defined(HAVE_ASM_CACHECTL_H)
#include <asm/cachectl.h>
#endif /* defined(HAVE_ASM_CACHECTL_H) */

#define	FLUSH_DCACHE(_start,_end) cacheflush((_start), (uintp)(_end) - (uintp)(_start), BCACHE)

/* Argument register mapping */
extern int* argMap;
extern int totalMaxArgs;
extern int max_args;

#endif
