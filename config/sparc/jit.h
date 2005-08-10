/*
 * sparc/jit.h
 * Common SPARC JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_jit_h
#define __sparc_jit_h

/**/
/* Exception handling information. */
/**/

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
	uintp	lcl[8];
	uintp	arg[6];
        uintp	retbp;
        uintp	retpc;
	uintp	retstruct;
	uintp	callargs[6];
} exceptionFrame;

/* Is this frame valid (ie. is it on the current stack) ? */
#define	FRAMEOKAY(f)							\
	((f)->retbp >= (int)TCTX(currentThread)->stackBase &&		\
	 (f)->retbp < (int)TCTX(currentThread)->stackEnd && (f)->retpc != 0)

/* Get the next frame in the chain */
#define	NEXTFRAME(f)							\
	(((exceptionFrame*)(f))->retbp)

/* Extract PC & FP from the given frame */
#define	PCFRAME(f)							\
	((f)->retpc)
#define	FPFRAME(f)							\
	((f)->retbp)

/* Get the first exception frame from a subroutine call */
#define	FIRSTFRAME(f, o)						\
	{								\
		register exceptionFrame* ef asm("%sp");			\
		asm volatile("ta 3");					\
		(f) = *(ef);						\
	}

/* Extract the object argument from given frame */
#define FRAMEOBJECT(obj, f, einfo)					\
	(obj) = (*(Hjava_lang_Object**)					\
		 (((exceptionFrame*)(f))->retbp+68))

/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

/* The layout of this struct is know by inline assembly in trampolines.c */

typedef struct _methodTrampoline {
	unsigned code[3];
	struct _jmethodID *meth;
	void **where;
} methodTrampoline;

extern void sparc_do_fixup_trampoline(void);

#define FILL_IN_TRAMPOLINE(t,m,w)					\
	do {								\
		(t)->code[0] = 0x8210000f;	/* mov %o7, %g1 */	\
		/* call sparc_do_fixup_trampoline */			\
		(t)->code[1] = ((size_t)sparc_do_fixup_trampoline	\
				- ((size_t)(t) + 4)) / 4 | 0x40000000;	\
		(t)->code[2] = 0x01000000;	/* nop */		\
		(t)->meth = (m);					\
		(t)->where = (w);					\
	} while (0)

#define FIXUP_TRAMPOLINE_DECL	Method *_meth, void **_where
#define FIXUP_TRAMPOLINE_INIT	meth = _meth; where = _where;


/**/
/* Register management information. */
/**/

/* Define the register set */
#define	REGISTER_SET							\
	{ /* g0 */	0, 0, Reserved,		0, 0, 0   },		\
	{ /* g1 */	0, 0, Reserved,		0, 0, 1   },		\
	{ /* g2 */	0, 0, Reserved,		0, 0, 2   },		\
	{ /* g3 */	0, 0, Reserved,		0, 0, 3   },		\
	{ /* g4 */	0, 0, Reserved,		0, 0, 4   },		\
	{ /* g5 */	0, 0, Reserved,		0, 0, 5   },		\
	{ /* g6 */	0, 0, Reserved,		0, 0, 6   },		\
	{ /* g7 */	0, 0, Reserved,		0, 0, 7   },		\
	{ /* o0 */	0, 0, Reserved|Rint|Rref, 0, 0, 8   },		\
	{ /* o1 */	0, 0, Reserved|Rint|Rref, 0, 0, 9   },		\
	{ /* o2 */	0, 0, Reserved,		0, 0, 10   },		\
	{ /* o3 */	0, 0, Reserved,		0, 0, 11   },		\
	{ /* o4 */	0, 0, Reserved,		0, 0, 12   },		\
	{ /* o5 */	0, 0, Reserved,		0, 0, 13   },		\
	{ /* o6 */	0, 0, Reserved,		0, 0, 14   },		\
	{ /* o7 */	0, 0, Reserved,		0, 0, 15   },		\
	{ /* l0 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 16   },\
	{ /* l1 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 17   },\
	{ /* l2 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 18   },\
	{ /* l3 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 19   },\
	{ /* l4 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 20   },\
	{ /* l5 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 21   },\
	{ /* l6 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 22   },\
	{ /* l7 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 23   },\
	{ /* i0 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 24   },\
	{ /* i1 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 25   },\
	{ /* i2 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 26   },\
	{ /* i3 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 27   },\
	{ /* i4 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 28   },\
	{ /* i5 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 29   },\
	{ /* i6 */	0, 0, Reserved,		0, 0, 30   },		\
	{ /* i7 */	0, 0, Reserved,		0, 0, 31   },		\
	{ /* f0 */	0, 0, Rfloat|Rdouble,	0, 0, 0   },		\
	{ /* f1 */	0, 0, Reserved,		0, 0, 1   },		\
	{ /* f2 */	0, 0, Rfloat|Rdouble,	0, 0, 2   },		\
	{ /* f3 */	0, 0, Reserved,		0, 0, 3   },		\
	{ /* f4 */	0, 0, Rfloat|Rdouble,	0, 0, 4   },		\
	{ /* f5 */	0, 0, Reserved,		0, 0, 5   },		\
	{ /* f6 */	0, 0, Rfloat|Rdouble,	0, 0, 6   },		\
	{ /* f7 */	0, 0, Reserved,		0, 0, 7   },		\
	{ /* f8 */	0, 0, Rfloat|Rdouble,	0, 0, 8   },		\
	{ /* f9 */	0, 0, Reserved,		0, 0, 9   },		\
	{ /* f10 */	0, 0, Rfloat|Rdouble,	0, 0, 10   },		\
	{ /* f11 */	0, 0, Reserved,		0, 0, 11   },		\
	{ /* f12 */	0, 0, Rfloat|Rdouble,	0, 0, 12   },		\
	{ /* f13 */	0, 0, Reserved,		0, 0, 13   },		\
	{ /* f14 */	0, 0, Rfloat|Rdouble,	0, 0, 14   },		\
	{ /* f15 */	0, 0, Reserved,		0, 0, 15   },		\
	{ /* f16 */	0, 0, Rfloat|Rdouble,	0, 0, 16   },		\
	{ /* f17 */	0, 0, Reserved,		0, 0, 17   },		\
	{ /* f18 */	0, 0, Rfloat|Rdouble,	0, 0, 18   },		\
	{ /* f19 */	0, 0, Reserved,		0, 0, 19   },		\
	{ /* f20 */	0, 0, Rfloat|Rdouble,	0, 0, 20   },		\
	{ /* f21 */	0, 0, Reserved,		0, 0, 21   },		\
	{ /* f22 */	0, 0, Rfloat|Rdouble,	0, 0, 22   },		\
	{ /* f23 */	0, 0, Reserved,		0, 0, 23   },		\
	{ /* f24 */	0, 0, Rfloat|Rdouble,	0, 0, 24   },		\
	{ /* f25 */	0, 0, Reserved,		0, 0, 25   },		\
	{ /* f26 */	0, 0, Rfloat|Rdouble,	0, 0, 26   },		\
	{ /* f27 */	0, 0, Reserved,		0, 0, 27   },		\
	{ /* f28 */	0, 0, Rfloat|Rdouble,	0, 0, 28   },		\
	{ /* f29 */	0, 0, Reserved,		0, 0, 29   },		\
	{ /* f30 */	0, 0, Rfloat|Rdouble,	0, 0, 30   },		\
	{ /* f31 */	0, 0, Reserved,		0, 0, 31   },		\

/* Number of registers in the register set */
#define	NR_REGISTERS	64

/**/
/* Opcode generation. */
/**/

/* Extra label types */
#define Llong30		(Larchdepend+0)	/* Label is 30 bits long */
#define Llong22		(Larchdepend+1)	/* Label is 22 bits long */
#define Llong22x10	(Larchdepend+2)	/* Label is split into 22 and 10 bits */

#define	LABEL_Lframe(P,V,L) \
	{ \
		int framesize = STACKALIGN( \
			STACKALIGN((maxPush < 6 ? 6 : maxPush) * SLOTSIZE) + \
			STACKALIGN(((maxLocal-maxArgs)+maxStack+maxTemp) * \
			SLOTSIZE) + WINDOWSIZE + WINDOWEXTRA); \
		assert((framesize & 0xFFFFF000) == 0); \
		*(P) = (*(P) & 0xFFFFE000) | ((-framesize) & 0x1FFF); \
	}
#define	LABEL_Llong30(P,V,L)	(P)[0] = (((P)[0] & 0xC0000000) | (((V) / 4) & 0x3FFFFFFF))
#define	LABEL_Llong22(P,V,L)	(P)[0] = (((P)[0] & 0xFFC00000) | (((V) / 4) & 0x003FFFFF))
#define	LABEL_Llong22x10(P,V,L)	(P)[0] = (((P)[0] & 0xFFC00000) | (((V) >> 10) & 0x003FFFFF)); \
				(P)[1] = (((P)[1] & 0xFFFFFC00) | ((V) & 0x000003FF));

#define EXTRA_LABELS(P,D,L)						\
	case Lframe:		LABEL_Lframe(P,D,L);		break;	\
	case Llong30:		LABEL_Llong30(P,D,L);		break;	\
	case Llong22:		LABEL_Llong22(P,D,L);		break;	\
	case Llong22x10:	LABEL_Llong22x10(P,D,L);	break;

/**/
/* Slot management information. */
/**/

/* Size of each slot */
#define	SLOTSIZE		4

/*
 * A stack frame looks like:
 * 
 * %fp->|                               |
 *      |-------------------------------|
 *      |  Locals, temps, saved floats  |
 *      |-------------------------------|
 *      |  outgoing parameters past 6   |
 *      |-------------------------------|-\
 *      |  6 words for callee to dump   | |
 *      |  register arguments           | |
 *      |-------------------------------|  > minimum stack frame
 *      |  One word struct-ret address  | |
 *      |-------------------------------| |
 *      |  16 words to save IN and      | |
 * %sp->|  LOCAL register on overflow   | |
 *      |-------------------------------|-/
 */

#define	FRAMEALIGN		8
#define	STACKALIGN(v)		(((v) + FRAMEALIGN - 1) & -FRAMEALIGN)

#define	WINDOWSIZE		(16*SLOTSIZE)
#define	INARGSIZE		(6*SLOTSIZE)
#define	WINDOWEXTRA		(5*SLOTSIZE)

/* Generate slot offset for an argument */
#define SLOT2ARGOFFSET(_n) \
	(WINDOWSIZE + SLOTSIZE + SLOTSIZE * (_n))

/* Generate slot offset for a local (non-argument) */
#define SLOT2LOCALOFFSET(_n) \
	(-(WINDOWEXTRA + SLOTSIZE * ((maxLocal+maxStack+maxTemp) - (_n))))

/* Generate slot offset for an push */
#define SLOT2PUSHOFFSET(_n) \
	(WINDOWSIZE + SLOTSIZE + SLOTSIZE * (_n))

/* Wrap up a native call for the JIT */
#define KAFFEJIT_TO_NATIVE(_m)

/* On the sparc we need to flush the data cache after generating new code */
#include <sys/mman.h>
#define	FLUSH_DCACHE(beg, end)						\
	do {								\
		int *_p = (int *)(beg), *_e = (int *)(end);		\
		while (_p < _e) {					\
			asm volatile("iflush %0" : : "r"(_p++));	\
		} 							\
	} while (0)

#endif
