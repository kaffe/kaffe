/*
 * arm/jit.h
 * Common ARM JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm_jit_h
#define __arm_jit_h

/* The stack backtrace structure is as follows:
fp points to here:    |  save code pointer  |      
		      |  return link value  |      [fp - 4 ]
                      |  return sp value    |      [fp - 8 ]
exceptionFrame * ->   |  return fp value    |      [fp - 12]
                     [|  saved r10 value    |]
                     [|  saved r9 value     |]
                     [|  saved r8 value     |]
                     [|  saved r7 value     |]
                     [|  saved r6 value     |]
                     [|  saved r5 value     |]
                     [|  saved r4 value     |]
                     [|  saved f7 value     |]     three words
                     [|  saved f6 value     |]     three words
                     [|  saved f5 value     |]     three words
                     [|  saved f4 value     |]     three words
  r0-r3 are not normally saved in a C function.  */

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
        char*   retfp;		/* should be "addr_t" */
        char*   retsp;
        char*   retpc;
} exceptionFrame;

/* Get the next frame in the chain */
#define NEXTFRAME(f)                                                    \
        ((uintp)((exceptionFrame*)(f))->retfp - sizeof(exceptionFrame))

/* Extract the PC from the given frame */
#define PCFRAME(f)                                                      \
        ((uintp)((f)->retpc - 4))

/* Get the first exception frame from a subroutine call */
#define FIRSTFRAME(f, o)                                                \
	({ char* v ; asm volatile("mov %0,fp" : "=r" (v)) ; 	\
	    (f) = *(exceptionFrame*)(v - sizeof(exceptionFrame)); })

/* Extract the object argument from given frame */
#define FRAMEOBJECT(obj, f, einfo)	do {				\
	const char *str;						\
	int maxLocal, maxStack, maxArgs, maxTemp;			\
	/* Set up the necessary state for the SLOT2 macros to work	\
	 * in local variables to not get the translator lock.  */	\
	maxLocal = einfo.method->localsz;				\
	maxStack = einfo.method->stacksz;				\
	str = einfo.method->signature->data;				\
	maxArgs = sizeofSig(&str, false);				\
	maxTemp = MAXTEMPS - 1;						\
	/* NB: we assume that the JIT will have				\
	 * spilled the 'this' object in the				\
	 * stack location for slot zero.				\
	 */								\
	obj = ((Hjava_lang_Object**)(f))[SLOT2ARGOFFSET(0)/SLOTSIZE];	\
} while (0)

	
/* Call the relevant exception handler (rewinding the stack as
   necessary). */
#define CALL_KAFFE_EXCEPTION(frame, info, object)			\
	asm volatile ("							\n\
		mov r0, %0						\n\
		mov fp, %2						\n\
		mov pc, %1						\n\
	" : : "r" (object), "r" (info.handler), "r" (frame->retfp) : "r0")

/**/
/* Method dispatch.  */
/**/

#define HAVE_TRAMPOLINE

//
// In an environment using shared libraries, the trampoline address is
// a 32-bit signed address, but the BL instruction class only supports
// 24 bit signed offsets. On entry to the soft_fixup_trampoline code, we 
// need to have r0 pointing to the "meth" field.
// On entry to arm_do_fixup_trampoline, we need to have ip (=r12) pointing
// there.  arm_do_fixup_trampoline will then invoke soft_fixup_trampoline
//
// We accomplish this by using a PC-relative move to initialize
// ip to the address of "meth", and then a PC relative load
// to load the contents of "trampaddr". This makes the trampoline
// much larger, but it's not clear what else to do.
//
// Note that using the PC as a source register on the ARM returns
// PC+8 as the value (exposed pipeline). Bleh. 
//
// Dirk & Godmar
//

typedef struct _methodTrampoline {
	unsigned int loadlr	__attribute__((packed));
	unsigned int branch	__attribute__((packed));
	struct _methods* meth	__attribute__((packed));
	void** trampaddr	__attribute__((packed));
} methodTrampoline;

extern void arm_do_fixup_trampoline(void);

//
// Notes: 0xE1A0000F = mov ip, pc
//        0xE59FF000 = ldr pc, [pc, #0]
// NB:    0xE59FF004 = ldr pc, [pc, #4]

#define FILL_IN_TRAMPOLINE(T,M)			\
	do {					\
	  (T)->loadlr = 0xE1A0c00F; 		\
	  (T)->branch = 0xE59FF000; 		\
	  (T)->meth = (M);			\
	  (T)->trampaddr = arm_do_fixup_trampoline; \
	} while(0)

/* _pmeth will be $ip from FILL_IN_TRAMPOLINE */
#define FIXUP_TRAMPOLINE_DECL   Method** _pmeth

/* The actual method to fix up is then *_pmeth */
#define FIXUP_TRAMPOLINE_INIT   (meth = *_pmeth)

/**/
/* Register management information. */
/**/

/* Define the register set */
	// slot, ctype, type, flags, used, regno
#define	REGISTER_SET							\
	{ /* r0 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 0    },		\
	{ /* r1 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 1    },		\
	{ /* r2 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 2    },		\
	{ /* r3 */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 3    },		\
	{ /* r4 */	0, 0, Rint|Rref,	0, 0, 4    },		\
	{ /* r5 */	0, 0, Rint|Rref,	0, 0, 5    },		\
	{ /* r6 */	0, 0, Rint|Rref,	0, 0, 6    },		\
	{ /* r7 */	0, 0, Rint|Rref,	0, 0, 7    },		\
	{ /* r8 */	0, 0, Rint|Rref,	0, 0, 8    },		\
	{ /* r9 */	0, 0, Reserved,		0, 0, 9    },	/* Static base */	\
	{ /* r10 */	0, 0, Rint|Rref,	0, 0, 10   },	/* Stack limit */	\
	{ /* fp */	0, 0, Reserved,		0, 0, 11   },		\
	{ /* ip */	0, 0, Rint|Rref,	Rnosaveoncall, 0, 12   },		\
	{ /* sp */	0, 0, Reserved,		0, 0, 13   },		\
	{ /* lr */	0, 0, Reserved,		0, 0, 14   },		\
	{ /* pc */	0, 0, Reserved,		0, 0, 15   },		\
	{ /* f0  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 0    },		\
	{ /* f1  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 1    },		\
	{ /* f2  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 2    },		\
	{ /* f3  */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 3    },		\
	{ /* f4  */	0, 0, Reserved,		0, 0, 4    },		\
	{ /* f5  */	0, 0, Reserved,		0, 0, 5    },		\
	{ /* f6  */	0, 0, Reserved,		0, 0, 6    },		\
	{ /* f7  */	0, 0, Reserved,		0, 0, 7    },

/* Number of registers in the register set */
#define	NR_REGISTERS	24

/**/
/* Opcode generation. */
/**/

/* Extra label types */
#define Llong26		(Larchdepend+0)	/* Label is 26 bits long */
#define Llong8x8x8x8	(Larchdepend+1)	/* Label is split into 4x8 bit parts */


#define	LABEL_Llong26(P,V,L) \
        { \
          assert(((V) & 0xFF000000) == 0 || ((V) & 0xFF000000) == 0xff000000); \
	 (P)[0] = ((P)[0] & 0xFF000000)|(((V) >> 2) & 0x00FFFFFF); \
        }

#define	LABEL_Llong8x8x8x8(P,V,L)					\
	(P)[0] = ((P)[0] & 0xFFFFFF00)|((V) & 0xFF);			\
	(P)[1] = ((P)[1] & 0xFFFFFF00)|(((V)>> 8) & 0xFF);		\
	(P)[2] = ((P)[2] & 0xFFFFFF00)|(((V)>>16) & 0xFF);		\
	(P)[3] = ((P)[3] & 0xFFFFFF00)|(((V)>>24) & 0xFF)

#define EXTRA_LABELS(P,D,L)						\
	case Lframe:		LABEL_Lframe(P,D,L);		break;	\
	case Llong26:		LABEL_Llong26(P,D,L);		break;	\
	case Llong8x8x8x8:	LABEL_Llong8x8x8x8(P,D,L);	break;

/**/
/* Slot management information. */
/**/

/* Size of each slot */
#define	SLOTSIZE	4

//
// We push ..BFP|BIP|BLR|BR4|BR5|BR6|BR7|BR8|BR9|BR10
//
// We save 10 registers
#define REGISTERS_SAVED (10)

//
// First four arguments are in registers, and then we need
// to compensate for the save code pointer, which is stored
// at (fp+4). The first argument is at (FP+8).We allocate
// slots for the first four arguments in the "locals"
// region.
//
/* Generate slot offset for a local (non-argument) */
//

/* 
 * We reserve (maxLocal + maxStack + maxTemp - X) slots on the stack,
 * where X is the number of arguments to this function past the fourth.
 * Those slots are accessible via positive offsets from the FP.
 * The macro maps a slot number to a memory offset.
 * We skip those memory offsets that correspond to slots that are
 * arguments past the forth.
 *
 * NB: Kaffe's jit assumes that SLOT2LOCALOFFSET is monotonic in (N), i.e.,
 * OFF(N) > OFF(M) iff N > M.  
 *
 * XXX: simplify this macro
 */
#define SLOT2LOCALOFFSET(N)  						 \
	(- SLOTSIZE * (REGISTERS_SAVED + 				 \
		(maxLocal+maxStack+maxTemp-((maxArgs <= 4)?0:maxArgs-4)) \
			- ((N < 4 || maxArgs <= 4) ? N : (N - maxArgs + 4))))

/*
 * FP[1] contains the first extra argument.
 */
#define SLOT2ARGOFFSET(N) \
	((N) >= 4 ? (SLOTSIZE * ((N) - 4 + 1)) : (SLOT2LOCALOFFSET(N)))

/* This may waste some space if the required frame size does not
 * exactly fit in a shifted 8bit value.
 */
#define LABEL_Lframe(P,V,L)						\
	{								\
		int shift = 0;						\
		int val = (maxLocal + maxStack + maxTemp);	        \
		int orig = val;                                         \
		if (maxArgs > 4) { val -= maxArgs - 4;}                 \
		val *= SLOTSIZE;                                        \
		while ((val & 0xFFFFFF00) != 0) {			\
			val = val >> 2;					\
			shift += 2;					\
		}							\
		shift /= 2; shift = (32 - shift); shift &= 0xf;         \
		*(P) = (*(P) & 0xFFFFF000) | (shift<<8) | val;		\
	}



/* Wrap up a native call for the JIT */
#define KAFFEJIT_TO_NATIVE(M)

/* For now, ignore BEG and END since we flush the complete I&D cache FIXME */
extern void flush_dcache(void);
#define	FLUSH_DCACHE(BEG, END)	flush_dcache()


#endif
