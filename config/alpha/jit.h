/*
 * alpha/jit.h
 * Common Alpha JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *     Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_jit_h
#define __alpha_jit_h

#include <stdarg.h>

struct Hjava_lang_Object;
struct Hjava_lang_Class;
struct _jmethodID;

/* Collected JIT infos used to create DEC OSF/1 Procedure descriptor for
 * exception handling, or GCJ eh_table info.
 */
typedef struct _alpha_jit_info {
    	int	rsa_size;		/* Size of Register Save Area in 64 bit words */
	int	sp_set;			/* offset of inst that set sp (in insts number) */
	int	entry_length;		/* # of insts in prologue */
	int	imask;
	int	fmask;
	int	ieee;
} alpha_jit_info_t;

/* Protected by translatorlock */
extern alpha_jit_info_t alpha_jit_info;


/**/
/* Exception handling information. */
/**/

/* exceptionFrame, NEXTFRAME(), PCFRAME(), FPFRAME() and FIRSTFRAME() are
 * move to $os/jit-md.h.  */

/* Extract the object argument from given frame */
#define FRAMEOBJECT(obj, f, einfo)					\
	/* rebuild alpha_slot2argoffset[0] as in jit-alpha.def */	\
	(obj) = *((Hjava_lang_Object**)((uintp)(f) - 8))

/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

/* The layout of this struct are known by inline assembly.  */

typedef struct _methodTrampoline {
	unsigned code[2];
	void *fixup;
	struct _jmethodID *meth;
	void **where;
} methodTrampoline;

extern void alpha_do_fixup_trampoline(void);

#define FILL_IN_TRAMPOLINE(t,m,w)					\
	do {								\
/* 0 */		(t)->code[0] = 0xa77b0008;	/* ldq $27,8($27) */	\
/* 4 */		(t)->code[1] = 0x683b0000;	/* jmp $1,($27),0 */	\
/* 8 */		(t)->fixup = alpha_do_fixup_trampoline;			\
/* 16 */	(t)->meth = (m);					\
/* 24 */	(t)->where = (w);					\
	} while (0)

#define FIXUP_TRAMPOLINE_DECL	void** _data
#define FIXUP_TRAMPOLINE_INIT	(meth = (Method*)_data[0], \
				 where = (void**)_data[1])


/* Wrap up a native call for the JIT */
#define KAFFEJIT_TO_NATIVE(_m)


/**/
/* Register management information. */
/**/

/* Define the register set */

/* The arg registers are set reserved so that they don't get tromped
   while we are setting up the outgoing arguments.  */

/* Until we have some method of determining in the prologue which
   Rnosaveoncall registers are used by a method, it is not a win to
   have any at all.  We save all of them in prologue to match Alpha
   Calling convention.  */

#define	REGISTER_SET							\
	{ /* v0  */	0, 0, Rint|Rlong|Rref,	0, 0, 0  },		\
	{ /* t0  */	0, 0, Rint|Rlong|Rref,	0, 0, 1  },		\
	{ /* t1  */	0, 0, Rint|Rlong|Rref,	0, 0, 2  },		\
	{ /* t2  */	0, 0, Rint|Rlong|Rref,	0, 0, 3  },		\
	{ /* t3  */	0, 0, Rint|Rlong|Rref,	0, 0, 4  },		\
	{ /* t4  */	0, 0, Rint|Rlong|Rref,	0, 0, 5  },		\
	{ /* t5  */	0, 0, Rint|Rlong|Rref,	0, 0, 6  },		\
	{ /* t6  */	0, 0, Rint|Rlong|Rref,	0, 0, 7  },		\
	{ /* t7  */	0, 0, Rint|Rlong|Rref,	0, 0, 8  },		\
	{ /* s0  */	0, 0, Rint|Rlong|Rref,	Rnosaveoncall, 0, 9  }, \
	{ /* s1  */	0, 0, Rint|Rlong|Rref,	Rnosaveoncall, 0, 10 }, \
	{ /* s2  */	0, 0, Rint|Rlong|Rref,	Rnosaveoncall, 0, 11 }, \
	{ /* s3  */	0, 0, Rint|Rlong|Rref,	Rnosaveoncall, 0, 12 }, \
	{ /* s4  */	0, 0, Rint|Rlong|Rref,	Rnosaveoncall, 0, 13 }, \
	{ /* s5  */	0, 0, Rint|Rlong|Rref,	Rnosaveoncall, 0, 14 }, \
	{ /* fp  */	0, 0, Reserved,		0, 0, 15 },		\
	{ /* a0  */	0, 0, Reserved|Rint|Rlong|Rref,	0, 0, 16 },	\
	{ /* a1  */	0, 0, Reserved|Rint|Rlong|Rref,	0, 0, 17 },	\
	{ /* a2  */	0, 0, Reserved|Rint|Rlong|Rref,	0, 0, 18 },	\
	{ /* a3  */	0, 0, Reserved|Rint|Rlong|Rref,	0, 0, 19 },	\
	{ /* a4  */	0, 0, Reserved|Rint|Rlong|Rref,	0, 0, 20 },	\
	{ /* a5  */	0, 0, Reserved|Rint|Rlong|Rref,	0, 0, 21 },	\
	{ /* t8  */	0, 0, Rint|Rlong|Rref,	0, 0, 22 },		\
	{ /* t9  */	0, 0, Rint|Rlong|Rref,	0, 0, 23 },		\
	{ /* t10 */	0, 0, Rint|Rlong|Rref,	0, 0, 24 },		\
	{ /* t11 */	0, 0, Rint|Rlong|Rref,	0, 0, 25 },		\
	{ /* ra  */	0, 0, Rint|Rlong|Rref,	0, 0, 26 },		\
	{ /* t12/pv */	0, 0, Rint|Rlong|Rref,	0, 0, 27 },		\
	{ /* at  */	0, 0, Reserved,		0, 0, 28 },		\
	{ /* gp  */	0, 0, Reserved,		0, 0, 29 },		\
	{ /* sp  */	0, 0, Reserved,		0, 0, 30 },		\
	{ /* zero */	0, 0, Reserved,		0, 0, 31 },		\
	{ /* f0  */	0, 0, Rfloat|Rdouble,	0, 0, 32 },		\
	{ /* f1  */	0, 0, Rfloat|Rdouble,	0, 0, 33 },		\
	{ /* f2  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 34 }, \
	{ /* f3  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 35 }, \
	{ /* f4  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 36 }, \
	{ /* f5  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 37 }, \
	{ /* f6  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 38 }, \
	{ /* f7  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 39 }, \
	{ /* f8  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 40 }, \
	{ /* f9  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 41 }, \
	{ /* f10 */	0, 0, Rfloat|Rdouble,	0, 0, 42 },		\
	{ /* f11 */	0, 0, Rfloat|Rdouble,	0, 0, 43 },		\
	{ /* f12 */	0, 0, Rfloat|Rdouble,	0, 0, 44 },		\
	{ /* f13 */	0, 0, Rfloat|Rdouble,	0, 0, 45 },		\
	{ /* f14 */	0, 0, Rfloat|Rdouble,	0, 0, 46 },		\
	{ /* f15 */	0, 0, Rfloat|Rdouble,	0, 0, 47 },		\
	{ /* f16 */	0, 0, Reserved|Rfloat|Rdouble,	0, 0, 48 },	\
	{ /* f17 */	0, 0, Reserved|Rfloat|Rdouble,	0, 0, 49 },	\
	{ /* f18 */	0, 0, Reserved|Rfloat|Rdouble,	0, 0, 50 },	\
	{ /* f19 */	0, 0, Reserved|Rfloat|Rdouble,	0, 0, 51 },	\
	{ /* f20 */	0, 0, Reserved|Rfloat|Rdouble,	0, 0, 52 },	\
	{ /* f21 */	0, 0, Reserved|Rfloat|Rdouble,	0, 0, 53 },	\
	{ /* f22 */	0, 0, Rfloat|Rdouble,	0, 0, 54 },		\
	{ /* f23 */	0, 0, Rfloat|Rdouble,	0, 0, 55 },		\
	{ /* f24 */	0, 0, Rfloat|Rdouble,	0, 0, 56 },		\
	{ /* f25 */	0, 0, Rfloat|Rdouble,	0, 0, 57 },		\
	{ /* f26 */	0, 0, Rfloat|Rdouble,	0, 0, 58 },		\
	{ /* f27 */	0, 0, Rfloat|Rdouble,	0, 0, 59 },		\
	{ /* f28 */	0, 0, Rfloat|Rdouble,	0, 0, 60 },		\
	{ /* f29 */	0, 0, Rfloat|Rdouble,	0, 0, 61 },		\
	{ /* f30 */	0, 0, Reserved,		0, 0, 62 },		\
	{ /* fzero */	0, 0, Reserved,		0, 0, 63 },


/* Number of registers in the register set */
#define	NR_REGISTERS	64

/**/
/* Opcode generation. */
/**/

/* Extra label types */
#define Llong21		(Larchdepend+0)	/* Label is 21 bits long */
#define Llong16x16	(Larchdepend+1)	/* Label is split into 16 bit parts */
#define Llong16		(Larchdepend+2)	/* Label is 16 bits long */
#define Lrsa		(Larchdepend+3)	/* Register Save Area */

/* if you change LABEL_Lframe() or LABEL_Lrsa() change also all
 * REGISTER_JIT_METHOD() in config/alpha/.../md.c  */
#define LABEL_Lframe(P,V,L)						\
	do {								\
		int framesize =						\
			STACKALIGN(SLOTSIZE * (maxLocal + maxStack +	\
				maxTemp + (maxArgs < 6 ? maxArgs : 6) +	\
				alpha_jit_info.rsa_size +		\
				(maxPush <= 6 ? 0 : maxPush - 6)));	\
		assert(framesize < 0x8000);				\
		if (((L)->type & Ltypemask) == Lnegframe)		\
			framesize = -framesize;				\
		*(short *)(P) += framesize;				\
	} while (0)
#define LABEL_Lrsa(P,V,L)						\
	do {								\
		int frameoffset =					\
			SLOTSIZE * (maxPush <= 6 ? 0 : maxPush - 6);	\
		assert(frameoffset < 0x8000);				\
		*(short *)(P) += frameoffset;				\
	} while (0)
#define	LABEL_Llong21(P,V,L)	(P)[0] = (((P)[0] & 0xFFE00000) | (((V) / 4) & 0x001FFFFF))
#define	LABEL_Llong16x16(P,V,L)	(P)[1] = ((P)[1] & 0xFFFF0000) | ((V) & 0xFFFF); \
				(P)[0] = ((P)[0] & 0xFFFF0000) | (((V) >> 16) & 0xFFFF)
#define	LABEL_Llong16(P,V,L)	*(short*)(P) = (V)

#define EXTRA_LABELS(P,D,L)						\
	case Lframe:		LABEL_Lframe(P,D,L);		break;	\
	case Lnegframe:		LABEL_Lframe(P,D,L);		break;	\
	case Lrsa:		LABEL_Lrsa(P,D,L);		break;	\
	case Llong21:		LABEL_Llong21(P,D,L);		break;	\
	case Llong16x16:	LABEL_Llong16x16(P,D,L);	break;	\
	case Llong16:		LABEL_Llong16(P,D,L);		break;


/**/
/* Slot management information. */
/**/

/* Size of each slot */
#define	SLOTSIZE		8

/*
 * A stack frame looks like:
 *
 *	|  Arguments above 6            |
 * fp-> |-------------------------------|
 *	|  Spill for up to 6 args       |
 *      |-------------------------------|
 *      |  Locals, temps                |
 *      |-------------------------------|
 *      |  Register save area           |
 *      |-------------------------------|
 *      |  outgoing parameters past 6   |
 * sp-> |-------------------------------|
 */

#define	FRAMEALIGN		16
#define	STACKALIGN(v)		(((v) + FRAMEALIGN - 1) & -FRAMEALIGN)

/* We push arguments low to high. */
#define	PUSHARG_FORWARDS	1


/* Generate slot offset for an argument (relative to fp) */
#define SLOT2ARGOFFSET(_n)	alpha_slot2argoffset[_n]
extern int *alpha_slot2argoffset;

/* Generate slot offset for a local (non-argument) (relative to fp) */
#define SLOT2LOCALOFFSET(_n)	alpha_slot2argoffset[_n]
/**
#define SLOT2LOCALOFFSET(_n) \
	(-SLOTSIZE * ((_n) + (maxArgs < 6 ? maxArgs : 6) + 3))
 **/

/* Generate slot offset for an push (relative to $sp); _n >= 6 */
#define SLOT2PUSHOFFSET(_n)	(SLOTSIZE * ((_n) - 6))


/* On the Alpha we need to execute an instruction memory barrier before
   running new code. 

   For all the jillions of macros the OSF/1 assembler knows, you'd think
   "imb" would be one of them.  It isn't worth tracking down some pal.h
   that's at different spots on different systems just for this, so we
   hardcode the number.  It's not like _that_ is ever going to change.  */

#define FLUSH_DCACHE(beg, end) \
	__asm__ __volatile__("call_pal 134" : : : "memory")


#endif
