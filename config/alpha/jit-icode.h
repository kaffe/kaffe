/* alpha/jit-icode.h
 * Define the instructions which are present on the Alpha.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_jit_icode_h
#define __alpha_jit_icode_h

 
/*
 * Size of longs compared to refs.
 */
#define	pusharg_long_idx_inc    1

/*
 * Define the range checking macros.
 */

#define	alpha_zero_rangecheck(v)  ((v) == 0)
#define	alpha_u8_rangecheck(v)    ((v) >= 0 && (v) <= 0xFF)
#define alpha_s9_rangecheck(v)    ((v) >= -0xFF && (v) <= 0xFF)
#define	alpha_s16_rangecheck(v)   ((v) >= -0x8000 && (v) < 0x8000)
#define	alpha_s32_rangecheck(v)   ((v) >= -0x80000000L && (v) < 0x80000000L)

extern int alpha_andconst_rangecheck(long v);

/*
 * These must be defined for any architecture.
 */

#define	HAVE_prologue			prologue_xxC
#define	HAVE_epilogue			epilogue_xxx
#define HAVE_exception_prologue		eprologue_xxx

#define	HAVE_spill_int			spilli_RCx
#define	HAVE_spill_float		spillf_RCx
#define	HAVE_spill_double		spilld_RCx
#define	HAVE_reload_int			reloadi_RCx
#define	HAVE_reload_float		reloadf_RCx
#define	HAVE_reload_double		reloadd_RCx

#define	HAVE_move_int_const		movei_RxC
#define	HAVE_move_int			movei_RxR
#define	HAVE_move_float_const		movef_RxC
#define	HAVE_move_float			movef_RxR
#define	HAVE_move_double_const		moved_RxC
#define	HAVE_move_double		moved_RxR
#define	HAVE_move_label_const		move_RxL

#define HAVE_move_int_const_rangecheck(v)	(1)
#define	HAVE_move_float_const_rangecheck(v)	(1)
#define	HAVE_move_double_const_rangecheck(v)	(1)

#define	HAVE_add_int			addi_RRR
#define	HAVE_sub_int			subi_RRR
#define	HAVE_and_int			andi_RRR
#define	HAVE_or_int			ori_RRR
#define	HAVE_xor_int			xori_RRR
#define	HAVE_ashr_int			ashri_RRR
#define	HAVE_lshr_int			lshri_RRR
#define	HAVE_lshl_int			lshli_RRR

#define	HAVE_add_float			addf_RRR
#define	HAVE_sub_float			subf_RRR
#define	HAVE_mul_float			mulf_RRR
#define	HAVE_div_float			divf_RRR

#define	HAVE_add_double			addd_RRR
#define	HAVE_sub_double			subd_RRR
#define	HAVE_mul_double			muld_RRR
#define	HAVE_div_double			divd_RRR

#define	HAVE_load_int			loadi_RxR
#define	HAVE_store_int			storei_xRR

#define	HAVE_load_float			loadf_RxR
#define	HAVE_store_float		storef_xRR
#define	HAVE_load_double		loadd_RxR
#define	HAVE_store_double		stored_xRR

#define	HAVE_pusharg_int		pushi_xRC
#define	HAVE_pusharg_float		pushf_xRC
#define	HAVE_pusharg_double		pushd_xRC
#define	HAVE_popargs			popargs_xxC

#define	HAVE_cbranch_int		cbranchi_xRRLC

#define	HAVE_branch			branch_xCC
#define	HAVE_branch_indirect		branch_indirect_xRC
#define	HAVE_call_ref			call_xCC
#define	HAVE_call			call_xRC

#define	HAVE_return_int			returni_Rxx
#define HAVE_return_ref			returnr_Rxx
#define	HAVE_return_long		returnl_Rxx
#define	HAVE_return_float		returnf_Rxx
#define	HAVE_return_double		returnd_Rxx

#define	HAVE_returnarg_int		returnargi_xxR
#define HAVE_returnarg_ref		returnargr_xxR
#define	HAVE_returnarg_long		returnargl_xxR
#define	HAVE_returnarg_float		returnargf_xxR
#define	HAVE_returnarg_double		returnargd_xxR

#define	HAVE_set_label			set_label_xxC
#define	HAVE_build_key			set_word_xxC
#define	HAVE_build_code_ref		set_wordpc_xxC
#define HAVE_load_code_ref		loadpc_RxR

#define	HAVE_cvt_int_double		cvtid_RxR
#define	HAVE_cvt_int_float		cvtif_RxR
#define	HAVE_cvt_float_double		cvtfd_RxR
#define	HAVE_cvt_double_float		cvtdf_RxR


/*
 * These must be defined for 64-bit architectures.
 */

#define HAVE_spill_ref			spillr_RCx
#define HAVE_spill_long			spilll_RCx
#define HAVE_reload_ref			reloadr_RCx
#define HAVE_reload_long		reloadl_RCx

#define HAVE_move_ref_const		mover_RxC
#define HAVE_move_long_const		movel_RxC
#define HAVE_move_ref			mover_RxR
#define HAVE_move_long			movel_RxR
#define	HAVE_move_any			mover_RxR

#define HAVE_move_ref_const_rangecheck(v)	(1)
#define HAVE_move_long_const_rangecheck(v)	(1)

#define HAVE_add_ref			addr_RRR
#define HAVE_add_long			addl_RRR

#define HAVE_load_ref			loadr_RxR
#define HAVE_load_long			loadl_RxR
#define HAVE_store_ref			storer_xRR
#define HAVE_store_long			storel_xRR

#define HAVE_pusharg_ref		pushr_xRC
#define HAVE_pusharg_long		pushl_xRC

#define HAVE_cbranch_ref		cbranchr_xRRLC

/*
 * This one is needed for proper exception handling if you can't
 * backtrace C code.
 */

#define HAVE_call_soft			call_soft_xCC

/*
 * These are optional but help to optimise the code generated.
 */

#define HAVE_add_int_const		addi_RRC
#define HAVE_sub_int_const		subi_RRC
#define HAVE_pusharg_int_const		pushi_xCC
#define HAVE_load_offset_int		loadi_RRC
#define HAVE_store_offset_int		storei_xRRC
#define	HAVE_lshl_int_const		lshli_RRC
#define	HAVE_cbranch_int_const		cbranchi_xRCLC

#define	HAVE_add_int_const_rangecheck(v)	alpha_s9_rangecheck(v)
#define	HAVE_sub_int_const_rangecheck(v)	alpha_s9_rangecheck(v)
#define HAVE_pusharg_int_const_rangecheck(v)	(1)
#define	HAVE_load_offset_int_rangecheck(v)	alpha_s16_rangecheck(v)
#define	HAVE_store_offset_int_rangecheck(v)	alpha_s16_rangecheck(v)
#define HAVE_lshl_int_const_rangecheck(v)	(1)
#define HAVE_cbranch_int_const_rangecheck(v)	alpha_s9_rangecheck(v)

#define	HAVE_load_byte			loadb_RxR
#define	HAVE_load_char			loadc_RxR
#define	HAVE_load_short			loads_RxR
#define	HAVE_store_byte			storeb_xRR
#define	HAVE_store_char			stores_xRR
#define	HAVE_store_short		stores_xRR

/*
 * These are optional if the architecture supports them.
 */

#define	HAVE_mul_int_const		muli_RRC
#define	HAVE_mul_int			muli_RRR
#define HAVE_div_int			divi_RRR
#define HAVE_rem_int			remi_RRR
#define	HAVE_neg_int			negi_RxR
#define	HAVE_and_int_const		andi_RRC
#define	HAVE_ashr_int_const		ashri_RRC
#define	HAVE_lshr_int_const		lshri_RRC

#define HAVE_mul_int_const_rangecheck(v)	alpha_u8_rangecheck(v)
#define HAVE_and_int_const_rangecheck(v)	alpha_andconst_rangecheck(v)
#define HAVE_ashr_int_const_rangecheck(v)	(1)
#define HAVE_lshr_int_const_rangecheck(v)	(1)

#define HAVE_add_ref_const		addr_RRC
#define HAVE_pusharg_ref_const		pushr_xCC

#define	HAVE_add_ref_const_rangecheck(v)	alpha_s16_rangecheck(v)
#define HAVE_pusharg_ref_const_rangecheck(v)	(1)

#define HAVE_add_long_const		addl_RRC
#define HAVE_sub_long_const		subl_RRC
#define	HAVE_sub_long			subl_RRR
#define	HAVE_mul_long_const		mull_RRC
#define	HAVE_mul_long			mull_RRR
#define HAVE_div_long			divl_RRR
#define HAVE_rem_long			reml_RRR
#define	HAVE_neg_long			negl_RxR
#define	HAVE_and_long_const		andl_RRC
#define	HAVE_and_long			andl_RRR
#define	HAVE_or_long			orl_RRR
#define	HAVE_xor_long			xorl_RRR
#define	HAVE_ashr_long_const		ashrl_RRC
#define	HAVE_ashr_long			ashrl_RRR
#define	HAVE_lshr_long_const		lshrl_RRC
#define	HAVE_lshr_long			lshrl_RRR
#define	HAVE_lshl_long_const		lshll_RRC
#define	HAVE_lshl_long			lshll_RRR

#define	HAVE_add_long_const_rangecheck(v)	alpha_s16_rangecheck(v)
#define	HAVE_sub_long_const_rangecheck(v)	alpha_s16_rangecheck(-(v))
#define HAVE_pusharg_long_const_rangecheck(v)	(1)
#define HAVE_mul_long_const_rangecheck(v)	alpha_u8_rangecheck(v)
#define HAVE_and_long_const_rangecheck(v)	alpha_andconst_rangecheck(v)
#define HAVE_ashr_long_const_rangecheck(v)	(1)
#define HAVE_lshr_long_const_rangecheck(v)	(1)
#define HAVE_lshl_long_const_rangecheck(v)	(1)

#define HAVE_load_offset_ref		loadr_RRC
#define HAVE_load_offset_long		loadl_RRC
#define HAVE_store_offset_ref		storer_xRRC
#define HAVE_store_offset_long		storel_xRRC
#define HAVE_cbranch_ref_const		cbranchr_xRCLC

#define	HAVE_load_offset_ref_rangecheck(v)	alpha_s16_rangecheck(v)
#define	HAVE_load_offset_long_rangecheck(v)	alpha_s16_rangecheck(v)
#define	HAVE_store_offset_ref_rangecheck(v)	alpha_s16_rangecheck(v)
#define	HAVE_store_offset_long_rangecheck(v)	alpha_s16_rangecheck(v)
#define HAVE_cbranch_ref_const_rangecheck(v)	alpha_s16_rangecheck(v)

#undef	HAVE_swap_int
#undef	HAVE_swap_long

#define	HAVE_lcmp			lcmp_RRR

#define	HAVE_neg_float			negf_RxR
#define	HAVE_neg_double			negd_RxR

#undef	HAVE_rem_float
#undef	HAVE_rem_double

#define HAVE_cvt_int_byte		cvtib_RxR
#define HAVE_cvt_int_short		cvtis_RxR
#define	HAVE_cvt_int_long		cvtil_RxR
#define	HAVE_cvt_long_int		cvtli_RxR
#define	HAVE_cvt_long_double		cvtld_RxR
#define	HAVE_cvt_long_float		cvtlf_RxR


#if 0
/* These functions does not work correctly, use soft_cvtXX */

#define	HAVE_cvt_float_int		cvtfi_RxR
#define	HAVE_cvt_double_int		cvtdi_RxR
#define	HAVE_cvt_float_long		cvtfl_RxR
#define	HAVE_cvt_double_long		cvtdl_RxR

#else

#undef HAVE_cvt_float_int
#undef HAVE_cvt_float_int_ieee
#undef HAVE_cvt_double_int
#undef HAVE_cvt_double_int_ieee
#undef HAVE_cvt_float_long
#undef HAVE_cvt_double_long

#endif

#endif
