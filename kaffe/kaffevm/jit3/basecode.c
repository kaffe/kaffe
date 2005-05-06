/*
 * basecode.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "slots.h"
#include "seq.h"
#include "md.h"
#include "registers.h"
#include "icode.h"
#include "basecode.h"
#include "labels.h"
#include "codeproto.h"
#include "kaffe_jni.h"

void
writeslot(sequence* seq, int slot, SlotInfo* data, int len)
{
	SlotData* sdata;

	if (data == 0) {
		seq->u[slot].slot = NULL;
		return;
	}
	sdata = data->slot;

	/* We look for the last use of this SlotInfo and locate the sequence
	 * which used it.  We mark the sequence so we know that this was the
	 * last use of the associated SlotData so we can avoid writing back
	 * registers which are dead.  Note that we can only do this if we're
	 * not about to write to it (in which case it's still live).
	 */
	if (sdata->rseq == seq) {
		/* Skip */
	}
	else if (len == 2 && data[1].slot->rseq == seq) {
		/* Skip */
	}
	else {
		lastuse_slot(data, len);
	}

	seq->u[slot].slot = sdata;

	/* This slot writes a value - but this might not be used so we
	 * clear the 'refed' flag an only set it if someone reads what
	 * was written.
	 */
	seq->refed = 0;
	sdata->wseq = seq;
	if (len == 2) {
		data[1].slot->wseq = seq;
	}
	assert(sdata->rseq == 0 || sdata->rseq == seq);
}

void
readslot(sequence* seq, int slot, SlotInfo* data, int len)
{
	SlotData* sdata;

	if (data == 0) {
		seq->u[slot].slot = NULL;
		return;
	}

	sdata = data->slot;
	seq->u[slot].slot = sdata;

	/* We note on the slot which sequence used this slot.  This
	 * allows us to determine liveliness when the slot get's
	 * overwritten.
	 */
	sdata->rseq = seq;
	sdata->rseqslot = slot;
	if (len == 2) {
		data[1].slot->rseq = seq;
		data[1].slot->rseqslot = slot;
	}

	/* Note on the sequence which wrote this value that it was refed. */
	if (sdata->wseq != 0) {
		sdata->wseq->refed = 1;
	}
	if (len == 2 && data[1].slot->wseq != 0) {
		data[1].slot->wseq->refed = 1;
	}
}

#define	ASSIGNSLOT_R(E, D, S)	readslot((E), (D), (S), 1);
#define	ASSIGNSLOT_W(E, D, S)	writeslot((E), (D), (S), 1);

#define	LASSIGNSLOT_R(E, D, S)	readslot((E), (D), (S), 2);
#define	LASSIGNSLOT_W(E, D, S)	writeslot((E), (D), (S), 2);

#define	SEQ_TYPE(T)		seq->type = (T)

int
slot_type(SlotInfo* data)
{
	if (data->slot == 0 || data->slot->wseq == 0) {
		return (Tcomplex);
	}
	return (data->slot->wseq->type);
}

jvalue
*slot_value(SlotInfo* data)
{
	return &(data->slot->wseq->u[2].value);
}

void
slot_const_const(SlotInfo* dst, jword s1, jword s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	seq->u[1].value.i = s1;
	seq->u[2].value.i = s2;
	if (type == Tstore) {
		ASSIGNSLOT_R(seq, 0, dst);
	}
	else {
		ASSIGNSLOT_W(seq, 0, dst);
	}

	seq->func = f;
}

void
slot_slot_const(SlotInfo* dst, SlotInfo* s1, jword s2, ifunc f, int type)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	if (s1 != 0 && dst != 0) {
		if (s1 != dst && (type != Tload && type != Tstore)) {
			move_any(dst, s1);
			s1 = dst;
		}
	}
#endif
	seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.i = s2;
	/* For a store we don't overwrite any ops */
	if (type == Tstore) {
		ASSIGNSLOT_R(seq, 0, dst);
	}
	else {
		ASSIGNSLOT_W(seq, 0, dst);
	}

	SEQ_TYPE(type);
	seq->func = f;
}

void
slot_slot_fconst(SlotInfo* dst, SlotInfo* s1, float s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.f = s2;
	ASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;
}

void
slot_slot_slot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	SlotInfo* olddst = NULL;

	/* Two operand systems cannot handle three operand ops.
	 * We need to fix it so the dst is one of the source ops.
	 */
	if (s1 != 0 && s2 != 0 && dst != 0) {
		if (type == Tcomm) {
			if (s2 == dst) {
				s2 = s1;
				s1 = dst;
			}
		}
		if (s2 == dst) {
			olddst = dst;
			slot_alloctmp(dst);
		}
		if (s1 != dst) {
			move_any(dst, s1);
			s1 = dst;
		}
	}
#endif
	seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	ASSIGNSLOT_R(seq, 2, s2);
	ASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != 0) {
		move_any(olddst, dst);
		slot_freetmp(dst);
	}
#endif
}

void
lslot_lslot_lslot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	SlotInfo* olddst = NULL;
	if (s1 != 0 && s2 != 0 && dst != 0) {
		if (s2 == dst) {
			olddst = dst;
			slot_alloc2tmp(dst);
		}
		if (s1 != dst) {
			move_anylong(dst, s1);
			s1 = dst;
		}
	}
#endif
	seq = nextSeq();

	LASSIGNSLOT_R(seq, 1, s1);
	LASSIGNSLOT_R(seq, 2, s2);
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != 0) {
		move_anylong(olddst, dst);
		slot_free2tmp(dst);
	}
#endif
}

void
lslot_lslot_slot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	SlotInfo* olddst = NULL;
	if (s1 != 0 && s2 != 0 && dst != 0) {
		if (s2 == dst) {
			olddst = dst;
			slot_alloctmp(dst);
		}
		if (s1 != dst) {
			move_any(dst, s1);
			s1 = dst;
		}
	}
#endif
	seq = nextSeq();

	LASSIGNSLOT_R(seq, 1, s1);
	ASSIGNSLOT_R(seq, 2, s2);
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != 0) {
		move_any(olddst, dst);
		slot_freetmp(dst);
	}
#endif
}

void
slot_slot_lslot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	SlotInfo* olddst = NULL;
	if (s1 != 0 && s2 != 0 && dst != 0) {
		if (s2 == dst) {
			olddst = dst;
			slot_alloctmp(dst);
		}
		if (s1 != dst) {
			move_any(dst, s1);
			s1 = dst;
		}
	}
#endif
	seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	LASSIGNSLOT_R(seq, 2, s2);
	ASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != 0) {
		move_any(olddst, dst);
		slot_freetmp(dst);
	}
#endif
}

void
slot_lslot_lslot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type)
{
	sequence* seq;

#if defined(TWO_OPERAND)
	KAFFEVM_ABORT();
#endif

	seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	LASSIGNSLOT_R(seq, 2, s2);
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;
}

void
lslot_lslot_const(SlotInfo* dst, SlotInfo* s1, jword s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	LASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.i = s2;
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;
}

void
lslot_lslot_lconst(SlotInfo* dst, SlotInfo* s1, jlong s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	LASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.j = s2;
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;
}

void
lslot_slot_const(SlotInfo* dst, SlotInfo* s1, jword s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.i = s2;
	if( type == Tstore ) {
	    LASSIGNSLOT_R(seq, 0, dst);
	} else {
	    LASSIGNSLOT_W(seq, 0, dst);
	}

	SEQ_TYPE(type);
	seq->func = f;
}

void
lslot_slot_lconst(SlotInfo* dst, SlotInfo* s1, jlong s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.j = s2;
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;
}

void
lslot_slot_fconst(SlotInfo* dst, SlotInfo* s1, double s2, ifunc f, int type)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT_R(seq, 1, s1);
	seq->u[2].value.d = s2;
	LASSIGNSLOT_W(seq, 0, dst);

	SEQ_TYPE(type);
	seq->func = f;
}

void
slot_slot_slot_const_const(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2,
			    jword s3, jword s4, ifunc f,
			    int type)
{
	sequence* seq = nextSeq();

	seq->u[4].value.i = s4;
	seq->u[3].value.i = s3;
	ASSIGNSLOT_R(seq, 2, s2);
	ASSIGNSLOT_R(seq, 1, s1);
	if (type == Tstore) {
		ASSIGNSLOT_R(seq, 0, dst);
	}
	else {
		ASSIGNSLOT_W(seq, 0, dst);
	}

	SEQ_TYPE(type);
	seq->func = f;
}

void
slot_slot_const_const_const(SlotInfo* dst, SlotInfo* s1, jword s2,
			     jword s3, jword s4, ifunc f,
			     int type)
{
	sequence* seq = nextSeq();

	seq->u[4].value.i = s4;
	seq->u[3].value.i = s3;
	seq->u[2].value.i = s2;
	ASSIGNSLOT_R(seq, 1, s1);
	if (type == Tstore) {
		ASSIGNSLOT_R(seq, 0, dst);
	}
	else {
		ASSIGNSLOT_W(seq, 0, dst);
	}

	SEQ_TYPE(type);
	seq->func = f;
}
