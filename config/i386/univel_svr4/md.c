/*
 * i386/univel_svr4/md.c
 * Unixware i386 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-io.h"
#include "config-mem.h"

void
init_md(void)
{
#if defined(HAVE_MMAP)
    /* On NCR MP-RAS, address 0x0 to 0x1000 are mapped with PROT_READ so
     * NullPointerException are not handled correctly.
     * remap this range as PROT_NONE.
     * Edouard.Parmelan@France.NCR.COM
     */
    {	int fd;
	if ((fd = open("/dev/zero", O_RDONLY, 0)) != -1) {
	    mmap(NULL, 0x1000, PROT_NONE, MAP_PRIVATE|MAP_FIXED, fd, 0);
	    close(fd);
	}
    }
#endif
}
