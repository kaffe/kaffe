/* m68k/jit-icode.h
 * Define the instructions which are present on the m68k.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_icode_h
#define __m68k_icode_h

/* Since it won't be set for us by virtue of having one of the above set... */
#define pusharg_long_idx_inc		2

/*
 * Define the range checking macros.
 */
#define m68k_s16_rangecheck(v)		((v) >= -0x8000 && (v) < 0x8000)
#define m68k_1_8_rangecheck(v)		((v) >= 1 && (v) <= 8)

/*
 * These must be defined for any architecture.
 */
#define HAVE_prologue			prologue_xxC
#define HAVE_epilogue			epilogue_xxx
#define HAVE_exception_prologue		eprologue_xxx

#define	HAVE_check_stack_limit		check_stack_limit_xRC

#define HAVE_spill_int			spilli_RCx
#define HAVE_reload_int			reloadi_RCx

#define HAVE_move_int_const		movei_RxC
#define HAVE_move_int			movei_RxR
#define HAVE_move_label_const		mover_RxL

#define HAVE_move_int_const_rangecheck(v)	(1)

#define HAVE_add_int			addi_RRR
#define HAVE_sub_int			subi_RRR
#define HAVE_mul_int			muli_RRR
#define HAVE_div_int			divi_RRR
#define HAVE_rem_int			remi_RRR
#define HAVE_and_int			andi_RRR
#define HAVE_or_int			ori_RRR
#define HAVE_xor_int			xori_RRR
#define HAVE_ashr_int			ashri_RRR
#define HAVE_lshr_int			lshri_RRR
#define HAVE_lshl_int			lshli_RRR

#define HAVE_load_int			loadi_RxR
#define HAVE_store_int			storei_xRR

#define HAVE_pusharg_int		pushi_xRC
#define HAVE_popargs			popargs_xxC

#define HAVE_cmp_int			cmpi_xRR

#define HAVE_branch			branch_xCC
#define HAVE_branch_indirect		branch_indirect_xRC
#define HAVE_call_ref			call_xCC
#define HAVE_call			call_xRC

#define HAVE_return_int			returni_Rxx
#define HAVE_return_long		returnl_Rxx

#define HAVE_returnarg_int		returnargi_xxR
#define HAVE_returnarg_long		returnargl_xxR

#define HAVE_set_label			set_label_xxC
#define HAVE_build_key			set_word_xxC
#define HAVE_build_code_ref		set_wordpc_xxC
#undef	HAVE_build_const_ref

/*
 * These are required since we distinguish between refs and ints.
 */
#define HAVE_spill_ref			spillr_RCx
#define HAVE_reload_ref			reloadr_RCx

#define HAVE_move_ref_const		mover_RxC
#define HAVE_move_ref			mover_RxR
#define HAVE_move_any			movea_RxR

#define HAVE_move_ref_const_rangecheck(v)	(1)

#define HAVE_add_ref			addr_RRR
#define HAVE_cmp_ref			cmpr_xRR

#define HAVE_load_ref			loadr_RxR
#define HAVE_store_ref			storer_xRR
#define HAVE_pusharg_ref		pushr_xRC
#define HAVE_return_ref			returnr_Rxx
#define HAVE_returnarg_ref		returnargr_xxR

/*
 * These are sometimes optional (if long operators are defined)
 */
#define HAVE_adc_int			adci_RRR
#define HAVE_sbc_int			sbci_RRR
#define HAVE_ngc_int			ngci_RxR

/*
 * These are optional but help to optimise the code generated.
 */
#define HAVE_add_int_const		addi_RRC
#define HAVE_sub_int_const		subi_RRC
#define HAVE_cmp_int_const		cmpi_xRC
#define HAVE_pusharg_int_const		pushi_xCC
#define HAVE_load_offset_int		loadi_RRC
#define HAVE_store_offset_int		storei_xRRC
#define HAVE_lshl_int_const		lshli_RRC

#define HAVE_add_int_const_rangecheck(v)	(1)
#define HAVE_sub_int_const_rangecheck(v)	(1)
#define HAVE_cmp_int_const_rangecheck(v)	(1)
#define HAVE_pusharg_int_const_rangecheck(v)	(1)
#define HAVE_load_offset_int_rangecheck(v)	m68k_s16_rangecheck(v)
#define HAVE_store_offset_int_rangecheck(v)	m68k_s16_rangecheck(v)
#define HAVE_lshl_int_const_rangecheck(v)	m68k_1_8_rangecheck(v)

#define HAVE_add_ref_const		addr_RRC
#define HAVE_sub_ref_const		subr_RRC
#undef	HAVE_cmp_ref_const
#define HAVE_pusharg_ref_const		pushr_xCC
#define HAVE_load_offset_ref		loadr_RRC
#define HAVE_store_offset_ref		storer_xRRC

#define HAVE_add_ref_const_rangecheck(v)	(1)
#define HAVE_sub_ref_const_rangecheck(v)	(1)
#undef	HAVE_cmp_ref_const_rangecheck(v)
#define HAVE_pusharg_ref_const_rangecheck(v)	(1)
#define HAVE_load_offset_ref_rangecheck(v)	m68k_s16_rangecheck(v)
#define HAVE_store_offset_ref_rangecheck(v)	m68k_s16_rangecheck(v)

#define HAVE_load_byte			loadb_RxR
#define HAVE_load_char			loadc_RxR
#define HAVE_load_short			loads_RxR
#define HAVE_store_byte			storeb_xRR
#define HAVE_store_char			stores_xRR
#define HAVE_store_short		stores_xRR

/*
 * These are optional if the architecture supports them.
 */
#if !defined(HAVE_NO_SWAP_ANY)
#define HAVE_swap_any			swap_RxR
#endif

#if !defined(HAVE_NO_MULINTCONST)
#define HAVE_mul_int_const		muli_RRC
#endif
#define HAVE_neg_int			negi_RxR
#define HAVE_and_int_const		andi_RRC
#define HAVE_ashr_int_const		ashri_RRC
#define HAVE_lshr_int_const		lshri_RRC

#define HAVE_mul_int_const_rangecheck(v)	(1)
#define HAVE_and_int_const_rangecheck(v)	(1)
#define HAVE_ashr_int_const_rangecheck(v)	m68k_1_8_rangecheck(v)
#define HAVE_lshr_int_const_rangecheck(v)	m68k_1_8_rangecheck(v)

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

#undef	HAVE_lcmp

#undef	HAVE_cvt_int_long
#define HAVE_cvt_int_byte		cvtib_RxR
#undef	HAVE_cvt_int_char
#undef	HAVE_cvt_int_short		cvtis_RxR
#undef	HAVE_cvt_long_int

#undef	HAVE_get_arg_ptr

#if !defined(HAVE_NO_FLOATING_POINT)

#define m68k_floatconst_rangecheck(v)	0

#define HAVE_spill_float		spillf_RCx
#define HAVE_reload_float		reloadf_RCx
#define HAVE_spill_double		spilld_RCx
#define HAVE_reload_double		reloadd_RCx

#define HAVE_move_float			movef_RxR
#define HAVE_move_double		moved_RxR

#define HAVE_add_float			addf_RRR
#define HAVE_sub_float			subf_RRR
#define HAVE_mul_float			mulf_RRR
#define HAVE_div_float			divf_RRR

#define HAVE_add_double			addd_RRR
#define HAVE_sub_double			subd_RRR
#define HAVE_mul_double			muld_RRR
#define HAVE_div_double			divd_RRR

#define HAVE_load_float			loadf_RxR
#define HAVE_store_float		storef_xRR
#define HAVE_load_double		loadd_RxR
#define HAVE_store_double		stored_xRR

#define HAVE_pusharg_float		pushf_xRC
#define HAVE_pusharg_double		pushd_xRC

#define HAVE_return_float		returnf_Rxx
#define HAVE_return_double		returnd_Rxx
#define HAVE_returnarg_float		returnargf_xxR
#define HAVE_returnarg_double		returnargd_xxR

#define HAVE_cvt_int_double		cvtid_RxR
#define HAVE_cvt_int_float		cvtif_RxR
#define HAVE_cvt_float_double		cvtfd_RxR
#define HAVE_cvt_double_float		cvtdf_RxR

#undef	HAVE_cmpg_float
#undef	HAVE_cmpg_double
#undef	HAVE_cmpl_float
#undef	HAVE_cmpl_double

#define HAVE_neg_float			negf_RxR
#define HAVE_neg_double			negd_RxR
#define HAVE_rem_float			remf_RRR
#define HAVE_rem_double			remd_RRR

#endif

#endif
