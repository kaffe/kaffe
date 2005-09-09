/*
 * java.util.zip.Inflater.c
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
#include "java_util_zip_Inflater.h"

#if defined(HAVE_LIBZ) && defined(HAVE_ZLIB_H)

#include <zlib.h>

#define WSIZE   	0x8000
#define	WSIZEBITS	15

#define	MAXSTREAM	16

static inline 
z_stream*
getStream(struct Hjava_util_zip_Inflater* this)
{
  return *(z_stream**)&unhand(this)->strm;
}

static inline 
void
setStream(struct Hjava_util_zip_Inflater* this, z_stream* stream)
{
  *(z_stream**)&unhand(this)->strm = stream;
}

void
java_util_zip_Inflater_setDictionary(struct Hjava_util_zip_Inflater* this, HArrayOfByte* buf, jint from, jint len)
{
	int r;
	z_stream* dstream;

	dstream = getStream(this);
	// XXX What happens if out of bounds ? 
	if (from >= 0 && len > 0 && from + len <= obj_length(buf)) {
	        void* dictionary = &unhand_array(buf)->body[from];
		r = inflateSetDictionary (dstream, dictionary, (unsigned)len);
		if (r < 0) {
			SignalError("java.lang.Error", dstream->msg ? dstream->msg : "unknown error");
		}
	}
}

jint
java_util_zip_Inflater_inflate0(struct Hjava_util_zip_Inflater* this, HArrayOfByte* buf, jint off, jint len)
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

	r = inflate(dstream, Z_SYNC_FLUSH);

	switch (r) {
	case Z_OK:
		break;

	case Z_STREAM_END:
		unhand(this)->finished = 1;
		break;

	case Z_NEED_DICT:
		unhand(this)->needsDictionary = 1;
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
java_util_zip_Inflater_getAdler(struct Hjava_util_zip_Inflater* this)
{
	return (getStream(this)->adler);
}

jint
java_util_zip_Inflater_getTotalIn(struct Hjava_util_zip_Inflater* this)
{
	return (getStream(this)->total_in);
}

jint
java_util_zip_Inflater_getTotalOut(struct Hjava_util_zip_Inflater* this)
{
	return (getStream(this)->total_out);
}

void
java_util_zip_Inflater_reset(struct Hjava_util_zip_Inflater* this)
{
	inflateReset(getStream(this));

	unhand(this)->finished = 0;
	unhand(this)->len = 0;
}

void
java_util_zip_Inflater_end(struct Hjava_util_zip_Inflater* this)
{
	z_stream* dstream;

	dstream = getStream(this);
	inflateEnd(dstream);
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
java_util_zip_Inflater_init(struct Hjava_util_zip_Inflater* this, jboolean val)
{
	int r;
	z_stream* dstream;

	dstream = KMALLOC(sizeof(z_stream));
	if (!dstream) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}
	dstream->next_out = NULL;
	dstream->zalloc = kaffe_zalloc;
	dstream->zfree = kaffe_zfree;
	dstream->opaque = NULL;

	r = inflateInit2(dstream, val ? -WSIZEBITS : WSIZEBITS);

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
java_util_zip_Inflater_setDictionary(struct Hjava_util_zip_Inflater* this, HArrayOfByte* buf, jint from, jint len)
{
	supportDisabled();
}

jint
java_util_zip_Inflater_inflate(struct Hjava_util_zip_Inflater* this, HArrayOfByte* buf, jint off, jint len)
{
	supportDisabled();
	return 0;
}

jint
java_util_zip_Inflater_getAdler(struct Hjava_util_zip_Inflater* this)
{
	supportDisabled();
	return 0;
}

jint
java_util_zip_Inflater_getTotalIn(struct Hjava_util_zip_Inflater* this)
{
	supportDisabled();
	return 0;
}

jint
java_util_zip_Inflater_getTotalOut(struct Hjava_util_zip_Inflater* this)
{
	supportDisabled();
	return 0;
}

void
java_util_zip_Inflater_reset(struct Hjava_util_zip_Inflater* this)
{
	supportDisabled();
}

void
java_util_zip_Inflater_end(struct Hjava_util_zip_Inflater* this)
{
	supportDisabled();
}

void
java_util_zip_Inflater_init(struct Hjava_util_zip_Inflater* this, jboolean val)
{
	supportDisabled();
}

#endif
