/*
 * powerpc/linux/md.h
 * Linux PowerPC configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __powerpc_linux_md_h
#define __powerpc_linux_md_h

#include "powerpc/threads.h"

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#endif
