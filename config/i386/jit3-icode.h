#include "i386/jit-icode.h"

#define	HAVE_check_stack_limit		check_stack_limit_xRC

#define	HAVE_spill_ref			HAVE_spill_int
#define	HAVE_reload_ref			HAVE_reload_int

#define HAVE_mul_int_const              mul_RRC
#define HAVE_and_int_const              and_RRC
#define HAVE_or_int_const               or_RRC
#define HAVE_xor_int_const              xor_RRC
#define HAVE_ashr_int_const             ashr_RRC
#define HAVE_lshr_int_const             lshr_RRC
#define HAVE_neg_int                    neg_RxR

#define HAVE_mul_int_const_rangecheck(v)        __intconst_rangecheck(v)
#define HAVE_and_int_const_rangecheck(v)        __intconst_rangecheck(v)
#define HAVE_or_int_const_rangecheck(v)         __intconst_rangecheck(v)
#define HAVE_xor_int_const_rangecheck(v)        __intconst_rangecheck(v)
#define HAVE_ashr_int_const_rangecheck(v)       __intshiftconst_rangecheck(v)
#define HAVE_lshr_int_const_rangecheck(v)       __intshiftconst_rangecheck(v)

#define	HAVE_load_offset_scaled_int	load_RRRC
#define	HAVE_load_offset_scaled_ref	load_RRRC
#undef	HAVE_load_offset_scaled_long
#undef	HAVE_load_offset_scaled_float
#undef	HAVE_load_offset_scaled_double
#define	HAVE_load_offset_scaled_byte	loadb_RRRC
#define	HAVE_load_offset_scaled_char	loadc_RRRC
#undef	HAVE_load_offset_scaled_short

#define	HAVE_store_offset_scaled_int	store_RRRC
#define	HAVE_store_offset_scaled_ref	store_RRRC
#undef	HAVE_store_offset_scaled_long
#undef	HAVE_store_offset_scaled_float
#undef	HAVE_store_offset_scaled_double
#define	HAVE_store_offset_scaled_byte	storeb_RRRC
#define	HAVE_store_offset_scaled_char	stores_RRRC
#define	HAVE_store_offset_scaled_short	stores_RRRC

#define	HAVE_store_offset_byte_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_store_offset_char_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_store_offset_short_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_store_offset_byte		storeb_xRRC
#define	HAVE_store_offset_char		stores_xRRC
#define	HAVE_store_offset_short		stores_xRRC

#define	HAVE_store_const_offset_byte_rangecheck(c) __intconst_rangecheck(v)
#define	HAVE_store_const_offset_int_rangecheck(c) __intconst_rangecheck(v)
#define	HAVE_store_const_offset_byte	storeb_xRCC
#define	HAVE_store_const_offset_int	store_xRCC

#define	HAVE_store_const_offset_scaled_byte	storeb_RRCC


#define	HAVE_load_addr_int		load_RxA
#define	HAVE_load_addr_ref		load_RxA
#undef	HAVE_load_addr_long
#undef	HAVE_load_addr_float
#undef	HAVE_load_addr_double
#undef	HAVE_load_addr_byte
#undef	HAVE_load_addr_char
#undef	HAVE_load_addr_short

#define	HAVE_store_addr_int		store_xRA
#define	HAVE_store_addr_ref		store_xRA
#undef	HAVE_store_addr_long
#undef	HAVE_store_addr_float
#undef	HAVE_store_addr_double
#undef	HAVE_store_addr_byte
#undef	HAVE_store_addr_char
#undef	HAVE_store_addr_short

#define	HAVE_pusharg_ref_const_rangecheck(v)	__intconst_rangecheck(v)
#define	HAVE_pusharg_ref_const			push_xCC

#define	HAVE_fakecall				fakecall_xCC

#define	HAVE_cmp_offset_int			cmp_xRRC
