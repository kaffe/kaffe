/* seq.h
 * Pseudo instruction sequences.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __seq_hh
#define __seq_hh

struct _sequence;
struct _label_;

typedef void (*ifunc)(struct _sequence*);

#define	SEQSLOTS		5

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
	struct _sequence*	next;
} sequence;

void initSeq(void);
sequence* nextSeq(void);

extern sequence*		firstSeq;
extern sequence*		lastSeq;
extern sequence*		currSeq;

#define	ALLOCSEQNR		1024

#define	seq(s)			((s)->insn)

#define	seq_slot(_s, _i)	((_s)->u[_i].slot)
#define	seq_value(_s, _i)	((_s)->u[_i].value.i)
#define	seq_dst(_s)		seq_slot(_s, 0)
#define	seq_ret(_s)		seq_slot(_s, 0)

#endif
