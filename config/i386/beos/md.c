/*
 * i386/beos/md.c
 * BeOS i386 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <stdlib.h>
#include <errno.h>
#include "config.h"

/* This is the faked getsockopt for BeOS R4 */
int
getsockopt(int s, int level, int optname, void* optval, int *optlen)
{
	errno = EOPNOTSUPP;
	return -1;
}
