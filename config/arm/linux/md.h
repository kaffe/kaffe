/*    
 * arm/linux/md.h
 * Linux arm configuration information.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 
#ifndef __arm_linux_md_h
#define __arm_linux_md_h
 
#include "arm/common.h"
#include "arm/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define INIT_MD()       init_md()

/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses
 *
 * Actually, when debugging, it would be nice to don't catch unexpected
 * exception, so...
 */

#ifdef  DEBUG
#define CREATE_NULLPOINTER_CHECKS
#endif

#undef SP_OFFSET
#undef FP_OFFSET

/* arm/linux/elf (NetWinder) */
#define SP_OFFSET               20
#define FP_OFFSET               19

#endif
