/*
 * arm/netbsd/md.c
 * NetBSD arm specific functions.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */     
                
#include "config.h"
#include <malloc.h>

#define CACHE_SIZE (32 * 1024)
#define LINE_SIZE sizeof(int)
#define CACHE_LINES (CACHE_SIZE / LINE_SIZE)

static volatile int flusher[CACHE_LINES];

void
flush_dcache(void)
{
        int i;
        for (i = 0; i < CACHE_LINES; i++) {
                flusher[i]++;
        }
        /* sched_yield(); */
}
