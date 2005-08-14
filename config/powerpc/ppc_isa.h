
#ifndef _ppc_isa_h_
#define _ppc_isa_h_

#ifndef ppc_op_debug
#define ppc_op_ctxt 0
#define ppc_op_debug(x) 0
#endif

typedef unsigned long ppc_code_t;

enum {
	PPC_R0,
	PPC_R1,
	PPC_R2,
	PPC_R3,
	PPC_R4,
	PPC_R5,
	PPC_R6,
	PPC_R7,
	PPC_R8,
	PPC_R9,
	PPC_R10,
	PPC_R11,
	PPC_R12,
	PPC_R13,
	PPC_R14,
	PPC_R15,
	PPC_R16,
	PPC_R17,
	PPC_R18,
	PPC_R19,
	PPC_R20,
	PPC_R21,
	PPC_R22,
	PPC_R23,
	PPC_R24,
	PPC_R25,
	PPC_R26,
	PPC_R27,
	PPC_R28,
	PPC_R29,
	PPC_R30,
	PPC_R31,
};

enum {
	PPC_FPR = 31,
	PPC_FPR0,
	PPC_FPR1,
	PPC_FPR2,
	PPC_FPR3,
	PPC_FPR4,
	PPC_FPR5,
	PPC_FPR6,
	PPC_FPR7,
	PPC_FPR8,
	PPC_FPR9,
	PPC_FPR10,
	PPC_FPR11,
	PPC_FPR12,
	PPC_FPR13,
	PPC_FPR14,
	PPC_FPR15,
	PPC_FPR16,
	PPC_FPR17,
	PPC_FPR18,
	PPC_FPR19,
	PPC_FPR20,
	PPC_FPR21,
	PPC_FPR22,
	PPC_FPR23,
	PPC_FPR24,
	PPC_FPR25,
	PPC_FPR26,
	PPC_FPR27,
	PPC_FPR28,
	PPC_FPR29,
	PPC_FPR30,
	PPC_FPR31,
};

#define PPC_RSP PPC_R1

#define PPC_RTOC PPC_R2

#define PPC_RARG0 PPC_R3

#define PPC_RARG1 PPC_R4

#define PPC_RARG2 PPC_R5

#define PPC_RARG3 PPC_R6

#define PPC_RARG4 PPC_R7

#define PPC_RARG5 PPC_R8

#define PPC_RARG6 PPC_R9

#define PPC_RARG7 PPC_R10

#define PPC_RCONST_POOL PPC_R31

#define PPC_RFUNCTION_BASE PPC_R31

#define PPC_RRET PPC_R3

#define PPC_RRETHI PPC_R3

#define PPC_RRETLO PPC_R4

#define PPC_FPRARG0 PPC_FPR1

#define PPC_FPRARG1 PPC_FPR2

#define PPC_FPRARG2 PPC_FPR3

#define PPC_FPRARG3 PPC_FPR4

#define PPC_FPRARG4 PPC_FPR5

#define PPC_FPRARG5 PPC_FPR6

#define PPC_FPRARG6 PPC_FPR7

#define PPC_FPRARG7 PPC_FPR8

#define PPC_FPRARG8 PPC_FPR9

#define PPC_FPRARG9 PPC_FPR10

#define PPC_FPRARG10 PPC_FPR11

#define PPC_FPRARG11 PPC_FPR12

#define PPC_FPRARG12 PPC_FPR13

#define PPC_FPRRET PPC_FPR1

#define PPC_FPRRETHI PPC_FPR1

#define PPC_FPRRETLO PPC_FPR2

#define PPC_BD_OFFSET 2UL
#define PPC_BD_MASK (0x3fffUL << PPC_BD_OFFSET)
#define PPC_SET_BD(x) \
	(((x) & 0x3fff) << PPC_BD_OFFSET)
#define PPC_GET_BD(x) \
	(((x) >> PPC_BD_OFFSET) & 0x3fff)

#define PPC_BI_OFFSET 16UL
#define PPC_BI_MASK (0x1fUL << PPC_BI_OFFSET)
#define PPC_SET_BI(x) \
	(((x) & 0x1f) << PPC_BI_OFFSET)
#define PPC_GET_BI(x) \
	(((x) >> PPC_BI_OFFSET) & 0x1f)

#define PPC_BO_OFFSET 21UL
#define PPC_BO_MASK (0x1fUL << PPC_BO_OFFSET)
#define PPC_SET_BO(x) \
	(((x) & 0x1f) << PPC_BO_OFFSET)
#define PPC_GET_BO(x) \
	(((x) >> PPC_BO_OFFSET) & 0x1f)

#define PPC_CRBA_OFFSET 16UL
#define PPC_CRBA_MASK (0x1fUL << PPC_CRBA_OFFSET)
#define PPC_SET_CRBA(x) \
	(((x) & 0x1f) << PPC_CRBA_OFFSET)
#define PPC_GET_CRBA(x) \
	(((x) >> PPC_CRBA_OFFSET) & 0x1f)

#define PPC_CRBB_OFFSET 11UL
#define PPC_CRBB_MASK (0x1fUL << PPC_CRBB_OFFSET)
#define PPC_SET_CRBB(x) \
	(((x) & 0x1f) << PPC_CRBB_OFFSET)
#define PPC_GET_CRBB(x) \
	(((x) >> PPC_CRBB_OFFSET) & 0x1f)

#define PPC_CRBD_OFFSET 21UL
#define PPC_CRBD_MASK (0x1fUL << PPC_CRBD_OFFSET)
#define PPC_SET_CRBD(x) \
	(((x) & 0x1f) << PPC_CRBD_OFFSET)
#define PPC_GET_CRBD(x) \
	(((x) >> PPC_CRBD_OFFSET) & 0x1f)

#define PPC_CRFD_OFFSET 23UL
#define PPC_CRFD_MASK (0x7UL << PPC_CRFD_OFFSET)
#define PPC_SET_CRFD(x) \
	(((x) & 0x7) << PPC_CRFD_OFFSET)
#define PPC_GET_CRFD(x) \
	(((x) >> PPC_CRFD_OFFSET) & 0x7)

#define PPC_CRFS_OFFSET 18UL
#define PPC_CRFS_MASK (0x7UL << PPC_CRFS_OFFSET)
#define PPC_SET_CRFS(x) \
	(((x) & 0x7) << PPC_CRFS_OFFSET)
#define PPC_GET_CRFS(x) \
	(((x) >> PPC_CRFS_OFFSET) & 0x7)

#define PPC_CRM_OFFSET 12UL
#define PPC_CRM_MASK (0xffUL << PPC_CRM_OFFSET)
#define PPC_SET_CRM(x) \
	(((x) & 0xff) << PPC_CRM_OFFSET)
#define PPC_GET_CRM(x) \
	(((x) >> PPC_CRM_OFFSET) & 0xff)

#define PPC_D_OFFSET 0UL
#define PPC_D_MASK (0xffffUL << PPC_D_OFFSET)
#define PPC_SET_D(x) \
	(((x) & 0xffff) << PPC_D_OFFSET)
#define PPC_GET_D(x) \
	(((x) >> PPC_D_OFFSET) & 0xffff)

#define PPC_FM_OFFSET 17UL
#define PPC_FM_MASK (0xffUL << PPC_FM_OFFSET)
#define PPC_SET_FM(x) \
	(((x) & 0xff) << PPC_FM_OFFSET)
#define PPC_GET_FM(x) \
	(((x) >> PPC_FM_OFFSET) & 0xff)

#define PPC_FRA_OFFSET 16UL
#define PPC_FRA_MASK (0x1fUL << PPC_FRA_OFFSET)
#define PPC_SET_FRA(x) \
	(((x) & 0x1f) << PPC_FRA_OFFSET)
#define PPC_GET_FRA(x) \
	(((x) >> PPC_FRA_OFFSET) & 0x1f)

#define PPC_FRB_OFFSET 11UL
#define PPC_FRB_MASK (0x1fUL << PPC_FRB_OFFSET)
#define PPC_SET_FRB(x) \
	(((x) & 0x1f) << PPC_FRB_OFFSET)
#define PPC_GET_FRB(x) \
	(((x) >> PPC_FRB_OFFSET) & 0x1f)

#define PPC_FRC_OFFSET 6UL
#define PPC_FRC_MASK (0x1fUL << PPC_FRC_OFFSET)
#define PPC_SET_FRC(x) \
	(((x) & 0x1f) << PPC_FRC_OFFSET)
#define PPC_GET_FRC(x) \
	(((x) >> PPC_FRC_OFFSET) & 0x1f)

#define PPC_FRD_OFFSET 21UL
#define PPC_FRD_MASK (0x1fUL << PPC_FRD_OFFSET)
#define PPC_SET_FRD(x) \
	(((x) & 0x1f) << PPC_FRD_OFFSET)
#define PPC_GET_FRD(x) \
	(((x) >> PPC_FRD_OFFSET) & 0x1f)

#define PPC_FRS_OFFSET 21UL
#define PPC_FRS_MASK (0x1fUL << PPC_FRS_OFFSET)
#define PPC_SET_FRS(x) \
	(((x) & 0x1f) << PPC_FRS_OFFSET)
#define PPC_GET_FRS(x) \
	(((x) >> PPC_FRS_OFFSET) & 0x1f)

#define PPC_IMM_OFFSET 12UL
#define PPC_IMM_MASK (0xfUL << PPC_IMM_OFFSET)
#define PPC_SET_IMM(x) \
	(((x) & 0xf) << PPC_IMM_OFFSET)
#define PPC_GET_IMM(x) \
	(((x) >> PPC_IMM_OFFSET) & 0xf)

#define PPC_LI_OFFSET 2UL
#define PPC_LI_MASK (0xffffffUL << PPC_LI_OFFSET)
#define PPC_SET_LI(x) \
	(((x) & 0xffffff) << PPC_LI_OFFSET)
#define PPC_GET_LI(x) \
	(((x) >> PPC_LI_OFFSET) & 0xffffff)

#define PPC_MB_OFFSET 6UL
#define PPC_MB_MASK (0x1fUL << PPC_MB_OFFSET)
#define PPC_SET_MB(x) \
	(((x) & 0x1f) << PPC_MB_OFFSET)
#define PPC_GET_MB(x) \
	(((x) >> PPC_MB_OFFSET) & 0x1f)

#define PPC_ME_OFFSET 1UL
#define PPC_ME_MASK (0x1fUL << PPC_ME_OFFSET)
#define PPC_SET_ME(x) \
	(((x) & 0x1f) << PPC_ME_OFFSET)
#define PPC_GET_ME(x) \
	(((x) >> PPC_ME_OFFSET) & 0x1f)

#define PPC_NB_OFFSET 11UL
#define PPC_NB_MASK (0x1fUL << PPC_NB_OFFSET)
#define PPC_SET_NB(x) \
	(((x) & 0x1f) << PPC_NB_OFFSET)
#define PPC_GET_NB(x) \
	(((x) >> PPC_NB_OFFSET) & 0x1f)

#define PPC_OPCD_OFFSET 26UL
#define PPC_OPCD_MASK (0x3fUL << PPC_OPCD_OFFSET)
#define PPC_SET_OPCD(x) \
	(((x) & 0x3f) << PPC_OPCD_OFFSET)
#define PPC_GET_OPCD(x) \
	(((x) >> PPC_OPCD_OFFSET) & 0x3f)

#define PPC_RA_OFFSET 16UL
#define PPC_RA_MASK (0x1fUL << PPC_RA_OFFSET)
#define PPC_SET_RA(x) \
	(((x) & 0x1f) << PPC_RA_OFFSET)
#define PPC_GET_RA(x) \
	(((x) >> PPC_RA_OFFSET) & 0x1f)

#define PPC_RB_OFFSET 11UL
#define PPC_RB_MASK (0x1fUL << PPC_RB_OFFSET)
#define PPC_SET_RB(x) \
	(((x) & 0x1f) << PPC_RB_OFFSET)
#define PPC_GET_RB(x) \
	(((x) >> PPC_RB_OFFSET) & 0x1f)

#define PPC_RD_OFFSET 21UL
#define PPC_RD_MASK (0x1fUL << PPC_RD_OFFSET)
#define PPC_SET_RD(x) \
	(((x) & 0x1f) << PPC_RD_OFFSET)
#define PPC_GET_RD(x) \
	(((x) >> PPC_RD_OFFSET) & 0x1f)

#define PPC_RS_OFFSET 21UL
#define PPC_RS_MASK (0x1fUL << PPC_RS_OFFSET)
#define PPC_SET_RS(x) \
	(((x) & 0x1f) << PPC_RS_OFFSET)
#define PPC_GET_RS(x) \
	(((x) >> PPC_RS_OFFSET) & 0x1f)

#define PPC_SH_OFFSET 11UL
#define PPC_SH_MASK (0x1fUL << PPC_SH_OFFSET)
#define PPC_SET_SH(x) \
	(((x) & 0x1f) << PPC_SH_OFFSET)
#define PPC_GET_SH(x) \
	(((x) >> PPC_SH_OFFSET) & 0x1f)

#define PPC_SIMM_OFFSET 0UL
#define PPC_SIMM_MASK (0xffffUL << PPC_SIMM_OFFSET)
#define PPC_SET_SIMM(x) \
	(((x) & 0xffff) << PPC_SIMM_OFFSET)
#define PPC_GET_SIMM(x) \
	(((x) >> PPC_SIMM_OFFSET) & 0xffff)

#define PPC_SR_OFFSET 16UL
#define PPC_SR_MASK (0xfUL << PPC_SR_OFFSET)
#define PPC_SET_SR(x) \
	(((x) & 0xf) << PPC_SR_OFFSET)
#define PPC_GET_SR(x) \
	(((x) >> PPC_SR_OFFSET) & 0xf)

#define PPC_TO_OFFSET 21UL
#define PPC_TO_MASK (0x1fUL << PPC_TO_OFFSET)
#define PPC_SET_TO(x) \
	(((x) & 0x1f) << PPC_TO_OFFSET)
#define PPC_GET_TO(x) \
	(((x) >> PPC_TO_OFFSET) & 0x1f)

#define PPC_UIMM_OFFSET 0UL
#define PPC_UIMM_MASK (0xffffUL << PPC_UIMM_OFFSET)
#define PPC_SET_UIMM(x) \
	(((x) & 0xffff) << PPC_UIMM_OFFSET)
#define PPC_GET_UIMM(x) \
	(((x) >> PPC_UIMM_OFFSET) & 0xffff)

#define PPC_AA_OFFSET 1UL
#define PPC_OPTION_AA (1L << 1)

#define PPC_L_OFFSET 21UL
#define PPC_OPTION_L (1L << 21)

#define PPC_LK_OFFSET 0UL
#define PPC_OPTION_LK (1L << 0)

#define PPC_OE_OFFSET 10UL
#define PPC_OPTION_OE (1L << 10)

#define PPC_RC_OFFSET 0UL
#define PPC_OPTION_RC (1L << 0)

#define PPC_RES31_OFFSET 0UL
#define PPC_OPTION_RES31 (1L << 0)

#define PPC_BO_DECR_NE_FALSE(y) (0x0 | !!(y))

#define PPC_BO_DECR_EQ_FALSE(y) (0x2 | !!(y))

#define PPC_BO_FALSE(y) (0x4 | !!(y))

#define PPC_BO_DECR_NE_TRUE(y) (0x8 | !!(y))

#define PPC_BO_DECR_EQ_TRUE(y) (0xa | !!(y))

#define PPC_BO_TRUE(y) (0xc | !!(y))

#define PPC_BO_DECR_NE(y) (0x10 | !!(y))

#define PPC_BO_DECR_EQ(y) (0x12 | !!(y))

#define PPC_BO_ALWAYS 0x14

#define PPC_BI_LT 0x0

#define PPC_BI_GT 0x1

#define PPC_BI_EQ 0x2

#define PPC_BI_SO 0x3

#define PPC_BI_CR_0 0x0

#define PPC_BI_CR_1 0x4

#define PPC_BI_CR_2 0x8

#define PPC_CR_0 0x0

#define PPC_CR_1 0x1

#define PPC_CR_2 0x2

#define PPC_CR_3 0x3

#define PPC_CR_4 0x4

#define PPC_CR_5 0x5

#define PPC_CR_6 0x6

#define PPC_CR_7 0x7

#define PPC_SPR_XER 0x1

#define PPC_SPR_LR 0x8

#define PPC_SPR_CTR 0x9

// rd = ra + rb
#define ppc_op_add(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:add rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (266UL << 1)))

#define ppc_op_addc(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:addc rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (10UL << 1)))

#define ppc_op_adde(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:adde rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (138UL << 1)))

#define ppc_op_addi(rd, ra, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:addi rd(%d) ra(%d) simm(%d) ", __FUNCTION__, rd, ra, simm)), \
	 (PPC_SET_OPCD(14) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_addic(rd, ra, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:addic rd(%d) ra(%d) simm(%d) ", __FUNCTION__, rd, ra, simm)), \
	 (PPC_SET_OPCD(12) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_addis(rd, ra, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:addis rd(%d) ra(%d) simm(%d) ", __FUNCTION__, rd, ra, simm)), \
	 (PPC_SET_OPCD(15) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_addme(rd, ra) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:addme rd(%d) ra(%d) ", __FUNCTION__, rd, ra)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  (0UL << 11) | \
	  (234UL << 1)))

#define ppc_op_addze(rd, ra) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:addze rd(%d) ra(%d) ", __FUNCTION__, rd, ra)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  (0UL << 11) | \
	  (202UL << 1)))

#define ppc_op_and(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:and ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (28UL << 1)))

#define ppc_op_andc(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:andc ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (60UL << 1)))

#define ppc_op_andi(ra, rs, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:andi ra(%d) rs(%d) uimm(%d) ", __FUNCTION__, ra, rs, uimm)), \
	 (PPC_SET_OPCD(28) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_UIMM(uimm)))

#define ppc_op_andis(ra, rs, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:andis ra(%d) rs(%d) uimm(%d) ", __FUNCTION__, ra, rs, uimm)), \
	 (PPC_SET_OPCD(29) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_UIMM(uimm)))

#define ppc_op_b(li) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:b li(%d) ", __FUNCTION__, li)), \
	 (PPC_SET_OPCD(18) | \
	  PPC_SET_LI(li)))

#define ppc_op_bc(bo, bi, bd) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:bc bo(%d) bi(%d) bd(%d) ", __FUNCTION__, bo, bi, bd)), \
	 (PPC_SET_OPCD(16) | \
	  PPC_SET_BO(bo) | \
	  PPC_SET_BI(bi) | \
	  PPC_SET_BD(bd)))

#define ppc_op_bcctr(bo, bi) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:bcctr bo(%d) bi(%d) ", __FUNCTION__, bo, bi)), \
	 (PPC_SET_OPCD(19) | \
	  PPC_SET_BO(bo) | \
	  PPC_SET_BI(bi) | \
	  (528UL << 1)))

#define ppc_op_bctr() \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:bctr ", __FUNCTION__)), \
	 (PPC_SET_OPCD(19) | \
	  PPC_SET_BO(20) | \
	  PPC_SET_BI(0) | \
	  (528UL << 1)))

#define ppc_op_bctrl() \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:bctrl ", __FUNCTION__)), \
	 (PPC_SET_OPCD(19) | \
	  PPC_SET_BO(20) | \
	  PPC_SET_BI(0) | \
	  (528UL << 1) | \
	  PPC_OPTION_LK))

#define ppc_op_bclr(bo, bi) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:bclr bo(%d) bi(%d) ", __FUNCTION__, bo, bi)), \
	 (PPC_SET_OPCD(19) | \
	  PPC_SET_BO(bo) | \
	  PPC_SET_BI(bi) | \
	  (16UL << 1)))

#define ppc_op_blr() \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:blr ", __FUNCTION__)), \
	 (PPC_SET_OPCD(19) | \
	  PPC_SET_BO(20) | \
	  PPC_SET_BI(0) | \
	  (16UL << 1)))

#define ppc_op_cmp(crfd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:cmp crfd(%d) ra(%d) rb(%d) ", __FUNCTION__, crfd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_CRFD(crfd) | \
	  (0UL << 22) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  0))

#define ppc_op_cmpi(crfd, ra, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:cmpi crfd(%d) ra(%d) simm(%d) ", __FUNCTION__, crfd, ra, simm)), \
	 (PPC_SET_OPCD(11) | \
	  PPC_SET_CRFD(crfd) | \
	  (0UL << 22) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_cmpl(crfd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:cmpl crfd(%d) ra(%d) rb(%d) ", __FUNCTION__, crfd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_CRFD(crfd) | \
	  (0UL << 22) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (32UL << 1)))

#define ppc_op_cmpli(crfd, ra, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:cmpli crfd(%d) ra(%d) uimm(%d) ", __FUNCTION__, crfd, ra, uimm)), \
	 (PPC_SET_OPCD(10) | \
	  PPC_SET_CRFD(crfd) | \
	  (0UL << 22) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_UIMM(uimm)))

#define ppc_op_divw(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:divw rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (491UL << 1)))

#define ppc_op_divwu(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:divwu rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (459UL << 1)))

#define ppc_op_fabs(frd, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fabs frd(%d) frb(%d) ", __FUNCTION__, frd, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  (0UL << 16) | \
	  PPC_SET_FRB(frb) | \
	  (264UL << 1)))

#define ppc_op_fadd(frd, fra, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fadd frd(%d) fra(%d) frb(%d) ", __FUNCTION__, frd, fra, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  PPC_SET_FRB(frb) | \
	  (21UL << 1)))

#define ppc_op_fadds(frd, fra, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fadds frd(%d) fra(%d) frb(%d) ", __FUNCTION__, frd, fra, frb)), \
	 (PPC_SET_OPCD(59) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  PPC_SET_FRB(frb) | \
	  (21UL << 1)))

#define ppc_op_fctiw(frd, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fctiw frd(%d) frb(%d) ", __FUNCTION__, frd, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  (0UL << 16) | \
	  PPC_SET_FRB(frb) | \
	  (14UL << 1)))

#define ppc_op_fctiwz(frd, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fctiwz frd(%d) frb(%d) ", __FUNCTION__, frd, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  (0UL << 16) | \
	  PPC_SET_FRB(frb) | \
	  (15UL << 1)))

#define ppc_op_fdiv(frd, fra, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fdiv frd(%d) fra(%d) frb(%d) ", __FUNCTION__, frd, fra, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  PPC_SET_FRB(frb) | \
	  (18UL << 1)))

#define ppc_op_fdivs(frd, fra, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fdivs frd(%d) fra(%d) frb(%d) ", __FUNCTION__, frd, fra, frb)), \
	 (PPC_SET_OPCD(59) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  PPC_SET_FRB(frb) | \
	  (18UL << 1)))

#define ppc_op_fmul(frd, fra, frc) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fmul frd(%d) fra(%d) frc(%d) ", __FUNCTION__, frd, fra, frc)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  (0UL << 11) | \
	  PPC_SET_FRC(frc) | \
	  (25UL << 1)))

#define ppc_op_fmuls(frd, fra, frc) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fmuls frd(%d) fra(%d) frc(%d) ", __FUNCTION__, frd, fra, frc)), \
	 (PPC_SET_OPCD(59) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  (0UL << 11) | \
	  PPC_SET_FRC(frc) | \
	  (25UL << 1)))

#define ppc_op_fneg(frd, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fneg frd(%d) frb(%d) ", __FUNCTION__, frd, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  (0UL << 16) | \
	  PPC_SET_FRB(frb) | \
	  (40UL << 1)))

#define ppc_op_frsp(frd, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:frsp frd(%d) frb(%d) ", __FUNCTION__, frd, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  (0UL << 16) | \
	  PPC_SET_FRB(frb) | \
	  (12UL << 1)))

#define ppc_op_fsub(frd, fra, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fsub frd(%d) fra(%d) frb(%d) ", __FUNCTION__, frd, fra, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  PPC_SET_FRB(frb) | \
	  (20UL << 1)))

#define ppc_op_fsubs(frd, fra, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fsubs frd(%d) fra(%d) frb(%d) ", __FUNCTION__, frd, fra, frb)), \
	 (PPC_SET_OPCD(59) | \
	  PPC_SET_FRD(frd) | \
	  PPC_SET_FRA(fra) | \
	  PPC_SET_FRB(frb) | \
	  (20UL << 1)))

#define ppc_op_fmr(frd, frb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:fmr frd(%d) frb(%d) ", __FUNCTION__, frd, frb)), \
	 (PPC_SET_OPCD(63) | \
	  PPC_SET_FRD(frd) | \
	  (0UL << 16) | \
	  PPC_SET_FRB(frb) | \
	  (72UL << 1)))

#define ppc_op_lbz(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lbz rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(34) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lbzu(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lbzu rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(35) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lbzux(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lbzux rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (119UL << 1)))

#define ppc_op_lbzx(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lbzx rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (87UL << 1)))

#define ppc_op_lfd(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfd rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(50) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lfdu(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfdu rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(51) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lfdux(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfdux rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (631UL << 1)))

#define ppc_op_lfdx(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfdx rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (599UL << 1)))

#define ppc_op_lfs(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfs rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(48) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lfsu(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfsu rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(49) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lfsux(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfsux rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (567UL << 1)))

#define ppc_op_lfsx(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lfsx rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (535UL << 1)))

#define ppc_op_lha(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lha rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(42) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lhau(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhau rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(43) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lhaux(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhaux rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (375UL << 1)))

#define ppc_op_lhax(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhax rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (343UL << 1)))

#define ppc_op_lhbrx(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhbrx rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (790UL << 1)))

#define ppc_op_lhz(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhz rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(40) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lhzu(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhzu rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(40) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lhzux(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhzux rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (311UL << 1)))

#define ppc_op_lhzx(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lhzx rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (279UL << 1)))

#define ppc_op_lmw(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lmw rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(46) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lwz(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lwz rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(32) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lwzu(rd, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lwzu rd(%d) ra(%d) d(%d) ", __FUNCTION__, rd, ra, d)), \
	 (PPC_SET_OPCD(33) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_lwzux(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lwzux rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (55UL << 1)))

#define ppc_op_lwzx(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lwzx rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (23UL << 1)))

#define ppc_op_mfspr(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mfspr rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (339UL << 1)))

#define ppc_op_mflr(rd) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mflr rd(%d) ", __FUNCTION__, rd)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(8) | \
	  PPC_SET_RB(0) | \
	  (339UL << 1)))

#define ppc_op_mr(ra, rs) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mr ra(%d) rs(%d) ", __FUNCTION__, ra, rs)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rs) | \
	  (444UL << 1)))

#define ppc_op_mtspr(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mtspr rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (467UL << 1)))

#define ppc_op_mtctr(rs) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mtctr rs(%d) ", __FUNCTION__, rs)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(9) | \
	  PPC_SET_RB(0) | \
	  (467UL << 1)))

#define ppc_op_mtlr(rs) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mtlr rs(%d) ", __FUNCTION__, rs)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(8) | \
	  PPC_SET_RB(0) | \
	  (467UL << 1)))

#define ppc_op_mulhw(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mulhw rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (75UL << 1)))

#define ppc_op_mulhwu(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mulhwu rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (11UL << 1)))

#define ppc_op_mulli(rd, ra, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mulli rd(%d) ra(%d) simm(%d) ", __FUNCTION__, rd, ra, simm)), \
	 (PPC_SET_OPCD(7) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_mullw(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:mullw rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (235UL << 1)))

#define ppc_op_neg(rd, ra) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:neg rd(%d) ra(%d) ", __FUNCTION__, rd, ra)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  (104UL << 1)))

#define ppc_op_nop() \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:nop ", __FUNCTION__)), \
	 (PPC_SET_OPCD(24) | \
	  PPC_SET_RA(PPC_R0) | \
	  PPC_SET_RS(PPC_R0) | \
	  0))

#define ppc_op_or(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:or ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (444UL << 1)))

#define ppc_op_orc(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:orc ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (412UL << 1)))

#define ppc_op_ori(ra, rs, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:ori ra(%d) rs(%d) uimm(%d) ", __FUNCTION__, ra, rs, uimm)), \
	 (PPC_SET_OPCD(24) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_UIMM(uimm)))

#define ppc_op_oris(ra, rs, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:oris ra(%d) rs(%d) uimm(%d) ", __FUNCTION__, ra, rs, uimm)), \
	 (PPC_SET_OPCD(24) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_UIMM(uimm)))

#define ppc_op_rlwimi(ra, rs, sh, mb, me) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:rlwimi ra(%d) rs(%d) sh(%d) mb(%d) me(%d) ", __FUNCTION__, ra, rs, sh, mb, me)), \
	 (PPC_SET_OPCD(20) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_SH(sh) | \
	  PPC_SET_MB(mb) | \
	  PPC_SET_ME(me)))

#define ppc_op_rlwinm(ra, rs, sh, mb, me) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:rlwinm ra(%d) rs(%d) sh(%d) mb(%d) me(%d) ", __FUNCTION__, ra, rs, sh, mb, me)), \
	 (PPC_SET_OPCD(21) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_SH(sh) | \
	  PPC_SET_MB(mb) | \
	  PPC_SET_ME(me)))

#define ppc_op_slwi(ra, rs, sh) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:slwi ra(%d) rs(%d) sh(%d) ", __FUNCTION__, ra, rs, sh)), \
	 (PPC_SET_OPCD(21) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_SH(sh) | \
	  PPC_SET_MB(0) | \
	  PPC_SET_ME(31-sh)))

#define ppc_op_slw(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:slw ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (24UL << 1)))

#define ppc_op_sraw(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:sraw ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (792UL << 1)))

#define ppc_op_srawi(ra, rs, sh) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:srawi ra(%d) rs(%d) sh(%d) ", __FUNCTION__, ra, rs, sh)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_SH(sh) | \
	  (824UL << 1)))

#define ppc_op_srw(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:srw ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (536UL << 1)))

#define ppc_op_stb(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stb rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(38) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stbu(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stbu rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(39) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stbux(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stbux rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (247UL << 1)))

#define ppc_op_stbx(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stbx rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (215UL << 1)))

#define ppc_op_stfd(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfd rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(54) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stfdu(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfdu rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(55) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stfdux(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfdux rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (759UL << 1)))

#define ppc_op_stfdx(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfdx rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (727UL << 1)))

#define ppc_op_stfs(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfs rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(52) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stfsu(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfsu rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(53) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stfsux(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfsux rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (695UL << 1)))

#define ppc_op_stfsx(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stfsx rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (663UL << 1)))

#define ppc_op_sth(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:sth rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(44) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_sthbrx(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:sthbrx rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (918UL << 1)))

#define ppc_op_sthu(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:sthu rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(45) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_sthux(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:sthux rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (439UL << 1)))

#define ppc_op_sthx(rs, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:sthx rs(%d) ra(%d) rb(%d) ", __FUNCTION__, rs, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (407UL << 1)))

#define ppc_op_stmw(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stmw rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(47) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stw(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stw rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(36) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_stwu(rs, ra, d) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:stwu rs(%d) ra(%d) d(%d) ", __FUNCTION__, rs, ra, d)), \
	 (PPC_SET_OPCD(37) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_D(d)))

#define ppc_op_subf(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:subf rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (40UL << 1)))

#define ppc_op_subfc(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:subfc rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (8UL << 1)))

#define ppc_op_subfe(rd, ra, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:subfe rd(%d) ra(%d) rb(%d) ", __FUNCTION__, rd, ra, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RB(rb) | \
	  (136UL << 1)))

#define ppc_op_subfic(rd, ra, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:subfic rd(%d) ra(%d) simm(%d) ", __FUNCTION__, rd, ra, simm)), \
	 (PPC_SET_OPCD(8) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_subfme(rd, ra) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:subfme rd(%d) ra(%d) ", __FUNCTION__, rd, ra)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  (0UL << 11) | \
	  (232UL << 1)))

#define ppc_op_subfze(rd, ra) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:subfze rd(%d) ra(%d) ", __FUNCTION__, rd, ra)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(ra) | \
	  (0UL << 11) | \
	  (200UL << 1)))

#define ppc_op_li(rd, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:li rd(%d) simm(%d) ", __FUNCTION__, rd, simm)), \
	 (PPC_SET_OPCD(14) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(PPC_R0) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_lis(rd, simm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:lis rd(%d) simm(%d) ", __FUNCTION__, rd, simm)), \
	 (PPC_SET_OPCD(15) | \
	  PPC_SET_RD(rd) | \
	  PPC_SET_RA(PPC_R0) | \
	  PPC_SET_SIMM(simm)))

#define ppc_op_xor(ra, rs, rb) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:xor ra(%d) rs(%d) rb(%d) ", __FUNCTION__, ra, rs, rb)), \
	 (PPC_SET_OPCD(31) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_RB(rb) | \
	  (316UL << 1)))

#define ppc_op_xori(ra, rs, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:xori ra(%d) rs(%d) uimm(%d) ", __FUNCTION__, ra, rs, uimm)), \
	 (PPC_SET_OPCD(26) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_UIMM(uimm)))

#define ppc_op_xoris(ra, rs, uimm) \
	((void)ppc_op_debug((ppc_op_ctxt, "%s:xoris ra(%d) rs(%d) uimm(%d) ", __FUNCTION__, ra, rs, uimm)), \
	 (PPC_SET_OPCD(27) | \
	  PPC_SET_RA(ra) | \
	  PPC_SET_RS(rs) | \
	  PPC_SET_UIMM(uimm)))

#define REGISTER_SET							\
  { /* r0 */	0, 0, Reserved,	        0, 0, 0, 0  },			\
  { /* r1 */	0, 0, Reserved,	        0, 0, 1, 0  },			\
  { /* r2 */	0, 0, Reserved,	        0, 0, 2, 0  },			\
  { /* r3 */	0, 0, Rint|Rref,        0, 0, 3, 0  },			\
  { /* r4 */	0, 0, Rint|Rref,        0, 0, 4, 0  },			\
  { /* r5 */	0, 0, Rint|Rref,        0, 0, 5, 0  },			\
  { /* r6 */	0, 0, Rint|Rref,        0, 0, 6, 0  },			\
  { /* r7 */	0, 0, Rint|Rref,        0, 0, 7, 0  },			\
  { /* r8 */	0, 0, Rint|Rref,        0, 0, 8, 0  },			\
  { /* r9 */	0, 0, Rint|Rref,        0, 0, 9, 0  },			\
  { /* r10 */	0, 0, Rint|Rref,        0, 0, 10, 0  },			\
  { /* r11 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 11, 0  }, \
  { /* r12 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 12, 0  }, \
  { /* r13 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 13, 0  }, \
  { /* r14 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 14, 0  }, \
  { /* r15 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 15, 0  }, \
  { /* r16 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 16, 0  }, \
  { /* r17 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 17, 0  }, \
  { /* r18 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 18, 0  }, \
  { /* r19 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 19, 0  }, \
  { /* r20 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 20, 0  }, \
  { /* r21 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 21, 0  }, \
  { /* r22 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 22, 0  }, \
  { /* r23 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 23, 0  }, \
  { /* r24 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 24, 0  }, \
  { /* r25 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 25, 0  }, \
  { /* r26 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 26, 0  }, \
  { /* r27 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 27, 0  }, \
  { /* r28 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 28, 0  }, \
  { /* r29 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 29, 0  }, \
  { /* r30 */	0, 0, Rint|Rref,        Rglobal|Rnosaveoncall, 0, 30, 0  }, \
  { /* r31 */	0, 0, Reserved,	        0, 0, 31, 0  },			\
  { /* fpr0 */	0, 0, Reserved,	        0, 0, 32, 0  },			\
  { /* fpr1 */	0, 0, Rfloat|Rdouble,	0, 0, 33, 0  },			\
  { /* fpr2 */	0, 0, Rfloat|Rdouble,	0, 0, 34, 0  },			\
  { /* fpr3 */	0, 0, Rfloat|Rdouble,	0, 0, 35, 0  },			\
  { /* fpr4 */	0, 0, Rfloat|Rdouble,	0, 0, 36, 0  },			\
  { /* fpr5 */	0, 0, Rfloat|Rdouble,	0, 0, 37, 0  },			\
  { /* fpr6 */	0, 0, Rfloat|Rdouble,	0, 0, 38, 0  },			\
  { /* fpr7 */	0, 0, Rfloat|Rdouble,	0, 0, 39, 0  },			\
  { /* fpr8 */	0, 0, Rfloat|Rdouble,	0, 0, 40, 0  },			\
  { /* fpr9 */	0, 0, Rfloat|Rdouble,	0, 0, 41, 0  },			\
  { /* fpr10 */	0, 0, Rfloat|Rdouble,	0, 0, 42, 0  },			\
  { /* fpr11 */	0, 0, Rfloat|Rdouble,	0, 0, 43, 0  },			\
  { /* fpr12 */	0, 0, Rfloat|Rdouble,	0, 0, 44, 0  },			\
  { /* fpr13 */	0, 0, Rfloat|Rdouble,	0, 0, 45, 0  },			\
  { /* fpr14 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 46, 0  },	\
  { /* fpr15 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 47, 0  },	\
  { /* fpr16 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 48, 0  },	\
  { /* fpr17 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 49, 0  },	\
  { /* fpr18 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 50, 0  },	\
  { /* fpr19 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 51, 0  },	\
  { /* fpr20 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 52, 0  },	\
  { /* fpr21 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 53, 0  },	\
  { /* fpr22 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 54, 0  },	\
  { /* fpr23 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 55, 0  },	\
  { /* fpr24 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 56, 0  },	\
  { /* fpr25 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 57, 0  },	\
  { /* fpr26 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 58, 0  },	\
  { /* fpr27 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 59, 0  },	\
  { /* fpr28 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 60, 0  },	\
  { /* fpr29 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 61, 0  },	\
  { /* fpr30 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 62, 0  },	\
  { /* fpr31 */	0, 0, Rfloat|Rdouble,	Rnosaveoncall, 0, 63, 0  },	\

#define NR_REGISTERS 64

#endif
