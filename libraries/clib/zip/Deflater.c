/*
 * java.util.zip.Deflater.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include <native.h>
#include "Deflater.h"

#if defined(HAVE_LIBZ) && defined(HAVE_ZLIB_H)

#include <zlib.h>

#define	WSIZE		0x8000
#define	WSIZEBITS	15

#define GET_STREAM(THIS)        (*(z_stream**)&unhand(this)->strm)

void
java_util_zip_Deflater_setDictionary(struct Hjava_util_zip_Deflater* this, HArrayOfByte* buf, jint from, jint len)
{
	int r;
        z_stream* dstream;

        dstream = GET_STREAM(this);

	r = deflateSetDictionary (dstream, &unhand(buf)->body[from], len);
	if (r < 0) {
		SignalError("java.lang.Error", dstream->msg ? dstream->msg : "unknown error");
	}
}

jint
java_util_zip_Deflater_deflate(struct Hjava_util_zip_Deflater* this, HArrayOfByte* buf, jint off, jint len)
{
	int r;
	int ilen;
        z_stream* dstream;

        dstream = GET_STREAM(this);

	ilen = unhand(this)->len;

	dstream->next_in = &unhand(unhand(this)->buf)->body[unhand(this)->off];
	dstream->avail_in = ilen;

	dstream->next_out = &unhand(buf)->body[off];
	dstream->avail_out = len;

	r = deflate(dstream, unhand(this)->finish ? Z_FINISH : Z_NO_FLUSH);

DBG(	printf("Deflate: in %d left %d out %d status %d\n", ilen, dstream->avail_in, len - dstream->avail_out, r);	)

	switch (r) {
	case Z_OK:
		break;

	case Z_STREAM_END:
		unhand(this)->finished = 1;
		break;

	default:
		SignalError("java.lang.Error", dstream->msg ? dstream->msg : "unknown error");
	}

	unhand(this)->off += (ilen - dstream->avail_in);
	unhand(this)->len = dstream->avail_in;

	return (len - dstream->avail_out);
}

jint
java_util_zip_Deflater_getAdler(struct Hjava_util_zip_Deflater* this)
{
	return (GET_STREAM(this)->adler);
}

jint
java_util_zip_Deflater_getTotalIn(struct Hjava_util_zip_Deflater* this)
{
	return (GET_STREAM(this)->total_in);
}

jint
java_util_zip_Deflater_getTotalOut(struct Hjava_util_zip_Deflater* this)
{
	return (GET_STREAM(this)->total_out);
}

void
java_util_zip_Deflater_reset(struct Hjava_util_zip_Deflater* this)
{
	deflateReset(GET_STREAM(this));

	unhand(this)->finish = 0;
	unhand(this)->finished = 0;
}

void
java_util_zip_Deflater_end(struct Hjava_util_zip_Deflater* this)
{
	z_stream* dstream;

	dstream = GET_STREAM(this);
	GET_STREAM(this) = 0;

	deflateEnd(dstream);
	free(dstream);
}

static voidpf
kaffe_zalloc(voidpf opaque, uInt items, uInt size) {
  /* allocate through the garbage collector interface */
  return malloc(items*size);
}

static void
kaffe_zfree(voidpf opaque, voidpf address) {
  /* dispose through the garbage collector interface */
  free(address);
}

void
java_util_zip_Deflater_init(struct Hjava_util_zip_Deflater* this, jbool val)
{
	int r;
        z_stream* dstream;

	dstream = malloc(sizeof(*dstream));
	dstream->next_in = 0;
	dstream->zalloc = kaffe_zalloc;
	dstream->zfree = kaffe_zfree;
	dstream->opaque = 0;

	r = deflateInit2(dstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (val ? -WSIZEBITS : WSIZEBITS), 9, Z_DEFAULT_STRATEGY);

	if (r != Z_OK) {
		SignalError("java.lang.Error", dstream ? dstream->msg : "");
	}

	GET_STREAM(this) = dstream;
}

#endif
