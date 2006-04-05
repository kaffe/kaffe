/*
 * java.util.zip.CRC32.c
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
#include "java_util_zip_CRC32.h"
#include "gtypes.h"

#include <zlib.h>

void
java_util_zip_CRC32_update(struct Hjava_util_zip_CRC32* this, HArrayOfByte* buf, jint from, jint len)
{
	unhand(this)->crc = crc32((uLong)unhand(this)->crc, 
				  (const Bytef *)&unhand_array(buf)->body[from], 
				  (uInt)len);
}

void
java_util_zip_CRC32_update1(struct Hjava_util_zip_CRC32* this, jint val)
{
	jbyte b;

	b = val;
	unhand(this)->crc = crc32((uLong)unhand(this)->crc,
				  (const Bytef *)&b,
				  (uInt)sizeof(b));
}

