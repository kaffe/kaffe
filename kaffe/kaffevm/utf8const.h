/*
 * utf8const.h
 *
 * Copyright (c) 1998, 2002
 *	Transvirtual Technologies, Inc.  All rights reserved.
 */

#ifndef _kaffevm_utf8const_h
#define _kaffevm_utf8const_h

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "debug.h"

/* Initialize utf8const support system */
extern void		  utf8ConstInit(void);

/* Create a Utf8Const from a UTF-8 encoded array. The returned pointer
   remains valid until Utf8ConstRelease() is called. */
extern Utf8Const*	  utf8ConstNew(const char*, int);

/* Add a reference to a Utf8Const */
extern void		  utf8ConstAddRef(Utf8Const*);

/* Release a Utf8Const previously created via utf8ConstNew(). */
extern void		  utf8ConstRelease(Utf8Const*);

/* Assign a Utf8Const B to A and do proper reference counting */
/* Must be a macro because it needs to assign to A. */
/* XXX come up with a better way.  Uses (A) 3 times! */
#define utf8ConstAssign(A, B) 			\
	do {					\
		if ((A) != 0) {			\
			utf8ConstRelease((A));	\
		}				\
		utf8ConstAddRef((B));		\
		(A) = (B);			\
	} while (0)

/* Check if a string is a valid UTF-8 string */
extern int		  utf8ConstIsValidUtf8(const char *, unsigned int);

/* Return the length of the corresponding Unicode string */
extern int		  utf8ConstUniLength(const Utf8Const*);

/* Decode a Utf8Const (to Unicode) into the buffer (which must be big enough) */
extern void		  utf8ConstDecode(const Utf8Const*, jchar*);

/* 
 * Encode a jchar[] Array into a zero-terminated C string
 * that contains the array's utf8 encoding.
 */
extern char * utf8ConstEncode(const jchar *chars, int clength);
/* 
 * Encode a jchar[] Array into a zero-terminated C string
 * that contains the array's utf8 encoding.
 *
 * WARNING: buf is assumed to have the sufficient size (clength*2 is sufficient).
 */
extern void utf8ConstEncodeTo(const jchar *chars, int clength, char *buf);

extern bool utf8ConstEqual(Utf8Const* a, Utf8Const* b);

/* Since we compute hash values at creation time, we can do this: */
static inline int32 utf8ConstHashValue(Utf8Const* a) __UNUSED__;
static inline int32 utf8ConstHashValue(Utf8Const* a)
{
	assert(a != NULL);
	assert(a->nrefs >= 1);
	return a->hash;
}

/*
 * Extract a character from a Java-style Utf8 string.
 * PTR points to the current UTF-8 byte; END points to the end of the string.
 * PTR is incremented to point after the character that gets returns.
 * On an error, -1 is returned and PTR is no longer valid.
 */
#define UTF8_GET(PTR, END)						\
  ((PTR) >= (END)							\
     ? -1								\
   : (PTR)[0] == 0							\
     ? (PTR)++, -1							\
   : ((PTR)[0]&0x80) == 0						\
     ? *(PTR)++								\
   : ((PTR)+2)<=(END)							\
       && ((PTR)[0]&0xE0) == 0xC0					\
       && ((PTR)[1]&0xC0) == 0x80					\
       && ((PTR)+=2, 1)							\
     ? (((PTR)[-2] & 0x1F) << 6) + ((PTR)[-1] & 0x3F)			\
   : ((PTR)+3)<=(END)							\
       && ((PTR)[0] & 0xF0) == 0xE0					\
       && ((PTR)[1] & 0xC0) == 0x80					\
       && ((PTR)[2] & 0xC0) == 0x80					\
       && ((PTR)+=3, 1)							\
     ? (((PTR)[-3]&0x1F) << 12)						\
       + (((PTR)[-2]&0x3F) << 6) + ((PTR)[-1]&0x3F)			\
   : -1)


#endif /* _kaffevm_utf8const_h */
