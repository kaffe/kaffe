/*
 * java.util.zip.Deflater.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define DBG(s)
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include <native.h>
#include "java_util_zip_Deflater.h"

#if defined(HAVE_LIBZ) && defined(HAVE_ZLIB_H)

#include <zlib.h>

#define	WSIZE		0x8000
#define	WSIZEBITS	15

static inline 
z_stream*
getStream(struct Hjava_util_zip_Deflater* this)
{
  return *(z_stream**)&unhand(this)->strm;
}

static inline 
void
setStream(struct Hjava_util_zip_Deflater* this, z_stream* stream)
{
  *(z_stream**)&unhand(this)->strm = stream;
}

void
java_util_zip_Deflater_setDictionary(struct Hjava_util_zip_Deflater* this, HArrayOfByte* buf, jint from, jint len)
{
	int r;
	z_stream* dstream;

 	dstream = getStream(this);

	// XXX What happens if out of bounds ? 
	if (from >= 0 && len > 0 && from + len <= obj_length(buf)) {
	        void* dictionary = &unhand_array(buf)->body[from];
		r = deflateSetDictionary (dstream, dictionary, (unsigned)len);
		if (r < 0) {
			SignalError("java.lang.Error", dstream->msg ? dstream->msg : "unknown error");
		}
	}
}

jint
java_util_zip_Deflater_deflate(struct Hjava_util_zip_Deflater* this, HArrayOfByte* buf, jint off, jint len)
{
	int r;
	int ilen;
	z_stream* dstream;
	void* next_available_input = &unhand_array(unhand(this)->buf)->body[unhand(this)->off];
	void* next_available_output = &unhand_array(buf)->body[off];

 	dstream = getStream(this);

	ilen = unhand(this)->len;

	dstream->next_in = next_available_input;
	dstream->avail_in = ilen;

	dstream->next_out = next_available_output;
	dstream->avail_out = len;

	r = deflate(dstream, unhand(this)->finish ? Z_FINISH : Z_NO_FLUSH);

DBG(	dprintf("Deflate: in %d left %d out %d status %d\n", ilen, dstream->avail_in, len - dstream->avail_out, r);	)

	switch (r) {
	case Z_OK:
		break;

	case Z_STREAM_END:
		unhand(this)->finished = 1;
		break;

	case Z_MEM_ERROR:
		{
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
		}

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
	return (getStream(this)->adler);
}

jint
java_util_zip_Deflater_getTotalIn(struct Hjava_util_zip_Deflater* this)
{
	return (getStream(this)->total_in);
}

jint
java_util_zip_Deflater_getTotalOut(struct Hjava_util_zip_Deflater* this)
{
	return (getStream(this)->total_out);
}

void
java_util_zip_Deflater_reset(struct Hjava_util_zip_Deflater* this)
{
	deflateReset(getStream(this));

	unhand(this)->finish = 0;
	unhand(this)->finished = 0;
}

void
java_util_zip_Deflater_end(struct Hjava_util_zip_Deflater* this)
{
	z_stream* dstream;

	dstream = getStream(this);
	setStream(this, NULL);

	deflateEnd(dstream);
	KFREE(dstream);
}

static voidpf
kaffe_zalloc(voidpf opaque UNUSED, uInt items, uInt size) {
  /* allocate through the garbage collector interface */
  return KMALLOC(items*size);
}

static void
kaffe_zfree(voidpf opaque UNUSED, voidpf address) {
  /* dispose through the garbage collector interface */
  KFREE(address);
}

void
java_util_zip_Deflater_init(struct Hjava_util_zip_Deflater* this, jboolean val)
{
	int r;
 	z_stream* dstream;

	dstream = KMALLOC(sizeof(*dstream));
	if (!dstream) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}
	dstream->next_in = NULL;
	dstream->zalloc = kaffe_zalloc;
	dstream->zfree = kaffe_zfree;
	dstream->opaque = NULL;

	r = deflateInit2(dstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (val ? -WSIZEBITS : WSIZEBITS), 9, Z_DEFAULT_STRATEGY);

	switch (r) {
	case Z_OK:
		break;

	case Z_MEM_ERROR:
		{
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
		}

	default:
		SignalError("java.lang.Error", dstream->msg ? dstream->msg : "");
	}

	setStream(this, dstream);
}

#else

static void 
supportDisabled (void)
{
	SignalError ("kaffe.util.SupportDisabled",
		     "libz was not found by Kaffe configure script");
}
    
void
java_util_zip_Deflater_setDictionary(struct Hjava_util_zip_Deflater* this, HArrayOfByte* buf, jint from, jint len)
{
	supportDisabled();
}

jint
java_util_zip_Deflater_deflate(struct Hjava_util_zip_Deflater* this, HArrayOfByte* buf, jint off, jint len)
{
	supportDisabled();
	return 0;
}

jint
java_util_zip_Deflater_getAdler(struct Hjava_util_zip_Deflater* this)
{
	supportDisabled();
	return 0;
}

jint
java_util_zip_Deflater_getTotalIn(struct Hjava_util_zip_Deflater* this)
{
	supportDisabled();
	return 0;
}

jint
java_util_zip_Deflater_getTotalOut(struct Hjava_util_zip_Deflater* this)
{
	supportDisabled();
	return 0;
}

void
java_util_zip_Deflater_reset(struct Hjava_util_zip_Deflater* this)
{
	supportDisabled();
}

void
java_util_zip_Deflater_end(struct Hjava_util_zip_Deflater* this)
{
	supportDisabled();
}

void
java_util_zip_Deflater_init(struct Hjava_util_zip_Deflater* this, jboolean val)
{
	supportDisabled();
}

#endif
