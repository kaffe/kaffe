/*
 * java.util.zip.Adler32.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include <native.h>
#include "java_util_zip_Adler32.h"
#include "gtypes.h"

#if defined(HAVE_LIBZ) && defined(HAVE_ZLIB_H)

#include <zlib.h>

void
java_util_zip_Adler32_update(struct Hjava_util_zip_Adler32* this, HArrayOfByte* buf, jint from, jint len)
{
	// XXX What happens if out of bounds ? 
	if (from >= 0 && len > 0 && from + len <= obj_length(buf)) {
		unhand(this)->adler = adler32((uint32)unhand(this)->adler, &unhand_array(buf)->body[from], (unsigned)len);
	}
}

void
java_util_zip_Adler32_update1(struct Hjava_util_zip_Adler32* this, jint val)
{
	jbyte b;

	b = val;
	unhand(this)->adler = adler32((uint32)unhand(this)->adler, &b, sizeof(b));
}

#else

static void 
supportDisabled (void)
{
	SignalError ("kaffe.util.SupportDisabled",
		     "libz was not found by Kaffe configure script");
}

void
java_util_zip_Adler32_update(struct Hjava_util_zip_Adler32* this, HArrayOfByte* buf, jint from, jint len)
{
	supportDisabled();
}

void
java_util_zip_Adler32_update1(struct Hjava_util_zip_Adler32* this, jint val)
{
	supportDisabled();
}

#endif
