/* funcs.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __funcs_h
#define __funcs_h

extern void unimplemented (sequence* s);
extern void nop (sequence* s);

/* In the likely event that a jit3 implementation uses defines different HAVE_*
 * macros for the same backend function, you may get compiler warnings about
 * redundant declarations.
 */
#if defined(HAVE_adc_int)
extern void HAVE_adc_int (sequence* s);
#endif /* defined(HAVE_adc_int) */

#if defined(HAVE_add_double)
extern void HAVE_add_double (sequence* s);
#endif /* defined(HAVE_add_double) */

#if defined(HAVE_add_float)
extern void HAVE_add_float (sequence* s);
#endif /* defined(HAVE_add_float) */

#if defined(HAVE_add_int)
extern void HAVE_add_int (sequence* s);
#endif /* defined(HAVE_add_int) */

#if defined(HAVE_add_int_const)
extern void HAVE_add_int_const (sequence* s);
#endif /* defined(HAVE_add_int_const) */

#if defined(HAVE_add_long)
extern void HAVE_add_long (sequence* s);
#endif /* defined(HAVE_add_long) */

#if defined(HAVE_add_long_const)
extern void HAVE_add_long_const (sequence* s);
#endif /* defined(HAVE_add_long_const) */

#if defined(HAVE_add_ref)
extern void HAVE_add_ref (sequence* s);
#endif /* defined(HAVE_add_ref) */

#if defined(HAVE_add_ref_const)
extern void HAVE_add_ref_const (sequence* s);
#endif /* defined(HAVE_add_ref_const) */

#if defined(HAVE_and_int)
extern void HAVE_and_int (sequence* s);
#endif /* defined(HAVE_and_int) */

#if defined(HAVE_and_int_const)
extern void HAVE_and_int_const (sequence* s);
#endif /* defined(HAVE_and_int_const) */

#if defined(HAVE_and_long)
extern void HAVE_and_long (sequence* s);
#endif /* defined(HAVE_and_long) */

#if defined(HAVE_and_long_const)
extern void HAVE_and_long_const (sequence* s);
#endif /* defined(HAVE_and_long_const) */

#if defined(HAVE_ashr_int)
extern void HAVE_ashr_int (sequence* s);
#endif /* defined(HAVE_ashr_int) */

#if defined(HAVE_ashr_int_const)
extern void HAVE_ashr_int_const (sequence* s);
#endif /* defined(HAVE_ashr_int_const) */

#if defined(HAVE_ashr_long)
extern void HAVE_ashr_long (sequence* s);
#endif /* defined(HAVE_ashr_long) */

#if defined(HAVE_branch)
extern void HAVE_branch (sequence* s);
#endif /* defined(HAVE_branch) */

#if defined(HAVE_branch_indirect)
extern void HAVE_branch_indirect (sequence* s);
#endif /* defined(HAVE_branch_indirect) */

#if defined(HAVE_build_code_ref)
extern void HAVE_build_code_ref (sequence* s);
#endif /* defined(HAVE_build_code_ref) */

#if defined(HAVE_build_const_ref)
extern void HAVE_build_const_ref (sequence* s);
#endif /* defined(HAVE_build_const_ref) */

#if defined(HAVE_branch_and_link)
extern void HAVE_branch_and_link (sequence* s);
#endif /* defined(HAVE_branch_and_link) */

#if defined(HAVE_build_key)
extern void HAVE_build_key (sequence* s);
#endif /* defined(HAVE_build_key) */

#if defined(HAVE_call)
extern void HAVE_call (sequence* s);
#endif /* defined(HAVE_call) */

#if defined(HAVE_call_indirect_const)
extern void HAVE_call_indirect_const (sequence* s);
#endif /* defined(HAVE_call_indirect_const) */

#if defined(HAVE_call_ref)
extern void HAVE_call_ref (sequence* s);
#endif /* defined(HAVE_call_ref) */

#if defined(HAVE_cbranch_int)
extern void HAVE_cbranch_int (sequence* s);
#endif /* defined(HAVE_cbranch_int) */

#if defined(HAVE_cbranch_int_const)
extern void HAVE_cbranch_int_const (sequence* s);
#endif /* defined(HAVE_cbranch_int_const) */

#if defined(HAVE_cbranch_ref)
extern void HAVE_cbranch_ref (sequence* s);
#endif /* defined(HAVE_cbranch_ref) */

#if defined(HAVE_cbranch_ref_const)
extern void HAVE_cbranch_ref_const (sequence* s);
#endif /* defined(HAVE_cbranch_ref_const) */

#if defined(HAVE_check_stack_limit)
extern void HAVE_check_stack_limit (sequence* s);
#endif /* defined(HAVE_check_stack_limit) */

#if defined(HAVE_check_stack_limit_constpool)
extern void HAVE_check_stack_limit_constpool (sequence* s);
#endif /* defined(HAVE_check_stack_limit_constpool) */

#if defined(HAVE_cmpg_double)
extern void HAVE_cmpg_double (sequence* s);
#endif /* defined(HAVE_cmpg_double) */

#if defined(HAVE_cmpg_float)
extern void HAVE_cmpg_float (sequence* s);
#endif /* defined(HAVE_cmpg_float) */

#if defined(HAVE_cmp_int)
extern void HAVE_cmp_int (sequence* s);
#endif /* defined(HAVE_cmp_int) */

#if defined(HAVE_cmp_int_const)
extern void HAVE_cmp_int_const (sequence* s);
#endif /* defined(HAVE_cmp_int_const) */

#if defined(HAVE_cmpl_int)
extern void HAVE_cmpl_int (sequence* s);
#endif /* defined(HAVE_cmpl_int) */

#if defined(HAVE_cmpl_double)
extern void HAVE_cmpl_double (sequence* s);
#endif /* defined(HAVE_cmpl_double) */

#if defined(HAVE_cmpl_float)
extern void HAVE_cmpl_float (sequence* s);
#endif /* defined(HAVE_cmpl_float) */

#if defined(HAVE_cmp_long)
extern void HAVE_cmp_long (sequence* s);
#endif /* defined(HAVE_cmp_long) */

#if defined(HAVE_cmp_offset_int)
extern void HAVE_cmp_offset_int (sequence* s);
#endif /* defined(HAVE_cmp_offset_int) */

#if defined(HAVE_cmp_ref)
extern void HAVE_cmp_ref (sequence* s);
#endif /* defined(HAVE_cmp_ref) */

#if defined(HAVE_cmp_ref_const)
extern void HAVE_cmp_ref_const (sequence* s);
#endif /* defined(HAVE_cmp_ref_const) */

#if defined(HAVE_cvt_double_float)
extern void HAVE_cvt_double_float (sequence* s);
#endif /* defined(HAVE_cvt_double_float) */

#if defined(HAVE_cvt_double_int)
extern void HAVE_cvt_double_int (sequence* s);
#endif /* defined(HAVE_cvt_double_int) */

#if defined(HAVE_cvt_double_int_ieee)
extern void HAVE_cvt_double_int_ieee (sequence* s);
#endif /* defined(HAVE_cvt_double_int_ieee) */

#if defined(HAVE_cvt_double_long)
extern void HAVE_cvt_double_long (sequence* s);
#endif /* defined(HAVE_cvt_double_long) */

#if defined(HAVE_cvt_float_double)
extern void HAVE_cvt_float_double (sequence* s);
#endif /* defined(HAVE_cvt_float_double) */

#if defined(HAVE_cvt_float_int)
extern void HAVE_cvt_float_int (sequence* s);
#endif /* defined(HAVE_cvt_float_int) */

#if defined(HAVE_cvt_float_long)
extern void HAVE_cvt_float_long (sequence* s);
#endif /* defined(HAVE_cvt_float_long) */

#if defined(HAVE_cvt_float_int_ieee)
extern void HAVE_cvt_float_int_ieee (sequence* s);
#endif /* defined(HAVE_cvt_float_int_ieee) */

#if defined(HAVE_cvt_int_byte)
extern void HAVE_cvt_int_byte (sequence* s);
#endif /* defined(HAVE_cvt_int_byte) */

#if defined(HAVE_cvt_int_char)
extern void HAVE_cvt_int_char (sequence* s);
#endif /* defined(HAVE_cvt_int_char) */

#if defined(HAVE_cvt_int_double)
extern void HAVE_cvt_int_double (sequence* s);
#endif /* defined(HAVE_cvt_int_double) */

#if defined(HAVE_cvt_int_float)
extern void HAVE_cvt_int_float (sequence* s);
#endif /* defined(HAVE_cvt_int_float) */

#if defined(HAVE_cvt_int_long)
extern void HAVE_cvt_int_long (sequence* s);
#endif /* defined(HAVE_cvt_int_long) */

#if defined(HAVE_cvt_int_short)
extern void HAVE_cvt_int_short (sequence* s);
#endif /* defined(HAVE_cvt_int_short) */

#if defined(HAVE_cvt_long_double)
extern void HAVE_cvt_long_double (sequence* s);
#endif /* defined(HAVE_cvt_long_double) */

#if defined(HAVE_cvt_long_float)
extern void HAVE_cvt_long_float (sequence* s);
#endif /* defined(HAVE_cvt_long_float) */

#if defined(HAVE_cvt_long_int)
extern void HAVE_cvt_long_int (sequence* s);
#endif /* defined(HAVE_cvt_long_int) */

#if defined(HAVE_div_double)
extern void HAVE_div_double (sequence* s);
#endif /* defined(HAVE_div_double) */

#if defined(HAVE_div_float)
extern void HAVE_div_float (sequence* s);
#endif /* defined(HAVE_div_float) */

#if defined(HAVE_div_int)
extern void HAVE_div_int (sequence* s);
#endif /* defined(HAVE_div_int) */

#if defined(HAVE_div_long)
extern void HAVE_div_long (sequence* s);
#endif /* defined(HAVE_div_long) */

#if defined(HAVE_epilogue)
extern void HAVE_epilogue (sequence* s);
#endif /* defined(HAVE_epilogue) */

#if defined(HAVE_exception_prologue)
extern void HAVE_exception_prologue (sequence* s);
#endif /* defined(HAVE_exception_prologue) */

#if defined(HAVE_fakecall)
extern void HAVE_fakecall (sequence* s);
#endif /* defined(HAVE_fakecall) */

#if defined(HAVE_fakecall_constpool)
extern void HAVE_fakecall_constpool (sequence* s);
#endif /* defined(HAVE_fakecall_constpool) */

#if defined(HAVE_get_arg_ptr)
extern void HAVE_get_arg_ptr (sequence* s);
#endif /* defined(HAVE_get_arg_ptr) */

#if defined(HAVE_kill_readonce_register)
extern void HAVE_kill_readonce_register (SlotData *s);
#endif /* defined(HAVE_kill_readonce_register) */

#if defined(HAVE_lcmp)
extern void HAVE_lcmp (sequence* s);
#endif /* defined(HAVE_lcmp) */

#if defined(HAVE_load_addr_byte)
extern void HAVE_load_addr_byte (sequence* s);
#endif /* defined(HAVE_load_addr_byte) */

#if defined(HAVE_load_addr_char)
extern void HAVE_load_addr_char (sequence* s);
#endif /* defined(HAVE_load_addr_char) */

#if defined(HAVE_load_addr_double)
extern void HAVE_load_addr_double (sequence* s);
#endif /* defined(HAVE_load_addr_double) */

#if defined(HAVE_load_addr_float)
extern void HAVE_load_addr_float (sequence* s);
#endif /* defined(HAVE_load_addr_float) */

#if defined(HAVE_load_addr_int)
extern void HAVE_load_addr_int (sequence* s);
#endif /* defined(HAVE_load_addr_int) */

#if defined(HAVE_load_addr_long)
extern void HAVE_load_addr_long (sequence* s);
#endif /* defined(HAVE_load_addr_long) */

#if defined(HAVE_load_addr_ref)
extern void HAVE_load_addr_ref (sequence* s);
#endif /* defined(HAVE_load_addr_ref) */

#if defined(HAVE_load_addr_short)
extern void HAVE_load_addr_short (sequence* s);
#endif /* defined(HAVE_load_addr_short) */

#if defined(HAVE_load_byte)
extern void HAVE_load_byte (sequence* s);
#endif /* defined(HAVE_load_byte) */

#if defined(HAVE_load_char)
extern void HAVE_load_char (sequence* s);
#endif /* defined(HAVE_load_char) */

#if defined(HAVE_load_constpool_double)
extern void HAVE_load_constpool_double (sequence* s);
#endif /* defined(HAVE_load_constpool_double) */

#if defined(HAVE_load_constpool_float)
extern void HAVE_load_constpool_float (sequence* s);
#endif /* defined(HAVE_load_constpool_float) */

#if defined(HAVE_load_constpool_int)
extern void HAVE_load_constpool_int (sequence* s);
#endif /* defined(HAVE_load_constpool_int) */

#if defined(HAVE_load_constpool_ref)
extern void HAVE_load_constpool_ref (sequence* s);
#endif /* defined(HAVE_load_constpool_ref) */

#if defined(HAVE_load_double)
extern void HAVE_load_double (sequence* s);
#endif /* defined(HAVE_load_double) */

#if defined(HAVE_load_float)
extern void HAVE_load_float (sequence* s);
#endif /* defined(HAVE_load_float) */

#if defined(HAVE_load_int)
extern void HAVE_load_int (sequence* s);
#endif /* defined(HAVE_load_int) */

#if defined(HAVE_load_long)
extern void HAVE_load_long (sequence* s);
#endif /* defined(HAVE_load_long) */

#if defined(HAVE_load_offset_byte)
extern void HAVE_load_offset_byte (sequence* s);
#endif /* defined(HAVE_load_offset_byte) */

#if defined(HAVE_load_offset_char)
extern void HAVE_load_offset_char (sequence* s);
#endif /* defined(HAVE_load_offset_char) */

#if defined(HAVE_load_offset_int)
extern void HAVE_load_offset_int (sequence* s);
#endif /* defined(HAVE_load_offset_int) */

#if defined(HAVE_load_offset_float)
extern void HAVE_load_offset_float (sequence* s);
#endif /* defined(HAVE_load_offset_float) */

#if defined(HAVE_load_offset_long)
extern void HAVE_load_offset_long (sequence* s);
#endif /* defined(HAVE_load_offset_long) */

#if defined(HAVE_load_offset_double)
extern void HAVE_load_offset_double (sequence* s);
#endif /* defined(HAVE_load_offset_double) */

#if defined(HAVE_load_offset_ref)
extern void HAVE_load_offset_ref (sequence* s);
#endif /* defined(HAVE_load_offset_ref) */

#if defined(HAVE_load_offset_scaled_byte)
extern void HAVE_load_offset_scaled_byte (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_byte) */

#if defined(HAVE_load_offset_scaled_char)
extern void HAVE_load_offset_scaled_char (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_char) */

#if defined(HAVE_load_offset_scaled_double)
extern void HAVE_load_offset_scaled_double (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_double) */

#if defined(HAVE_load_offset_scaled_float)
extern void HAVE_load_offset_scaled_float (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_float) */

#if defined(HAVE_load_offset_scaled_int)
extern void HAVE_load_offset_scaled_int (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_int) */

#if defined(HAVE_load_offset_scaled_long)
extern void HAVE_load_offset_scaled_long (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_long) */

#if defined(HAVE_load_offset_scaled_ref)
extern void HAVE_load_offset_scaled_ref (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_ref) */

#if defined(HAVE_load_offset_scaled_short)
extern void HAVE_load_offset_scaled_short (sequence* s);
#endif /* defined(HAVE_load_offset_scaled_short) */

#if defined(HAVE_load_ref)
extern void HAVE_load_ref (sequence* s);
#endif /* defined(HAVE_load_ref) */

#if defined(HAVE_load_short)
extern void HAVE_load_short (sequence* s);
#endif /* defined(HAVE_load_short) */

#if defined(HAVE_lshl_int)
extern void HAVE_lshl_int (sequence* s);
#endif /* defined(HAVE_lshl_int) */

#if defined(HAVE_lshl_int_const)
extern void HAVE_lshl_int_const (sequence* s);
#endif /* defined(HAVE_lshl_int_const) */

#if defined(HAVE_lshl_long)
extern void HAVE_lshl_long (sequence* s);
#endif /* defined(HAVE_lshl_long) */

#if defined(HAVE_lshl_long_const)
extern void HAVE_lshl_long_const (sequence* s);
#endif /* defined(HAVE_lshl_long_const) */

#if defined(HAVE_lshr_int)
extern void HAVE_lshr_int (sequence* s);
#endif /* defined(HAVE_lshr_int) */

#if defined(HAVE_lshr_int_const)
extern void HAVE_lshr_int_const (sequence* s);
#endif /* defined(HAVE_lshr_int_const) */

#if defined(HAVE_lshr_long)
extern void HAVE_lshr_long (sequence* s);
#endif /* defined(HAVE_lshr_long) */

#if defined(HAVE_mon_enter)
extern void HAVE_mon_enter (sequence* s);
#endif /* defined(HAVE_mon_enter) */

#if defined(HAVE_mon_exit)
extern void HAVE_mon_exit (sequence* s);
#endif /* defined(HAVE_mon_exit) */

#if defined(HAVE_move_any)
extern void HAVE_move_any (sequence* s);
#endif /* defined(HAVE_move_any) */

#if defined(HAVE_move_double)
extern void HAVE_move_double (sequence* s);
#endif /* defined(HAVE_move_double) */

#if defined(HAVE_move_double_const)
extern void HAVE_move_double_const (sequence* s);
#endif /* defined(HAVE_move_double_const) */

#if defined(HAVE_move_float)
extern void HAVE_move_float (sequence* s);
#endif /* defined(HAVE_move_float) */

#if defined(HAVE_move_float_const)
extern void HAVE_move_float_const (sequence* s);
#endif /* defined(HAVE_move_float_const) */

#if defined(HAVE_move_int)
extern void HAVE_move_int (sequence* s);
#endif /* defined(HAVE_move_int) */

#if defined(HAVE_move_int_const)
extern void HAVE_move_int_const (sequence* s);
#endif /* defined(HAVE_move_int_const) */

#if defined(HAVE_move_int_const_incode)
extern void HAVE_move_int_const_incode (sequence* s);
#endif /* defined(HAVE_move_int_const_incode) */

/*
#if defined(HAVE_move_int_const_rangecheck)
extern void HAVE_move_int_const_rangecheck (sequence* s);
#endif *//* defined(HAVE_move_int_const_rangecheck) */


#if defined(HAVE_move_label_const)
extern void HAVE_move_label_const (sequence* s);
#endif /* defined(HAVE_move_label_const) */

#if defined(HAVE_move_long)
extern void HAVE_move_long (sequence* s);
#endif /* defined(HAVE_move_long) */

#if defined(HAVE_move_long_const)
extern void HAVE_move_long_const (sequence* s);
#endif /* defined(HAVE_move_long_const) */

#if defined(HAVE_move_ref)
extern void HAVE_move_ref (sequence* s);
#endif /* defined(HAVE_move_ref) */

#if defined(HAVE_move_ref_const)
extern void HAVE_move_ref_const (sequence* s);
#endif /* defined(HAVE_move_ref_const) */

#if defined(HAVE_move_register_int)
extern void HAVE_move_register_int (int toreg, int fromreg);
#endif /* defined(HAVE_move_register_int) */

#if defined(HAVE_move_register_ref)
extern void HAVE_move_register_ref (int toreg, int fromreg);
#endif /* defined(HAVE_move_register_ref) */

#if defined(HAVE_move_register_float)
extern void HAVE_move_register_float (int toreg, int fromreg);
#endif /* defined(HAVE_move_register_float) */

#if defined(HAVE_move_register_double)
extern void HAVE_move_register_double (int toreg, int fromreg);
#endif /* defined(HAVE_move_register_double) */

#if defined(HAVE_mul_double)
extern void HAVE_mul_double (sequence* s);
#endif /* defined(HAVE_mul_double) */

#if defined(HAVE_mul_float)
extern void HAVE_mul_float (sequence* s);
#endif /* defined(HAVE_mul_float) */

#if defined(HAVE_mul_int)
extern void HAVE_mul_int (sequence* s);
#endif /* defined(HAVE_mul_int) */

#if defined(HAVE_mul_int_const)
extern void HAVE_mul_int_const (sequence* s);
#endif /* defined(HAVE_mul_int_const) */

#if defined(HAVE_mul_long)
extern void HAVE_mul_long (sequence* s);
#endif /* defined(HAVE_mul_long) */

#if defined(HAVE_neg_double)
extern void HAVE_neg_double (sequence* s);
#endif /* defined(HAVE_neg_double) */

#if defined(HAVE_neg_float)
extern void HAVE_neg_float (sequence* s);
#endif /* defined(HAVE_neg_float) */

#if defined(HAVE_neg_int)
extern void HAVE_neg_int (sequence* s);
#endif /* defined(HAVE_neg_int) */

#if defined(HAVE_neg_long)
extern void HAVE_neg_long (sequence* s);
#endif /* defined(HAVE_neg_long) */

#if defined(HAVE_ngc_int)
extern void HAVE_ngc_int (sequence* s);
#endif /* defined(HAVE_ngc_int) */

#if defined(HAVE_nor_int)
extern void HAVE_nor_int (sequence* s);
#endif /* defined(HAVE_nor_int) */

#if defined(HAVE_or_int)
extern void HAVE_or_int (sequence* s);
#endif /* defined(HAVE_or_int) */

#if defined(HAVE_or_int_const)
extern void HAVE_or_int_const (sequence* s);
#endif /* defined(HAVE_or_int_const) */

#if defined(HAVE_or_long)
extern void HAVE_or_long (sequence* s);
#endif /* defined(HAVE_or_long) */

#if defined(HAVE_popargs)
extern void HAVE_popargs (sequence* s);
#endif /* defined(HAVE_popargs) */

#if defined(HAVE_pop_frame)
extern void HAVE_pop_frame (sequence* s);
#endif /* defined(HAVE_pop_frame) */

#if defined(HAVE_prologue)
extern void HAVE_prologue (sequence* s);
#endif /* defined(HAVE_prologue) */

#if defined(HAVE_pusharg_double)
extern void HAVE_pusharg_double (sequence* s);
#endif /* defined(HAVE_pusharg_double) */

#if defined(HAVE_pusharg_float)
extern void HAVE_pusharg_float (sequence* s);
#endif /* defined(HAVE_pusharg_float) */

#if defined(HAVE_pusharg_int)
extern void HAVE_pusharg_int (sequence* s);
#endif /* defined(HAVE_pusharg_int) */

#if defined(HAVE_pusharg_int_const)
extern void HAVE_pusharg_int_const (sequence* s);
#endif /* defined(HAVE_pusharg_int_const) */

#if defined(HAVE_pusharg_long)
extern void HAVE_pusharg_long (sequence* s);
#endif /* defined(HAVE_pusharg_long) */

#if defined(HAVE_pusharg_ref)
extern void HAVE_pusharg_ref (sequence* s);
#endif /* defined(HAVE_pusharg_ref) */

#if defined(HAVE_pusharg_ref_const)
extern void HAVE_pusharg_ref_const (sequence* s);
#endif /* defined(HAVE_pusharg_ref_const) */

#if defined(HAVE_push_frame)
extern void HAVE_push_frame (sequence* s);
#endif /* defined(HAVE_push_frame) */

#if defined(HAVE_reload_double)
extern void HAVE_reload_double (sequence* s);
#endif /* defined(HAVE_reload_double) */

#if defined(HAVE_reload_float)
extern void HAVE_reload_float (sequence* s);
#endif /* defined(HAVE_reload_float) */

#if defined(HAVE_reload_int)
extern void HAVE_reload_int (sequence* s);
#endif /* defined(HAVE_reload_int) */

#if defined(HAVE_reload_ref)
extern void HAVE_reload_ref (sequence* s);
#endif /* defined(HAVE_reload_ref) */

#if defined(HAVE_rem_double)
extern void HAVE_rem_double (sequence* s);
#endif /* defined(HAVE_rem_double) */

#if defined(HAVE_rem_float)
extern void HAVE_rem_float (sequence* s);
#endif /* defined(HAVE_rem_float) */

#if defined(HAVE_rem_int)
extern void HAVE_rem_int (sequence* s);
#endif /* defined(HAVE_rem_int) */

#if defined(HAVE_rem_long)
extern void HAVE_rem_long (sequence* s);
#endif /* defined(HAVE_rem_long) */

#if defined(HAVE_returnarg_double)
extern void HAVE_returnarg_double (sequence* s);
#endif /* defined(HAVE_returnarg_double) */

#if defined(HAVE_returnarg_float)
extern void HAVE_returnarg_float (sequence* s);
#endif /* defined(HAVE_returnarg_float) */

#if defined(HAVE_returnarg_int)
extern void HAVE_returnarg_int (sequence* s);
#endif /* defined(HAVE_returnarg_int) */

#if defined(HAVE_returnarg_long)
extern void HAVE_returnarg_long (sequence* s);
#endif /* defined(HAVE_returnarg_long) */

#if defined(HAVE_returnarg_ref)
extern void HAVE_returnarg_ref (sequence* s);
#endif /* defined(HAVE_returnarg_ref) */

#if defined(HAVE_return_double)
extern void HAVE_return_double (sequence* s);
#endif /* defined(HAVE_return_double) */

#if defined(HAVE_return_float)
extern void HAVE_return_float (sequence* s);
#endif /* defined(HAVE_return_float) */

#if defined(HAVE_return_int)
extern void HAVE_return_int (sequence* s);
#endif /* defined(HAVE_return_int) */

#if defined(HAVE_return_long)
extern void HAVE_return_long (sequence* s);
#endif /* defined(HAVE_return_long) */

#if defined(HAVE_return_ref)
extern void HAVE_return_ref (sequence* s);
#endif /* defined(HAVE_return_ref) */

#if defined(HAVE_sbc_int)
extern void HAVE_sbc_int (sequence* s);
#endif /* defined(HAVE_sbc_int) */

#if defined(HAVE_set_label)
extern void HAVE_set_label (sequence* s);
#endif /* defined(HAVE_set_label) */

#if defined(HAVE_set_lt_int)
extern void HAVE_set_lt_int (sequence* s);
#endif /* defined(HAVE_set_lt_int) */

#if defined(HAVE_set_lt_int_const)
extern void HAVE_set_lt_int_const (sequence* s);
#endif /* defined(HAVE_set_lt_int_const) */

#if defined(HAVE_spill_double)
extern void HAVE_spill_double (sequence* s);
#endif /* defined(HAVE_spill_double) */

#if defined(HAVE_spill_float)
extern void HAVE_spill_float (sequence* s);
#endif /* defined(HAVE_spill_float) */

#if defined(HAVE_spill_int)
extern void HAVE_spill_int (sequence* s);
#endif /* defined(HAVE_spill_int) */

#if defined(HAVE_spill_ref)
extern void HAVE_spill_ref (sequence* s);
#endif /* defined(HAVE_spill_ref) */

#if defined(HAVE_store_addr_byte)
extern void HAVE_store_addr_byte (sequence* s);
#endif /* defined(HAVE_store_addr_byte) */

#if defined(HAVE_store_addr_char)
extern void HAVE_store_addr_char (sequence* s);
#endif /* defined(HAVE_store_addr_char) */

#if defined(HAVE_store_addr_double)
extern void HAVE_store_addr_double (sequence* s);
#endif /* defined(HAVE_store_addr_double) */

#if defined(HAVE_store_addr_float)
extern void HAVE_store_addr_float (sequence* s);
#endif /* defined(HAVE_store_addr_float) */

#if defined(HAVE_store_addr_int)
extern void HAVE_store_addr_int (sequence* s);
#endif /* defined(HAVE_store_addr_int) */

#if defined(HAVE_store_addr_long)
extern void HAVE_store_addr_long (sequence* s);
#endif /* defined(HAVE_store_addr_long) */

#if defined(HAVE_store_addr_ref)
extern void HAVE_store_addr_ref (sequence* s);
#endif /* defined(HAVE_store_addr_ref) */

#if defined(HAVE_store_addr_short)
extern void HAVE_store_addr_short (sequence* s);
#endif /* defined(HAVE_store_addr_short) */

#if defined(HAVE_store_byte)
extern void HAVE_store_byte (sequence* s);
#endif /* defined(HAVE_store_byte) */

#if defined(HAVE_store_char)
extern void HAVE_store_char (sequence* s);
#endif /* defined(HAVE_store_char) */

#if defined(HAVE_store_const_offset_byte)
extern void HAVE_store_const_offset_byte (sequence* s);
#endif /* defined(HAVE_store_const_offset_byte) */

#if defined(HAVE_store_const_offset_int)
extern void HAVE_store_const_offset_int (sequence* s);
#endif /* defined(HAVE_store_const_offset_int) */

#if defined(HAVE_store_const_offset_scaled_byte)
extern void HAVE_store_const_offset_scaled_byte (sequence* s);
#endif /* defined(HAVE_store_const_offset_scaled_byte) */

#if defined(HAVE_store_double)
extern void HAVE_store_double (sequence* s);
#endif /* defined(HAVE_store_double) */

#if defined(HAVE_store_float)
extern void HAVE_store_float (sequence* s);
#endif /* defined(HAVE_store_float) */

#if defined(HAVE_store_int)
extern void HAVE_store_int (sequence* s);
#endif /* defined(HAVE_store_int) */

#if defined(HAVE_store_long)
extern void HAVE_store_long (sequence* s);
#endif /* defined(HAVE_store_long) */

#if defined(HAVE_store_offset_byte)
extern void HAVE_store_offset_byte (sequence* s);
#endif /* defined(HAVE_store_offset_byte) */

#if defined(HAVE_store_offset_char)
extern void HAVE_store_offset_char (sequence* s);
#endif /* defined(HAVE_store_offset_char) */

#if defined(HAVE_store_offset_int)
extern void HAVE_store_offset_int (sequence* s);
#endif /* defined(HAVE_store_offset_int) */

#if defined(HAVE_store_offset_float)
extern void HAVE_store_offset_float (sequence* s);
#endif /* defined(HAVE_store_offset_float) */

#if defined(HAVE_store_offset_long)
extern void HAVE_store_offset_long (sequence* s);
#endif /* defined(HAVE_store_offset_long) */

#if defined(HAVE_store_offset_double)
extern void HAVE_store_offset_double (sequence* s);
#endif /* defined(HAVE_store_offset_double) */

#if defined(HAVE_store_offset_ref)
extern void HAVE_store_offset_ref (sequence* s);
#endif /* defined(HAVE_store_offset_ref) */

#if defined(HAVE_store_offset_scaled_byte)
extern void HAVE_store_offset_scaled_byte (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_byte) */

#if defined(HAVE_store_offset_scaled_char)
extern void HAVE_store_offset_scaled_char (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_char) */

#if defined(HAVE_store_offset_scaled_double)
extern void HAVE_store_offset_scaled_double (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_double) */

#if defined(HAVE_store_offset_scaled_float)
extern void HAVE_store_offset_scaled_float (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_float) */

#if defined(HAVE_store_offset_scaled_int)
extern void HAVE_store_offset_scaled_int (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_int) */

#if defined(HAVE_store_offset_scaled_long)
extern void HAVE_store_offset_scaled_long (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_long) */

#if defined(HAVE_store_offset_scaled_ref)
extern void HAVE_store_offset_scaled_ref (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_ref) */

#if defined(HAVE_store_offset_scaled_short)
extern void HAVE_store_offset_scaled_short (sequence* s);
#endif /* defined(HAVE_store_offset_scaled_short) */

#if defined(HAVE_store_offset_short)
extern void HAVE_store_offset_short (sequence* s);
#endif /* defined(HAVE_store_offset_short) */

#if defined(HAVE_store_ref)
extern void HAVE_store_ref (sequence* s);
#endif /* defined(HAVE_store_ref) */

#if defined(HAVE_store_short)
extern void HAVE_store_short (sequence* s);
#endif /* defined(HAVE_store_short) */

#if defined(HAVE_sub_double)
extern void HAVE_sub_double (sequence* s);
#endif /* defined(HAVE_sub_double) */

#if defined(HAVE_sub_float)
extern void HAVE_sub_float (sequence* s);
#endif /* defined(HAVE_sub_float) */

#if defined(HAVE_sub_int)
extern void HAVE_sub_int (sequence* s);
#endif /* defined(HAVE_sub_int) */

#if defined(HAVE_sub_int_const)
extern void HAVE_sub_int_const (sequence* s);
#endif /* defined(HAVE_sub_int_const) */

#if defined(HAVE_sub_long)
extern void HAVE_sub_long (sequence* s);
#endif /* defined(HAVE_sub_long) */

#if defined(HAVE_sub_long_const)
extern void HAVE_sub_long_const (sequence* s);
#endif /* defined(HAVE_sub_long_const) */

#if defined(HAVE_sub_ref)
extern void HAVE_sub_ref (sequence* s);
#endif /* defined(HAVE_sub_ref) */

#if defined(HAVE_sub_ref_const)
extern void HAVE_sub_ref_const (sequence* s);
#endif /* defined(HAVE_sub_ref_const) */

#if defined(HAVE_swap_any)
extern void HAVE_swap_any (sequence* s);
#endif /* defined(HAVE_swap_any) */

#if defined(HAVE_swap_int)
extern void HAVE_swap_int (sequence* s);
#endif /* defined(HAVE_swap_int) */

#if defined(HAVE_xor_int)
extern void HAVE_xor_int (sequence* s);
#endif /* defined(HAVE_xor_int) */

#if defined(HAVE_xor_int_const)
extern void HAVE_xor_int_const (sequence* s);
#endif /* defined(HAVE_xor_int_const) */

#if defined(HAVE_xor_long)
extern void HAVE_xor_long (sequence* s);
#endif /* defined(HAVE_xor_long) */

#endif
