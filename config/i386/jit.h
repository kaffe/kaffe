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

#include "config.h"

#if defined(HAVE_VALGRIND_MEMCHECK_H)
#include <valgrind/memcheck.h>
#endif

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
#define	FRAMEOBJECT(obj, f, einfo)					\
	(obj) = (*(Hjava_lang_Object**)((f) + 8))

/* Get the first exception frame from a subroutine call */
#define	FIRSTFRAME(f, o)						\
	((f) = *(exceptionFrame*)__builtin_frame_address(0))

/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

#if defined(_MSC_VER)
#pragma pack ( push, 1 )
#endif

extern void i386_do_fixup_trampoline(void);

typedef struct _methodTrampoline {
	unsigned char call;
	int fixup PACKED;
	struct _jmethodID* meth PACKED;
	void** where PACKED;
} methodTrampoline;

#if defined(_MSC_VER)
#pragma pack ( pop )
#endif

/* NB: the E8 jmp instruction uses relative addressing */
#define FILL_IN_TRAMPOLINE(t,m,w)					\
	do {								\
		(t)->call = 0xe8;					\
		(t)->fixup = (int)i386_do_fixup_trampoline - (int)(t) - 5; \
		(t)->meth = (m);					\
		(t)->where = (w);					\
	} while (0)

#define FIXUP_TRAMPOLINE_DECL	void** _data
#define FIXUP_TRAMPOLINE_INIT	(meth = (Method*)_data[0], \
				 where = (void**)_data[1])

/**/
/* Register management information. */
/**/
#define HAVE_FLOATING_POINT_STACK

/* Define the register set */
#define	REGISTER_SET							\
	{ /* eax */	NULL, 0, Rint|Rsubint|Rref,0, 0, 0    },		\
	{ /* ecx */	NULL, 0, Rint|Rsubint|Rref,0, 0, 1    },		\
	{ /* edx */	NULL, 0, Rint|Rsubint|Rref,0, 0, 2    },		\
	{ /* ebx */	NULL, 0, Rint|Rsubint|Rref,Rglobal|Rnosaveoncall, 0, 3 },\
	{ /* esp */	NULL, 0, Reserved,		0, 0, 4    },		\
	{ /* ebp */	NULL, 0, Reserved,		0, 0, 5    },		\
	{ /* esi */	NULL, 0, Rint|Rref,	Rglobal|Rnosaveoncall, 0, 6 },\
	{ /* edi */	NULL, 0, Rint|Rref,	Rglobal|Rnosaveoncall, 0, 7 },\
	{ /* f0  */	NULL, 0, Rfloat|Rdouble,	Rreadonce, 0, 8    },

/* Number of registers in the register set */
#define	NR_REGISTERS	9

/* Number of function globals in register set - JIT3 */
#define	NR_GLOBALS	3

/**/
/* Opcode generation. */
/**/

/* Extra label types */
#define Llong8		(Larchdepend+0)	/* Label is 8 bits long */

#define LABEL_Lframe(P,D,L)						\
	{								\
		int framesize = SLOTSIZE * (maxLocal + maxStack +	\
			maxTemp - maxArgs);				\
		*(uint32*)(P) = framesize;				\
	}
#define LABEL_Llong8(P,V,L)	*(char*)(P) = (V)

#define	EXTRA_LABELS(P,D,L)						\
	case Llong8:	LABEL_Llong8(P,D,L);	break;			\
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

#if defined(JIT3) && !defined(HAVE_GCJ_SUPPORT)
/* Generate the slot offset to the stack limit */
#define	STACK_LIMIT()		SLOT2ARGOFFSET(maxArgs)
#endif

/* Wrap up a native call for the JIT */
#define KAFFEJIT_TO_NATIVE(_m)

/* We don't have to flush the code out of cache on the i386 */
#if defined(HAVE_VALGRIND_MEMCHECK_H)
#define FLUSH_DCACHE(beg, end)  VALGRIND_DISCARD_TRANSLATIONS(beg, end)
#else
#define	FLUSH_DCACHE(beg, end)	/* Do nothing */
#endif

/* The Pentium optimization manual recommends these */
#define CALLTARGET_ALIGNMENT	16

#if defined (HAVE_GCJ_SUPPORT)

/*
 * If we provide gcj support, we must include some architecture specific
 * information about gcc here.
 */

/* from egcs/gcc/config/i386/i386.h 
 *
 * How to renumber registers for dbx and gdb.
 *
 *   #define CALL_USED_REGISTERS \
 *    ax,dx,cx,bx,si,di,bp,sp,st,st1,st2,st3,st4,st5,st6,st7,arg,flags,fpsr   \
 *  {  1, 1, 1, 0, 0, 0, 0, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,    1,   1 }
 *
 */

/* {0,2,1,3,6,7,4,5,12,13,14,15,16,17}  */
#define DBX_REGISTER_NUMBER(n) \
((n) == 0 ? 0 : \
 (n) == 1 ? 2 : \
 (n) == 2 ? 1 : \
 (n) == 3 ? 3 : \
 (n) == 4 ? 6 : \
 (n) == 5 ? 7 : \
 (n) == 6 ? 4 : \
 (n) == 7 ? 5 : \
 (n) + 4)

#define DWARF_FRAME_REGISTERS 		17
#define DWARF_FRAME_RETURN_COLUMN	8

/* offset of CFA from cfa register */
#define CFA_OFFSET			8
#define CFA_REGISTER			DBX_REGISTER_NUMBER(/* gcc_esp */ 7)

/* offset of return address from CFA */
#define RETADDR_SAVED_OFFSET		-4
#define CFA_SAVED_OFFSET		-8

/*
 * Frame layout on the x86 is like so
 *
 *	CFA	-> +0  			Canonical Frame Address -- by definition
 *		   -4  			caller's ret pc
 *	ebp	-> -8  			caller's ebp
 *		   -12 			local 1
 *		   -16 			local 2
 *		   -20 			local 3
 *		   ...	
 *		   -8 -meth->framesize	local n
 *		   -12-meth->framesize  caller's %edi
 *		   -16-meth->framesize  caller's %esi
 *		   -20-meth->framesize  caller's %ebx
 *	esp	->
 *
 */

struct kaffe_frame_descriptor;

/* 
 * Establish JIT3 callee-saved register information for i386
 * We must tell gcj where esi, edi, and ebx have been stored
 * on this frame, as an offset from the CFA.
 * In addition, we must tell it where the caller's esp was stored
 *
 * Compare prologue_xxx in jit3-i386.def
 */
void arch_get_frame_description(int framesize, 
	struct kaffe_frame_descriptor frame_desc[], 
	int *n);

/* See libgcj's MAKE_THROW_FRAME */
#define FAKE_THROW_FRAME()                                      \
do {                                                            \
        register unsigned long _ebp = frame->retbp;             \
        register unsigned char *_eip = (unsigned char *)(frame->retpc); \
        _eip += 2;                                              \
								\
        asm volatile ("mov %0, (%%ebp); mov %1, 4(%%ebp)"       \
                  : : "r"(_ebp), "r"(_eip));                    \
} while (0)

/* The following information is needed to compile gcj/eh.c */
#define STACK_GROWS_DOWNWARD   1
typedef void *word_type;

#undef INCOMING_REGNO   /* only for sparc */
#undef EH_TABLE_LOOKUP  /* only for some archs */

#define DWARF2_UNWIND_INFO

/* as far as gcc is concerned, we're single-threaded XXX FIXME */
#undef __GTHREADS

/* End of eh.c specific information */

#endif

#endif
