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
#include "registers.h"
#include "icode.h"
#include "basecode.h"
#include "labels.h"
#include "codeproto.h"
#include "md.h"

#define	ASSIGNSLOT(D, S)	(D).s.slot = (S); \
				if ((S) != 0) (S)->info = SI_SLOT

void
_slot_const_const(SlotInfo* dst, jword s1, jword s2, ifunc f, int type UNUSED)
{
	sequence* seq = nextSeq();

	seq->u[1].iconst = s1;
	seq->u[2].iconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_slot_slot_const(SlotInfo* dst, SlotInfo* s1, jword s2, ifunc f, int type)
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

	ASSIGNSLOT(seq->u[1], s1);
	seq->u[2].iconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_slot_slot_fconst(SlotInfo* dst, SlotInfo* s1, double s2, ifunc f, int type UNUSED)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT(seq->u[1], s1);
	seq->u[2].fconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_slot_slot_slot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	SlotInfo* olddst = NULL;

	/* Two operand systems cannot handle three operand ops.
	 * We need to fix it so the dst is one of the source ops.
	 */
	if (s1 != NULL && s2 != NULL && dst != NULL) {
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

	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[2], s2);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != NULL) {
		move_any(olddst, dst);
	}
#endif
}

void
_lslot_lslot_lslot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type UNUSED)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	SlotInfo* olddst = NULL;
	if (s1 != NULL && s2 != NULL && dst != NULL) {
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

	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[2], s2);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != NULL) {
		move_anylong(olddst, dst);
	}
#endif
}

void
_lslot_lslot_slot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type UNUSED)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	SlotInfo* olddst = NULL;
	if (s1 != NULL && s2 != NULL && dst != NULL) {
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

	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[2], s2);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != NULL) {
		move_any(olddst, dst);
	}
#endif
}

void
_slot_slot_lslot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type UNUSED)
{
	sequence* seq;
#if defined(TWO_OPERAND)
	/* Two operand systems cannot handle three operand ops.
	 * We need to fixit so the dst is one of the source ops.
	 */
	SlotInfo* olddst = NULL;
	if (s1 != NULL && s2 != NULL && dst != NULL) {
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

	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[2], s2);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;

#if defined(TWO_OPERAND)
	if (olddst != NULL) {
		move_any(olddst, dst);
	}
#endif
}

void
_slot_lslot_lslot(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2, ifunc f, int type UNUSED)
{
	sequence* seq;

#if defined(TWO_OPERAND)
	KAFFEVM_ABORT();
#endif

	seq = nextSeq();

	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[2], s2);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_lslot_lslot_const(SlotInfo* dst, SlotInfo* s1, jword s2, ifunc f, int type UNUSED)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT(seq->u[1], s1);
	seq->u[2].iconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_lslot_lslot_lconst(SlotInfo* dst, SlotInfo* s1, jlong s2, ifunc f, int type UNUSED)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT(seq->u[1], s1);
	seq->u[2].lconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_lslot_slot_lconst(SlotInfo* dst, SlotInfo* s1, jlong s2, ifunc f, int type UNUSED)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT(seq->u[1], s1);
	seq->u[2].lconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_lslot_slot_fconst(SlotInfo* dst, SlotInfo* s1, double s2, ifunc f, int type UNUSED)
{
	sequence* seq = nextSeq();

	ASSIGNSLOT(seq->u[1], s1);
	seq->u[2].fconst = s2;
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_slot_slot_slot_const_const(SlotInfo* dst, SlotInfo* s1, SlotInfo* s2,
			    jword s3, jword s4, ifunc f,
			    int type UNUSED)
{
	sequence* seq = nextSeq();

	seq->u[4].iconst = s4;
	seq->u[3].iconst = s3;
	ASSIGNSLOT(seq->u[2], s2);
	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}

void
_slot_slot_const_const_const(SlotInfo* dst, SlotInfo* s1, jword s2,
			     jword s3, jword s4, ifunc f,
			     int type UNUSED)
{
	sequence* seq = nextSeq();

	seq->u[4].iconst = s4;
	seq->u[3].iconst = s3;
	seq->u[2].iconst = s2;
	ASSIGNSLOT(seq->u[1], s1);
	ASSIGNSLOT(seq->u[0], dst);

	seq->func = f;
}
