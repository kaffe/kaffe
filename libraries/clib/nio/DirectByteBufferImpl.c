/*
 * DirectByteBufferImpl.c
 *
 * Copyright (c) 2003 Kaffe's team.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 
#include "config.h"
#include "config-std.h"
#include "kaffe/jmalloc.h"
#include "java_nio_DirectByteBufferImpl.h"

struct Hgnu_classpath_RawData*
java_nio_DirectByteBufferImpl_allocateImpl(jint bytes)
{
  return (struct Hgnu_classpath_RawData*)jmalloc(bytes);
}

void
java_nio_DirectByteBufferImpl_freeImpl(struct Hgnu_classpath_RawData* data)
{
  jfree(data);
}

jbyte
java_nio_DirectByteBufferImpl_getImpl(struct Hjava_nio_DirectByteBufferImpl* this, jint index)
{
  return ((jbyte *)unhand(this)->address)[index];
}

void
java_nio_DirectByteBufferImpl_putImpl(struct Hjava_nio_DirectByteBufferImpl* this, jint index,
				      jbyte b)
{
  ((jbyte *)unhand(this)->address)[index] = b;
}
