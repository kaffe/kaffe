/*
 * kaffe.management.JIT.c
 *
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Godmar Back <gback@pa.dec.com>
 */

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "kaffe_management_JIT.h"
#include "classMethod.h"
#include "lookup.h"
#include "support.h"
#include "debug.h"
#include "stringSupport.h"
#include "methodCache.h"

/* XXX Temporary for now until we define interface header file that
 * declares "translate"
 */
#if defined(TRANSLATOR) 
#if defined(JIT3)
#include "jit3/machine.h"
#else
#include "jit/machine.h"
#endif
#endif

#include "md.h"
#include <native.h>

/*
 * Translate a method given class, method name, and signature
 */
void
Java_kaffe_management_JIT_translateMethod(JNIEnv *env UNUSED, jclass _vmclass UNUSED, 
	jstring _cname, jstring _mname, 
	jstring _signature, jobject loader)	
	/* JNI? what's that? */
{
	errorInfo info;
	Hjava_lang_Class *cls;
	Method *meth;
	char* cname = stringJava2C(_cname);
	char* mname = stringJava2C(_mname);
	char* signature = stringJava2C(_signature);
	Utf8Const* u8cname = utf8ConstNew(cname, -1);
	Utf8Const* u8mname = utf8ConstNew(mname, -1);
	Utf8Const* u8sig = utf8ConstNew(signature, -1);

	/* 
	dprintf("translating %s.%s%s\n", cname, mname, signature);
	*/
	cls = loadClass(u8cname, loader, &info);
	if (cls == 0) {
		throwError(&info);
	}
	meth = findMethodLocal(cls, u8mname, u8sig);

	if (meth == 0) {
		dprintf("Didn't find method %s%s in class %s\n",
			mname, signature, cname);
	} else {
#if defined(TRANSLATOR)
		if (!translate(meth, &info)) {
			throwError(&info);
		}
#else
		dprintf(
			"Interpreter does not translate %s%s in class %s\n",
			mname, signature, cname);
#endif
	}

	utf8ConstRelease(u8cname);
	utf8ConstRelease(u8mname);
	utf8ConstRelease(u8sig);
	gc_free(cname);
	gc_free(mname);
	gc_free(signature);
}

/*
 * Flush the dcache 
 */
void
Java_kaffe_management_JIT_flushCache(JNIEnv *env UNUSED, jclass clazz UNUSED)
{
#if defined(TRANSLATOR) 
#if defined(FULL_CACHE_FLUSH)
	FULL_CACHE_FLUSH();
#else
	unimp("FULL_CACHE_FLUSH");
#endif
#endif
}

