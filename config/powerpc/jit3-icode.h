/* ppc/jit-icode.h
 * Define the instructions which are present on the PPC.
 *
 * Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_JANOSVM_LICENSE@
 */

#ifndef __icode_h
#define __icode_h

/*
 * Size of long's compared to references.
 */
#define pusharg_long_idx_inc            2

/*
 * Define the range checking macros.
 */
#define	__anyvalue_rangecheck(v)	(1)

//
// Shifts
//
#define	__U5const_rangecheck(v)		((v) >= 0 && (v) < 32)

#define __I26const_rangecheck(x) \
	(((((x) >> 2) & 0xfc000000) == 0xfc000000) || \
	 ((((x) >> 2) & 0xfc000000) == 0))

#define __S16const_rangecheck(v) ((-32768 <= (v)) && ((v) <= 32767))

#define HAVE_ashr_int_const_rangecheck(x) __U5const_rangecheck(x)
#define HAVE_lshr_int_const_rangecheck(x) __U5const_rangecheck(x)

/*
 * These must be defined for any architecture.
 */
#define	HAVE_spill_int			spill_Rxx
#define	HAVE_reload_int			reload_Rxx
#define	HAVE_spill_ref			spill_Rxx
#define	HAVE_reload_ref			reload_Rxx

#define	HAVE_move_register_int		movereg_RR
#define	HAVE_move_register_ref		movereg_RR
#define	HAVE_move_register_float	fmovereg_RR
#define	HAVE_move_register_double	fmovereg_RR

#define	HAVE_prologue			prologue_xxx
#define	HAVE_epilogue			epilogue_xxx
#define	HAVE_exception_prologue		eprologue_xxx

/*#define	HAVE_move_int_const		move_RxC*/
#define	HAVE_move_int_const_incode	move_RxC
/*#define	HAVE_move_ref_const		move_RxC*/

#define	HAVE_move_int_const_rangecheck(v)	__moveable_constant_rangecheck(v)
#define	HAVE_move_int_const_incode_rangecheck(v)	__anyvalue_rangecheck(v)
#define	HAVE_move_ref_const_rangecheck(v)	__moveable_constant_rangecheck((int)v)

#define	HAVE_move_int			move_RxR

#define	HAVE_move_label_const		move_RxL

#define	HAVE_load_constpool_ref		ld_RxL
#define	HAVE_load_constpool_int		ld_RxL

#define	HAVE_move_ref			move_RxR
#define	HAVE_move_any			move_RxR

#define HAVE_neg_int			neg_RxR
#define	HAVE_add_int			add_RRR
#define	HAVE_sub_int			sub_RRR
#define	HAVE_mul_int			mul_RRR
#define	HAVE_div_int			div_RRR
#undef	HAVE_rem_int
#define	HAVE_and_int			and_RRR
#define	HAVE_or_int			or_RRR
#define	HAVE_xor_int			xor_RRR
#define	HAVE_ashr_int			ashr_RRR
#define	HAVE_lshr_int			lshr_RRR
#define	HAVE_lshl_int			lshl_RRR
#define	HAVE_ashr_int_const		ashr_RRC
#define	HAVE_lshr_int_const		lshr_RRC

#define	HAVE_load_int			load_RxR
#define	HAVE_store_int			store_xRR
#define	HAVE_load_ref			load_RxR
#define	HAVE_store_ref			store_xRR

#define	HAVE_pusharg_int		push_xRC
#define	HAVE_pusharg_ref		push_xRC
#define	HAVE_popargs			popargs_xxC

#define	HAVE_cmp_int			cmp_xRR
#define	HAVE_cmpl_int			cmpl_xRR
// #define HAVE_cmp_offset_int		cmpl_xRRC

//
// Ref's are just int's
//
#define	HAVE_cmp_ref		cmp_xRR
#define	HAVE_add_ref		add_RRR
#define	HAVE_add_ref_const	add_RRC
#define	HAVE_cmp_ref_const	cmp_xRC


#ifdef I_THINK_THESE_DO_NOT_WORK_ALTHOUGH_I_DEFINED_THE_CODE_CORRECTLY
#  define	HAVE_add_ref_const	addref_RRC
#  define	HAVE_cmp_ref_const	cmpref_xRC
#endif


#define HAVE_branch			branch_xCC
#define HAVE_branch_and_link		branchl_xCC

//
// It's problematic to implement call_ref using relative branches.
//
#define	HAVE_call_ref			call_xCC
#define	HAVE_call_ref_rangecheck(v)	__I26const_rangecheck(v)
// #undef HAVE_call_ref

#define	HAVE_call			call_xRC
#define	HAVE_branch_indirect		branch_indirect_xRC
#define	HAVE_return_int			return_Rxx
#define	HAVE_return_long		returnl_Rxx
#define	HAVE_return_ref			return_Rxx
#define	HAVE_returnarg_int		returnarg_xxR
#define	HAVE_returnarg_long		returnargl_xxR
#define	HAVE_returnarg_ref		returnarg_xxR

#define	HAVE_set_label			set_label_xxC
#define	HAVE_build_key			set_word_xxC
#define	HAVE_build_code_ref		set_wordpc_xxC
#undef	HAVE_build_const_ref

/*
 * These are sometimes optional (if long operators are defined)
 */
#define	HAVE_adc_int			adc_RRR
#define	HAVE_sbc_int			sbc_RRR

/*
 * These are optional but help to optimise the code generated.
 */
#define	HAVE_add_int_const	add_RRC
#define	HAVE_cmp_int_const	cmp_xRC
#define	HAVE_sub_int_const	sub_RRC
#define	HAVE_and_int_const	and_RRC


#define	HAVE_load_offset_int	load_RRC
#define	HAVE_store_offset_int	store_RRC
#define	HAVE_load_offset_ref	load_RRC
#define	HAVE_store_offset_ref	store_RRC

#define HAVE_load_offset_int_rangecheck(v)  __S16const_rangecheck(v)
#define HAVE_store_offset_int_rangecheck(v) __S16const_rangecheck(v)
#define HAVE_load_offset_ref_rangecheck(v)  __S16const_rangecheck(v)
#define HAVE_store_offset_ref_rangecheck(v) __S16const_rangecheck(v)

#define	HAVE_lshl_int_const	lshl_RRC
#define HAVE_lshl_int_const_rangecheck(v)	__U5const_rangecheck(v)


#undef	HAVE_pusharg_int_const


//
// We handle a signed value in the add/sub form
// by flipping the operator
//
#define	HAVE_add_int_const_rangecheck(v)	__S16const_rangecheck(v)
#define	HAVE_sub_int_const_rangecheck(v)	__S16const_rangecheck(v)
#define	HAVE_add_ref_const_rangecheck(v)	HAVE_add_int_const_rangecheck(v)

#define	HAVE_cmp_int_const_rangecheck(v)	__S16const_rangecheck(v)
#define	HAVE_cmp_ref_const_rangecheck(v)	__S16const_rangecheck(v)
#define	HAVE_pusharg_int_const_rangecheck(v)	__S16const_rangecheck(v)
#define	HAVE_and_int_const_rangecheck(v)	__S16const_rangecheck(v)

#define	HAVE_load_byte			loadb_RxR
#define	HAVE_load_char			loadc_RxR
#define	HAVE_load_short			loads_RxR
#define	HAVE_store_byte			storeb_xRR
#define	HAVE_store_char			stores_xRR
#define	HAVE_store_short		stores_xRR

#define HAVE_load_offset_byte	loadb_RRC
#define HAVE_load_offset_byte_rangecheck(x) __S16const_rangecheck(x)
#define HAVE_load_offset_short	loads_RRC
#define HAVE_load_offset_short_rangecheck(x) __S16const_rangecheck(x)
#define HAVE_load_offset_char	loads_RRC
#define HAVE_load_offset_char_rangecheck(x) __S16const_rangecheck(x)

#define HAVE_store_offset_byte	storeb_RRC
#define HAVE_store_offset_byte_rangecheck(x) __S16const_rangecheck(x)
#define HAVE_store_offset_short	stores_RRC
#define HAVE_store_offset_short_rangecheck(x) __S16const_rangecheck(x)
#define HAVE_store_offset_char	stores_RRC
#define HAVE_store_offset_char_rangecheck(x) __S16const_rangecheck(x)

#define HAVE_load_offset_float	fload_RRC
#define HAVE_load_offset_float_rangecheck(x) __S16const_rangecheck(x)
#define HAVE_load_offset_double	floadl_RRC
#define HAVE_load_offset_double_rangecheck(x) __S16const_rangecheck(x)

#define HAVE_store_offset_float	fstore_RRC
#define HAVE_store_offset_float_rangecheck(x) __S16const_rangecheck(x)
#define HAVE_store_offset_double	fstorel_RRC
#define HAVE_store_offset_double_rangecheck(x) __S16const_rangecheck(x)

/*
 * These are optional if the architecture supports them.
 */
#undef	HAVE_mul_int_const
#undef	HAVE_swap_int

#undef	HAVE_move_long_const
#undef	HAVE_move_long

#undef	HAVE_add_long
#undef	HAVE_sub_long
#undef	HAVE_mul_long
#undef	HAVE_div_long
#undef	HAVE_rem_long
#undef	HAVE_neg_long
#undef	HAVE_and_long
#undef	HAVE_or_long
#undef	HAVE_xor_long
#undef	HAVE_ashr_long
#undef	HAVE_lshl_long
#undef	HAVE_lshr_long

#undef	HAVE_load_long
#undef	HAVE_store_long

#undef	HAVE_pusharg_long

#undef	HAVE_cmp_long

#undef	HAVE_cvt_int_long
#undef	HAVE_cvt_int_byte
#undef	HAVE_cvt_int_char
#undef	HAVE_cvt_int_short
#undef	HAVE_cvt_long_int

#undef	HAVE_call_indirect_const

#define	HAVE_fakecall			fakecall_xCC

#if defined(STACK_LIMIT)
#define	HAVE_check_stack_limit_constpool check_stack_limit_xRC
#endif

#if !defined(HAVE_NO_FLOATING_POINT)
/*
 * We only include these operations if we actually support floating point.
 */

#define __floatconst_rangecheck(v)	0

#define __builtin_FP_values(v)	( (v) == 0 || (v) == 1.0 || (v) == 2.0 || (v) == 3.0 || (v) == 4.0 || (v) == 5.0 || (v) == 0.5 || (v) == 10)

#define __doubleconst_rangecheck(v)	0

#define	HAVE_spill_float		fspill_Rxx
#define	HAVE_reload_float		freload_Rxx
#define	HAVE_spill_double		fspilll_Rxx
#define	HAVE_reload_double		freloadl_Rxx

#define	HAVE_move_float			fmove_RxR
#define	HAVE_move_double		fmovel_RxR

#define	HAVE_load_float			fload_RxR
#define	HAVE_store_float		fstore_RxR
#define	HAVE_load_double		floadl_RxR
#define	HAVE_store_double		fstorel_RxR

#define	HAVE_pusharg_float		fpush_xRC
#define	HAVE_pusharg_double		fpushl_xRC

#define	HAVE_return_float		freturn_Rxx
#define	HAVE_return_double		freturnl_Rxx
#define	HAVE_returnarg_float		freturnarg_xxR
#define	HAVE_returnarg_double		freturnargl_xxR

#define	HAVE_load_constpool_float	fld_RxL
#define	HAVE_load_constpool_double	fldl_RxL

#if 0
#define	HAVE_move_float_const_rangecheck(v)	__builtin_FP_values(v)
#define	HAVE_move_double_const_rangecheck(v)	__builtin_FP_values(v)
#endif

#define	HAVE_add_float			fadd_RRR
#define	HAVE_sub_float			fsub_RRR
#define	HAVE_mul_float			fmul_RRR
#define HAVE_div_float			fdiv_RRR

#define	HAVE_add_double			faddl_RRR
#define	HAVE_sub_double			fsubl_RRR
#define	HAVE_mul_double			fmull_RRR
#define	HAVE_div_double			fdivl_RRR

#undef	HAVE_cvt_float_int
#undef	HAVE_cvt_double_int
#define	HAVE_cvt_float_int_ieee		cvtfi_RxR
#define	HAVE_cvt_double_int_ieee	cvtdi_RxR

#define	HAVE_cvt_int_float		cvtif_RxR
#define	HAVE_cvt_int_double		cvtid_RxR

#undef	HAVE_cvt_long_double
#undef	HAVE_cvt_long_float
#undef	HAVE_cvt_float_long
#undef	HAVE_cvt_double_long

#define	HAVE_cvt_float_double		cvtfd_RxR
#define	HAVE_cvt_double_float		cvtdf_RxR

#undef	HAVE_cmpg_float
#undef	HAVE_cmpg_double
#undef	HAVE_cmpl_float
#undef	HAVE_cmpl_double

#define	HAVE_neg_float			fneg_RxR
#define	HAVE_neg_double			fnegl_RxR
#undef	HAVE_rem_float
#undef	HAVE_rem_double

#endif

#endif
