/* mips/jit3-icode.h
 * Define the instructions which are present on the MIPS.
 *
 * Copyright (c) 1996 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Christian Krusel <chrkr@uni-paderborn.de>, 1997.
 * last changed: 19.3.97
 */

#ifndef __mips_jit_icode_h
#define __mips_jit_icode_h

/*
 * Size of longs compared to refs.
 */
#define	pusharg_long_idx_inc	2

/*
 * Define the range checking macros.
 */
#define	__intconst_rangecheck(v)	((v) >= -32768  && (v) <= 32767)
#define	__uintconst_rangecheck(v)	((v) >= 0  && (v) <= 65535)

/*
 * These must be defined for any architecture.
 */
#define	HAVE_spill_int			spill_Rxx
#define	HAVE_reload_int			reload_Rxx
#define	HAVE_spill_ref			spill_Rxx
#define	HAVE_reload_ref			reload_Rxx
#define	HAVE_spill_float		fspill_Rxx
#define	HAVE_reload_float		freload_Rxx
#define	HAVE_spill_double		fspilll_Rxx
#define	HAVE_reload_double		freloadl_Rxx

#define	HAVE_move_register_int		movereg_RR
#define	HAVE_move_register_ref		movereg_RR

#define	HAVE_prologue			prologue_xxC
#define	HAVE_epilogue			epilogue_xxx
#define HAVE_exception_prologue		eprologue_xxx

#define	HAVE_check_stack_limit_constpool check_stack_limit_xRC

#define	HAVE_move_int_const_rangecheck(v)        (1)

#define	HAVE_move_int_const		move_RxC
#define	HAVE_move_int			move_RxR
#define	HAVE_move_float			fmove_RxR
#ifdef PS2LINUX
#undef	HAVE_move_double
#else
#define	HAVE_move_double		fmovel_RxR
#endif
#define	HAVE_move_label_const		move_RxL
#define	HAVE_move_ref			move_RxR
#define	HAVE_move_any			move_RxR

#if defined(USE_JIT_CONSTANT_POOL)
/*
 * Load constant from the constant pool directly, without intermediate
 * register.
 */
#define	HAVE_load_constpool_ref		ld_RxL
#define	HAVE_load_constpool_int		ld_RxL
#endif

#define	HAVE_add_int			add_RRR
#define	HAVE_sub_int			sub_RRR
#define	HAVE_and_int			and_RRR
#define	HAVE_or_int			or_RRR
#define	HAVE_xor_int			xor_RRR
#define	HAVE_nor_int			nor_RRR /* new */
#define	HAVE_ashr_int			ashr_RRR
#define	HAVE_lshr_int			lshr_RRR
#define	HAVE_lshl_int			lshl_RRR

#define	HAVE_add_float			fadd_RRR
#define	HAVE_sub_float			fsub_RRR
#define	HAVE_mul_float			fmul_RRR
#undef	HAVE_div_float

#ifdef PS2LINUX
#undef	HAVE_add_double
#undef	HAVE_sub_double
#undef	HAVE_mul_double
#else
#define	HAVE_add_double			faddl_RRR
#define	HAVE_sub_double			fsubl_RRR
#define	HAVE_mul_double			fmull_RRR
#endif
#undef	HAVE_div_double

#define	HAVE_add_ref			addu_RRR /* new */
#define	HAVE_sub_ref			subu_RRR /* new */

#define	HAVE_load_int			load_RRx
#define	HAVE_store_int			store_RRx
#define	HAVE_load_ref			load_RRx
#define	HAVE_store_ref			store_RRx

#define	HAVE_load_float			fload_RRx
#define	HAVE_store_float		fstore_RRx
#ifdef PS2LINUX
#undef	HAVE_load_double
#undef	HAVE_store_double
#else
#define	HAVE_load_double		floadl_RRx
#define	HAVE_store_double		fstorel_RRx
#endif

#define	HAVE_pusharg_int		push_xRC
#define	HAVE_pusharg_float		fpush_xRC
#ifdef PS2LINUX
#undef	HAVE_pusharg_double
#else
#define	HAVE_pusharg_double		fpushl_xRC
#endif
#define	HAVE_pusharg_ref		push_xRC
#define	HAVE_pusharg_long		pushl_xRC
#define	HAVE_popargs			popargs_xxC

#undef	HAVE_cmp_int
#undef	HAVE_cmp_int_const
#undef	HAVE_cmp_ref
#undef	HAVE_cmp_ref_const

#define	HAVE_branch			branch_xCC
#define	HAVE_branch_indirect		branch_indirect_xRC
#define	HAVE_call_ref			call_xCC
#if defined(USE_JIT_CONSTANT_POOL)
#undef	HAVE_call_ref
#endif
#define	HAVE_call			call_xRC
#define	HAVE_return_int			return_Rxx
#define	HAVE_return_long		returnl_Rxx
#define	HAVE_return_float		freturn_Rxx
#ifdef PS2LINUX
#undef	HAVE_return_double
#else
#define	HAVE_return_double		freturnl_Rxx
#endif
#define	HAVE_return_ref			return_Rxx
#define	HAVE_returnarg_int		returnarg_xxR
#define	HAVE_returnarg_long		returnargl_xxR
#define	HAVE_returnarg_float		freturnarg_xxR
#ifdef PS2LINUX
#undef	HAVE_returnarg_double
#else
#define	HAVE_returnarg_double		freturnargl_xxR
#endif
#define	HAVE_returnarg_ref		returnarg_xxR

#define	HAVE_set_label			set_label_xxC
#define	HAVE_build_key			set_word_xxC
#define	HAVE_build_code_ref		set_wordpc_xxC

#undef	HAVE_cvt_int_double
#undef	HAVE_cvt_int_float
/* MIPS cannot handle the NaN conditions so we do these by hand */
#undef	HAVE_cvt_double_float
#undef	HAVE_cvt_float_double
#undef	HAVE_cvt_float_int
#undef	HAVE_cvt_double_int

/*
 * These are sometimes optional (if long operators are defined)
 */
#undef	HAVE_adc_int			/* no equiv */
#undef	HAVE_sbc_int			/* no equiv */
#undef 	HAVE_ngc_int			/* no equiv */

/*
 * These are optional but help to optimise the code generated.
 */
#define HAVE_add_int_const		add_RRC
#define HAVE_add_ref_const		addu_RRC /* new */
#define HAVE_sub_int_const		sub_RRC 
#undef  HAVE_cmp_int_const		/* no */
#undef  HAVE_cmp_ref_const		/* no */
#define HAVE_load_offset_int		load_RRC
#define HAVE_load_offset_ref		load_RRC
#define HAVE_store_offset_int		store_RRC
#define HAVE_store_offset_ref		store_RRC
#define	HAVE_lshl_int_const		lshl_RRC

#define	HAVE_add_int_const_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_add_ref_const_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_sub_int_const_rangecheck(v)        ((v) >= -32767  && (v) <= 32768) /*swapped -67,68*/
#undef	HAVE_cmp_int_const_rangecheck
#define	HAVE_load_offset_int_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_load_offset_ref_rangecheck(v)	__intconst_rangecheck(v) /* new */
#define	HAVE_store_offset_int_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_store_offset_ref_rangecheck(v)	__intconst_rangecheck(v) /* new */
#define	HAVE_lshl_int_const_rangecheck(v)	((v) > 0  && (v) < 31)   /*__intconst_rangecheck(v) */

#define	HAVE_load_byte			loadb_RRx
#define	HAVE_load_char			loadc_RRx
#define	HAVE_load_short			loads_RRx
#define	HAVE_store_byte			storeb_RRx
#define	HAVE_store_char			stores_RRx
#define	HAVE_store_short		stores_RRx

#define HAVE_set_lt_int			sltu_RRR
#define HAVE_set_lt_int_const		sltu_RRC
#define HAVE_cbranch_int		cbranch_RRC
#define HAVE_cbranch_int_const		cbranch_RRCC
#define HAVE_cbranch_int_const_rangecheck(v)	1
#define HAVE_cbranch_ref		cbranch_RRC
#define HAVE_cbranch_ref_const		cbranch_RRCC
#define HAVE_cbranch_ref_const_rangecheck(v)	1

/*
 * These are optional if the architecture supports them.
 */

#undef	HAVE_move_float_const		/* no */
#undef	HAVE_move_double_const		/* no */


#define	HAVE_and_int_const_rangecheck(v)	__uintconst_rangecheck(v)
#define	HAVE_ashr_int_const_rangecheck(v)	((v) > 0  && (v) < 31)
#define	HAVE_lshr_int_const_rangecheck(v)	((v) > 0  && (v) < 31)

#undef	HAVE_move_ref_const		/* future implement */
#undef	HAVE_move_ref_const_rangecheck	/* future implement */

#undef	HAVE_move_long_const		/* no */
#undef	HAVE_move_float_const		/* no */
#undef	HAVE_move_double_const		/* no */

#undef	HAVE_swap_int			/* no */
#undef	HAVE_swap_any			/* no */
#define	HAVE_neg_int			neg_RRR
#define	HAVE_mul_int                    mul_RRR
#define	HAVE_div_int                    div_RRR
#define	HAVE_rem_int                    rem_RRR
#undef	HAVE_mul_int_const		/* no */
#define	HAVE_and_int_const              and_RRC
#define	HAVE_ashr_int_const             ashr_RRC
#define	HAVE_lshr_int_const             lshr_RRC

#undef	HAVE_cmpg_float			/* c.cond.fmt */
#undef	HAVE_cmpg_double		/* c.cond.fmt */
#undef	HAVE_cmpl_float			/* c.cond.fmt */
#undef	HAVE_cmpl_double		/* c.cond.fmt */

#undef	HAVE_move_long_const		/* 64-bit only */
#undef	HAVE_move_long			/* 64-bit only */

#undef	HAVE_add_long			/* 64-bit only */
#undef	HAVE_sub_long			/* 64-bit only */
#undef	HAVE_mul_long			/* 64-bit only */
#undef	HAVE_div_long			/* 64-bit only */
#undef	HAVE_rem_long			/* 64-bit only */
#undef	HAVE_neg_long			/* 64-bit only */
#undef	HAVE_and_long			/* 64-bit only */
#undef	HAVE_or_long			/* 64-bit only */
#undef	HAVE_xor_long			/* 64-bit only */
#undef	HAVE_ashr_long			/* 64-bit only */
#undef	HAVE_lshl_long			/* 64-bit only */
#undef	HAVE_lshr_long			/* 64-bit only */
#undef	HAVE_add_long_const		/* 64-bit only */
#undef	HAVE_sub_long_const		/* 64-bit only */
#undef	HAVE_and_long_const		/* 64-bit only */
#undef	HAVE_lshl_long_const		/* 64-bit only */

#undef	HAVE_load_long			/* 64-bit only */
#undef	HAVE_store_long			/* 64-bit only */
#undef	HAVE_load_offset_long		/* 64-bit only */
#undef	HAVE_store_offset_long		/* 64-bit only */

#undef	HAVE_pusharg_int_const		/* no */

#undef	HAVE_cmp_long			/* no */

#define	HAVE_neg_float			fneg_RRR
#ifdef PS2LINUX
#undef	HAVE_neg_double	
#else
#define	HAVE_neg_double			fnegl_RRR
#endif
#undef	HAVE_rem_float			/* no */
#undef	HAVE_rem_double			/* no */

#undef	HAVE_cvt_long_double		/* implement future */
#undef	HAVE_cvt_float_long		/* implement future */
#undef	HAVE_cvt_double_long		/* implement future */
#undef	HAVE_cvt_long_float		/* implement future */
#undef	HAVE_cvt_int_long		/* 64-bit only */
#undef	HAVE_cvt_int_byte		/* no */
#undef	HAVE_cvt_int_char		/* no */
#undef	HAVE_cvt_int_short		/* no */
#undef	HAVE_cvt_long_int		/* no */

#define HAVE_push_frame			push_frame_xxx
#define HAVE_pop_frame			pop_frame_xxx
#define HAVE_get_arg_ptr		get_arg_ptr_R

#define	HAVE_fakecall_constpool		fakecall_xCC

#endif
