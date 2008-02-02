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
#if defined(TRANSLATOR)
#if defined(__ARM_EABI__)
#include "linux-eabi.h"
#else
#include "linux-gas.h"
#endif /* defined(__ARM_EABI__) */
#endif /* defined(TRANSLATOR) */
#include <malloc.h>

void            
init_md(void)
{       
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT) 
        mallopt(M_MMAP_MAX, 0);
#endif
}

#ifdef TRANSLATOR
void flush_dcache(void *start, void *end) {
  CLEAR_INSN_CACHE(start, end);
}
#endif
