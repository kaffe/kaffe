/*
 * i386/jit.h
 * Common i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_jit_h
#define __i386_jit_h

/**/
/* Exception handling information. */
/**/

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
        uintp	retbp;
        uintp	retpc;
} exceptionFrame;

/* Get the next frame in the chain */
#define	NEXTFRAME(f)	(((exceptionFrame*)(f))->retbp)

/* Extract the PC & FP from the given frame */
#define	PCFRAME(f)	((f)->retpc-1)
#define	FPFRAME(f)	((f)->retbp)

/* Extract a local argument from given frame */
#define	FRAMEOBJECT(f)	(*(Hjava_lang_Object**)(f->retbp + 8))

/* Get the first exception frame from a subroutine call */
#define	FIRSTFRAME(f, o) (f) = *((exceptionFrame*)(((uintp)&(o))-8))

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(F, H, O)					\
        asm("   movl %2,%%eax						\n\
		movl %0,%%ebp						\n\
		jmp *%1							\n\
		" : : "g" (F), "r" (H), "g" (O) : "eax")


/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#define	PACKED
#endif

#if defined(_MSC_VER)
#pragma pack ( push, 1 )
#endif

extern void i386_do_fixup_trampoline(void);

typedef struct _methodTrampoline {
	unsigned char call PACKED;
	int fixup PACKED;
	struct _methods* meth PACKED;
} methodTrampoline;

#if defined(_MSC_VER)
#pragma pack ( pop )
#endif

#define FILL_IN_TRAMPOLINE(t,m)						\
	do {								\
		(t)->call = 0xe8;					\
		(t)->fixup = (int)i386_do_fixup_trampoline - (int)(t) - 5; \
		(t)->meth = (m);					\
	} while (0)

#define FIXUP_TRAMPOLINE_DECL	Method** _pmeth
#define FIXUP_TRAMPOLINE_INIT	(meth = *_pmeth)

/**/
/* Register management information. */
/**/

/* Define the register set */
#define	REGISTER_SET							\
	{ /* eax */	0, 0, Rint|Rsubint|Rref,0, 0, 0    },		\
	{ /* ecx */	0, 0, Rint|Rsubint|Rref,0, 0, 1    },		\
	{ /* edx */	0, 0, Rint|Rsubint|Rref,0, 0, 2    },		\
	{ /* ebx */	0, 0, Rint|Rsubint|Rref,Rglobal|Rnosaveoncall, 0, 3 },\
	{ /* esp */	0, 0, Reserved,		0, 0, 4    },		\
	{ /* ebp */	0, 0, Reserved,		0, 0, 5    },		\
	{ /* esi */	0, 0, Rint|Rref,	Rglobal|Rnosaveoncall, 0, 6 },\
	{ /* edi */	0, 0, Rint|Rref,	Rglobal|Rnosaveoncall, 0, 7 },\
	{ /* f0  */	0, 0, Rfloat|Rdouble,	Rreadonce, 0, 8    },

/* Number of registers in the register set */
#define	NR_REGISTERS	9

/* Number of function globals in register set - JIT3 */
#define	NR_GLOBALS	3

/**/
/* Opcode generation. */
/**/

#define LABEL_Lframe(P,D,L)						\
	{								\
		int framesize = SLOTSIZE * (maxLocal + maxStack +	\
			maxTemp - maxArgs);				\
		*(uint32*)(P) = framesize;				\
	}

#define	EXTRA_LABELS(P,D,L)						\
	case Lframe:	LABEL_Lframe(P,D,L);	break;

/* Define if generated code uses two operands rather than one */
#define	TWO_OPERAND

/**/
/* Slot management information. */
/**/

/* Size of each slot */
#define	SLOTSIZE	4

/* Generate slot offset for an argument */
#define SLOT2ARGOFFSET(_n)	(8 + SLOTSIZE * (_n))

/* Generate slot offset for a local (non-argument) */
#if defined(JIT3)
#define	SLOT2LOCALOFFSET(N)	(-SLOTSIZE * ((N) - maxArgs + 1))
#else
#define SLOT2LOCALOFFSET(_n)	(-SLOTSIZE * (maxTemp+maxLocal+maxStack - (_n)))
#endif

#if defined(JIT3)
/* Generate the slot offset to the stack limit */
#define	STACK_LIMIT()		SLOT2ARGOFFSET(maxArgs)
#endif

/* Wrap up a native call for the JIT */
#define KAFFEJIT_TO_NATIVE(_m)

/* We don't have to flush the code out of cache on the i386 */
#define	FLUSH_DCACHE(beg, end)	/* Do nothing */

/* The Pentium optimization manual recommends these */
#define CALLTARGET_ALIGNMENT	16

#endif
