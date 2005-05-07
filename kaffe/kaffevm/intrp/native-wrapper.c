/*
 * native-wrapper.c
 * Java Native Interface - JNI wrapper builder.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "gtypes.h"
#include "classMethod.h"
#include "native-wrapper.h"

void
engine_create_wrapper (Method *meth, void *func)
{
	setMethodCodeStart(meth, func);
}
