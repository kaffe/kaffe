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
#include "kaffe/jmalloc.h"
#include "slots.h"
#include "seq.h"
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
#include "support.h"
#include "stats.h"
#include "itypes.h"
#include "md.h"
#include "locks.h"
#include "machine.h"
#include "codeproto.h"
#include "fp.h"

#if defined(WORDS_BIGENDIAN)
#define	LSLOT(_s)	((_s)+1)
#define	HSLOT(_s)	(_s)
#else
#define	LSLOT(_s)	(_s)
#define	HSLOT(_s)	((_s)+1)
#endif

void startBlock(sequence*);
void endBlock(sequence*);
void startSubBlock(sequence*);
void endSubBlock(sequence*);
void prepareFunctionCall(sequence*);
void fixupFunctionCall(sequence*);
void syncRegisters(sequence*);
void nowritebackSlot(sequence*);

bool used_ieee_rounding;
bool used_ieee_division;

#define	MAXLABTAB	64
label* labtab[MAXLABTAB];

/* ----------------------------------------------------------------------- */
/* Register loads and spills.						   */
/*									   */

#if defined(HAVE_spill_int)
void
spill_int(SlotInfo* src)
{
	void HAVE_spill_int(sequence*);
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(src);
	HAVE_spill_int(&s);
}
#endif

#if defined(HAVE_reload_int)
void
reload_int(SlotInfo* dst)
{
	void HAVE_reload_int(sequence*);
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(dst);
	HAVE_reload_int(&s);
}
#endif

#if defined(HAVE_spill_ref)
void
spill_ref(SlotInfo* src)
{
	void HAVE_spill_ref(sequence*);
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(src);
	HAVE_spill_ref(&s);
}
#endif

#if defined(HAVE_reload_ref)
void
reload_ref(SlotInfo* dst)
{
	void HAVE_reload_ref(sequence*);
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(dst);
	HAVE_reload_ref(&s);
}
#endif

#if defined(HAVE_spill_long)
void
spill_long(SlotInfo* src)
{
	void HAVE_spill_long(sequence*);
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(src);
	HAVE_spill_long(&s);
}
#endif

#if defined(HAVE_reload_long)
void
reload_long(SlotInfo* dst)
{
	void HAVE_reload_long(sequence*);
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(dst);
	HAVE_reload_long(&s);
}
#endif

#if defined(HAVE_spill_float)
void
spill_float(SlotInfo* src)
{
	void HAVE_spill_float(sequence*);
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(src);
	HAVE_spill_float(&s);
}
#endif

#if defined(HAVE_reload_float)
void
reload_float(SlotInfo* dst)
{
	void HAVE_reload_float(sequence*);
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(dst);
	HAVE_reload_float(&s);
}
#endif

#if defined(HAVE_spill_double)
void
spill_double(SlotInfo* src)
{
	void HAVE_spill_double(sequence*);
	sequence s;
	seq_dst(&s) = src;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(src);
	HAVE_spill_double(&s);
}
#endif

#if defined(HAVE_reload_double)
void
reload_double(SlotInfo* dst)
{
	void HAVE_reload_double(sequence*);
	sequence s;
	seq_dst(&s) = dst;
	seq_value(&s, 1) = SLOT2FRAMEOFFSET(dst);
	HAVE_reload_double(&s);
}
#endif

/* ----------------------------------------------------------------------- */
/* Prologues and epilogues.						   */
/*									   */

void
prologue(Method* meth)
{
	label* l;

	used_ieee_rounding = false;
	used_ieee_division = false;

	l = KaffeJIT_newLabel();
	l->type = Lnull;
	l->at = 0;
	l->to = 0;
	l->from = 0;

	/* Emit prologue code */
	slot_const_const(NULL, (jword)l, (jword)meth, HAVE_prologue, Tnull);

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
exception_prologue(void)
{
	label* l;

	l = KaffeJIT_newLabel();
	l->type = Lnull;
	l->at = 0;
	l->to = 0;
	l->from = 0;

	/* Emit exception prologue code */
	slot_const_const(NULL, (jword)l, 0, HAVE_exception_prologue, Tnull);
}

void
epilogue(void)
{
	slot_slot_slot(NULL, NULL, NULL, HAVE_epilogue, Tnull);
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
	
	l = KaffeJIT_newLabel();
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
	end_sub_block();
	if ((meth->accflags & ACC_STATIC) != 0) {
		pusharg_class_const(meth->class, 0);
	}
	else {
		pusharg_ref(obj, 0);
	}
	call_soft(lockObject);
	popargs();
	start_sub_block();
}

void
mon_exit(methods* meth, SlotInfo* obj)
{
	end_sub_block();
	if ((meth->accflags & ACC_STATIC) != 0) {
		pusharg_class_const(meth->class, 0);
	}
	else {
		pusharg_ref(obj, 0);
	}
	call_soft(unlockObject);
	popargs();
	start_sub_block();
}

/* ----------------------------------------------------------------------- */
/* Basic block and instruction management.				   */
/*									   */

void
_start_basic_block(void)
{
	int i;

	_slot_const_const(NULL, 0, 0, startBlock, Tnull);

	for (i = maxslot - 1; i >= 0; i--) {
		slotinfo[i].info = SI_SLOT;
	}
}

void
_end_basic_block(uintp stk, uintp temp)
{
	_slot_const_const(NULL, stk, temp, endBlock, Tnull);
}

void
_start_sub_block(void)
{
	_slot_const_const(NULL, 0, 0, startSubBlock, Tnull);
}

void
_end_sub_block(uintp stk, uintp temp)
{
	_slot_const_const(NULL, stk, temp, endSubBlock, Tnull);
}

void
_start_instruction(uintp pc)
{
	void startInsn(sequence*);

	_slot_const_const(NULL, 0, pc, startInsn, Tnull);
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

void
_fixup_function_call(void)
{
	_slot_const_const(NULL, 0, 0, fixupFunctionCall, Tnull);
}

void
_prepare_function_call(uintp stk, uintp temp)
{
	_slot_const_const(NULL, stk, temp, prepareFunctionCall, Tnull);
}

void
_slot_nowriteback(SlotInfo* slt)
{
	_slot_const_const(slt, 0, 0, nowritebackSlot, Tnull);
}

void 
_syncRegisters(uintp stk, uintp temp)
{
	_slot_const_const(NULL, stk, temp, syncRegisters, Tnull);
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
		SlotInfo* tmp;

		c = KaffeJIT_newConstant(CPint, val);
		l = KaffeJIT_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

		slot_alloctmp(tmp);
		move_label_const(tmp, l);
		load_int(dst, tmp);
		slot_freetmp(tmp);
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
		SlotInfo* tmp;

		c = KaffeJIT_newConstant(CPref, val);
		l = KaffeJIT_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

		slot_alloctmp(tmp);
		move_label_const(tmp, l);
		load_ref(dst, tmp);
		slot_freetmp(tmp);
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

	c = KaffeJIT_newConstant(CPstring, val);
	l = KaffeJIT_newLabel();
	l->type = Lconstant;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

	slot_alloctmp(tmp);
	move_label_const(tmp, l);
	load_ref(dst, tmp);
	slot_freetmp(tmp);
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

		c = KaffeJIT_newConstant(CPlong, val);
		l = KaffeJIT_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

		slot_alloctmp(tmp);
		move_label_const(tmp, l);
		load_long(dst, tmp);
		slot_freetmp(tmp);
	}
#else
	move_int_const(LSLOT(dst), (jint)(val & 0xFFFFFFFF));
	move_int_const(HSLOT(dst), (jint)((val >> 32) & 0xFFFFFFFF));
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
		SlotInfo* tmp;

		c = KaffeJIT_newConstant(CPfloat, val);
		l = KaffeJIT_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

		slot_alloctmp(tmp);
		move_label_const(tmp, l);
		load_float(dst, tmp);
		slot_freetmp(tmp);
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
		SlotInfo* tmp;

		c = KaffeJIT_newConstant(CPdouble, val);
		l = KaffeJIT_newLabel();
		l->type = Lconstant;
		l->at = 0;
		l->to = (uintp)c;
		l->from = 0;

		slot_alloctmp(tmp);
		move_label_const(tmp, l);
		load_double(dst, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_move_any)
void
move_any(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_move_any, Tcopy);
}
#endif

#if defined(HAVE_move_int)
void
move_int(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_move_int, Tcopy);
}
#endif

void
move_ref(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_move_ref, Tcopy);
}

void
move_anylong(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_move_anylong)
	lslot_lslot_lslot(dst, NULL, src, HAVE_move_anylong, Tcopy);
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
	lslot_lslot_lslot(dst, NULL, src, HAVE_move_long, Tcopy);
#else
	assert(LSLOT(dst) != HSLOT(src));
	move_int(LSLOT(dst), LSLOT(src));
	move_int(HSLOT(dst), HSLOT(src));
#endif
}

#if defined(HAVE_move_float)
void
move_float(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_move_float, Tcopy);
}
#endif

#if defined(HAVE_move_double)
void
move_double(SlotInfo* dst, SlotInfo* src)
{
	lslot_lslot_lslot(dst, NULL, src, HAVE_move_double, Tcopy);
}
#endif

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
		add_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_add_int)
void
add_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_add_int, Tcomm);
}
#endif

#if defined(HAVE_add_ref)
void
add_ref(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_add_ref, Tcomm);
}
#endif

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
		add_ref(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
add_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_add_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_add_long, Tcomplex);
#else
	add_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	adc_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
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

#if defined(HAVE_add_float)
void
add_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_add_float, Tcomplex);
}
#endif

#if defined(HAVE_add_double)
void
add_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	lslot_lslot_lslot(dst, src, src2, HAVE_add_double, Tcomplex);
}
#endif

#if defined(HAVE_sbc_int)
void
sbc_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_sbc_int, Tcomplex);
}
#endif

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
		sub_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_sub_int)
void
sub_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_sub_int, Tcomplex);
}
#endif

void
sub_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_sub_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_sub_long, Tcomplex);
#else
	sub_int(LSLOT(dst), LSLOT(src), LSLOT(src2));
	sbc_int(HSLOT(dst), HSLOT(src), HSLOT(src2));
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

#if defined(HAVE_sub_float)
void
sub_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_sub_float, Tcomplex);
}
#endif

#if defined(HAVE_sub_double)
void
sub_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	lslot_lslot_lslot(dst, src, src2, HAVE_sub_double, Tcomplex);
}
#endif

void
mul_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
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
		mul_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
mul_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_mul_int)
	slot_slot_slot(dst, src, src2, HAVE_mul_int, Tcomplex);
#else
	end_sub_block();
	pusharg_int(src2, 1);
	pusharg_int(src, 0);
	call_soft(soft_mul);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
mul_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_mul_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_mul_long, Tcomplex);
#else
	end_sub_block();
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_lmul);
	popargs();
	start_sub_block();
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

#if defined(HAVE_mul_float)
void
mul_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_mul_float, Tcomplex);
}
#endif

#if defined(HAVE_mul_double)
void
mul_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	lslot_lslot_lslot(dst, src, src2, HAVE_mul_double, Tcomplex);
}
#endif

void
div_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
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
		div_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
div_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_div_int)
	slot_slot_slot(dst, src, src2, HAVE_div_int, Tcomplex);
#else
	end_sub_block();
	pusharg_int(src2, 1);
	pusharg_int(src, 0);
	call_soft(soft_div);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
div_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_div_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_div_long, Tcomplex);
#else
	end_sub_block();
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_ldiv);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
	call_soft(soft_fdiv);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
	call_soft(soft_fdivl);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_int(src2, 1);
	pusharg_int(src, 0);
	call_soft(soft_rem);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
rem_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_rem_long)
	lslot_lslot_lslot(dst, src, src2, HAVE_rem_long, Tcomplex);
#else
	end_sub_block();
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_lrem);
	popargs();
	start_sub_block();
	return_long(dst);
#endif
}

void
rem_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	used_ieee_division = true;
	end_sub_block();
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
	call_soft(soft_frem);
	popargs();
	start_sub_block();
	return_float(dst);
}

void
rem_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	used_ieee_division = true;
	end_sub_block();
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
	call_soft(soft_freml);
	popargs();
	start_sub_block();
	return_double(dst);
}

void
neg_int(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_neg_int)
	slot_slot_slot(dst, 0, src, HAVE_neg_int, Tcomplex);
#else
	SlotInfo* zero;
	slot_alloctmp(zero);
	move_int_const(zero, 0);
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

void
and_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
#if defined(HAVE_and_int_const)
	if (HAVE_and_int_const_rangecheck(val)) {
		slot_slot_const(dst, src, val, HAVE_and_int_const, Tcomplex);
	}
	else
#endif
	{
		SlotInfo* tmp;
		slot_alloctmp(tmp);
		move_int_const(tmp, val);
		and_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_and_int)
void
and_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_and_int, Tcomplex);
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

void
or_int_const(SlotInfo* dst, SlotInfo* src, jint val)
{
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
		or_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_or_int)
void
or_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_or_int, Tcomplex);
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
		xor_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_xor_int)
void
xor_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_xor_int, Tcomplex);
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
		lshl_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_lshl_int)
void
lshl_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_lshl_int, Tcomplex);
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
	end_sub_block();
	pusharg_int(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_lshll);
	popargs();
	start_sub_block();
	return_long(dst);
#endif
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
		ashr_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_ashr_int)
void
ashr_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_ashr_int, Tcomplex);
}
#endif

void
ashr_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_ashr_long)
	lslot_lslot_slot(dst, src, src2, HAVE_ashr_long, Tcomplex);
#else
	end_sub_block();
	pusharg_int(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_ashrl);
	popargs();
	start_sub_block();
	return_long(dst);
#endif
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
		lshr_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

#if defined(HAVE_lshr_int)
void
lshr_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_lshr_int, Tcomplex);
}
#endif

void
lshr_long(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lshr_long)
	lslot_lslot_slot(dst, src, src2, HAVE_lshr_long, Tcomplex);
#else
	end_sub_block();
	pusharg_int(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_lshrl);
	popargs();
	start_sub_block();
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_int(dst, tmp);
	slot_freetmp(tmp);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_ref(dst, tmp);
	slot_freetmp(tmp);
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
	load_addr_int(dst, addr);
	load_addr_int(dst+1, (char*)addr+4);
}

#if defined(HAVE_load_float)
void
load_float(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(dst, NULL, src, HAVE_load_float, Tload);
}
#endif

void
load_addr_float(SlotInfo* dst, void* addr)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_float(dst, tmp);
	slot_freetmp(tmp);
}

#if defined(HAVE_load_double)
void
load_double(SlotInfo* dst, SlotInfo* src)
{
	lslot_lslot_slot(dst, NULL, src, HAVE_load_double, Tload);
}
#endif

void
load_addr_double(SlotInfo* dst, void* addr)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_double(dst, tmp);
	slot_freetmp(tmp);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_char(dst, tmp);
	slot_freetmp(tmp);
}

void
load_addr_byte(SlotInfo* dst, void* addr)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_byte(dst, tmp);
	slot_freetmp(tmp);
}

void
load_addr_short(SlotInfo* dst, void* addr)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	load_short(dst, tmp);
	slot_freetmp(tmp);
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
		lslot_lslot_const(dst, src, offset, HAVE_load_offset_long, Tload);
	}
	else
#endif
	{
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
		lslot_lslot_const(dst, src, offset, HAVE_load_offset_byte, Tload);
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
		lslot_lslot_const(dst, src, offset, HAVE_load_offset_char, Tload);
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
		lslot_lslot_const(dst, src, offset, HAVE_load_offset_short, Tload);
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
		lslot_lslot_const(dst, src, offset, HAVE_load_offset_float, Tload);
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
		lslot_lslot_const(dst, src, offset, HAVE_load_offset_double, Tload);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jint);
	add_ref(tmp, src, tmp);
	load_offset_int(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_ref(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jref);
	add_ref(tmp, src, tmp);
	load_offset_ref(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_long(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jlong);
	add_ref(tmp, src, tmp);
	load_offset_long(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_float(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jfloat);
	add_ref(tmp, src, tmp);
	load_offset_float(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_double(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jdouble);
	add_ref(tmp, src, tmp);
	load_offset_double(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_byte(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	add_ref(tmp, src, idx);
	load_offset_byte(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_char(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jchar);
	add_ref(tmp, src, tmp);
	load_offset_char(dst, tmp, offset);
	slot_freetmp(tmp);
}

void
load_offset_scaled_short(SlotInfo* dst, SlotInfo* src, SlotInfo* idx, int offset)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jshort);
	add_ref(tmp, src, tmp);
	load_offset_short(dst, tmp, offset);
	slot_freetmp(tmp);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_int(tmp, src);
	slot_freetmp(tmp);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_ref(tmp, src);
	slot_freetmp(tmp);
}

void
store_long(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_store_long)
	lslot_lslot_lslot(NULL, dst, src, HAVE_store_long, Tstore);
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
		lslot_lslot_const(src, dst, offset, HAVE_store_offset_long, Tstore);
	}
	else
#endif
	{
		store_offset_int(dst, offset, src);
		store_offset_int(dst, offset+4, src+1);
	}
}

void
store_addr_long(void* addr, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_long(tmp, src);
	slot_freetmp(tmp);
}

#if defined(HAVE_store_float)
void
store_float(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_slot(NULL, dst, src, HAVE_store_float, Tstore);
}
#endif

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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_float(tmp, src);
	slot_freetmp(tmp);
}

#if defined(HAVE_store_double)
void
store_double(SlotInfo* dst, SlotInfo* src)
{
	slot_slot_lslot(NULL, dst, src, HAVE_store_double, Tstore);
}
#endif

void
store_offset_double(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_double(dst, src);
	}
	else
#if defined(HAVE_store_offset_double)
	if (HAVE_store_offset_double_rangecheck(offset)) {
		slot_slot_const(src, dst, offset, HAVE_store_offset_double, Tstore);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_double(tmp, src);
	slot_freetmp(tmp);
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
	slot_freetmp(tmp2);
	slot_freetmp(tmp);
#endif
}

void
store_offset_byte(SlotInfo* dst, jint offset, SlotInfo* src)
{
	if (offset == 0) {
		store_byte(dst, src);
	}
	else
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_byte(tmp, src);
	slot_freetmp(tmp);
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
	slot_freetmp(tmp2);
	slot_freetmp(tmp);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_char(tmp, src);
	slot_freetmp(tmp);
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
	slot_freetmp(tmp2);
	slot_freetmp(tmp);
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
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	move_ref_const(tmp, addr);
	store_short(tmp, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_int(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jint);
	add_ref(tmp, dst, tmp);
	store_offset_int(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_ref(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jref);
	add_ref(tmp, dst, tmp);
	store_offset_ref(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_long(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jlong);
	add_ref(tmp, dst, tmp);
	store_offset_long(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_float(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jfloat);
	add_ref(tmp, dst, tmp);
	store_offset_float(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_double(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jdouble);
	add_ref(tmp, dst, tmp);
	store_offset_double(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_byte(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	add_ref(tmp, dst, idx);
	store_offset_byte(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_char(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jchar);
	add_ref(tmp, dst, tmp);
	store_offset_char(tmp, offset, src);
	slot_freetmp(tmp);
}

void
store_offset_scaled_short(SlotInfo* dst, SlotInfo* idx, int offset, SlotInfo* src)
{
	SlotInfo* tmp;
	slot_alloctmp(tmp);
	lshl_int_const(tmp, idx, SHIFT_jshort);
	add_ref(tmp, dst, tmp);
	store_offset_short(tmp, offset, src);
	slot_freetmp(tmp);
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
		move_int_cosnt(tmp2, val);
		store_int(tmp, tmp2);
		slot_freetmp(tmp2);
		slot_freetmp(tmp);
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
		move_int_cosnt(tmp2, val);
		store_byte(tmp, tmp2);
		slot_freetmp(tmp2);
		slot_freetmp(tmp);
	}
}
#endif


/* ----------------------------------------------------------------------- */
/* Function argument management.					   */
/*									   */

void
pusharg_int_const(jint val, int idx)
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
	slot_slot_const(NULL, src, idx, HAVE_pusharg_int, Tnull);
	argcount += 1;
}
#endif

#if defined(HAVE_pusharg_ref)
void
pusharg_ref(SlotInfo* src, int idx)
{
	slot_slot_const(NULL, src, idx, HAVE_pusharg_ref, Tnull);
	argcount += 1;
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
pusharg_class_const(void* cls, int idx)
{
#if 1
	pusharg_ref_const(cls, idx);
#else
	???
#endif
}

void
pusharg_utf8_const(void* val, int idx)
{
#if 1
	pusharg_ref_const(val, idx);
#else
	???
#endif
}

#if defined(HAVE_pusharg_float)
void
pusharg_float(SlotInfo* src, int idx)
{
	slot_slot_const(NULL, src, idx, HAVE_pusharg_float, Tnull);
	argcount += 1;
}
#endif

#if defined(HAVE_pusharg_double)
void
pusharg_double(SlotInfo* src, int idx)
{
	lslot_lslot_const(NULL, src, idx, HAVE_pusharg_double, Tnull);
	argcount += pusharg_long_idx_inc;
}
#endif

void
pusharg_long(SlotInfo* src, int idx)
{
#if defined(HAVE_pusharg_long)
	lslot_lslot_const(NULL, src, idx, HAVE_pusharg_long, Tnull);
	argcount += pusharg_long_idx_inc;
#else
	/* Don't use LSLOT & HSLOT here */
	pusharg_int(src+1, idx+1);
	pusharg_int(src, idx);
#endif
}

void
popargs(void)
{
 	if (argcount != 0) {
#if defined(HAVE_popargs)
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

#if defined(HAVE_branch)
void
branch(label* dst, int type)
{
	slot_const_const(NULL, (jword)dst, type, HAVE_branch, Tnull);
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
cbranch_int_const(SlotInfo* s1, jint val, label* dst, int type)
{
#if defined(HAVE_cbranch_int_const)
	if (HAVE_cbranch_int_const_rangecheck(val)) {
		slot_slot_const_const_const(NULL, s1, val, (jword)dst, type,
					    HAVE_cbranch_int_const, Tcomplex);
	}
	else
	{
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
	slot_slot_slot_const_const(NULL, s1, s2, (jword)dst, type,
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

	ptr = PMETHOD_NATIVECODE(meth);

#if defined(HAVE_call_indirect_const)
	slot_const_const(NULL, (jword)ptr, ba, HAVE_call_indirect_const, Tnull);
#else
	{
		SlotInfo *tmp;

		slot_alloctmp(tmp);
		move_ref_const(tmp, ptr);
		load_ref(tmp, tmp);
		call(tmp);
		slot_freetmp(tmp);
	}
#endif
}

void
call_soft(void *routine)
{
#if defined(HAVE_call_soft)
	label* l = KaffeJIT_newLabel();
	l->type = Labsolute|Lexternal|Lnoprofile;
	l->at = 0;
	l->to = (uintp)routine;	/* What place does it goto */
	l->from = 0;

	slot_const_const(0, (jword)l, ba, HAVE_call_soft, Tnull);
#elif defined(HAVE_call_ref)
	label* l;
	l = KaffeJIT_newLabel();
	l->type = Lexternal|Lnoprofile;
	l->at = 0;
	l->to = (uintp)routine;	/* What place does it goto */
	l->from = 0;

	slot_const_const(NULL, (jword)l, ba, HAVE_call_ref, Tnull);
#else
	label* l;
	constpool* c;
	SlotInfo* tmp;

	l = KaffeJIT_newLabel();
	c = KaffeJIT_newConstant(CPref, routine);
	l->type = Lconstant;
	l->at = 0;
	l->to = (uintp)c;
	l->from = 0;

	slot_alloctmp(tmp);
	move_label_const(tmp, l);
	load_ref(tmp, tmp);
	call(tmp);
	slot_freetmp(tmp);
#endif
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

#if defined(HAVE_return_float)
void
return_float(SlotInfo* dst)
{
	slot_slot_slot(dst, NULL, NULL, HAVE_return_float, Tnull);
}
#endif

#if defined(HAVE_return_double)
void
return_double(SlotInfo* dst)
{
	lslot_lslot_lslot(dst, NULL, NULL, HAVE_return_double, Tnull);
}
#endif

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

#if defined(HAVE_returnarg_float)
void
returnarg_float(SlotInfo* src)
{
	slot_slot_slot(NULL, NULL, src, HAVE_returnarg_float, Tcopy);
}
#endif

#if defined(HAVE_returnarg_double)
void
returnarg_double(SlotInfo* src)
{
	lslot_lslot_lslot(NULL, NULL, src, HAVE_returnarg_double, Tcopy);
}
#endif


/* ----------------------------------------------------------------------- */
/* Labels.								   */
/*									   */

label*
reference_label(int32 i UNUSED, int32 n)
{
	label* l;

	assert(n < MAXLABTAB);
	if (labtab[n] == 0) {
		l = KaffeJIT_newLabel();
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
	label* l = KaffeJIT_newLabel();
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
		l = KaffeJIT_newLabel();
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
		labtab[n] = KaffeJIT_newLabel();
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
build_code_ref(uint8* pos, uintp pc)
{
	label* l;
	jint offset;

	offset = (pos[0] * 0x01000000 + pos[1] * 0x00010000 +
		  pos[2] * 0x00000100 + pos[3] * 0x00000001);
	l = reference_code_label(pc+offset);

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
		cmp_int(dst, src, tmp);
		slot_freetmp(tmp);
	}
}

void
cmp_int(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
	slot_slot_slot(dst, src, src2, HAVE_cmp_int, Tcomplex);
}
#endif

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
	slot_slot_slot(dst, src, src2, HAVE_cmp_ref, Tcomplex);
}
#endif

void
lcmp(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_lcmp)
	slot_lslot_lslot(dst, src, src2, HAVE_lcmp, Tcomplex);
#else
	end_sub_block();
	pusharg_long(src2, pusharg_long_idx_inc);
	pusharg_long(src, 0);
	call_soft(soft_lcmp);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
cmpl_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpl_float)
	slot_slot_slot(dst, src, src2, HAVE_cmpl_float, Tcomplex);
#else
	end_sub_block();
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
	call_soft(soft_fcmpl);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
cmpl_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpl_double)
	slot_lslot_lslot(dst, src, src2, HAVE_cmpl_double, Tcomplex);
#else
	end_sub_block();
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
	call_soft(soft_dcmpl);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
cmpg_float(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpg_float)
	slot_slot_slot(dst, src, src2, HAVE_cmpg_float, Tcomplex);
#else
	end_sub_block();
	pusharg_float(src2, 1);
	pusharg_float(src, 0);
	call_soft(soft_fcmpg);
	popargs();
	start_sub_block();
	return_int(dst);
#endif
}

void
cmpg_double(SlotInfo* dst, SlotInfo* src, SlotInfo* src2)
{
#if defined(HAVE_cmpg_double)
	slot_lslot_lslot(dst, src, src2, HAVE_cmpg_double, Tcomplex);
#else
	end_sub_block();
	pusharg_double(src2, pusharg_long_idx_inc);
	pusharg_double(src, 0);
	call_soft(soft_dcmpg);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_int(src, 0);
	call_soft(soft_cvtif);
	popargs();
	start_sub_block();
	return_float(dst);
#endif
}

void
cvt_int_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_double)
	lslot_lslot_slot(dst, NULL, src, HAVE_cvt_int_double, Tcomplex);
#else
	end_sub_block();
	pusharg_int(src, 0);
	call_soft(soft_cvtid);
	popargs();
	start_sub_block();
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
	slot_slot_lslot(dst, NULL, src, HAVE_cvt_long_float, Tcomplex);
#else
	end_sub_block();
	pusharg_long(src, 0);
	call_soft(soft_cvtlf);
	popargs();
	start_sub_block();
	return_float(dst);
#endif
}

void
cvt_long_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_long_double)
	lslot_lslot_lslot(dst, NULL, src, HAVE_cvt_long_double, Tcomplex);
#else
	end_sub_block();
	pusharg_long(src, 0);
	call_soft(soft_cvtld);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_float(src, 0);
	call_soft(soft_cvtfi);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_float(src, 0);
	call_soft(soft_cvtfl);
	popargs();
	start_sub_block();
	return_long(dst);
#endif
}

void
cvt_float_double(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_float_double)
	lslot_lslot_slot(dst, NULL, src, HAVE_cvt_float_double, Tcomplex);
#else
	end_sub_block();
	pusharg_float(src, 0);
	call_soft(soft_cvtfd);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_double(src, 0);
	call_soft(soft_cvtdi);
	popargs();
	start_sub_block();
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
	end_sub_block();
	pusharg_double(src, 0);
	call_soft(soft_cvtdl);
	popargs();
	start_sub_block();
	return_long(dst);
#endif
}

void
cvt_double_float(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_double_float)
	slot_slot_lslot(dst, NULL, src, HAVE_cvt_double_float, Tcomplex);
#else
	end_sub_block();
	pusharg_double(src, 0);
	call_soft(soft_cvtdf);
	popargs();
	start_sub_block();
	return_float(dst);
#endif
}

void
cvt_int_byte(SlotInfo* dst, SlotInfo* src)
{
#if defined(HAVE_cvt_int_byte)
	slot_slot_slot(dst, 0, src, HAVE_cvt_int_byte, Tcomplex);
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
softcall_breakpoint()
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
	const char* sigptr;

	/* Make sure we have enough argument space */
	if (sp_idx + 1 > sz_args) {
		/* add differential */
		addToCounter(&jitmem, "jitmem-temp", 1,
			(sp_idx + 1 - sz_args) * sizeof(struct pusharg_info));
		sz_args = sp_idx + 1;
		args = KREALLOC(args, sizeof(struct pusharg_info) * sz_args);
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

	for (idx--; idx >= 0; idx--) {
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
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			{
				pusharg_int(stack(sp_idx), arg_idx);
			}
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
		}
	}
}

/* ----------------------------------------------------------------------- */
/* Soft calls.								   */
/*									   */

void
softcall_lookupinterfacemethod(SlotInfo* dst, Method* meth, SlotInfo* obj)
{
	/* 'obj' must be written back since it will be reused */
	prepare_function_call();
	/* FIXME: should there be a pusharg_int_const? */
	pusharg_ref_const((void*) meth->idx, 2);		
	pusharg_class_const(meth->class, 1);
	pusharg_ref(obj, 0);
	call_soft(soft_lookupinterfacemethod);
	popargs();
	fixup_function_call();
	return_ref(dst);
}

void
check_array_index(SlotInfo* obj, SlotInfo* idx)
{
	SlotInfo* tmp;

	slot_alloctmp(tmp);
	load_offset_int(tmp, obj, object_array_length);
#if defined(HAVE_ccall_ugt)
	ccall_int_ugt(tmp, idx, soft_badarrayindex);
#else
	end_sub_block();
	cbranch_int_ult(idx, tmp, reference_label(1, 1));
	call_soft(soft_badarrayindex);
	start_sub_block();
	set_label(1, 1);
#endif
	slot_freetmp(tmp);
}

void
check_array_constindex(SlotInfo* obj, jint idx)
{
	SlotInfo* tmp;

	slot_alloctmp(tmp);
	load_offset_int(tmp, obj, object_array_length);
#if defined(HAVE_ccall_ugt)
	ccall_int_const_ugt(tmp, idx, soft_badarrayindex);
#else
	end_sub_block();
	cbranch_int_const_ugt(tmp, idx, reference_label(1, 1));
	call_soft(soft_badarrayindex);
	start_sub_block();
	set_label(1, 1);
#endif
	slot_freetmp(tmp);
}

#if defined(HAVE_ccall_ugt)
void
ccall_ugt(void* func)
{
	slot_const_const(0, 0, (jword)func, HAVE_ccall_ugt, Tnull);
}

void
ccall_int_ugt(SlotInfo* s1, SlotInfo* s2, void* func)
{
	cmp_int(0, s1, s2);
	ccall_ugt(func);
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
	sync_registers();
	call_soft(soft_nullpointer);
}


void
softcall_nosuchclass(Utf8Const* name)
{
	sync_registers();
	prepare_function_call();
	pusharg_utf8_const(name, 0);
	call_soft(soft_nosuchclass);
	popargs();
	fixup_function_call();
}

void
softcall_nosuchmethod(Hjava_lang_Class* cls, Utf8Const* name, Utf8Const* sig)
{
	sync_registers();
	prepare_function_call();
	pusharg_utf8_const(sig, 2);
	pusharg_utf8_const(name, 1);
	pusharg_class_const(cls, 0);
	call_soft(soft_nosuchmethod);
	popargs();
	fixup_function_call();
}

void
softcall_nosuchfield(Utf8Const* cls, Utf8Const* name)
{
	sync_registers();
	prepare_function_call();
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
	call_soft(soft_nosuchfield);
	popargs();
	fixup_function_call();
}

void
softcall_linkage(Utf8Const* cls, Utf8Const* name)
{
	sync_registers();
	prepare_function_call();
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
	call_soft(soft_linkage);
	popargs();
	fixup_function_call();
}

void
softcall_illegalaccess(Utf8Const* cls, Utf8Const* name)
{
	sync_registers();
	prepare_function_call();
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
	call_soft(soft_illegalaccess);
	popargs();
	fixup_function_call();
}

void
softcall_incompatibleclasschange(Utf8Const* cls, Utf8Const* name)
{
        sync_registers();
        prepare_function_call();
        pusharg_utf8_const(name, 1);
        pusharg_utf8_const(cls, 0);
        call_soft(soft_incompatibleclasschange);
        popargs();
        fixup_function_call();
}

void
softcall_abstractmethod(Utf8Const* cls, Utf8Const* name)
{
	sync_registers();
	prepare_function_call();
	pusharg_utf8_const(name, 1);
	pusharg_utf8_const(cls, 0);
	call_soft(soft_abstractmethod);
	popargs();
	fixup_function_call();
}

void
softcall_new(SlotInfo* dst, Hjava_lang_Class* classobj)
{
	prepare_function_call();
	pusharg_class_const(classobj, 0);
	call_soft(soft_new);
	popargs();
	fixup_function_call();
	return_ref(dst);
}

void
softcall_newarray(SlotInfo* dst, SlotInfo* size, int type)
{
	if (size == dst) {
		slot_nowriteback(size);
	}
	prepare_function_call();
	pusharg_int(size, 1);
	pusharg_int_const(type, 0);
	call_soft(soft_newarray);
	popargs();
	fixup_function_call();
	return_ref(dst);
}

void
softcall_anewarray(SlotInfo* dst, SlotInfo* size, Hjava_lang_Class* type)
{
	if (dst == size) {
		slot_nowriteback(size);
	}
	prepare_function_call();
	pusharg_int(size, 1);
	pusharg_class_const(type, 0);
	call_soft(soft_anewarray);
	popargs();
	fixup_function_call();
	return_ref(dst);
}


void
softcall_multianewarray(SlotInfo* dst, int size, SlotInfo* stktop, Hjava_lang_Class* classobj)
{
	int i;

	prepare_function_call();
	for (i = 0; i < size; i++) {
		pusharg_int(&stktop[i], 1+size-i);
	}
	pusharg_int_const(size, 1);
	pusharg_class_const(classobj, 0);
	call_soft(soft_multianewarray);
	popargs();
	pop(size);
	fixup_function_call();
	push(1);
	return_ref(dst);
}

void
softcall_athrow(SlotInfo* obj)
{
	slot_nowriteback(obj);
	prepare_function_call();
	pusharg_ref(obj, 0);
	call_soft(soft_athrow);
	popargs();
	fixup_function_call();
}

void
softcall_checkcast(SlotInfo* objw, SlotInfo* objr, Hjava_lang_Class* class)
{
	/* Must keep 'obj' */
	prepare_function_call();
	pusharg_ref(objr, 1);
	pusharg_class_const(class, 0);
	call_soft(soft_checkcast);
	popargs();
	fixup_function_call();
	move_ref(objw, objr);
}

void
softcall_instanceof(SlotInfo* dst, SlotInfo* obj, Hjava_lang_Class* class)
{
	if (dst == obj) {
		slot_nowriteback(obj);
	}
	prepare_function_call();
	pusharg_ref(obj, 1);
	pusharg_class_const(class, 0);
	call_soft(soft_instanceof);
	popargs();
	fixup_function_call();
	return_int(dst);
}

void
softcall_monitorenter(SlotInfo* mon)
{
	slot_nowriteback(mon);
	prepare_function_call();
	pusharg_ref(mon, 0);
	call_soft(lockObject);
	popargs();
	fixup_function_call();
}

void
softcall_monitorexit(SlotInfo* mon)
{
	slot_nowriteback(mon);
	prepare_function_call();
	pusharg_ref(mon, 0);
	call_soft(unlockObject);
	popargs();
	fixup_function_call();
}

void
softcall_initialise_class(Hjava_lang_Class* c)
{
	prepare_function_call();
	pusharg_class_const(c, 0);
	call_soft(soft_initialise_class);
	popargs();
	fixup_function_call();
}

void
softcall_checkarraystore(SlotInfo* array, SlotInfo* obj)
{
	prepare_function_call();
	pusharg_ref(obj, 1);
	pusharg_ref(array, 0);
	call_soft(soft_checkarraystore);
	popargs();
	fixup_function_call();
}

#if defined(ENABLE_JVMPI)
void
softcall_exit_method(Method *meth)
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

void
softcall_enter_method(SlotInfo *obj, Method *meth)
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

#endif

#if defined(GC_INCREMENTAL)
void
softcall_addreference(SlotInfo* from, SlotInfo* to)
{
	prepare_function_call();
	pusharg_ref(to, 1);
	pusharg_ref(from, 0);
	call_soft(soft_addreference);
	popargs();
	fixup_function_call();
}

void
softcall_addreference_static(void* from, SlotInfo* to)
{
	prepare_function_call();
	pusharg_ref(to, 1);
	pusharg_ref_const(from, 0);
	call_soft(soft_addreference);
	popargs();
	fixup_function_call();
}
#endif
