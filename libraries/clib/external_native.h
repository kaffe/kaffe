/*
 * external_native.h
 * Wrap up the calls between Kaffe and native method calls for systems
 *  which don't support shared libraries.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __external_native_h
#define __external_native_h

#if defined(NO_SHARED_LIBRARIES)

#define	KAFFE_NATIVE_PROTOTYPE(_f)	extern void _f(void);
#define	KAFFE_NATIVE_METHOD(_n)		{ #_n, _n },

#define	KAFFE_NATIVE(_f)		KAFFE_NATIVE_PROTOTYPE(_f)

#include "external_wrappers.h"

#undef	KAFFE_NATIVE
#define	KAFFE_NATIVE(_f)		KAFFE_NATIVE_METHOD(_f)

nativeFunction default_natives[] = {

#include "external_wrappers.h"

	{ 0, 0 }
};

#undef	KAFFE_NATIVE

#endif

#endif
