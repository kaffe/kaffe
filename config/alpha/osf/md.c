/*
 * alpha/osf/md.c
 * OSF/1 Alpha specific functions.
 *
 * Copyright (c) 1999, 2000, 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 1999, 2000, 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <machine/fpu.h>
#include <sys/sysinfo.h>
#include <machine/hal_sysinfo.h>
#include <sys/proc.h>

#include <assert.h>

#include "md.h"

#ifndef FPCR_INED
#define FPCR_INED 0
#endif
#ifndef FPCR_UNFD
#define FPCR_UNFD 0
#endif
#ifndef FPCR_OVFD
#define FPCR_OVFD 0
#endif
#ifndef FPCR_DZED
#define FPCR_DZED 0
#endif

#if defined(USE_LIBEXC)
#include "locks.h"
/* libexc is not thread safe :-( */
static iStaticLock excLock;
#endif

void
init_md(void)
{
	/* `Alpha Architecture Handbook' say that's user mode _must
           not_ change fpcr but use OS function as next one.  */
#if 0
	/* Set the bits in the hw fpcr for cpu's that implement
	   all the bits.  */
	__asm__ __volatile__(
		"excb\n\t"
		"mt_fpcr %0\n\t"
		"excb"
		: : "f"(FPCR_INED | FPCR_UNFD | FPCR_DYN_NORMAL
			| FPCR_OVFD | FPCR_DZED));
#endif

#if 0 /* This breaks DoublePrint and DoubleComp tests.  */
	/* Set the software emulation bits in the kernel for
	   those that don't.  */
	ieee_set_fp_control(IEEE_TRAP_ENABLE_INV);
#endif

	initStaticLock(&excLock);
}


/* This bit turns off unaligned access fixups in favour of SIGBUS.  It
   is not called by Kaffe, but it is useful to invoke from within
   the debugger.  */

void alpha_disable_uac(void)
{
  int buf[2];

  buf[0] = SSIN_UACPROC;
  buf[1] = UAC_SIGBUS;
  setsysinfo(SSI_NVPAIRS, buf, 1, 0, 0);
}


#if defined(TRANSLATOR)
/* Native support for exception */

#if defined(USE_LIBEXC)
#include <pdsc.h>
#include <excpt.h>

#include "debug.h"

void __alpha_osf_firstFrame (exceptionFrame *frame)
{

	/* Retreive caller frame as current one will be invalidate
           after function exit. */
	lockStaticMutex (&excLock);
	exc_capture_context (&frame->sc);
	exc_virtual_unwind (NULL, &frame->sc);
	unlockStaticMutex (&excLock);

	DBG(STACKTRACE,
	    dprintf("__alpha_osf_firstFrame(0x%p) pc 0x%p fp 0x%p sp 0x%p\n", frame,
		    (void*)frame->sc.sc_pc, (void*)frame->sc.sc_regs[15],
		    (void*)frame->sc.sc_regs[30]); );
}

exceptionFrame * __alpha_osf_nextFrame (exceptionFrame *frame)
{

	DBG(STACKTRACE,
	    dprintf("__alpha_osf_nextFrame(0x%p) pc 0x%p fp 0x%p sp 0x%p\n", frame,
		    (void*)frame->sc.sc_pc, (void*)frame->sc.sc_regs[15],
		    (void*)frame->sc.sc_regs[30]); );
	/* exc_virtual_unwind() will call last chance handler if RPD
           is not found.  So lookup the corresponding table before
           real unwind.  Note that exc_lookup_function_table() return
           the table address, not the entry address that could be used
           by exc_virtual_unwind().  */
	lockStaticMutex (&excLock);
	if (exc_lookup_function_table (frame->sc.sc_pc) != NULL) {
		exc_virtual_unwind (NULL, &frame->sc);
	}
	else {
		/* No table found, is this method GCed after throw but
                   before printStackTrace() ??? */
#if 0
		DBG(STACKTRACE,
		    dprintf ("__alpha_osf_nextFrame(): no PDSC_RPD for pc %p\n",
			     (void*)frame->sc.sc_pc); );
#endif
		frame->sc.sc_pc = 0;
	}
	unlockStaticMutex (&excLock);
	DBG(STACKTRACE,
	    dprintf(" -> pc 0x%p fp 0x%p sp 0x%p\n",
		    (void*)frame->sc.sc_pc, (void*)frame->sc.sc_regs[15],
		    (void*)frame->sc.sc_regs[30]); );
	return frame->sc.sc_pc == 0 ? NULL : frame;
}

/* Construct JIT Exception information and register it.  */
void __alpha_osf_register_jit_exc (void *methblock, void *codebase, void *codeend)
{
	extern int maxLocal, maxStack, maxTemp, maxArgs, maxPush;
	struct {
		pdsc_crd crd[2];
		struct pdsc_long_stack_rpd rpd;
	} *pdsc = methblock;
	int framesize;			/* frame size in 64 bit words */
	int rsa_offset;			/* rsa offset from $sp in 64 bit words */
	int codelen = codeend - codebase;

	assert (sizeof (*pdsc) == MD_JIT_EXCEPTION_INFO_LENGTH);

	/* same as LABEL_Lframe() and LABEL_Lrsa() */
	framesize = maxLocal + maxStack +
		maxTemp + (maxArgs < 6 ? maxArgs : 6) +
		alpha_jit_info.rsa_size +
		(maxPush <= 6 ? 0 : maxPush - 6);
	framesize += framesize & 1;	/* octaword-aligned */
	rsa_offset = (maxPush < 6 ? 0 : maxPush - 6);

	/* create Code Range Descriptor Table */
	pdsc->crd[0].words.begin_address = (pdsc_offset) ((char *) codebase
		- (char *) pdsc);
	pdsc->crd[0].words.rpd_offset = (pdsc_offset) ((char *) &pdsc->rpd
		- (char *) &pdsc->crd[0].words.rpd_offset);
	pdsc->crd[1].words.begin_address = pdsc->crd[0].words.begin_address
		+ codelen;
	pdsc->crd[1].words.rpd_offset = 0;

	/* create Runtime Procedure Descriptor */
#if 0
	/* With -ieee, GCC alway add .eflag 48 event if function does
           not use float nor double.  */
	pdsc->rpd.flags = PDSC_EXC_IEEE;
#else
	pdsc->rpd.flags = (alpha_jit_info.ieee ? PDSC_EXC_IEEE : 0);
#endif
	pdsc->rpd.entry_ra = 26;
	pdsc->rpd.rsa_offset = rsa_offset;
	pdsc->rpd.sp_set = alpha_jit_info.sp_set;
	pdsc->rpd.entry_length = alpha_jit_info.entry_length;
	pdsc->rpd.frame_size = framesize;
	pdsc->rpd.reserved = 0;
	pdsc->rpd.imask = alpha_jit_info.imask;
	pdsc->rpd.fmask = alpha_jit_info.fmask;

	DBG(STACKTRACE,
	    dprintf("__alpha_osf_register_jit() 0x%p pc [0x%p - 0x%p[\n",
		    methblock, codebase, codebase + codelen);
	    dprintf(" crd[0] begin_address %d rpd_offset\n",
		    pdsc->crd[0].words.begin_address,
		    pdsc->crd[0].words.rpd_offset);
	    dprintf(" crd[1] begin_address %d rpd_offset\n",
		    pdsc->crd[1].words.begin_address,
		    pdsc->crd[1].words.rpd_offset);
	    dprintf(" maxLocal %d maxStack %d maxTemp %d maxArgs %d maxPush %d\n",
		    maxLocal, maxStack, maxTemp, maxArgs, maxPush);
	    dprintf(" framesize %d rsa_offset %d rsa_size %d\n",
		    framesize, rsa_offset, alpha_jit_info.rsa_size);
	    );
	/* Register this runtime procedure descriptor */
	lockStaticMutex (&excLock);
	exc_add_pc_range_table (methblock, 2);
	exc_add_gp_range ((exc_address) codebase, codelen, (exc_address) codebase);
	unlockStaticMutex (&excLock);
}

void __alpha_osf_unregister_jit_exc (void *methblock, void *codebase, void *codeend)
{
	int codelen = codeend - codebase;

	DBG(STACKTRACE,
	    dprintf("__alpha_osf_unregister_jit() 0x%p pc [0x%p - 0x%p[\n",
		    methblock, codebase, codebase + codelen) );
	/* Unregister this runtime procedure descriptor */
	lockStaticMutex (&excLock);
	exc_remove_pc_range_table (methblock);
	exc_remove_gp_range ((exc_address) codebase);
	unlockStaticMutex (&excLock);
}
#else
#include "alpha/alpha.c"
#endif
#endif
