/*
 * util.c
 * Extra routine need for support.
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
#include "jtypes.h"
#include "constants.h"

Utf8Const*
makeUtf8Const(char* s, int len)
{
	Utf8Const* m;

	m = (Utf8Const*)malloc(sizeof(Utf8Const) + len + 1);
	memcpy(m->data, s, len);
	m->data[len] = 0;
	m->length = len;
	return (m);
}

#if !INTERN_UTF8CONSTS

/*
 * Count number of unicode characters in UtF8 string.
 */
int
strLengthUtf8(char* str, int len)
{
	register unsigned char* ptr;
	register unsigned char* limit;
	int str_length;

	ptr = (unsigned char*) str;
	limit = ptr + len;
	str_length = 0;
	for (; ptr < limit; str_length++) {
		if (UTF8_GET (ptr, limit) < 0) {
			return (-1);
		}
	}
	return (str_length);
}

/* Calculate a hash value for a string encoded in Utf8 format.
 * This returns the same hash value as specified or java.lang.String.hashCode.
 */
int32
hashUtf8String (char* str, int len)
{
	int str_length;
	register unsigned char* ptr;
	register unsigned char* limit;
	int32 hash;

	str_length = strLengthUtf8 (str, len);
	ptr = (unsigned char*) str;
	limit = ptr + len;
	hash = 0;

	if (str_length <= 15) {
		for (; ptr < limit;) {
			int ch = UTF8_GET(ptr, limit);
			hash = (37 * hash) + ch;
		}
	}
	else {
		int i = 0;
		for (; i < str_length;  i++) {
			int ch = UTF8_GET(ptr, limit);
			if ((i & 7) == 0) {
				hash = (39 * hash) + ch;
			}
		}
	}
	return (hash);
}

#endif
