/*
 * parisc/common.h
 * Common configuration information for PA-RISC based machines.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * sysdepCallMethod is written by Pavel Roskin <pavel_roskin@geocities.com>
 */

#ifndef __parisc_common_h
#define __parisc_common_h

#include "generic/gentypes.h"

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

#include "gccbuiltin.h"
#include "generic/genatomic.h"
#include "katomic.h"
#include "generic/comparexch.h"

#endif
