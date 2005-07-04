/*
 * jit-3-md.h
 * Jit definitions for powerpc on netbsd
 *
 * Copyright (c) 2004-2005
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 

#ifndef __powerpc_netbsd_jit3_md_h
#define __powerpc_netbsd_jit3_md_h

/* mcontext.h gives us macros to access ucontext/mcontext */
#include <machine/mcontext.h>

#include "config.h"
#include "powerpc/jit.h"

/*
 * Initialize the exceptionFrame object "f" with the stack frame captured in
 * the given sigcontext structure.
 */

#define EXCEPTIONFRAME(f, c) \
do { \
    (f).sp = (ppc_stack_frame_t *)((register_storage_t *)c->sc_regs)[3]; \
} while( 0 )


/* 
#define EXCEPTIONFRAME(f, c) \
        (f).retbp = _UC_MACHINE_SP(((ucontext_t *)(c)));                \
	(f).retpc = _UC_MACHINE_PC(((ucontext_t *)(c)))
*/

/*
 * Flush newly generated instructions from the data cache and invalidate the
 * same blocks in the instruction cache.
 *
 * See section 5.2.5.2 of the PowerPC Programming Environments manual and
 * numerous usenet discussions on google.
 *
 * code_start - The start of the code, no special alignment required.
 * code_end - The end of the code.
 */
#define FLUSH_DCACHE linux_flush_cache
static inline void linux_flush_cache(void *code_start, void *code_end)
{
	/*
	 * The size of a block in the cache, we need to align the flushed
	 * addresses to these blocks.
	 */
	static int cache_block_size = 32; // bytes
	uintp curr, last;
	
	curr = (uintp)code_start;
	last = (uintp)code_end;
	curr = (curr - cache_block_size) & ~(cache_block_size - 1);
	last = (last + cache_block_size) & ~(cache_block_size - 1);
	for( ; curr < last; curr += cache_block_size )
	{
		asm("dcbst 0, %0\n"
		    "sync\n"
		    "icbi 0, %0\n"
		    "sync\n"
		    "isync\n" : : "r" (curr));
	}
}

#endif
