/*
 * java.util.zip.Inflater.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include <native.h>
#include "Inflater.h"

#if defined(HAVE_LIBZ) && defined(HAVE_ZLIB_H)

#include <zlib.h>

#define WSIZE   	0x8000
#define	WSIZEBITS	15

#define	MAXSTREAM	16

#define	GET_STREAM(THIS)	(*(z_stream**)&unhand(this)->strm)

void
java_util_zip_Inflater_setDictionary(struct Hjava_util_zip_Inflater* this, HArrayOfByte* buf, jint from, jint len)
{
	int r;
	z_stream* dstream;

	dstream = GET_STREAM(this);
	r = inflateSetDictionary (dstream, &unhand(buf)->body[from], len);
	if (r < 0) {
		SignalError("java.lang.Error", dstream->msg ? dstream->msg : "unknown error");
	}
}

jint
java_util_zip_Inflater_inflate(struct Hjava_util_zip_Inflater* this, HArrayOfByte* buf, jint off, jint len)
{
	int r;
	int ilen;
	z_stream* dstream;

	dstream = GET_STREAM(this);

	ilen = unhand(this)->len;

	/* No data - force us to get some more */
	if (ilen == 0) {
		return (0);
	}

	dstream->next_in = &unhand(unhand(this)->buf)->body[unhand(this)->off];
	dstream->avail_in = ilen;

	dstream->next_out = &unhand(buf)->body[off];
	dstream->avail_out = len;

	r = inflate(dstream, Z_PARTIAL_FLUSH);

	switch (r) {
	case Z_OK:
		break;

	case Z_STREAM_END:
		unhand(this)->finished = 1;
		break;

	case Z_NEED_DICT:
		unhand(this)->needsDictionary = 1;
		break;

	default:
		SignalError("java.lang.Error", dstream->msg ? dstream->msg : "unknown error");
	}

	unhand(this)->off += (ilen - dstream->avail_in);
	unhand(this)->len = dstream->avail_in;

	return (len - dstream->avail_out);
}

jint
java_util_zip_Inflater_getAdler(struct Hjava_util_zip_Inflater* this)
{
	return (GET_STREAM(this)->adler);
}

jint
java_util_zip_Inflater_getTotalIn(struct Hjava_util_zip_Inflater* this)
{
	return (GET_STREAM(this)->total_in);
}

jint
java_util_zip_Inflater_getTotalOut(struct Hjava_util_zip_Inflater* this)
{
	return (GET_STREAM(this)->total_out);
}

void
java_util_zip_Inflater_reset(struct Hjava_util_zip_Inflater* this)
{
	inflateReset(GET_STREAM(this));

	unhand(this)->finished = 0;
	unhand(this)->len = 0;
}

void
java_util_zip_Inflater_end(struct Hjava_util_zip_Inflater* this)
{
	z_stream* dstream;

	dstream = GET_STREAM(this);
	inflateEnd(dstream);
	free(dstream);
}

void
java_util_zip_Inflater_init(struct Hjava_util_zip_Inflater* this, jbool val)
{
	int r;
	z_stream* dstream;

	dstream = malloc(sizeof(z_stream));
	dstream->next_out = 0;
	dstream->zalloc = 0;
	dstream->zfree = 0; 
	dstream->opaque = 0;

	r = inflateInit2(dstream, val ? -WSIZEBITS : WSIZEBITS);
	if (r != Z_OK) {
		SignalError("java.lang.Error", dstream ? dstream->msg : "");
	}
	GET_STREAM(this) = dstream;
}

#endif
