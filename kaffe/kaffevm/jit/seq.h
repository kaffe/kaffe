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

typedef struct _sequence {
	ifunc			func;
	union {
		jword		iconst;
		jlong		lconst;
		jdouble		fconst;
		struct _label_* labconst;
		Method*		methconst;
		struct {
			struct SlotInfo*	slot;
			struct _sequence* seq;
		}		s;
	} u[5];
	struct _sequence*	next;
} sequence;

void initSeq(void);
sequence* nextSeq(void);

extern sequence*		firstSeq;
extern sequence*		lastSeq;
extern sequence*		currSeq;

#define	ALLOCSEQNR		1024

#define	seq(s)			((s)->insn)

#define	seq_slot(_s, _i)	((_s)->u[_i].s.slot)
#define	seq_seq(_s, _i)		((_s)->u[_i].s.seq)
#define	seq_value(_s, _i)	((_s)->u[_i].iconst)
#define	seq_dst(_s)		seq_slot(_s, 0)

#endif
