/*
 * arm/linux/md.c
 * Linux arm specific functions.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */     
                
#include "config.h"
#include "md.h"
#if defined(TRANSLATOR)
#include "arm/jit.h"
#endif
#include <malloc.h>
#include <sched.h>
#include <asm/unistd.h>

void            
init_md(void)
{       
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT) 
        mallopt(M_MMAP_MAX, 0);
#endif
}

/**
 * Shamelessly stolen from parrot... ([perl]/parrot/jit/arm/jit_emit.h arm_sync_d_i_cache)
 *
 * r2 should be zero for 2.4 (but it's ignored) so passing VM_EXEC (needed
 * for 2.6) should be okay.
 */
void flush_dcache(void *start, void *end) {
}
