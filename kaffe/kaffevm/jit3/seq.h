/* seq.h
 * Pseudo instruction sequences.
 *
 * Copyright (c) 1996, 1997, 2003
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __seq_hh
#define __seq_hh

#include "machine.h"

struct _sequence;
struct _label_;

typedef void (*ifunc)(struct _sequence*);

/** maximum number of arguments for one sequence */
#define	SEQSLOTS		5

/**
 * a struct _sequence is one instruction of the intermediate language of the jitter.
 *
 * func    - the function that is able to emit native code for this instruction
 * u       - parameters for this instruction, their exact meaning depends on func
 * type    - type of the instruction, one of the T* constants defined in slots.h 
 * refed   - true iff generation of native code for this instruction is necessary  
 * lastuse - bitmask of the slots not being accessed after this instruction, bits
 *           are used as an index into u
 * next    - pointer to the next instruction following this one
 *
 * These intermediate instructions are generated while reading the java bytecode
 * and are translated into native code afterwards. This translation is done
 * per basic block rather than per method.
 */
typedef struct _sequence {
	ifunc			func;
	union {
		jvalue		value;
		struct _label_* labconst;
		Method*		methconst;
		struct SlotData** smask;
		struct SlotData* slot;
	} u[SEQSLOTS];
	uint8			type;
	uint8			refed;
	uint32			lastuse;
	jitflags		jflags;
	struct _sequence*	next;
} sequence;

void initSeq(void);
sequence* nextSeq(void);

/*
 * These track the pool of sequence objects allcoated by the jitter. 
 * The set of valid sequence objects is the sublist between firstSeq
 * and currSeq, exclusive.
 */
extern sequence*		firstSeq;
extern sequence*		lastSeq;
extern sequence*		currSeq;

/*
 * sequence currently being created (return value of last call to nextSeq).
 */
extern sequence* 		activeSeq;

#define	ALLOCSEQNR		1024

typedef struct _sequencechunk {
	struct _sequencechunk *next;
	sequence data[ALLOCSEQNR];
} sequencechunk;

#define	seq(s)			((s)->insn)

/**
 * Evaluates to a SlotData* parameter of a struct _sequence.
 *
 * @param _s the sequence whose parameter is to be returned.
 * @param _i idx of the parameter.
 */
#define	seq_slot(_s, _i)	((_s)->u[_i].slot)

/**
 * Evaluates to an int parameter of a struct _sequence.
 *
 * @param _s the sequence whose parameter is to be returned.
 * @param _i idx of the parameter.
 */
#define	seq_value(_s, _i)	((_s)->u[_i].value.i)

/**
 * Evaluates to the slot that contains the result of a sequence.
 *
 * @param _s the sequence whose destination slot is to be returned.
 */
#define	seq_dst(_s)		seq_slot(_s, 0)

/**
 * Evaluates to the slot that contains the return value of a sequence.
 * 
 * @param _s the sequence whose return value is to be returned.
 */
#define	seq_ret(_s)		seq_slot(_s, 0)

#endif
