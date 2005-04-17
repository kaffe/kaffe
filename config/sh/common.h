/*
 * sh/common.h
 * Common Super-H configuration information.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if !defined(__sh_common_h)
#define __sh_common_h

#include "generic/gentypes.h"

/* The SuperH never aligns to more than a 4 byte boundary. */
#define	ALIGNMENT_OF_SIZE(S)	((S) < 4 ? (S) : 4)

#include <stddef.h>

#include "gccbuiltin.h"
#include "generic/genatomic.h"
#include "katomic.h"
#include "generic/comparexch.h"

#endif /* !defined(__sh_common_h) */
