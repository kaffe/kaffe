/*
 * arm/riscos/md.c
 * RISC OS arm specific functions.
 */     
                
#include "config.h"
#include <malloc.h>
#include <unixlib/features.h>

void            
init_md(void)
{       
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT) 
        mallopt(M_MMAP_MAX, 0);
#endif
	__feature_imagefs_is_file = 1;
}

/* This may not be correct for all ARMs, e.g. XScale */
#define CACHE_SIZE (32 * 1024)
#define LINE_SIZE sizeof(int)
#define CACHE_LINES (CACHE_SIZE / LINE_SIZE)

static volatile int flusher[CACHE_LINES];

void
flush_dcache(void)
{
        int cache_line;

        for (cache_line = 0; cache_line < CACHE_LINES; cache_line++) {
                flusher[cache_line]++;
        }
       /* Not a call we have yet */
/*        sched_yield(); */
}
