/*
 * m68k/jit.h
 * Common M68000 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Modified by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1999;
 *	to support the trampoline mechanism.
 */

#ifndef __m68k_jit_h
#define __m68k_jit_h

/**/
/* Exception handling information. */
/**/

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
        uintp	retfp;
        uintp	retpc;
} exceptionFrame;

/* Is this frame valid (ie. is it on the current stack) ? */
#define	FRAMEOKAY(f)							\
	((f) && (f)->retfp >= (uintp)TCTX(currentThread)->stackBase &&	\
	 (f)->retfp < (uintp)TCTX(currentThread)->stackEnd)

/* Get the next frame in the chain */
#define	NEXTFRAME(f)							\
	(((exceptionFrame*)(f))->retfp)

/* Extract the PC from the given frame */
#define	PCFRAME(f)		((f)->retpc)

/* Get the first exception frame from a subroutine call */
#define	FIRSTFRAME(f, o)						\
	((f) = *(exceptionFrame*)__builtin_frame_address(0))

/* Extract the object argument from given frame */
#define FRAMEOBJECT(obj, f, einfo)					\
	(obj) = *(Hjava_lang_Object**)((f) + 8)

/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(frame, info, obj)				\
	__asm__ __volatile__(						\
		"move%.l %1,%/d0\n\t"					\
		"move%.l %0,%/fp\n\t"					\
		"jmp (%2)"						\
		: : "g"(frame->retfp), "g"(obj), "a"(info.handler)	\
		: "d0", "cc", "memory")

/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

typedef struct _methodTrampoline {
        unsigned short call;
	int fixup;
	struct _methods* meth;
} methodTrampoline;

extern void m68k_do_fixup_trampoline(void);

#define FILL_IN_TRAMPOLINE(t,m)                                         \
        do {                                                            \
                (t)->call = 0x4eb9;	/* jsr abs.l */			\
                (t)->fixup = (int)m68k_do_fixup_trampoline;		\
                (t)->meth = (m);                                        \
        } while (0)

#define FIXUP_TRAMPOLINE_DECL   Method** _pmeth
#define FIXUP_TRAMPOLINE_INIT   (meth = *_pmeth)

/**/
/* Register management information. */
/**/

/* Define the register set */

/* Until we have some method of determining in the prologue which
   Rnosaveoncall registers are used by a method, it is not a win to
   have any at all.  */

#define	REGISTER_SET							\
	{ /* d0 */	0, 0, Rint, 		0, 0, 0  },		\
	{ /* d1 */	0, 0, Rint, 		0, 0, 1  },		\
	{ /* d2 */	0, 0, Rint, 		/*Rnosaveoncall*/0, 0, 2  }, \
	{ /* d3 */	0, 0, Rint, 		/*Rnosaveoncall*/0, 0, 3  }, \
	{ /* d4 */	0, 0, Rint, 		/*Rnosaveoncall*/0, 0, 4  }, \
	{ /* d5 */	0, 0, Rint, 		/*Rnosaveoncall*/0, 0, 5  }, \
	{ /* d6 */	0, 0, Rint, 		/*Rnosaveoncall*/0, 0, 6  }, \
	{ /* d7 */	0, 0, Rint, 		/*Rnosaveoncall*/0, 0, 7  }, \
	{ /* a0 */	0, 0, Rref,		0, 0, 8  },		\
	{ /* a1 */	0, 0, Rref,		0, 0, 9  },		\
	{ /* a2 */	0, 0, Rref,		/*Rnosaveoncall*/0, 0, 10 }, \
	{ /* a3 */	0, 0, Rref,		/*Rnosaveoncall*/0, 0, 11 }, \
	{ /* a4 */	0, 0, Rref,		/*Rnosaveoncall*/0, 0, 12 }, \
	{ /* a5 */	0, 0, Rref,		/*Rnosaveoncall*/0, 0, 13 }, \
	{ /* fp */	0, 0, Reserved,		0, 0, 14 },		\
	{ /* sp */	0, 0, Reserved,		0, 0, 15 },		\
	{ /* fp0 */	0, 0, Rfloat|Rdouble,	0, 0, 16 },		\
	{ /* fp1 */	0, 0, Rfloat|Rdouble,	0, 0, 17 },		\
	{ /* fp2 */	0, 0, Rfloat|Rdouble,	/*Rnosaveoncall*/0, 0, 18 }, \
	{ /* fp3 */	0, 0, Rfloat|Rdouble,	/*Rnosaveoncall*/0, 0, 19 }, \
	{ /* fp4 */	0, 0, Rfloat|Rdouble,	/*Rnosaveoncall*/0, 0, 20 }, \
	{ /* fp5 */	0, 0, Rfloat|Rdouble,	/*Rnosaveoncall*/0, 0, 21 }, \
	{ /* fp6 */	0, 0, Rfloat|Rdouble,	/*Rnosaveoncall*/0, 0, 22 }, \
	{ /* fp7 */	0, 0, Rfloat|Rdouble,	/*Rnosaveoncall*/0, 0, 23 }, 

/* Number of registers in the register set */
#define	NR_REGISTERS	24

/**/
/* Opcode generation. */
/**/
#define LABEL_FRAMESIZE(L,P) \
	{ \
		int framesize = SLOTSIZE * (maxLocal + maxStack + \
			maxTemp - maxArgs); \
			*(P) = framesize; \
	}

#define EXTRA_LABELS(P,D,L)

/* Define if generated code uses two operands rather than one */
#define	TWO_OPERAND

/**/
/* Slot management information. */
/**/

/* Size of each slot */
#define	SLOTSIZE		4

/*
 * A stack frame looks like:
 *
 *      |				|
 *	|  Arguments			|
 *      |-------------------------------|
 *	|  Return address		|
 *      |-------------------------------|
 *      |  Saved fp			|
 * fp-> |-------------------------------|
 *      |  Temporaries			|
 * sp-> |-------------------------------|
 */

/* Generate slot offset for an argument (relative to fp) */
#define SLOT2ARGOFFSET(_n)    (8 + SLOTSIZE * (_n))

/* Generate slot offset for a local (non-argument) (relative to fp) */
#define SLOT2LOCALOFFSET(_n)  (-SLOTSIZE * (maxTemp+maxLocal+maxStack - (_n)))

/* Wrap up a native call for the JIT */
#define	KAFFEJIT_TO_NATIVE(_m)

/* On the 68040 (and 060 I presume) we need to flush the instruction
   cache before running new code.  Stupidly, Motorola made cpush a
   privledged instruction, so we have to get operating system help
   for this.  Naturally, there is no standard there. 

   We are partially rescued by gcc, which must be taught how to do this
   in order to build bound-function trampolines.  This works at least
   for A/UX, Linux and NeXT.  The rest of you with 040's should
   consider fixing gcc, as you are broken there too.  

   For those with 020's and 030's, this is effectively a no-op.  */

#define FLUSH_DCACHE(beg,end)	__clear_cache((beg), (end))


#endif
