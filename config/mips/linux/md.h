/*
 * mips/linux/md.h
 * Linux MIPS configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_linux_md_h
#define __mips_linux_md_h

#include "mips/common.h"
#include "mips/threads.h"

#undef	SP_OFFSET
#undef	FP_OFFSET

#define	SP_OFFSET	1
#define	FP_OFFSET	10

#endif
