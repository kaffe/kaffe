/* icode.c
 * Define the instructions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "slots.h"
#include "seq.h"
#include "md.h"
#include "registers.h"
#include "basecode.h"
#include "labels.h"
#include "constpool.h"
#include "icode.h"
#include "soft.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "gc.h"
#include "itypes.h"
#include "locks.h"
#include "machine.h"
#include "codeproto.h"
#include "thread.h"
#include "jthread.h"
#include "support.h"
#include "code-analyse.h"
#include "funcs.h"
#include "kaffe_jni.h"
#include "fp.h"
#include "global-regs.h"

#if defined(HAVE_branch_and_link)
#define blink 0x8000000
#else
#define blink 0
#endif

/*
 * This flag can turn off array bounds checking.
 *  - for experimental purposes only.
 */
int noArrayBoundsChecks = 0;

#if defined(WORDS_BIGENDIAN)
#define	LSLOT(_s)	((_s)+1)
#define	HSLOT(_s)	(_s)
#else
#define	LSLOT(_s)	(_s)
#define	HSLOT(_s)	((_s)+1)
#endif

bool used_ieee_rounding;
bool used_ieee_division;

sequence* lastSpill;

#define	MAXLABTAB	64
label* labtab[MAXLABTAB];

static void _call_soft(void *routine, int profiled);

/* ----------------------------------------------------------------------- */
/* Register loads and spills.						   */
/*									   */

#if defined(HAVE_spill_int)

void
spill_int(SlotData* src)
{
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = slotOffsetNoSpill(src, Rint);
	HAVE_spill_int(&s);
}
#endif

#if defined(HAVE_reload_int)

void
reload_int(SlotData* dst)
{
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = slotOffsetNoSpill(dst, Rint);
	HAVE_reload_int(&s);
}
#endif

#if defined(HAVE_spill_ref)

void
spill_ref(SlotData* src)
{
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = slotOffsetNoSpill(src, Rref);
	HAVE_spill_ref(&s);
}
#endif

#if defined(HAVE_reload_ref)

void
reload_ref(SlotData* dst)
{
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = slotOffsetNoSpill(dst, Rref);
	HAVE_reload_ref(&s);
}
#endif

#if defined(HAVE_spill_long)

void
spill_long(SlotData* src)
{
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = slotOffsetNoSpill(src, Rlong);
	HAVE_spill_long(&s);
}
#endif

#if defined(HAVE_reload_long)

void
reload_long(SlotData* dst)
{
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = slotOffsetNoSpill(dst, Rlong);
	HAVE_reload_long(&s);
}
#endif

#if defined(HAVE_spill_float)

void
spill_float(SlotData* src)
{
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = slotOffsetNoSpill(src, Rfloat);
	HAVE_spill_float(&s);
}
#endif

#if defined(HAVE_reload_float)

void
reload_float(SlotData* dst)
{
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = slotOffsetNoSpill(dst, Rfloat);
	HAVE_reload_float(&s);
}
#endif

#if defined(HAVE_spill_double)

void
spill_double(SlotData* src)
{
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = slotOffsetNoSpill(src, Rdouble);
	HAVE_spill_double(&s);
}
#endif

#if defined(HAVE_reload_double)

void
reload_double(SlotData* dst)
{
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = slotOffsetNoSpill(dst, Rdouble);
	HAVE_reload_double(&s);
}
#endif

void
copyslots(SlotInfo* dst, SlotInfo* src, int type)
{
	slot_slot_slot(dst, NULL, src, slotAlias, Tcopy);
	activeSeq->u[1].value.i = type;
}

void
copylslots(SlotInfo* dst, SlotInfo* src, int type)
{
	lslot_lslot_lslot(dst, NULL, src, slotAlias, Tcopy);
	activeSeq->u[1].value.i = type;
}

/* ----------------------------------------------------------------------- */
/* Prologues and epilogues.						   */
/*									   */

void
prologue(Method* meth)
{
	label* l;

	used_ieee_rounding = false;
	used_ieee_division = false;

	l = KaffeJIT3_newLabel();
	l->type = Lnull;
	l->at = 0;
	l->to = 0;
	l->from = 0;

	setupSlotsForBasicBlock();
#if defined(NR_GLOBALS)
	setupGlobalRegisters();
#endif
	setupArgumentRegisters();

	/* Emit prologue code */
	slot_const_const(NULL, (jword)l, (jword)meth, HAVE_prologue, Tnull);
        slot_const_const(NULL, (jword)createSpillMask(), SR_START, doReload, Tnull);
	
#if defined(ENABLE_JVMPI)
	{
		SlotInfo *tmp;

		slot_alloctmp(tmp);
		if( METHOD_IS_STATIC(meth) )
			move_ref_const(tmp, NULL);
		else
			move_ref(tmp, local(0));
		softcall_enter_method(tmp, meth);
		slot_freetmp(tmp);
	}
#endif
}

void
check_stack_limit(void)
{
#if defined(STACK_LIMIT)
#if defined(HAVE_check_stack_limit_constpool)
	label* l;
	constpool* c;

	l = KaffeJIT3_newLabel();
	c = KaffeJIT3_newConstant(CPref, soft_stackoverflow);
	l->type = Lexternal;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

	slot_slot_const(NULL, stack_limit, (jword)l, HAVE_check_stack_limit_constpool, Tnull);
#elif defined(HAVE_check_stack_limit)
	label* l;

	l = KaffeJIT3_newLabel();
	l->type = Lexternal;
	l->at = 0;
	l->to = (uintp)soft_stackoverflow;
	l->from = 0;

	slot_slot_const(NULL, stack_limit, (jword)l, HAVE_check_stack_limit, Tnull);
#endif
#endif /* STACK_LIMIT */
}

void
exception_prologue(void)
{
	label* l;

	l = KaffeJIT3_newLabel();
	l->type = Lnull;
	l->at = 0;
	l->to = 0;
	l->from = 0;

	/* Emit exception prologue code */
	slot_const_const(NULL, (jword)l, 0, HAVE_exception_prologue, Tnull);
        slot_const_const(NULL, (jword)createSpillMask(), SR_EXCEPTION, doReload, Tnull);
}

void
epilogue(Method* meth UNUSED)
{
	label* l;
	
	l = KaffeJIT3_newLabel();
	l->type = Lnull;	/* Lnegframe|Labsolute|Lgeneral */
	l->at = 0;
	l->to = 0;
	l->from = 0;
#if defined(TRACE_METHOD_END)
	if (Kaffe_JavaVMArgs.enableVerboseCall != 0) {
                begin_func_sync();
                call_soft(soft_end);
                popargs();
                end_func_sync();
        }
#endif

	slot_const_const(NULL, (jword)l, 0, HAVE_epilogue, Tnull);
}

void
exit_method(void)
{
#if defined(ENABLE_JVMPI)
	softcall_exit_method(globalMethod);
#endif
}

void
ret(void)
{
	label *l;

	l = KaffeJIT3_newLabel();
	l->at = 0;
	l->to = 0;
	l->from = 0;

	/* Jump to epilogue */
	l->type = Lepilogue;
	branch (l, ba);
}


/* ----------------------------------------------------------------------- */
/* Conditional monitor management.					   */
/*									   */

void
mon_enter(methods* meth, SlotInfo* obj)
{
	/* Emit monitor entry if required */
	if ((meth->accflags & ACC_SYNCHRONISED) == 0) {
		return;
	}
#if defined(HAVE_mon_enter)
	{
		label* l;

		begin_func_sync();
		l = KaffeJIT3_newLabel();
		l->type = Lexternal;
		l->at = 0;
		l->to = (uintp)slowLockObject;
		l->from = 0;
		if (METHOD_IS_STATIC(meth) == 0) {
			meth = NULL;
		}
		else {
			obj = NULL;
		}
		slot_slot_slot_const_const(NULL, NULL, obj, (jword)meth, (jword)l, HAVE_mon_enter, Tnull);
		end_func_sync();
	}
#else
	begin_func_sync();
	if ((meth->accflags & ACC_STATIC) != 0) {
		pusharg_class_const(meth->class, 0);
	}
	else {
		pusharg_ref(obj, 0);
	}
	call_soft(lockObject);
	popargs();
	end_func_sync();
#endif
}

void
mon_exit(methods* meth, SlotInfo* obj)
{
	/* Emit monitor entry if required */
	if ((meth->accflags & ACC_SYNCHRONISED) == 0) {
		return;
	}
#if defined(HAVE_mon_exit)
	{
		label* l;

		begin_func_sync();
		l = KaffeJIT3_newLabel();
		l->type = Lexternal;
		l->at = 0;
		l->to = (uintp)slowUnlockObject;
		l->from = 0;
		if (METHOD_IS_STATIC(meth) == 0) {
			meth = NULL;
		}
		else {
			obj = NULL;
		}
		slot_slot_slot_const_const(NULL, NULL, obj, (jword)meth, (jword)l, HAVE_mon_exit, Tnull);
		end_func_sync();
	}
#else
	begin_func_sync();
	if (METHOD_IS_STATIC(meth) != 0) {
		pusharg_class_const(meth->class, 0);
	}
	else {
		pusharg_ref(obj, 0);
	}
	call_soft(unlockObject);
	popargs();
	end_func_sync();
#endif
}

void
softcall_monitorenter(SlotInfo* mon)
{
#if defined(HAVE_mon_enter)
	label* l;

	l = KaffeJIT3_newLabel();
	l->type = Lexternal;
	l->at = 0;
	l->to = (uintp)slowLockObject;
	l->from = 0;
	slot_slot_slot_const_const(NULL, NULL, mon, 0, (jword)l, HAVE_mon_enter, Tnull);
#else
	pusharg_ref(mon, 0);
	call_soft(lockObject);
	popargs();
#endif
}

void
softcall_monitorexit(SlotInfo* mon)
{
#if defined(HAVE_mon_exit)
	label* l;

	l = KaffeJIT3_newLabel();
	l->type = Lexternal;
	l->at = 0;
	l->to = (uintp)slowUnlockObject;
	l->from = 0;
	slot_slot_slot_const_const(NULL, NULL, mon, 0, (jword)l, HAVE_mon_exit, Tnull);
#else
	pusharg_ref(mon, 0);
	call_soft(unlockObject);
	popargs();
#endif
}

/* ----------------------------------------------------------------------- */
/* Basic block and instruction management.				   */
/*									   */

void
mark_all_writes(void)
{
	int i;
	SlotData* sd;

	/* We must reference all slots sequence since they may be used */
	for (i = maxslot - 1; i >= 0; i--) {
		sd = slotinfo[i].slot;
		if (sd->wseq != 0) {
			sd->wseq->refed = 1;
		}
	}
}

void
_start_sub_block(void)
{
	slot_const_const(NULL, (jword)createSpillMask(), SR_SUBBASIC, doReload, Tnull);
	setupSlotsForBasicBlock();
}

void
_start_basic_block(void)
{
	slot_const_const(NULL, (jword)createSpillMask(), SR_BASIC, doReload, Tnull);
	setupSlotsForBasicBlock();
}

void
_end_sub_block(void)
{
	mark_all_writes();
	slot_const_const(NULL, (jword)createSpillMask(), SR_SUBBASIC, doSpill, Tnull);
}

void
_end_basic_block(void)
{
	mark_all_writes();
	slot_const_const(NULL, (jword)createSpillMask(), SR_BASIC, doSpill, Tnull);
}

void
_syncRegisters(uintp stk UNUSED, uintp temp UNUSED)
{
	slot_const_const(NULL, (jword)createSpillMask(), SR_SYNC, doSpill, Tnull);
}

void
_start_instruction(uintp _pc)
{
	slot_const_const(NULL, 0, _pc, startInsn, Tnull);
}

void
_start_exception_block(uintp stk)
{
	/* Exception blocks act like function returns - the return
	 * value is the exception object.
	 */
	start_basic_block();
	exception_prologue();
	return_ref(&localinfo[stk]);
}

/*
 * Begin a register/slot syncronization - this is the point we will
 * store all our cached values back to where they belong.  We don't know
 * which values to store yet - this is decided at the end_sync point.
 */
void
begin_sync(void)
{
	assert(lastSpill == 0);
	slot_const_const(NULL, 0, SR_BASIC, doSpill, Tnull);
	lastSpill = activeSeq;
}

void
end_sync(void)
{
	/* Make sure a sync is in progress */
	assert(lastSpill != 0);
	lastSpill->u[1].smask = createSpillMask();
	lastSpill = NULL;
	mark_all_writes();
}

void
begin_func_sync(void)
{
	assert(lastSpill == 0);
	slot_const_const(NULL, 0, SR_FUNCTION, doSpill, Tnull);
	lastSpill = activeSeq;

	/* If we might throw and catch and exception we better make everything
	 * is written which should be.
	 */
	if (canCatch(ANY)) {
		mark_all_writes();
	}
}

void
end_func_sync(void)
{
	SlotData** mask;

	/* Build a mask of the slots to spill and reload */
	mask = createSpillMask();

	/* Save the slots to spill */
	assert(lastSpill != 0);
	lastSpill->u[1].smask = mask;
	lastSpill = NULL;

	/* Create a reload and save the slots to reload */
	slot_const_const(NULL, (jword)mask, SR_FUNCTION, doReload, Tnull);
}


/* ----------------------------------------------------------------------- */
/* Moves.								   */
/*									   */

void
move_int_const(SlotInfo* dst, jint val)
{
#if defined(HAVE_move_int_const)
	if (HAVE_move_int_const_rangecheck(val)) {
		slot_slot_const(dst, NULL, val, HAVE_move_int_const, Tconst);
	}
	else
#endif
	{
		constpool *c;
		label* l;

		c = KaffeJIT3_newConstant(CPint, val);
		l = KaffeJIT3_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

#if defined(HAVE_load_constpool_int)
		slot_slot_const(dst, 0, (jword)l, HAVE_load_constpool_int, Tnull);
#else
		{
			SlotInfo* tmp;
			slot_alloctmp(tmp);
			move_label_const(tmp, l);
			load_int(dst, tmp);
			slot_freetmp(tmp);
		}
#endif
	}
}

void
move_ref_const(SlotInfo* dst, void *val)
{
#if defined(HAVE_move_ref_const)
	if (HAVE_move_ref_const_rangecheck(val)) {
		slot_slot_const(dst, NULL, (jword)val, HAVE_move_ref_const, Tconst);
	}
	else
#endif
	{
		constpool *c;
		label* l;

		c = KaffeJIT3_newConstant(CPref, val);
		l = KaffeJIT3_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

#if defined(HAVE_load_constpool_ref)
		slot_slot_const(dst, 0, (jword)l, HAVE_load_constpool_ref, Tnull);
#else
		{
			SlotInfo* tmp;
			slot_alloctmp(tmp);
			move_label_const(tmp, l);
			load_ref(dst, tmp);
			slot_freetmp(tmp);
		}
#endif
	}
}

void
move_string_const(SlotInfo* dst, void *val)
{
#if 1
	move_ref_const(dst, val);
#else
	label* l;
	constpool *c;
	SlotInfo* tmp;

	c = KaffeJIT3_newConstant(CPstring, val);
	l = KaffeJIT3_newLabel();
	l->type = Lconstant;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

#if defined(HAVE_load_constpool_ref)
	slot_slot_const(dst, 0, (jword)l, HAVE_load_constpool_ref, Tnull);
#else
	slot_alloctmp(tmp);
	move_label_const(tmp, l);
	load_ref(dst, tmp);
	slot_freetmp(tmp);
#endif

#endif
}

void
move_long_const(SlotInfo* dst, jlong val)
{
#if defined(HAVE_move_long_const)
	if (HAVE_move_long_const_rangecheck(val)) {
		lslot_slot_lconst(dst, 0, val, HAVE_move_long_const, Tconst);
	}
	else {
		constpool *c;
		label* l;
		SlotInfo* tmp;

		c = KaffeJIT3_newConstant(CPlong, val);
		l = KaffeJIT3_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;


#if defined(HAVE_load_constpool_long)
		slot_slot_const(dst, 0, (jword)l, HAVE_load_constpool_long, Tnull);
#else
		slot_alloctmp(tmp);
		move_label_const(tmp, l);
		load_long(dst, tmp);
		slot_freetmp(tmp);
#endif
	}
#else

#if defined(WORDS_BIGENDIAN)
	/*
	 * Switch the ordering so that we get a better register allocation
	 * ordering and don't have to swap immediately afterwards.  (sigh)
	 */
	move_int_const(HSLOT(dst), (jint)((val >> 32) & 0xFFFFFFFF));
	move_int_const(LSLOT(dst), (jint)(val & 0xFFFFFFFF));
#else
	move_int_const(LSLOT(dst), (jint)(val & 0xFFFFFFFF));
	move_int_const(HSLOT(dst), (jint)((val >> 32) & 0xFFFFFFFF));
#endif
	
#endif
}

void
move_float_const(SlotInfo* dst, float val)
{
#if defined(HAVE_move_float_const)
	if (HAVE_move_float_const_rangecheck(val)) {
		slot_slot_fconst(dst, NULL, val, HAVE_move_float_const, Tconst);
	}
	else
#endif
	{
		constpool *c;
		label* l;

		c = KaffeJIT3_newConstant(CPfloat, val);
		l = KaffeJIT3_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

#if defined(HAVE_load_constpool_float)
		slot_slot_const(dst, NULL, (jword)l, HAVE_load_constpool_float, Tnull);
#else
		{
			SlotInfo* tmp;
			slot_alloctmp(tmp);
			move_label_const(tmp, l);
			load_float(dst, tmp);
			slot_freetmp(tmp);
		}
#endif
	}
}

void
move_double_const(SlotInfo* dst, jdouble val)
{
#if defined(HAVE_move_double_const)
	if (HAVE_move_double_const_rangecheck(val)) {
		lslot_slot_fconst(dst, NULL, val, HAVE_move_double_const, Tconst);
	}
	else
#endif
	{
		constpool *c;
		label* l;

		c = KaffeJIT3_newConstant(CPdouble, val);
		l = KaffeJIT3_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

#if defined(HAVE_load_constpool_double)
		lslot_lslot_const(dst, 0, (jword)l, HAVE_load_constpool_double, Tnull);
#else
		{
			SlotInfo* tmp;
			slot_alloctmp(tmp);
			move_label_const(tmp, l);
			load_double(dst, tmp);
			slot_freetmp(tmp);
		}
#endif
	}
}

#if defined(HAVE_move_any)
void
move_any(SlotInfo* dst, SlotInfo* src)
{
	if (dst == src) {
	}
	else if (isGlobal(dst->slot)) {
		slot_slot_slot(dst, NULL, src, HAVE_move_any, Tcopy);
	}
	else {
		copyslots(dst, src, Rref);
	}
}
#endif

#if defined(HAVE_move_int)
void
move_int(SlotInfo* dst, SlotInfo* src)
{
	if (dst == src) {
	}
#if defined(HAVE_move_int_const)
	else if (slot_type(src) == Tconst) {
		move_int_const(dst, slot_value(src)->i);
	}
#endif
	else if (isGlobal(dst->slot)) {
		slot_slot_slot(dst, NULL, src, HAVE_move_int, Tcopy);
	}
	else {
		copyslots(dst, src, Rint);
	}
}
#endif

void
move_ref(SlotInfo* dst, SlotInfo* src)
{
	if (dst == src) {
	}
#if defined(HAVE_move_ref_const)
	else if (slot_type(src) == Tconst) {
		move_ref_const(dst, slot_value(src)->l);
	}
#endif
	else if (isGlobal(dst->slot)) {
		slot_slot_slot(dst, NULL, src, HAVE_move_ref, Tcopy);
	}
	else {
		copyslots(dst, src, Rref);
	}
}

void
move_anylong(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_move_anylong)
	lslot_lslot_lslot(dst, 0, src, HAVE_move_anylong, Tcopy);
#else
	assert(LSLOT(dst) != HSLOT(src));
	move_any(LSLOT(dst), LSLOT(src));
	move_any(HSLOT(dst), HSLOT(src));
#endif
}

void
move_long(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_move_long)
	lslot_lslot_lslot(dst, 0, src, HAVE_move_long, Tcopy);
#else
	assert(LSLOT(dst) != HSLOT(src));
	move_int(LSLOT(dst), LSLOT(src));
	move_int(HSLOT(dst), HSLOT(src));
#endif
}

void
move_float(SlotInfo* dst, SlotInfo* src)
{
	if (dst == src) {
	}
#if defined(HAVE_move_float_const)
	else if (slot_type(src) == Tconst) {
		move_float_const(dst, slot_value(src)->f);
	}
#endif
	else if (isGlobal(dst->slot)) {
#if defined(HAVE_move_float)
		slot_slot_slot(dst, NULL, src, HAVE_move_float, Tcopy);
#elif defined(HAVE_NO_FLOATING_POINT)
		move_int(dst, src);
#else
	KAFFEVM_ABORT();
#endif
	}
	else {
		copyslots(dst, src, Rfloat);
	}
}

void
move_double(SlotInfo* dst, SlotInfo* src)
{
	if (dst == src) {
	}
#if defined(HAVE_move_double_const)
	else if (slot_type(src) == Tconst) {
		move_double_const(dst, slot_value(src)->d);
	}
#endif
	else if (isGlobal(dst->slot)) {
#if defined(HAVE_move_double)
		lslot_lslot_lslot(dst, NULL, src, HAVE_move_double, Tcopy);
#elif defined(HAVE_NO_FLOATING_POINT) || defined(PS2LINUX)
		move_long(dst, src);
#else
		KAFFEVM_ABORT();
#endif
	}
	else {
		copylslots(dst, src, Rdouble);
	}
}

#if defined(HAVE_move_label_const)
void
move_label_const(SlotInfo* dst, label* lab)
{
	slot_slot_const(dst, NULL, (jword)lab, HAVE_move_label_const, Tnull);
}
#endif

void
swap_any(SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_swap_any)
	slot_slot_slot(src, 0, src2, HAVE_swap_any, Tcomplex);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref(tmp, src);
	move_ref(src, src2);
	move_ref(src2, tmp);
	slot_freetmp(tmp);
#endif
}

/* ----------------------------------------------------------------------- */
/* Arithmetic operators - add, sub, etc.				   */
/*									   */


#if defined(HAVE_adc_int)
void
adc_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_adc_int, Tcomplex);
}
#endif

#if defined(HAVE_add_int)
void
_add_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_add_int, Tcomm);
}

void
add_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_add_int_const)
	if (HAVE_add_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_add_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_add_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
add_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_add_int_const)
	if (slot_type(src) == Tconst) {
#if 0
		/* Disabled as it does not clear Carry and breaks
                   add_long() on ARM */
		if (slot_type(src2) == Tconst) {
			move_int_const(dst, slot_value(src)->i + slot_value(src2)->i);
		}
		else
#endif
		{
			add_int_const(dst, src2, slot_value(src)->i);
		}
	}
	else if (slot_type(src2) == Tconst) {
		add_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_add_int(dst, src, src2);
}
#endif

#if defined(HAVE_add_ref)
void
_add_ref(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_add_ref, Tcomm);
}

void
add_ref_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_add_ref_const)
	if (HAVE_add_ref_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_add_ref_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_add_ref(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
add_ref(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	_add_ref(dst, src, src2);
}
#endif

#if defined(HAVE_set_lt_int)
void
set_lt_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_set_lt_int, Tcomplex);
}
#endif

#if defined(HAVE_set_lt_int_const)
void
set_lt_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
	slot_slot_const(dst, src, val, HAVE_set_lt_int_const, Tcomplex);
}
#endif

void
add_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_add_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_add_long, Tcomplex);
#elif defined(HAVE_adc_int)
	add_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	adc_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
#elif defined(HAVE_set_lt_int)
	SlotInfo* res,*carry;

	slot_alloctmp(res);
	slot_alloctmp(carry);

	add_int(res, LSLOT(src), LSLOT(src2));
	set_lt_int(carry, res, LSLOT(src2));
	add_int(carry, HSLOT(src), carry);
	add_int(HSLOT(dst), carry, HSLOT(src2));
	move_int(LSLOT(dst),res);

	slot_freetmp(res);
	slot_freetmp(carry);
#else
	KAFFEVM_ABORT();
#endif
}

#if defined(HAVE_add_long_const)
void
add_long_const(SlotInfo* dst, SlotInfo* src, jlong val)
{
	if (HAVE_add_long_const_rangecheck(val)) {
		lslot_lslot_lconst(dst, src, val, HAVE_add_long_const, Tcomplex);
	}
	else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_long_const(tmp, val);
		add_long(dst, src, tmp);
		slot_freetmp(tmp);
	}
}
#endif

void
add_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_add_float)
	slot_slot_slot(dst, src, src2, HAVE_add_float, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_fadd);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
add_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_add_double)
	lslot_lslot_lslot(dst, src, src2, HAVE_add_double, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_faddl);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

#if defined(HAVE_sbc_int)
void
sbc_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_sbc_int, Tcomplex);
}
#endif

#if defined(HAVE_sub_int)
void
_sub_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_sub_int, Tcomplex);
}

void
sub_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_sub_int_const)
	if (HAVE_sub_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_sub_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_sub_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
sub_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_sub_int_const)
	if (slot_type(src2) == Tconst) {
		sub_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_sub_int(dst, src, src2);
}
#endif

void
sub_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_sub_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_sub_long, Tcomplex);
#elif defined(HAVE_sbc_int)
	sub_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	sbc_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
#elif defined(HAVE_set_lt_int)
        SlotInfo* carry;

        slot_alloctmp(carry);

        set_lt_int(carry, LSLOT(src), LSLOT(src2));
        sub_int(carry, HSLOT(src), carry);
        sub_int(HSLOT(dst), carry, HSLOT(src2));
        sub_int(LSLOT(dst), LSLOT(src), LSLOT(src2));

	slot_freetmp(carry);
#else
	KAFFEVM_ABORT();
#endif
}

#if defined(HAVE_sub_long_const)
void
sub_long_const(SlotInfo* dst, SlotInfo* src, jlong val)
{
	if (HAVE_sub_long_const_rangecheck(val)) {
		lslot_lslot_lconst(dst, src, val, HAVE_sub_long_const, Tcomplex);
	}
	else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_long_const(tmp, val);
		sub_long(dst, src, tmp);
		slot_freetmp(tmp);
	}
}
#endif

void
sub_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_sub_float)
	slot_slot_slot(dst, src, src2, HAVE_sub_float, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_fsub);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
sub_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_sub_double)
	lslot_lslot_lslot(dst, src, src2, HAVE_sub_double, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_fsubl);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

int
mul_int_const_optimize(SlotInfo* dst, SlotInfo* src, jint val)
{
	switch (val) {
	case 2:
		lshl_int_const(dst, src, 1);
		break;
	case 4:
		lshl_int_const(dst, src, 2);
		break;
	case 8:
		lshl_int_const(dst, src, 3);
		break;
	case 16:
		lshl_int_const(dst, src, 4);
		break;
	default:
		return (0);
	}
	return (1);
}

void
_mul_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_mul_int)
	slot_slot_slot(dst, src, src2, HAVE_mul_int, Tcomplex);
#else
	begin_func_sync();
#  if defined(PUSHARG_FORWARDS)
	pusharg_int(src, 0);
	pusharg_int(src2, 1);
#  else
	pusharg_int(src2, 1);
	pusharg_int(src, 0);
#  endif
	call_soft(soft_mul);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
mul_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
	if (mul_int_const_optimize(dst, src, val) != 0) {
		return;
	}
#if defined(HAVE_mul_int_const)
	if (HAVE_mul_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_mul_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_mul_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
mul_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	if (slot_type(src2) == Tconst) {
		if (mul_int_const_optimize(dst, src, slot_value(src2)->i) != 0) {
			return;
		}
	}
#if defined(HAVE_mul_int) && defined(HAVE_mul_int_const)
	if (slot_type(src) == Tconst) {
		mul_int_const(dst, src2, slot_value(src)->i);
	}
	else if (slot_type(src2) == Tconst) {
		mul_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_mul_int(dst, src, src2);
}

void
mul_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_mul_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_mul_long, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_long(src2, pusharg_long_idx_inc);
#else
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_lmul);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

#if defined(HAVE_mul_long_const)
void
mul_long_const(SlotInfo* dst, SlotInfo* src, jlong val)
{
	if (HAVE_mul_long_const_rangecheck(val)) {
		lslot_lslot_lconst(dst, src, val, HAVE_mul_long_const, Tcomplex);
	}
	else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_long_const(tmp, val);
		mul_long(dst, src, tmp);
		slot_freetmp(tmp);
	}
}
#endif

void
mul_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_mul_float)
	slot_slot_slot(dst, src, src2, HAVE_mul_float, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_fmul);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
mul_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_mul_double)
	lslot_lslot_lslot(dst, src, src2, HAVE_mul_double, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_fmull);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

int
div_int_const_optimize(SlotInfo* dst, SlotInfo* src, jint val)
{
	SlotInfo* tmp;
	
	switch (val) {
	case 2:
		slot_alloctmp(tmp);
		lshr_int_const(tmp, src, (sizeof(int) * 8 - 1));
		add_int(tmp, tmp, src);
		ashr_int_const(dst, tmp, 1);
		slot_freetmp(tmp);
		break;
	case 4:
		slot_alloctmp(tmp);
		ashr_int_const(tmp, src, 1);
		lshr_int_const(tmp, tmp, (sizeof(int) * 8 - 2));
		add_int(tmp, tmp, src);
		ashr_int_const(dst, tmp, 2);
		slot_freetmp(tmp);
		break;
	case 8:
		slot_alloctmp(tmp);
		ashr_int_const(tmp, src, 2);
		lshr_int_const(tmp, tmp, (sizeof(int) * 8 - 3));
		add_int(tmp, tmp, src);
		ashr_int_const(dst, tmp, 3);
		slot_freetmp(tmp);
		break;
	case 16:
		slot_alloctmp(tmp);
		ashr_int_const(tmp, src, 3);
		lshr_int_const(tmp, tmp, (sizeof(int) * 8 - 4));
		add_int(tmp, tmp, src);
		ashr_int_const(dst, tmp, 4);
		slot_freetmp(tmp);
		break;
	default:
		return (0);
	}
	return (1);
}

void
_div_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_div_int)
	slot_slot_slot(dst, src, src2, HAVE_div_int, Tcomplex);
#else
	begin_func_sync();
#  if defined(PUSHARG_FORWARDS)
	pusharg_int(src, 0);
	pusharg_int(src2, 1);
#  else
	pusharg_int(src2, 1);
	pusharg_int(src, 0);
#  endif
	call_soft(soft_div);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
div_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
	if (div_int_const_optimize(dst, src, val) != 0) {
		return;
	}
#if defined(HAVE_div_int_const)
	if (HAVE_div_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_div_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_div_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
div_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	if (slot_type(src2) == Tconst) {
		if (div_int_const_optimize(dst, src, slot_value(src2)->i) != 0) {
			return;
		}
	}
	_div_int(dst, src, src2);
}

void
div_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_div_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_div_long, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_long(src2, pusharg_long_idx_inc);
#else
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_ldiv);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

void
div_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	used_ieee_division = true;
#if defined(HAVE_div_float)
	slot_slot_slot(dst, src, src2, HAVE_div_float, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_fdiv);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
div_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	used_ieee_division = true;
#if defined(HAVE_div_double)
	lslot_lslot_lslot(dst, src, src2, HAVE_div_double, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_fdivl);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

void
rem_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_rem_int_const)
	if (HAVE_rem_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_rem_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		rem_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
rem_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_rem_int)
	slot_slot_slot(dst, src, src2, HAVE_rem_int, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_int(src, 0);
	pusharg_int(src2, 1);
#else
	pusharg_int(src2, 1);
	pusharg_int(src, 0);
#endif
	call_soft(soft_rem);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
rem_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_rem_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_rem_long, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_long(src2, pusharg_long_idx_inc);
#else
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_lrem);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

void
rem_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	used_ieee_division = true;
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_frem);
	popargs();
	end_func_sync();
	return_float(dst);
}

void
rem_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	used_ieee_division = true;
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_freml);
	popargs();
	end_func_sync();
	return_double(dst);
}

void
neg_int(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_neg_int)
	slot_slot_slot(dst, NULL, src, HAVE_neg_int, Tcomplex);
#else
	SlotInfo* zero;
	slot_alloctmp(zero);
	move_int_const(zero, NULL);
	sub_int(dst, zero, src);
	slot_freetmp(zero);
#endif
}

#if defined(HAVE_ngc_int)
void
ngc_int(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, 0, src, HAVE_ngc_int, Tcomplex);
}
#endif

void
neg_long(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_neg_long)
	lslot_lslot_lslot(dst, 0, src, HAVE_neg_long, Tcomplex);
#elif defined(HAVE_ngc_int)
	neg_int(LSLOT(dst), LSLOT(src));
	ngc_int(HSLOT(dst), HSLOT(src));
#elif defined(HAVE_sbc_int)
	SlotInfo* zero;
	slot_alloctmp(zero);
	move_int_const(zero, 0);
	sub_int(LSLOT(dst), zero, LSLOT(src));
	sbc_int(HSLOT(dst), zero, HSLOT(src));
	slot_freetmp(zero);
#elif defined(HAVE_adc_int_const)
	neg_int(LSLOT(dst), LSLOT(src));
	adc_int_const(HSLOT(dst), HSLOT(src), 0);
	neg_int(HSLOT(dst), HSLOT(dst));
#elif defined(HAVE_set_lt_int_const) && defined(HAVE_nor_int)
        SlotInfo* zero;
        SlotInfo* carry;

        slot_alloctmp(zero);
        slot_alloctmp(carry);

        move_int_const(zero, 0);
        set_lt_int_const(carry,LSLOT(src),1);
        sub_int(LSLOT(dst),zero,LSLOT(src));
        nor_int(HSLOT(dst),zero,HSLOT(src));
        add_int(HSLOT(dst),HSLOT(dst),carry);

	slot_freetmp(zero);
	slot_freetmp(carry);
#else
	KAFFEVM_ABORT();
#endif
}

void
neg_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_neg_float)
	slot_slot_slot(dst, NULL, src, HAVE_neg_float, Tcomplex);
#else
	SlotInfo* zero;
	slot_alloctmp(zero);
	move_float_const(zero, 0);
	sub_float(dst, zero, src);
	slot_freetmp(zero);
#endif
}

void
neg_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_neg_double)
	lslot_lslot_lslot(dst, NULL, src, HAVE_neg_double, Tcomplex);
#else
	SlotInfo* zero;
	slot_alloc2tmp(zero);
	move_double_const(zero, 0);
	sub_double(dst, zero, src);
	slot_free2tmp(zero);
#endif
}


/* ----------------------------------------------------------------------- */
/* Logical operators - and, or, etc.					   */
/*									   */

#if defined(HAVE_and_int)
void
_and_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_and_int, Tcomplex);
}

void
and_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
	if (val == -1) {
		move_int(dst, src);
	}
	else if (val == 0) {
		move_int_const(dst, 0);
	}
#if defined(HAVE_and_int_const)
	else if (HAVE_and_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_and_int_const, Tcomplex);
	}
#endif
	else {
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_and_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
and_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_and_int_const)
	if (slot_type(src) == Tconst) {
		and_int_const(dst, src2, slot_value(src)->i);
	}
	else if (slot_type(src2) == Tconst) {
		and_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_and_int(dst, src, src2);
}
#endif

void
and_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_and_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_and_long, Tcomplex);
#else
	and_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	and_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
#endif
}

void
and_long_const(SlotInfo* dst, SlotInfo* src, jlong val)
{
#if defined(HAVE_and_long_const)
	if (HAVE_and_long_const_rangecheck(val)) {
		lslot_lslot_lconst(dst, src, val, HAVE_and_long_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_long_const(tmp, val);
		and_long(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_or_int)
void
_or_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_or_int, Tcomplex);
}

void
or_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
	if (val == -1) {
		move_int_const(dst, -1);
	}
	else if (val == 0) {
		move_int(dst, src);
	}
#if defined(HAVE_or_int_const)
	if (HAVE_or_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_or_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_or_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
or_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_or_int_const)
	if (slot_type(src) == Tconst) {
		or_int_const(dst, src2, slot_value(src)->i);
	}
	else if (slot_type(src2) == Tconst) {
		or_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_or_int(dst, src, src2);
}
#endif

#if defined(HAVE_nor_int)
void
nor_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_nor_int, Tcomplex);
}
#endif

void
or_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_or_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_or_long, Tcomplex);
#else
	or_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	or_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
#endif
}

#if defined(HAVE_xor_int)
void
_xor_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_xor_int, Tcomplex);
}

void
xor_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_xor_int_const)
	if (HAVE_xor_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_xor_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_xor_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
xor_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_xor_int_const)
	if (slot_type(src) == Tconst) {
		xor_int_const(dst, src2, slot_value(src)->i);
	}
	else if (slot_type(src2) == Tconst) {
		xor_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_xor_int(dst, src, src2);
}
#endif

void
xor_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_xor_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_xor_long, Tcomplex);
#else
	xor_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	xor_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
#endif
}

#if defined(HAVE_lshl_int)
void
_lshl_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_lshl_int, Tcomplex);
}

void
lshl_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_lshl_int_const)
	if (HAVE_lshl_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_lshl_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_lshl_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
lshl_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lshl_int_const)
	if (slot_type(src2) == Tconst) {
		lshl_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_lshl_int(dst, src, src2);
}
#endif

#if defined(HAVE_lshl_long_const)
void
lshl_long_const(SlotInfo* dst, SlotInfo* src, jint val)
{
	if (HAVE_lshl_long_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_lshl_long_const, Tcomplex);
	}
	else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		lshl_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}
#endif

void
lshl_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lshl_long)
	lslot_lslot_slot(dst, src, src2, HAVE_lshl_long, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_int(src2, pusharg_long_idx_inc);
#else
	pusharg_int(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_lshll);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

#if defined(HAVE_ashr_int)
void
_ashr_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_ashr_int, Tcomplex);
}

void
ashr_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_ashr_int_const)
	if (HAVE_ashr_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_ashr_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_ashr_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
ashr_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_ashr_int_const)
	if (slot_type(src2) == Tconst) {
		ashr_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_ashr_int(dst, src, src2);
}
#endif

void
ashr_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_ashr_long)
	lslot_lslot_slot(dst, src, src2, HAVE_ashr_long, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_int(src2, pusharg_long_idx_inc);
#else
	pusharg_int(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_ashrl);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

#if defined(HAVE_lshr_int)
void
_lshr_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_lshr_int, Tcomplex);
}

void
lshr_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_lshr_int_const)
	if (HAVE_lshr_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_lshr_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_lshr_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
lshr_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lshr_int_const)
	if (slot_type(src2) == Tconst) {
		lshr_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_lshr_int(dst, src, src2);
}
#endif

void
lshr_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lshr_long)
	lslot_lslot_slot(dst, src, src2, HAVE_lshr_long, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_int(src2, pusharg_long_idx_inc);
#else
	pusharg_int(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_lshrl);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}




/* ----------------------------------------------------------------------- */
/* Load and store.							   */
/*									   */

#if defined(HAVE_load_int)
void
load_int(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_load_int, Tload);
}
#endif

void
load_offset_int(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_int(dst, src);
	}
	else
#if defined(HAVE_load_offset_int)
	if (HAVE_load_offset_int_rangecheck(offset)) {
		slot_slot_const(dst, src, offset, HAVE_load_offset_int, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_int(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_addr_int(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_int)
	slot_slot_const(dst, NULL, (uintp)addr, HAVE_load_addr_int, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_int(dst, tmp);
	slot_freetmp(tmp);
#endif
}

#if defined(HAVE_load_ref)
void
load_ref(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_load_ref, Tload);
}
#endif

void
load_offset_ref(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_ref(dst, src);
	}
	else
#if defined(HAVE_load_offset_ref)
	if (HAVE_load_offset_ref_rangecheck(offset)) {
		slot_slot_const(dst, src, offset, HAVE_load_offset_ref, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_ref(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_addr_ref(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_ref)
	slot_slot_const(dst, NULL, (uintp)addr, HAVE_load_addr_ref, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_ref(dst, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_long(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_long)
	lslot_lslot_lslot(dst, 0, src, HAVE_load_long, Tload);
#else
	SlotInfo* tmp;

	slot_alloctmp(tmp);
	add_ref_const(tmp, src, 4);
	/* Don't use LSLOT & HSLOT here */
	load_int(dst, src);
	load_int(dst+1, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_addr_long(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_long)
	slot_slot_const(dst, 0, (uintp)addr, HAVE_load_addr_long, Tload);
#else
	load_addr_int(dst, addr);
	load_addr_int(dst+1, (void*)((uintp)addr+4));
#endif
}

void
load_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_float)
	slot_slot_slot(dst, NULL, src, HAVE_load_float, Tload);
#elif defined(HAVE_NO_FLOATING_POINT)
	load_int(dst, src);
#else
	KAFFEVM_ABORT();
#endif
}

void
load_addr_float(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_float)
	slot_slot_const(dst, NULL, (uintp)addr, HAVE_load_addr_float, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_float(dst, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_double)
	lslot_lslot_slot(dst, NULL, src, HAVE_load_double, Tload);
#elif defined(HAVE_NO_FLOATING_POINT) || defined(PS2LINUX)
	load_long(dst, src);
#else
	KAFFEVM_ABORT();
#endif
}

void
load_addr_double(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_double)
	lslot_lslot_const(dst, 0, (uintp)addr, HAVE_load_addr_double, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_double(dst, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_byte(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_byte)
	slot_slot_slot(dst, NULL, src, HAVE_load_byte, Tload);
#else
	load_int(dst, src);
	lshl_int_const(dst, dst, 8 * (sizeof(jint) - sizeof(jbyte)));
	ashr_int_const(dst, dst, 8 * (sizeof(jint) - sizeof(jbyte)));
#endif
}

void
load_char(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_char)
	slot_slot_slot(dst, NULL, src, HAVE_load_char, Tload);
#else
	load_int(dst, src);
	and_int_const(dst, dst, (1 << (8 * sizeof(jchar))) - 1);
#endif
}

void
load_short(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_short)
	slot_slot_slot(dst, NULL, src, HAVE_load_short, Tload);
#else
	load_int(dst, src);
	lshl_int_const(dst, dst, 8 * (sizeof(jint) - sizeof(jshort)));
	ashr_int_const(dst, dst, 8 * (sizeof(jint) - sizeof(jshort)));
#endif
}

void
load_addr_char(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_char)
	slot_slot_const(dst, 0, (uintp)addr, HAVE_load_addr_char, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_char(dst, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_addr_byte(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_byte)
	slot_slot_const(dst, 0, (uintp)addr, HAVE_load_addr_byte, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_byte(dst, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_addr_short(SlotInfo* dst, void* addr)
{
#if defined(HAVE_load_addr_short)
	slot_slot_const(dst, 0, (uintp)addr, HAVE_load_addr_short, Tload);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_short(dst, tmp);
	slot_freetmp(tmp);
#endif
}

void
load_offset_long(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_long(dst, src);
	}
	else
#if defined(HAVE_load_offset_long)
	if (HAVE_load_offset_long_rangecheck(offset)) {
		lslot_slot_const(dst, src, offset, HAVE_load_offset_long, Tload);
	}
	else
#endif
	{
                /* Don't use LSLOT & HSLOT here */
		load_offset_int(dst, src, offset);
		load_offset_int(dst+1, src, offset+4);
	}
}

void
load_offset_byte(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_byte(dst, src);
	}
	else
#if defined(HAVE_load_offset_byte)
	if (HAVE_load_offset_byte_rangecheck(offset)) {
		slot_slot_const(dst, src, offset, HAVE_load_offset_byte, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_byte(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_offset_char(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_char(dst, src);
	}
	else
#if defined(HAVE_load_offset_char)
	if (HAVE_load_offset_char_rangecheck(offset)) {
		slot_slot_const(dst, src, offset, HAVE_load_offset_char, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_char(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_offset_short(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_short(dst, src);
	}
	else
#if defined(HAVE_load_offset_short)
	if (HAVE_load_offset_short_rangecheck(offset)) {
		slot_slot_const(dst, src, offset, HAVE_load_offset_short, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_short(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_offset_float(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_float(dst, src);
	}
	else
#if defined(HAVE_load_offset_float)
	if (HAVE_load_offset_float_rangecheck(offset)) {
		slot_slot_const(dst, src, offset, HAVE_load_offset_float, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_float(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_offset_double(SlotInfo* dst, SlotInfo* src, jint offset)
{
	if (offset == 0) {
		load_double(dst, src);
	}
	else
#if defined(HAVE_load_offset_double)
	if (HAVE_load_offset_double_rangecheck(offset)) {
		lslot_slot_const(dst, src, offset, HAVE_load_offset_double, Tload);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, src, offset);
		load_double(dst, tmp);
		slot_freetmp(tmp);
	}
}

void
load_offset_scaled_int(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_int(dst, src, (jint)(slot_value(idx)->i * sizeof(jint) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_int)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_int, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jint);
		add_ref(tmp, src, tmp);
		load_offset_int(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_ref(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_ref(dst, src, (jint)(slot_value(idx)->i * sizeof(jref) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_ref)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_ref, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jref);
		add_ref(tmp, src, tmp);
		load_offset_ref(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_long(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_long(dst, src, (jint)(slot_value(idx)->i * sizeof(jlong) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_long)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_long, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jlong);
		add_ref(tmp, src, tmp);
		load_offset_long(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_float(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_float(dst, src, (jint)(slot_value(idx)->i * sizeof(jfloat) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_float)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_float, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jfloat);
		add_ref(tmp, src, tmp);
		load_offset_float(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_double(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_double(dst, src, (jint)(slot_value(idx)->i * sizeof(jdouble) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_double)
	lslot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_double, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jdouble);
		add_ref(tmp, src, tmp);
		load_offset_double(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_byte(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_byte(dst, src, (jint)(slot_value(idx)->i * sizeof(jbyte) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_byte)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_byte, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref(tmp, src, idx);
		load_offset_byte(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_char(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_char(dst, src, (jint)(slot_value(idx)->i * sizeof(jchar) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_char)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_char, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jchar);
		add_ref(tmp, src, tmp);
		load_offset_char(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_offset_scaled_short(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	if (slot_type(idx) == Tconst) {
		load_offset_short(dst, src, (jint)(slot_value(idx)->i * sizeof(jshort) + offset));
	}
	else
#if defined(HAVE_load_offset_scaled_short)
	slot_slot_slot_const_const(dst, src, idx, offset, 0, HAVE_load_offset_scaled_short, Tload);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jshort);
		add_ref(tmp, src, tmp);
		load_offset_short(dst, tmp, offset);
		slot_freetmp(tmp);
	}
#endif
}

void
load_code_ref(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_load_code_ref)
	slot_slot_slot(dst, 0, src, HAVE_load_code_ref, Tload);
#else
	load_ref(dst, src);
#endif
}

void
load_key(SlotInfo* dst, SlotInfo* src)
{
	load_int(dst, src);
}

#if defined(HAVE_store_int)
void
store_int(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(NULL, dst, src, HAVE_store_int, Tstore);
}
#endif

void
store_offset_int(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_int(dst, src);
	}
	else
#if defined(HAVE_store_const_offset_int)
	if (slot_type(src) == Tconst) {
		store_const_offset_int(dst, offset, slot_value(src)->i);
	}
	else
#endif
#if defined(HAVE_store_offset_int)
	if (HAVE_store_offset_int_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_int, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_int(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_int(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_int)
	slot_slot_const(NULL, src, (uintp)addr, HAVE_store_addr_int, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_int(tmp, src);
	slot_freetmp(tmp);
#endif
}

#if defined(HAVE_store_ref)
void
store_ref(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(NULL, dst, src, HAVE_store_ref, Tstore);
}
#endif

void
store_offset_ref(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_ref(dst, src);
	}
	else
#if defined(HAVE_store_offset_ref)
	if (HAVE_store_offset_ref_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_ref, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_ref(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_ref(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_ref)
	slot_slot_const(NULL, src, (uintp)addr, HAVE_store_addr_ref, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_ref(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_long(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_long)
	lslot_slot_lslot(0, dst, src, HAVE_store_long, Tstore);
#else
	SlotInfo* tmp;

	slot_alloctmp(tmp);
	add_ref_const(tmp, dst, 4);
	/* Don't use LSLOT & HSLOT here */
	store_int(dst, src);
	store_int(tmp, src+1);
	slot_freetmp(tmp);
#endif
}

void
store_offset_long(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_long(dst, src);
	}
	else
#if defined(HAVE_store_offset_long)
	if (HAVE_store_offset_long_rangecheck(offset)) {
		lslot_slot_const(src, dst, offset, HAVE_store_offset_long, Tstore);
	}
	else
#endif
	{
                /* Don't use LSLOT & HSLOT here */
		store_offset_int(dst, offset, src);
		store_offset_int(dst, offset+4, src+1);
	}
}

void
store_addr_long(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_long)
	slot_slot_const(0, src, (uintp)addr, HAVE_store_addr_long, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_long(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_float)
	slot_slot_slot(NULL, dst, src, HAVE_store_float, Tstore);
#elif defined(HAVE_NO_FLOATING_POINT)
	store_int(dst, src);
#else
	KAFFEVM_ABORT();
#endif
}

void
store_offset_float(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_float(dst, src);
	}
	else
#if defined(HAVE_store_offset_float)
	if (HAVE_store_offset_float_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_float, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_float(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_float(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_float)
	slot_slot_const(0, src, (uintp)addr, HAVE_store_addr_float, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_float(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_double)
	slot_slot_lslot(NULL, dst, src, HAVE_store_double, Tstore);
#elif defined(HAVE_NO_FLOATING_POINT) || defined(PS2LINUX)
	store_long(dst, src);
#else
	KAFFEVM_ABORT();
#endif
}

void
store_offset_double(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_double(dst, src);
	}
	else
#if defined(HAVE_store_offset_double)
	if (HAVE_store_offset_double_rangecheck(offset)) {
		lslot_slot_const(src, dst, offset, HAVE_store_offset_double, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_double(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_double(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_double)
	lslot_lslot_const(0, src, (uintp)addr, HAVE_store_addr_double, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_double(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_byte(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_byte)
	slot_slot_slot(NULL, dst, src, HAVE_store_byte, Tstore);
#else
	/* FIXME -- this is unlikely to work as-is as it doesn't
	   allow for alignment requirements on the integer load.  */
	SlotInfo* tmp;
	SlotInfo* tmp2;
	slot_alloctmp(tmp);
	slot_alloctmp(tmp2);
	and_int_const(tmp, src, (1 << (8 * sizeof(jbyte))) - 1);
	load_int(tmp2, dst);
	and_int_const(tmp2, tmp2, -(1 << (8 * sizeof(jbyte))));
	or_int(tmp2, tmp2, tmp);
	store_int(dst, tmp2);
	slot_freetmp(tmp);
	slot_freetmp(tmp2);
#endif
}

void
store_offset_byte(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_byte(dst, src);
	}
	else
#if defined(HAVE_store_const_offset_byte)
	if (slot_type(src) == Tconst) {
		store_const_offset_byte(dst, offset, slot_value(src)->i);
	}
	else
#endif
#if defined(HAVE_store_offset_byte)
	if (HAVE_store_offset_byte_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_byte, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_byte(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_byte(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_byte)
	slot_slot_const(0, src, (uintp)addr, HAVE_store_addr_byte, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_byte(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_char(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_char)
	slot_slot_slot(NULL, dst, src, HAVE_store_char, Tstore);
#else
	/* FIXME -- this is unlikely to work as-is as it doesn't
	   allow for alignment requirements on the integer load.  */
	SlotInfo* tmp;
	SlotInfo* tmp2;
	slot_alloctmp(tmp);
	slot_alloctmp(tmp2);
	and_int_const(tmp, src, (1 << (8 * sizeof(jchar))) - 1);
	load_int(tmp2, dst);
	and_int_const(tmp2, tmp2, -(1 << (8 * sizeof(jchar))));
	or_int(tmp2, tmp2, tmp);
	store_int(dst, tmp2);
	slot_freetmp(tmp);
	slot_freetmp(tmp2);
#endif
}

void
store_offset_char(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_char(dst, src);
	}
	else
#if defined(HAVE_store_offset_char)
	if (HAVE_store_offset_char_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_char, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_char(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_char(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_char)
	slot_slot_const(0, src, (uintp)addr, HAVE_store_addr_char, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_char(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_short(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_short)
	slot_slot_slot(NULL, dst, src, HAVE_store_short, Tstore);
#else
	/* FIXME -- this is unlikely to work as-is as it doesn't
	   allow for alignment requirements on the integer load.  */
	SlotInfo* tmp;
	SlotInfo* tmp2;
	slot_alloctmp(tmp);
	slot_alloctmp(tmp2);
	and_int_const(tmp, src, (1 << (8 * sizeof(jshort))) - 1);
	load_int(tmp2, dst);
	and_int_const(tmp2, tmp2, -(1 << (8 * sizeof(jshort))));
	or_int(tmp2, tmp2, tmp);
	store_int(dst, tmp2);
	slot_freetmp(tmp);
	slot_freetmp(tmp2);
#endif
}

void
store_offset_short(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_short(dst, src);
	}
	else
#if defined(HAVE_store_offset_short)
	if (HAVE_store_offset_short_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_short, Tstore);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref_const(tmp, dst, offset);
		store_short(tmp, src);
		slot_freetmp(tmp);
	}
}

void
store_addr_short(void* addr, SlotInfo* src)
{
#if defined(HAVE_store_addr_short)
	slot_slot_const(0, src, (uintp)addr, HAVE_store_addr_short, Tstore);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_short(tmp, src);
	slot_freetmp(tmp);
#endif
}

void
store_offset_scaled_int(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_int(dst, (jint)(slot_value(idx)->i * sizeof(jint) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_int)
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_int, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jint);
		add_ref(tmp, dst, tmp);
		store_offset_int(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

void
store_offset_scaled_ref(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_ref(dst, (jint)(slot_value(idx)->i * sizeof(jref) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_ref)
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_ref, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jref);
		add_ref(tmp, dst, tmp);
		store_offset_ref(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

void
store_offset_scaled_long(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_long(dst, (jint)(slot_value(idx)->i * sizeof(jlong) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_long)
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_long, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jlong);
		add_ref(tmp, dst, tmp);
		store_offset_long(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

void
store_offset_scaled_float(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_float(dst, (jint)(slot_value(idx)->i * sizeof(jfloat) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_float)
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_float, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jfloat);
		add_ref(tmp, dst, tmp);
		store_offset_float(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

void
store_offset_scaled_double(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_double(dst, (jint)(slot_value(idx)->i * sizeof(jdouble) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_double)
	slot_slot_lslot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_double, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jdouble);
		add_ref(tmp, dst, tmp);
		store_offset_double(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

void
store_offset_scaled_byte(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_byte(dst, (jint)(slot_value(idx)->i * sizeof(jbyte) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_byte)
#if defined(HAVE_store_const_offset_scaled_byte)
	if (slot_type(src) == Tconst) {
		store_const_offset_scaled_byte(dst, idx, offset, slot_value(src)->i);
	}
	else
#endif
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_byte, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		add_ref(tmp, dst, idx);
		store_offset_byte(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

#if defined(HAVE_store_const_offset_scaled_byte)
void
store_const_offset_scaled_byte(SlotInfo* dst, SlotInfo* idx, int offset, jint src)
{
	slot_slot_const_const_const(dst, idx, offset, src, 0, HAVE_store_const_offset_scaled_byte, Tstore);
}
#endif

void
store_offset_scaled_char(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_char(dst, (jint)(slot_value(idx)->i * sizeof(jchar) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_char)
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_char, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jchar);
		add_ref(tmp, dst, tmp);
		store_offset_char(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

void
store_offset_scaled_short(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	if (slot_type(idx) == Tconst) {
		store_offset_short(dst, (jint)(slot_value(idx)->i * sizeof(jshort) + offset), src);
	}
	else
#if defined(HAVE_store_offset_scaled_short)
	slot_slot_slot_const_const(dst, idx, src, offset, 0, HAVE_store_offset_scaled_short, Tstore);
#else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		lshl_int_const(tmp, idx, SHIFT_jshort);
		add_ref(tmp, dst, tmp);
		store_offset_short(tmp, offset, src);
		slot_freetmp(tmp);
	}
#endif
}

#if defined(HAVE_store_const_offset_int)
void
store_const_offset_int(SlotInfo* dst, jint offset, jint val)
{
	if (HAVE_store_const_offset_int_rangecheck(offset)) {
		slot_const_const(dst, offset, val, HAVE_store_const_offset_int, Tstore);
	}
	else
	{
		SlotInfo* tmp;
		SlotInfo* tmp2;
		slot_alloctmp(tmp);
		slot_alloctmp(tmp2);
		add_ref_const(tmp, dst, offset);
		move_int_const(tmp2, val);
		store_int(tmp, tmp2);
		slot_freetmp(tmp);
		slot_freetmp(tmp2);
	}
}
#endif

#if defined(HAVE_store_const_offset_byte)
void
store_const_offset_byte(SlotInfo* dst, jint offset, jint val)
{
	if (HAVE_store_const_offset_byte_rangecheck(offset)) {
		slot_const_const(dst, offset, val, HAVE_store_const_offset_byte, Tstore);
	}
	else
	{
		SlotInfo* tmp;
		SlotInfo* tmp2;
		slot_alloctmp(tmp);
		slot_alloctmp(tmp2);
		add_ref_const(tmp, dst, offset);
		move_int_const(tmp2, val);
		store_byte(tmp, tmp2);
		slot_freetmp(tmp);
		slot_freetmp(tmp2);
	}
}
#endif


/* ----------------------------------------------------------------------- */
/* Function argument management.					   */
/*									   */

void
pusharg_int_const(int val, int idx)
{
#if defined(HAVE_pusharg_int_const)
	if (HAVE_pusharg_int_const_rangecheck(val)) {
		slot_const_const(NULL, val, idx, HAVE_pusharg_int_const, Tnull);
		argcount += 1;
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		pusharg_int(tmp, idx);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_pusharg_int)
void
pusharg_int(SlotInfo* src, int idx)
{
#if defined(HAVE_pusharg_int_const)
	if (slot_type(src) == Tconst) {
		pusharg_int_const(slot_value(src)->i, idx);
	}
	else
#endif
	{
		slot_slot_const(NULL, src, idx, HAVE_pusharg_int, Tnull);
		argcount += 1;
	}
}
#endif

#if defined(HAVE_pusharg_ref)
void
pusharg_ref(SlotInfo* src, int idx)
{
#if defined(HAVE_pusharg_ref_const)
	if (slot_type(src) == Tconst) {
		pusharg_ref_const(slot_value(src)->l, idx);
	}
	else
#endif
	{
		slot_slot_const(NULL, src, idx, HAVE_pusharg_ref, Tnull);
		argcount += 1;
	}
}
#endif

void
pusharg_ref_const(void* val, int idx)
{
#if defined(HAVE_pusharg_ref_const)
	if (HAVE_pusharg_ref_const_rangecheck((jword)val)) {
		slot_const_const(NULL, (jword)val, idx, HAVE_pusharg_ref_const, Tnull);
		argcount += 1;
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_ref_const(tmp, val);
		pusharg_ref(tmp, idx);
		slot_freetmp(tmp);
	}
}

void
pusharg_class_const(Hjava_lang_Class* cls, int idx)
{
#if 1
	pusharg_ref_const(cls, idx);
#else
	???
#endif
}

void
pusharg_utf8_const(Utf8Const* val, int idx)
{
#if 1
	pusharg_ref_const(val, idx);
#else
	???
#endif
}

void
pusharg_float(SlotInfo* src, int idx)
{
#if defined(HAVE_pusharg_float)
	slot_slot_const(NULL, src, idx, HAVE_pusharg_float, Tnull);
	argcount += 1;
#elif defined(HAVE_NO_FLOATING_POINT)
	pusharg_int(src, idx);
#else
	KAFFEVM_ABORT();
#endif
}

void
pusharg_double(SlotInfo* src, int idx)
{
#if defined(HAVE_pusharg_double)
	lslot_lslot_const(NULL, src, idx, HAVE_pusharg_double, Tnull);
	argcount += pusharg_long_idx_inc;
#elif defined(HAVE_NO_FLOATING_POINT) || defined(PS2LINUX)
	pusharg_long(src, idx);
#else
	KAFFEVM_ABORT();
#endif
}

void
pusharg_long(SlotInfo* src, int idx)
{
#if defined(HAVE_pusharg_long)
	lslot_lslot_const(0, src, idx, HAVE_pusharg_long, Tnull);
	argcount += pusharg_long_idx_inc;
#elif defined(PUSHARG_FORWARDS)
	/* Don't use LSLOT & HSLOT here */
	pusharg_int(src, idx);
	pusharg_int(src+1, idx+1);
#else
	/* Don't use LSLOT & HSLOT here */
	pusharg_int(src+1, idx+1);
	pusharg_int(src, idx);
#endif
}

void
popargs_internal(int does_return UNUSED)
{
 	if (argcount != 0) {
#if defined(HAVE_popargs)
		/*
		 * Must always call popargs so the backend can do cleanup.
		 *
		 * Might be able to get away with sending an argcount of zero,
		 * but, well just send the whole thing for now.  Alternatively,
		 * it might be better to add a new back end call to
		 * end_func_sync.
		 */
		slot_slot_const(NULL, NULL, argcount, HAVE_popargs, Tnull);
#endif
		if (argcount > maxPush) {
			maxPush = argcount;
		}
		argcount = 0;
	}
}



/* ----------------------------------------------------------------------- */
/* Control flow changes.						   */
/*									   */

#if defined(HAVE_branch_and_link)
void
branch_and_link(label* dst, int type)
{
	slot_const_const(NULL, (jword)dst, type & ~blink, HAVE_branch_and_link, Tnull);
}
#endif

#if defined(HAVE_branch)
void
branch(label* dst, int type)
{
#if defined(HAVE_branch_and_link)
	if( type & blink )
	{
		branch_and_link(dst, type);
	}
	else
#endif
	{
		slot_const_const(NULL, (jword)dst, type & ~blink, HAVE_branch, Tnull);
	}
}
#endif

void
cbranch_int(SlotInfo* s1, SlotInfo* s2, label* dst, int type)
{
#if defined(HAVE_cbranch_int)
	slot_slot_slot_const_const(NULL, s1, s2, (jword)dst, type,
				   HAVE_cbranch_int, Tcomplex);
#else
	cmp_int(NULL, s1, s2);
	branch(dst, type);
#endif
}

void
cbranch_offset_int(SlotInfo* s1, SlotInfo* s2, jint s3, label* dst, int type)
{
#if defined(HAVE_cmp_int)
	cmp_offset_int(NULL, s1, s2, s3);
	branch(dst, type);
#else
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	load_offset_int(tmp, s2, s3);
	cbranch_int(s1, tmp, dst, type);
	slot_freetmp(tmp);
#endif
}

void
cbranch_int_const(SlotInfo* s1, jint val, label* dst, int type)
{
#if defined(HAVE_cbranch_int_const)
	if (HAVE_cbranch_int_const_rangecheck(val)) {
		slot_slot_const_const_const(NULL, s1, val, (jword)dst, type, HAVE_cbranch_int_const, Tcomplex);
	}
	else {
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		cbranch_int(s1, tmp, dst, type);
		slot_freetmp(tmp);
	}
#else
	cmp_int_const(NULL, s1, val);
	branch(dst, type);
#endif
}

void
cbranch_ref(SlotInfo* s1, SlotInfo* s2, label* dst, int type)
{
#if defined(HAVE_cbranch_ref)
	slot_slot_slot_const_const(0, s1, s2, (jword)dst, type,
				   HAVE_cbranch_ref, Tcomplex);
#else
	cmp_ref(NULL, s1, s2);
	branch(dst, type);
#endif
}

void
cbranch_ref_const(SlotInfo* s1, void *val, label* dst, int type)
{
#if defined(HAVE_cbranch_ref_const)
	if (HAVE_cbranch_ref_const_rangecheck((jword)val)) {
		slot_slot_const_const_const(NULL, s1, (jword)val, (jword)dst, type,
					    HAVE_cbranch_ref_const, Tcomplex);
	}
	else
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_ref_const(tmp, val);
		cbranch_ref(s1, tmp, dst, type);
		slot_freetmp(tmp);
	}
#else
	cmp_ref_const(NULL, s1, val);
	branch(dst, type);
#endif
}

void
branch_indirect(SlotInfo* dst)
{
#if defined(HAVE_branch_indirect)
	slot_slot_const(NULL, dst, ba, HAVE_branch_indirect, Tnull);
#else
	load_ref(dst, dst);
	call(dst);
#endif
}

#if defined(HAVE_call)
void
call(SlotInfo* dst)
{
	slot_slot_const(NULL, dst, ba, HAVE_call, Tnull);
}
#endif

void
call_indirect_method(Method *meth)
{
	void* ptr;

	if (METHOD_TRANSLATED(meth)) {
		_call_soft(METHOD_NATIVECODE(meth), 1);
	}
	else {
		ptr = PMETHOD_NATIVECODE(meth);

#if defined(HAVE_call_indirect_const)
		slot_const_const(NULL, (jword)ptr, ba, HAVE_call_indirect_const, Tnull);
#else
		{
			SlotInfo* tmp;
			slot_alloctmp(tmp);
			move_ref_const(tmp, ptr);
			load_ref(tmp, tmp);
			call(tmp);
			slot_freetmp(tmp);
		}
#endif
	}
}

static void
_call_soft(void *routine, int profiled)
{
#if defined(HAVE_call_soft)
	label* l = KaffeJIT3_newLabel();
	l->type = Labsolute|Lexternal | (profiled ? 0 : Lnoprofile);
	l->at = 0;
	l->to = (uintp)routine;	/* What place does it goto */
	l->from = 0;

	slot_const_const(0, (jword)l, ba, HAVE_call_soft, Tnull);
#elif defined(HAVE_call_ref)
	label* l;
	l = KaffeJIT3_newLabel();
	l->type = Lexternal | (profiled ? 0 : Lnoprofile);
	l->at = 0;
	l->to = (uintp)routine;	/* What place does it goto */
	l->from = 0;

	slot_const_const(NULL, (jword)l, ba, HAVE_call_ref, Tnull);
#else
	label* l;
	constpool* c;
	SlotInfo* tmp;

	l = KaffeJIT3_newLabel();
	c = KaffeJIT3_newConstant(CPref, routine);
	l->type = Lconstant;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

	slot_alloctmp(tmp);
#if defined(HAVE_load_constpool_ref)
	slot_slot_const(tmp, 0, (jword)l, HAVE_load_constpool_ref, Tnull);
#else
	move_label_const(tmp, l);
	load_ref(tmp, tmp);
#endif
	call(tmp);
	slot_freetmp(tmp);
#endif
}

void
call_soft(void *routine)
{
	_call_soft(routine, 0);
}

#if defined(HAVE_return_int)
void
return_int(SlotInfo* dst)
{
	slot_slot_slot(dst, NULL, NULL, HAVE_return_int, Tnull);
}
#endif

#if defined(HAVE_return_ref)
void
return_ref(SlotInfo* dst)
{
	slot_slot_slot(dst, NULL, NULL, HAVE_return_ref, Tnull);
}
#endif

#if defined(HAVE_return_long)
void
return_long(SlotInfo* dst)
{
	lslot_lslot_lslot(dst, NULL, NULL, HAVE_return_long, Tnull);
}
#endif

void
return_float(SlotInfo* dst)
{
#if defined(HAVE_return_float)
	slot_slot_slot(dst, NULL, NULL, HAVE_return_float, Tnull);
#if defined(HAVE_FLOATING_POINT_STACK)
	begin_sync();
	end_sync();
#endif
#elif defined(HAVE_NO_FLOATING_POINT)
	return_int(dst);
#else
	KAFFEVM_ABORT();
#endif
}

void
return_double(SlotInfo* dst)
{
#if defined(HAVE_return_double)
	lslot_lslot_lslot(dst, NULL, NULL, HAVE_return_double, Tnull);
#if defined(HAVE_FLOATING_POINT_STACK)
	begin_sync();
	end_sync();
#endif
#elif defined(HAVE_NO_FLOATING_POINT) || defined(PS2LINUX)
	return_long(dst);
#else
	KAFFEVM_ABORT();
#endif
}

#if defined(HAVE_returnarg_int)
void
returnarg_int(SlotInfo* src)
{
	slot_slot_slot(NULL, NULL, src, HAVE_returnarg_int, Tcopy);
}
#endif

#if defined(HAVE_returnarg_ref)
void
returnarg_ref(SlotInfo* src)
{
	slot_slot_slot(NULL, NULL, src, HAVE_returnarg_ref, Tcopy);
}
#endif

#if defined(HAVE_returnarg_long)
void
returnarg_long(SlotInfo* src)
{
	lslot_lslot_lslot(NULL, NULL, src, HAVE_returnarg_long, Tcopy);
}
#endif

void
returnarg_float(SlotInfo* src)
{
#if defined(HAVE_returnarg_float)
	slot_slot_slot(NULL, NULL, src, HAVE_returnarg_float, Tcopy);
#elif defined(HAVE_NO_FLOATING_POINT)
	returnarg_int(src);
#else
	KAFFEVM_ABORT();
#endif
}

void
returnarg_double(SlotInfo* src)
{
#if defined(HAVE_returnarg_double)
	lslot_lslot_lslot(NULL, NULL, src, HAVE_returnarg_double, Tcopy);
#elif defined(HAVE_NO_FLOATING_POINT) || defined(PS2LINUX)
	returnarg_long(src);
#else
	KAFFEVM_ABORT();
#endif
}

/* ----------------------------------------------------------------------- */
/* Labels.								   */
/*									   */

label*
reference_label(int32 i UNUSED, int32 n)
{
	label* l;

	assert(n < MAXLABTAB);
	if (labtab[n] == 0) {
		l = KaffeJIT3_newLabel();
		labtab[n] = l;
		l->type = Lnull;
		l->at = 0;
		l->from = 0;
		l->to = 0;
	}
	else {
		l = labtab[n];
		labtab[n] = NULL;
	}
	return (l);
}

label*
reference_code_label(uintp offset)
{
	label* l = KaffeJIT3_newLabel();
	l->at = 0;		/* Where is the jump */
	l->to = offset;		/* What place does it goto */
	l->from = 0;
	l->type = Lcode;
	return (l);
}

label*
reference_table_label(int32 n)
{
	label* l;

	assert(n < MAXLABTAB);
	if (labtab[n] == 0) {
		l = KaffeJIT3_newLabel();
		labtab[n] = l;
		l->type = Lnull;
		l->at = 0;
		l->from = 0;
		l->to = 0;
	}
	else {
		l = labtab[n];
		labtab[n] = NULL;
	}
	return (l);
}

SlotInfo*
stored_code_label(SlotInfo* dst)
{
	return (dst);
}

SlotInfo*
table_code_label(SlotInfo* dst)
{
	return (dst);
}

#if defined(HAVE_set_label)
void
set_label(int i UNUSED, int n)
{
	assert(n < MAXLABTAB);
	if (labtab[n] == 0) {
		labtab[n] = KaffeJIT3_newLabel();
		labtab[n]->type = Linternal;
		labtab[n]->at = 0;
		labtab[n]->from = 0;
		labtab[n]->to = 0;
		slot_slot_const(NULL, NULL, (jword)labtab[n], HAVE_set_label, Tnull);
	}
	else {
		assert(labtab[n]->type == Lnull);
		labtab[n]->type = Linternal;
		slot_slot_const(NULL, NULL, (jword)labtab[n], HAVE_set_label, Tnull);
		labtab[n] = NULL;
	}
}
#endif

#if defined(HAVE_build_code_ref)
label*
build_code_ref(uint8* pos, uintp _pc)
{
	label* l;
	jint offset;

	offset = (pos[0] * 0x01000000 + pos[1] * 0x00010000 +
		  pos[2] * 0x00000100 + pos[3] * 0x00000001);
	l = reference_code_label(_pc+offset);

	slot_slot_const(NULL, NULL, (jword)l, HAVE_build_code_ref, Tnull);
	return (l);
}
#endif

#if defined(HAVE_build_key)
void
build_key(uint8* pos)
{
	jint val = (pos[0] * 0x01000000 + pos[1] * 0x00010000 +
		    pos[2] * 0x00000100 + pos[3] * 0x00000001);

	slot_slot_const(NULL, NULL, val, HAVE_build_key, Tnull);
}
#endif


/* ----------------------------------------------------------------------- */
/* Comparisons.								   */
/*									   */

#if defined(HAVE_cmp_int)
void
_cmp_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_cmp_int, Tcomplex);
}

void
cmp_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_cmp_int_const)
	if (HAVE_cmp_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_cmp_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		_cmp_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
cmp_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmp_int_const)
	if (slot_type(src2) == Tconst) {
		cmp_int_const(dst, src, slot_value(src2)->i);
	}
	else
#endif
	_cmp_int(dst, src, src2);
}
#endif

void
cmp_offset_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2, jint off)
{
#if defined(HAVE_cmp_offset_int)
	slot_slot_slot_const_const(dst, src, src2, off, 0, HAVE_cmp_offset_int, Tcomplex);
#elif defined(HAVE_cmpl_int)
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	load_offset_int(tmp, src2, off);
	slot_slot_slot(dst, src, tmp, HAVE_cmpl_int, Tcomplex);
	slot_freetmp(tmp);
#elif defined(HAVE_cmp_int)
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	load_offset_int(tmp, src2, off);
	cmp_int(dst, src, tmp);
	slot_freetmp(tmp);
#else
	KAFFEVM_ABORT();
#endif
}

#if defined(HAVE_cmp_ref)
void
cmp_ref_const(SlotInfo* dst, SlotInfo* src, void* val)
{
#if defined(HAVE_cmp_ref_const)
	if (HAVE_cmp_ref_const_rangecheck((jword)val)) {
		slot_slot_const(dst, src, (jword)val, HAVE_cmp_ref_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_ref_const(tmp, val);
		cmp_ref(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
cmp_ref(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmp_ref_const)
	/* Note: This optimization assumes that we only ever check pointer
	 * equality.
	 */
	if (slot_type(src2) == Tconst) {
		cmp_ref_const(dst, src, slot_value(src2)->l);
	}
	else if (slot_type(src) == Tconst) {
		cmp_ref_const(dst, src2, slot_value(src)->l);
	}
	else
#endif
	slot_slot_slot(dst, src, src2, HAVE_cmp_ref, Tcomplex);
}
#endif

void
lcmp(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lcmp)
	slot_lslot_lslot(dst, src, src2, HAVE_lcmp, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_long(src, 0);
	pusharg_long(src2, pusharg_long_idx_inc);
#else
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
#endif
	call_soft(soft_lcmp);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
cmpl_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpl_float)
	slot_slot_slot(dst, src, src2, HAVE_cmpl_float, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_fcmpl);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
cmpl_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpl_double)
	slot_lslot_lslot(dst, src, src2, HAVE_cmpl_double, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_dcmpl);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
cmpg_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpg_float)
	slot_slot_slot(dst, src, src2, HAVE_cmpg_float, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_float(src, 0);
	pusharg_float(src2, 1);
#else
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
#endif
	call_soft(soft_fcmpg);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
cmpg_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpg_double)
	slot_lslot_lslot(dst, src, src2, HAVE_cmpg_double, Tcomplex);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_double(src, 0);
	pusharg_double(src2, pusharg_long_idx_inc);
#else
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
#endif
	call_soft(soft_dcmpg);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

/* ----------------------------------------------------------------------- */
/* Conversions.								   */
/*									   */

void
cvt_int_long(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_long)
	lslot_lslot_slot(dst, 0, src, HAVE_cvt_int_long, Tcomplex);
#else
	move_int(LSLOT(dst), src);
	ashr_int_const(HSLOT(dst), src, (8 * sizeof(jint)) - 1);
#endif
}

void
cvt_int_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_float)
	slot_slot_slot(dst, NULL, src, HAVE_cvt_int_float, Tcomplex);
#else
	begin_func_sync();
	pusharg_int(src, NULL);
	call_soft(soft_cvtif);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
cvt_int_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_double)
	lslot_lslot_slot(dst, NULL, src, HAVE_cvt_int_double, Tcomplex);
#else
	begin_func_sync();
	pusharg_int(src, NULL);
	call_soft(soft_cvtid);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

void
cvt_long_int(SlotInfo* dst, SlotInfo* src)
{
	move_int(dst, LSLOT(src));
}

void
cvt_long_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_long_float)
	slot_slot_lslot(dst, 0, src, HAVE_cvt_long_float, Tcomplex);
#else
	begin_func_sync();
	pusharg_long(src, 0);
	call_soft(soft_cvtlf);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
cvt_long_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_long_double)
	lslot_lslot_lslot(dst, 0, src, HAVE_cvt_long_double, Tcomplex);
#else
	begin_func_sync();
	pusharg_long(src, 0);
	call_soft(soft_cvtld);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

void
cvt_float_int(SlotInfo* dst, SlotInfo* src)
{
	used_ieee_rounding = true;
#if defined(HAVE_cvt_float_int)
	slot_slot_slot(dst, 0, src, HAVE_cvt_float_int, Tcomplex);
#elif defined(HAVE_cvt_float_int_ieee)
	{
	  SlotInfo *tmp;
	  
	  slot_alloctmp(tmp);

	  end_sub_block();
	  and_int_const(tmp, src, FEXPMASK);
	  cbranch_int_const_ne(tmp, FEXPMASK, reference_label(1, 1));
	  
	  and_int_const(tmp, src, FMANMASK);
	  cbranch_int_const_eq(tmp, 0, reference_label(1, 2));

          start_sub_block();
	  move_int_const(dst, 0);
	  end_sub_block();
	  branch_a(reference_label(1, 3));

	  set_label(1, 1);
	  set_label(1, 2);
	  start_sub_block();
	  slot_slot_lslot(dst, 0, src, HAVE_cvt_float_int_ieee, Tcomplex);
	  end_sub_block();

	  set_label(1, 3);

	  start_sub_block();

	  slot_freetmp(tmp);
	}
#else
	begin_func_sync();
	pusharg_float(src, 0);
	call_soft(soft_cvtfi);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
cvt_float_long(SlotInfo* dst, SlotInfo* src)
{
	used_ieee_rounding = true;
#if defined(HAVE_cvt_float_long)
	lslot_lslot_slot(dst, 0, src, HAVE_cvt_float_long, Tcomplex);
#else
	begin_func_sync();
	pusharg_float(src, 0);
	call_soft(soft_cvtfl);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

void
cvt_float_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_float_double)
	lslot_lslot_slot(dst, NULL, src, HAVE_cvt_float_double, Tcomplex);
#else
	begin_func_sync();
	pusharg_float(src, 0);
	call_soft(soft_cvtfd);
	popargs();
	end_func_sync();
	return_double(dst);
#endif
}

void
cvt_double_int(SlotInfo* dst, SlotInfo* src)
{
	used_ieee_rounding = true;
#if defined(HAVE_cvt_double_int)
	slot_slot_lslot(dst, NULL, src, HAVE_cvt_double_int, Tcomplex);
#elif defined(HAVE_cvt_double_int_ieee)
	{
	  SlotInfo *tmp;

	  end_sub_block();
	  slot_alloc2tmp(tmp);

	  and_long_const(tmp, src, DEXPMASK);
	  cbranch_int_const_ne(LSLOT(tmp), (jint)(DEXPMASK & 0xffffffff), reference_label(1, 1));
	  cbranch_int_const_ne(HSLOT(tmp), (jint)((DEXPMASK >> 32) & 0xffffffff), reference_label(1, 2));
	  
	  and_long_const(tmp, src, DMANMASK);
	  cbranch_int_const_ne(LSLOT(tmp), 0, reference_label(1, 3));
	  cbranch_int_const_eq(HSLOT(tmp), 0, reference_label(1, 4));

	  set_label(1, 3);
	  start_sub_block();
	  move_int_const(dst, 0);
	  end_sub_block();
	  branch_a(reference_label(1, 5));

	  set_label(1, 4);
	  set_label(1, 1);
	  set_label(1, 2);
	  start_sub_block();
	  slot_slot_lslot(dst, 0, src, HAVE_cvt_double_int_ieee, Tcomplex);
	  end_sub_block();

	  set_label(1, 5);
	  slot_free2tmp(tmp);
	  start_sub_block();
	}
#else
	begin_func_sync();
	pusharg_double(src, 0);
	call_soft(soft_cvtdi);
	popargs();
	end_func_sync();
	return_int(dst);
#endif
}

void
cvt_double_long(SlotInfo* dst, SlotInfo* src)
{
	used_ieee_rounding = true;
#if defined(HAVE_cvt_double_long)
	lslot_lslot_lslot(dst, 0, src, HAVE_cvt_double_long, Tcomplex);
#else
	begin_func_sync();
	pusharg_double(src, 0);
	call_soft(soft_cvtdl);
	popargs();
	end_func_sync();
	return_long(dst);
#endif
}

void
cvt_double_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_double_float)
	slot_slot_lslot(dst, NULL, src, HAVE_cvt_double_float, Tcomplex);
#else
	begin_func_sync();
	pusharg_double(src, 0);
	call_soft(soft_cvtdf);
	popargs();
	end_func_sync();
	return_float(dst);
#endif
}

void
cvt_int_byte(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_byte)
	slot_slot_slot(dst, NULL, src, HAVE_cvt_int_byte, Tcomplex);
#else
	lshl_int_const(dst, src, 8 * (sizeof(jint) - sizeof(jbyte)));
	ashr_int_const(dst, dst, 8 * (sizeof(jint) - sizeof(jbyte)));
#endif
}

void
cvt_int_char(SlotInfo* dst, SlotInfo* src)
{
	and_int_const(dst, src, (1 << (8 * sizeof(jchar))) - 1);
}

void
cvt_int_short(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_short)
	slot_slot_slot(dst, 0, src, HAVE_cvt_int_short, Tcomplex);
#else
	lshl_int_const(dst, src, 8 * (sizeof(jint) - sizeof(jshort)));
	ashr_int_const(dst, dst, 8 * (sizeof(jint) - sizeof(jshort)));
#endif
}



/* ----------------------------------------------------------------------- */
/* Breakpoints.								   */
/*									   */

void
softcall_breakpoint(void)
{
	KAFFEVM_ABORT();
}

/* ----------------------------------------------------------------------- */
/* Build call frame.							   */
/*									   */

void
build_call_frame(Utf8Const* sig, SlotInfo* obj, int sp_idx)
{
	struct pusharg_info {
		char type;
		uint16 arg_idx;
		uint16 sp_idx;
	};
	static struct pusharg_info* args;
	static int sz_args = 0;
	int arg_idx;
	int idx;
	int limit;
	const char* sigptr;

	/* Make sure we have enough argument space */
	if (sp_idx + 2 > sz_args) {
		sz_args = sp_idx + 2;

		args = gc_realloc(args,
				  sizeof(struct pusharg_info) * sz_args,
				  KGC_ALLOC_JIT_ARGS);
		if( !args )
		{
			/* XXX We should be a little more graceful */
			KAFFEVM_ABORT();
		}
	}

	/* If we've got an object ... */
	idx = 0;
	if (obj != 0) {
		args[idx].type = 'O';
		args[idx].arg_idx = idx;
		args[idx].sp_idx = sp_idx;
		idx++;
	}
	sp_idx--;
	arg_idx = idx;

	sigptr = sig->data;
	assert(sigptr[0] == '(');
	for (sigptr++; *sigptr != ')'; sigptr++) {

		args[idx].arg_idx = arg_idx;
		args[idx].sp_idx = sp_idx;
		args[idx].type = *sigptr;

		switch (*sigptr) {
		case '[':
			while (*++sigptr == '[')
				;
			if (*sigptr == 'L') {
				sigptr = strchr(sigptr, ';');
			}
			break;

		case 'L':
			sigptr = strchr(sigptr, ';');
			break;

		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
		case 'F':
			break;

		case 'J':
		case 'D':
			sp_idx--;
			args[idx].sp_idx = sp_idx;
			arg_idx += pusharg_long_idx_inc - 1;
			break;

		case 'V':
		default:
			KAFFEVM_ABORT();
		}

		sp_idx--;
		arg_idx++;
		idx++;
	}

#if defined(STACK_LIMIT)
	args[idx].type = 'K';
	args[idx].arg_idx = arg_idx;
	args[idx].sp_idx = sp_idx;
	idx++;
#endif

	limit = idx;

#if defined(PUSHARG_FORWARDS)
	for (idx = 0; idx < limit; idx++) {
#else
	for (idx = limit-1; idx >= 0; idx--) {
#endif
		sp_idx = args[idx].sp_idx;
		arg_idx = args[idx].arg_idx;
		switch (args[idx].type) {
		case 'L':
		case '[':
			pusharg_ref(stack(sp_idx), arg_idx);
			break;
		case 'O':
			/* Special: object we've invoking on */
			pusharg_ref(obj, arg_idx);
			break;
#if defined(STACK_LIMIT)
		case 'K':
			/* Special: push stack limit */
			pusharg_ref(stack_limit, arg_idx);
			break;
#endif
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			pusharg_int(stack(sp_idx), arg_idx);
			break;
		case 'J':
			pusharg_long(stack_long(sp_idx), arg_idx);
			break;
		case 'F':
			pusharg_float(stack_float(sp_idx), arg_idx);
			break;
		case 'D':
			pusharg_double(stack_double(sp_idx), arg_idx);
			break;
		default:
			break;
		}
	}
}

/* ----------------------------------------------------------------------- */
/* Soft calls.								   */
/*									   */

#if 0
void
/* Custom edition */ softcall_lookupmethod(SlotInfo* dst, Method* meth, SlotInfo* obj)
{
	/* 'obj' must be written back since it will be reused */
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref(obj, 0);
	pusharg_utf8_const(meth->name, 1);
	pusharg_utf8_const(meth->sig, 2);
#else
	pusharg_utf8_const(meth->sig, 2);
	pusharg_utf8_const(meth->name, 1);
	pusharg_ref(obj, 0);
#endif
	/* Custom edition */ call_soft(soft_lookupmethod);
	popargs();
	end_func_sync();
	return_ref(dst);
}
#endif

void
softcall_lookupinterfacemethod(SlotInfo* dst, Method* meth, SlotInfo* obj)
{
	/* 'obj' must be written back since it will be reused */
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
        pusharg_ref(obj, 0);
        pusharg_class_const(meth->class, 1);
        pusharg_ref_const((void*)(int)meth->idx, 2);
#else
        pusharg_ref_const((void*)(int)meth->idx, 2);
        pusharg_class_const(meth->class, 1);
        pusharg_ref(obj, 0);
#endif
	call_soft(soft_lookupinterfacemethod);
	popargs();
	end_func_sync();
	return_ref(dst);
}

void
check_array_index(SlotInfo* obj, SlotInfo* idx)
{
	if (noArrayBoundsChecks != 0) {
		return;
	}
#if defined(HAVE_ccall)
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		load_offset_int(tmp, obj, object_array_length);
		ccall_soft_int_ugt(tmp, idx, soft_badarrayindex);
		slot_freetmp(tmp);
	}
#else
#if defined(HAVE_fakecall) || defined(HAVE_fakecall_constpool)
	if (!canCatch(BADARRAYINDEX)) {
		cbranch_offset_int(idx, obj, object_array_length, newFakeCall(soft_badarrayindex, pc), buge | blink);
	}
	else
#endif
	{
		end_sub_block();
		cbranch_offset_int_ult(idx, obj, object_array_length, reference_label(1, 1));
		call_soft(soft_badarrayindex);
		start_sub_block();
		set_label(1, 1);
	}
#endif
}

void
check_array_store(SlotInfo* array, SlotInfo* obj)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref(array, 0);
	pusharg_ref(obj, 1);
#else
	pusharg_ref(obj, 1);
	pusharg_ref(array, 0);
#endif
	call_soft(soft_checkarraystore);
	popargs();
	end_func_sync();
}

void
explicit_check_null(int x, SlotInfo* obj, int y)
{
#if 0
#if defined(HAVE_fakecall) || defined(HAVE_fakecall_constpool)
	if (!canCatch(ANY)) {
		cbranch_ref_const(obj, 0, newFakeCall(soft_nullpointer, pc), eq | blink);
	}
	else
#endif
#endif
	{
		end_sub_block();
		cbranch_ref_const_ne(obj, NULL, reference_label(x, y));
		softcall_nullpointer();
		start_sub_block();
		set_label(x, y);
	}
}

#if defined(CREATE_NULLPOINTER_CHECKS)
void
check_null(int x, SlotInfo* obj, int y)
{
	explicit_check_null(x, obj, y);
}
#else
void
check_null(int x UNUSED, SlotInfo* obj UNUSED, int y UNUSED)
{
	if (canCatch(ANY)) {
		begin_func_sync();
		end_func_sync();
	}
}
#endif

#if defined(CREATE_DIVZERO_CHECKS)
void
check_div(int x, SlotInfo* obj, int y)
{
#if defined(HAVE_fakecall) || defined(HAVE_fakecall_constpool)
	if (!canCatch(ANY)) {
		cbranch_int_const(obj, 0, newFakeCall(soft_divzero, pc), beq | blink);
	}
	else
#endif
	{
		end_sub_block();
		cbranch_int_const_ne(obj, 0, reference_label(x, y));
		softcall_divzero();
		start_sub_block();
		set_label(x, y);
	}
}
#else
void
check_div(int x UNUSED, SlotInfo* obj UNUSED, int y UNUSED)
{
	if (canCatch(ANY)) {
		begin_func_sync();
		end_func_sync();
	}
}
#endif

#if defined(CREATE_DIVZERO_CHECKS)
void
check_div_long(int x, SlotInfo* obj, int y)
{
#if 0
	THE CODE BELOW DOES NOT WORK - !!! FIX ME !!!
#if defined(HAVE_fakecall) || defined(HAVE_fakecall_constpool)
	if (!canCatch(ANY)) {
		cbranch_int_const(LSLOT(obj), 0, newFakeCall(soft_divzero, pc), eq | blink);
		cbranch_int_const(HSLOT(obj), 0, newFakeCall(soft_divzero, pc), eq | blink);
	}
	else
#endif
#endif
	{
		end_sub_block();
		cbranch_int_const_ne(LSLOT(obj), 0, reference_label(x, y+0));
		cbranch_int_const_ne(HSLOT(obj), 0, reference_label(x, y+1));
		softcall_divzero();
		start_sub_block();
		set_label(x, y+1);
		set_label(x, y+0);
	}
}
#else
void
check_div_long(int x UNUSED, SlotInfo* obj UNUSED, int y UNUSED)
{
	if (canCatch(ANY)) {
		begin_sync();
		end_sync();
	}
}
#endif

#if defined(HAVE_ccall)
void
ccall_ugt(SlotInfo* dst)
{
	slot_slot_const(0, dst, bugt, HAVE_ccall, Tnull);
}

void
ccall_soft_ugt(void* routine)
{
	label* l;
	constpool* c;
	SlotInfo* tmp;

	l = KaffeJIT3_newLabel();
	c = KaffeJIT3_newConstant(CPref, routine);
	l->type = Lconstant;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

	slot_alloctmp(tmp);
#if defined(HAVE_load_constpool_ref)
	slot_slot_const(tmp, 0, (jword)l, HAVE_load_constpool_ref, Tnull);
#else
	move_label_const(tmp, l);
	load_ref(tmp, tmp);
#endif
	ccall(tmp);
	slot_freetmp(tmp);
}

void
ccall_soft_int_ugt(SlotInfo* s1, SlotInfo* s2, void* func)
{
	cmp_int(0, s1, s2);
	ccall_soft_ugt(func);
}

void
ccall_int_const_ugt(SlotInfo* s1, jint s2, void* func)
{
	cmp_int_const(0, s1, s2);
	ccall_ugt(func);
}
#endif

void
softcall_nullpointer(void)
{
	if (canCatch(ANY)) {
		begin_func_sync();
		end_func_sync();
	}
	call_soft(soft_nullpointer);
}

void
softcall_divzero(void)
{
	if (canCatch(ANY)) {
		begin_func_sync();
		end_func_sync();
	}
	call_soft(soft_divzero);
}

void
softcall_fakecall(label* from, label* to, void* func)
{
#if defined(HAVE_fakecall_constpool)
	label* l;
	constpool* c;

	/* Set the label for getting here */
	slot_slot_const(NULL, NULL, (jword)to, HAVE_set_label, Tnull);

	/* We must put our 'from' in a constant.  We use the 'from' as
	 * the constant value so it won't be aliases with others.
	 */
	c = KaffeJIT3_newConstant(CPlabel, from);
	from->type |= Lconstantpool|Labsolute|Llong;
	from->at = (uintp)c;

	from = KaffeJIT3_newLabel();
	from->type = Lconstant;
	from->at = 0;
	from->to = (uintp)c;
	from->from = 0;

	/* Build a label to hold the fake call */
	l = KaffeJIT3_newLabel();
	c = KaffeJIT3_newConstant(CPref, func);
	l->type = Lconstant;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

	/* Make the fake call - passing the call label and the label we
	 * got here from.
	 */
	slot_const_const(NULL, (jword)from, (jword)l, HAVE_fakecall_constpool, Tnull);
#elif defined(HAVE_fakecall)
	label* l;

	/* Set the label for getting here */
	slot_slot_const(NULL, NULL, (jword)to, HAVE_set_label, Tnull);

	/* Build a label to hold the fake call */
	l = KaffeJIT3_newLabel();
	l->type = Lexternal;
	l->at = 0;
	l->to = (uintp)func;
	l->from = 0;

	/* Make the fake call - passing the call label and the label we
	 * got here from.
	 */
	slot_const_const(NULL, (jword)from, (jword)l, HAVE_fakecall, Tnull);
#endif
}

void
softcall_nosuchclass(Utf8Const* name)
{
	begin_func_sync();
	pusharg_utf8_const(name, 0);
	call_soft(soft_nosuchclass);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_nosuchmethod(struct Hjava_lang_Class* cls, Utf8Const* name, Utf8Const* sig)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_class_const(cls, 0);
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(sig, 2);
#else
	pusharg_utf8_const(sig, 2);
	pusharg_utf8_const(name, 1);
	pusharg_class_const(cls, 0);
#endif
	call_soft(soft_nosuchmethod);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_nosuchfield(Utf8Const* cls, Utf8Const* name)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_utf8_const(cls, 0);
	pusharg_utf8_const(name, 1);
#else
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
#endif
	call_soft(soft_nosuchfield);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_linkage(Utf8Const* cls, Utf8Const* name)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_utf8_const(cls, 0);
	pusharg_utf8_const(name, 1);
#else
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
#endif
	call_soft(soft_linkage);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_illegalaccess(Utf8Const* cls, Utf8Const* name)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_utf8_const(cls, 0);
	pusharg_utf8_const(name, 1);
#else
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
#endif
	call_soft(soft_illegalaccess);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_incompatibleclasschange(Utf8Const* cls, Utf8Const* name)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_utf8_const(cls, 0);
	pusharg_utf8_const(name, 1);
#else
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
#endif
	call_soft(soft_incompatibleclasschange);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_abstractmethod(Utf8Const* cls, Utf8Const* name)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_utf8_const(cls, 0);
	pusharg_utf8_const(name, 1);
#else
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
#endif
	call_soft(soft_abstractmethod);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_new(SlotInfo* dst, Hjava_lang_Class* classobj)
{
	begin_func_sync();
	pusharg_class_const(classobj, 0);
	call_soft(soft_new);
	popargs();
	end_func_sync();
	return_ref(dst);
}

void
softcall_newarray(SlotInfo* dst, SlotInfo* size, int type)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_int_const(type, 0);
	pusharg_int(size, 1);
#else
	pusharg_int(size, 1);
	pusharg_int_const(type, 0);
#endif
	call_soft(soft_newarray);
	popargs();
	pop(1);
	end_func_sync();
	push(1);
	return_ref(dst);
}

void
softcall_anewarray(SlotInfo* dst, SlotInfo* size, Hjava_lang_Class* type)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_class_const(type, 0);
	pusharg_int(size, 1);
#else
	pusharg_int(size, 1);
	pusharg_class_const(type, 0);
#endif
	call_soft(soft_anewarray);
	popargs();
	pop(1);
	end_func_sync();
	push(1);
	return_ref(dst);
}

void
softcall_multianewarray(SlotInfo* dst, int size, SlotInfo* stktop, Hjava_lang_Class* classobj)
{
	int i;

	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_class_const(classobj, 0);
	pusharg_int_const(size, 1);
	for (i = size - 1; i >= 0; i--) {
		pusharg_int(&stktop[i], 1+size-i);
	}
#else
	for (i = 0; i < size; i++) {
		pusharg_int(&stktop[i], 1+size-i);
	}
	pusharg_int_const(size, 1);
	pusharg_class_const(classobj, 0);
#endif
	call_soft(jit_soft_multianewarray);
	popargs();
	pop(size);
	end_func_sync();
	push(1);
	return_ref(dst);
}

void
softcall_athrow(SlotInfo* obj)
{
	begin_func_sync();
	pusharg_ref(obj, 0);
	call_soft(soft_athrow);
	popargs_noreturn();
	end_func_sync();
}

void
softcall_checkcast(SlotInfo* objw, SlotInfo* objr, Hjava_lang_Class* class)
{
	/* Must keep 'obj' */
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_class_const(class, 0);
	pusharg_ref(objr, 1);
#else
	pusharg_ref(objr, 1);
	pusharg_class_const(class, 0);
#endif
	call_soft(soft_checkcast);
	popargs();
	end_func_sync();
	return_ref(objw);
}

void
softcall_instanceof(SlotInfo* dst, SlotInfo* obj, Hjava_lang_Class* class)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_class_const(class, 0);
	pusharg_ref(obj, 1);
#else
	pusharg_ref(obj, 1);
	pusharg_class_const(class, 0);
#endif
	call_soft(soft_instanceof);
	popargs();
	pop(1);
	end_func_sync();
	push(1);
	return_int(dst);
}

void
softcall_initialise_class(Hjava_lang_Class* c)
{
	if (c != 0 && c->state != CSTATE_COMPLETE) {
		begin_func_sync();
		pusharg_class_const(c, 0);
		call_soft(soft_initialise_class);
		popargs();
		end_func_sync();
	}
}

void
softcall_debug1(void* a0, void* a1, void* a2)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref_const(a0, 0);
	pusharg_ref_const(a1, 1);
	pusharg_ref_const(a2, 2);
#else
	pusharg_ref_const(a2, 2);
	pusharg_ref_const(a1, 1);
	pusharg_ref_const(a0, 0);
#endif
	call_soft(soft_debug1);
	popargs();
	end_func_sync();
}

void
softcall_debug2(void* a0, void* a1, void* a2)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref_const(a0, 0);
	pusharg_ref_const(a1, 1);
	pusharg_ref_const(a2, 2);
#else
	pusharg_ref_const(a2, 2);
	pusharg_ref_const(a1, 1);
	pusharg_ref_const(a0, 0);
#endif
	call_soft(soft_debug2);
	popargs();
	end_func_sync();
}

void
softcall_trace(Method* meth)
{
#if defined(HAVE_get_arg_ptr)
        SlotInfo* tmp;

        slot_alloctmp(tmp);
        get_arg_ptr(tmp);
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
        pusharg_ref_const(meth, 0);
        pusharg_ref(tmp, 1);
#else
        pusharg_ref(tmp, 1);
        pusharg_ref_const(meth, 0);
#endif
        call_soft(soft_trace);
        popargs();
	end_func_sync();
        slot_freetmp(tmp);
#else
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref_const(meth, 0);
        pusharg_ref_const(0, 1);
#else
        pusharg_ref_const(0, 1);
	pusharg_ref_const(meth, 0);
#endif
	call_soft(soft_trace);
	popargs();
	end_func_sync();
#endif
}

void
softcall_enter_method(SlotInfo *obj, Method* meth)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref(obj, 0);
        pusharg_ref_const(meth, 1);
#else
        pusharg_ref_const(meth, 1);
	pusharg_ref(obj, 0);
#endif
	call_soft(soft_enter_method);
	popargs();
	end_func_sync();
}

void
softcall_exit_method(Method* meth)
{
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref_const(meth, 0);
#else
	pusharg_ref_const(meth, 0);
#endif
	call_soft(soft_exit_method);
	popargs();
	end_func_sync();
}

#if defined(KGC_INCREMENTAL)
#if defined(KGC_INCREMENTAL)
void
softcall_writeref(SlotInfo* from, SlotInfo* to)
{
	end_sub_block();

	cbranch_int_const_eq(to, 0, reference_label(1, 1));
	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref(from, 0);
	pusharg_ref(to, 1);
#else
	pusharg_ref(to, 1);
	pusharg_ref(from, 0);
#endif
	call_soft(gcWriteRef);
	popargs();
	end_func_sync();

	start_sub_block();
        set_label(1, 1);
}

void
softcall_writeref_static(void* from, SlotInfo* to)
{
	end_sub_block();
	cbranch_int_const_eq(to, 0, reference_label(1, 1));

	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref_const(from, 0);
	pusharg_ref(to, 1);
#else
	pusharg_ref(to, 1);
	pusharg_ref_const(from, 0);
#endif
	call_soft(gcWriteRef);
	popargs();
	end_func_sync();

	start_sub_block();
        set_label(1, 1);
}

#else
/* Smarter write barriers to save calls.
 * Benchmarking suggests this may not be that useful.
 */
void
softcall_writeref(SlotInfo* from, SlotInfo* to)
{
	SlotInfo* tmp1;

	end_sub_block();
	cbranch_int_const_eq(to, 0, reference_label(1, 2));
	slot_alloctmp(tmp1);
	load_offset_int(tmp1, from, object_gc_color);
	cbranch_int_const_ne(tmp1, COLOR_BLACK, reference_label(1, 1));
        slot_freetmp(tmp1);

	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref(from, 0);
	pusharg_ref(to, 1);
#else
	pusharg_ref(to, 1);
	pusharg_ref(from, 0);
#endif
	call_soft(gcWriteRef);
	popargs();
	end_func_sync();

	start_sub_block();
        set_label(1, 1);
	set_label(1, 2);
}

void
softcall_writeref_static(void* from, SlotInfo* to)
{
	SlotInfo* tmp1,*tmp2;

	slot_alloctmp(tmp1);

	end_sub_block();
	cbranch_int_const_eq(to, 0, reference_label(1, 2));
	load_addr_int(tmp1, (void*)((uintp)from + object_gc_color));
	cbranch_int_const_ne(tmp1, COLOR_BLACK, reference_label(1, 1));
        slot_freetmp(tmp1);

	begin_func_sync();
#if defined(PUSHARG_FORWARDS)
	pusharg_ref_const(from, 0);
	pusharg_ref(to, 1);
#else
	pusharg_ref(to, 1);
	pusharg_ref_const(from, 0);
#endif
	call_soft(gcWriteRef);
	popargs();
	end_func_sync();

	start_sub_block();
        set_label(1, 1);
	set_label(1, 2);
}
#endif
#endif

#if defined(HAVE_get_arg_ptr)
void
get_arg_ptr(SlotInfo* dst)
{
        slot_slot_slot(dst, NULL, NULL, HAVE_get_arg_ptr, Tcomplex);
}
#endif
