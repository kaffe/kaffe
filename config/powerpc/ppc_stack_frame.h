/*
 * ppc_stack_frame.h
 *
 * Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_JANOSVM_LICENSE@
 */
#ifndef _ppc_stack_frame_h
#define _ppc_stack_frame_h

typedef unsigned long register_storage_t;

#if defined(__APPLE__)

#define PPC_ARG_REGISTER_COUNT 8
#define PPC_FARG_REGISTER_COUNT 13

/*
 * Apple's PowerPC stack frame.
 *
 * sp - Link to the previous stack frame.
 * cr - The condition code register.
 * lr - The link register.
 * reserved1,reserved2 - OS X uses these with composite arguments.
 * gpr2 - Hmm, don't remember
 * args - Storage for function arguments in GPR3 through GPR10.
 */
typedef struct _ppc_stack_frame_t {
	struct _ppc_stack_frame_t *sp;
	register_storage_t cr;
	ppc_code_t *lr;
	void *reserved1;
	void *reserved2;
	register_storage_t gpr2;
	register_storage_t args[PPC_ARG_REGISTER_COUNT];
	register_storage_t scratch[0];
} ppc_stack_frame_t;
#else

#define PPC_ARG_REGISTER_COUNT 8
#define PPC_FARG_REGISTER_COUNT 8

typedef struct _ppc_stack_frame_t {
	struct _ppc_stack_frame_t *sp;
	ppc_code_t *lr;
	register_storage_t args[PPC_ARG_REGISTER_COUNT];
	register_storage_t scratch[0];
} ppc_stack_frame_t;
#endif

#define PPC_FRAME_FORMAT "sp=%p cr=0x%x lr=%p"
#define PPC_FRAME_FORMAT_ARGS(sf)  (sf)->sp, (sf)->sp->cr, (sf)->sp->lr

/*
 * Exception stack frame for Kaffe.  The Apple calling conventions store the
 * interesting bits in the caller's stack frame so we only need to keep a
 * pointer to it.  This is in contrast to conventions like the x86 where the
 * previous frame pointer and return pc are stored together in the callee's
 * frame.
 *
 * sp - Pointer to the current stack frame.
 */
typedef struct _exceptionFrame {
	ppc_stack_frame_t *sp;
} exceptionFrame;

/* Offset of the previous stack frame link in a ppc stack frame. */
#define PPC_FRAME_SP ((unsigned long)&((ppc_stack_frame_t *)0)->sp)
/* Offset of the link register in a stack frame. */
#define PPC_FRAME_LR ((unsigned long)&((ppc_stack_frame_t *)0)->lr)
/* Offset of the link register in a stack frame. */
#define PPC_FRAME_ARGS ((unsigned long)&((ppc_stack_frame_t *)0)->args)

/*
 * Get the link to the next frame.  We take advantage of the fact that an
 * exceptionFrame is structurally equivalent to the start of a
 * ppc_stack_frame_t.
 */
#define NEXTFRAME(f) ((exceptionFrame *)(((exceptionFrame *)(f))->sp))
/*
 * Get the return pc from an exceptionFrame.  Note that we need to check for
 * a NULL here otherwise we'll segfault when examining the top frame.
 */
#define PCFRAME(f) ((f)->sp == 0 ? 0 : (f)->sp->lr - 1)
/* Get the frame pointer address from an exceptionFrame. */
#define FPFRAME(f) ((unsigned long)(f)->sp)

/* Get the "this" object out of a stack frame. */
#define FRAMEOBJECT(obj, f, einfo) \
	(obj) = (void *)(((exceptionFrame *)(f))->sp->sp->args[0])

#define FRAMEARG(dst, src, index) \
	(dst) = ((src)->sp->args[index])

/*
 * Initialize the exceptionFrame object "f" with the pointer to the previous
 * stack frame.
 */
#define FIRSTFRAME(f, o) \
do { \
	ppc_stack_frame_t *sf; \
\
	sf = __builtin_frame_address(0); \
	(f).sp = sf->sp; \
} while(0)

/* Arguments for the signal handler. */
#define EXCEPTIONPROTO int sig, int code, struct sigcontext *ctx

#endif
