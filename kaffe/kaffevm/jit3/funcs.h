/* machine.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __funcs_h
#define __funcs_h

extern void unimplemented (sequence* s);
extern void nop (sequence* s);
extern void prologue_xxx (sequence* s);
extern void check_stack_limit_xRC (sequence* s);
extern void eprologue_xLx (sequence* s);
extern void epilogue_xxx (sequence* s);
extern void spill_Rxx (sequence* s);
extern void fspill_Rxx (sequence* s);
extern void fspilll_Rxx (sequence* s);
extern void reload_Rxx (sequence* s);
extern void freload_Rxx (sequence* s);
extern void freloadl_Rxx (sequence* s);
extern void move_RxC (sequence* s);
extern void move_RxL (sequence* s);
extern void move_RxR (sequence* s);
extern void fmove_RxC (sequence* s);
extern void fmove_RxR (sequence* s);
extern void fmovel_RxC (sequence* s);
extern void fmovel_RxR (sequence* s);
extern void add_RRR (sequence* s);
extern void adc_RRR (sequence* s);
extern void fadd_RRR (sequence* s);
extern void faddl_RRR (sequence* s);
extern void sub_RRR (sequence* s);
extern void sbc_RRR (sequence* s);
extern void fsub_RRR (sequence* s);
extern void fsubl_RRR (sequence* s);
extern void negf_RxR (sequence* s);
extern void negd_RxR (sequence* s);
extern void mul_RRR (sequence* s);
extern void fmul_RRR (sequence* s);
extern void fmull_RRR (sequence* s);
extern void div_RRR (sequence* s);
extern void fdiv_RRR (sequence* s);
extern void fdivl_RRR (sequence* s);
extern void rem_RRR (sequence* s);
extern void and_RRR (sequence* s);
extern void or_RRR (sequence* s);
extern void xor_RRR (sequence* s);
extern void ashr_RRR (sequence* s);
extern void lshr_RRR (sequence* s);
extern void lshl_RRR (sequence* s);
extern void load_RxR (sequence* s);
extern void fload_RxR (sequence* s);
extern void floadl_RxR (sequence* s);
extern void store_xRR (sequence* s);
extern void fstore_RxR (sequence* s);
extern void fstorel_RxR (sequence* s);
extern void cmp_xRR (sequence* s);
extern void cvtif_RxR (sequence* s);
extern void cvtid_RxR (sequence* s);
extern void cvtlf_RxR (sequence* s);
extern void cvtld_RxR (sequence* s);
extern void cvtfd_RxR (sequence* s);
extern void cvtdf_RxR (sequence* s);
extern void set_word_xxC (sequence* s);
extern void set_wordpc_xxC (sequence* s);
extern void set_label_xxC (sequence* s);
extern void branch_xCC (sequence* s);
extern void branch_indirect_xRC (sequence* s);
extern void call_xCC (sequence* s);
extern void call_xRC (sequence* s);
extern void call_ind_xCC (sequence* s);
extern void push_xRC (sequence* s);
extern void fpush_xRC (sequence* s);
extern void fpushl_xRC (sequence* s);
extern void popargs_xxC (sequence* s);
extern void return_Rxx (sequence* s);
extern void returnl_Rxx (sequence* s);
extern void freturn_Rxx (sequence* s);
extern void freturnl_Rxx (sequence* s);
extern void returnarg_xxR (sequence* s);
extern void returnargl_xxR (sequence* s);
extern void freturnarg_xxR (sequence* s);
extern void freturnargl_xxR (sequence* s);
extern void add_RRC (sequence* s);
extern void sub_RRC (sequence* s);
extern void load_RRC (sequence* s);
extern void loadb_RxR (sequence* s);
extern void loadc_RxR (sequence* s);
extern void loads_RxR (sequence* s);
extern void store_xRRC (sequence* s);
extern void storeb_xRR (sequence* s);
extern void stores_xRR (sequence* s);
extern void cmp_xRC (sequence* s);
extern void push_xCC (sequence* s);
extern void lshl_RRC (sequence* s);
extern void monenter_xxRCC (sequence* s);
extern void monexit_xxRCC (sequence* s);
extern void get_arg_ptr_R (sequence* s);
extern void and_RRC (sequence* s);
extern void or_RRC (sequence* s);
extern void xor_RRC (sequence* s);
extern void mul_RRC (sequence* s);
extern void lshr_RRC (sequence* s);
extern void ashr_RRC (sequence* s);
extern void neg_RxR (sequence* s);
extern void loadb_RRRC (sequence* s);
extern void load_RRRC (sequence* s);
extern void loadc_RRRC (sequence* s);
extern void storeb_RRRC (sequence* s);
extern void stores_RRRC (sequence* s);
extern void store_RRRC (sequence* s);
extern void load_RxA (sequence* s);
extern void store_xRA (sequence* s);
extern void storeb_xRRC (sequence* s);
extern void stores_xRRC (sequence* s);
extern void store_xRCC (sequence* s);
extern void storeb_xRCC (sequence* s);
extern void storeb_RRCC (sequence* s);
extern void fakecall_xCC (sequence* s);
extern void cmp_xRRC (sequence* s);

#endif
