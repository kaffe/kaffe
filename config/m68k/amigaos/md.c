/*
 * m68k/amigaos/md.c
 * AmigaOS specific functions.
 *
 * Copyright (c) 1996 Matthias Hopf <mshopf@informatik.uni-erlangen.de>
 *
 * Permission granted for Tim Wilkinson to include this source in his
 * Kaffe system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "m68k/amigaos/md.h"
#include "object.h"
#include "thread.h"

#include <stdio.h>
#include <assert.h>

#include <exec/exec.h>
#include <proto/exec.h>

struct Task *AmigaThisTask = NULL;

/*
 * System initialization
 */
void md_init (void)
{
    AmigaThisTask = FindTask (NULL);
	
	// Initialise FPU to round properly
	asm	volatile	(			\
	"fmove.l	fpcr,-(sp)	\n"		\
	"and.l		#0xffffff00,sp@	\n"		\
	"fmove.l	(sp)+,fpcr	\n"		\
	);
}
