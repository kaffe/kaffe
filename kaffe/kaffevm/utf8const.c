/*
 * utf8const.c
 *
 * Handle UTF-8 constant strings. These are intern'ed into a hash table.
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "jtypes.h"
#include "constants.h"
#include "object.h"
#include "itypes.h"
#include "locks.h"
#include "jsyscall.h"
#include "hashtab.h"
#include "stringSupport.h"

/* For kaffeh, don't use the hash table or locks. Instead, just make these
   function calls into macros in such a way as to avoid compiler warnings.
   Yuk! */
#ifdef KAFFEH
#undef  initStaticLock
#define initStaticLock(x)
#undef  staticLockIsInitialized		0
#define staticLockIsInitialized(x)	0
#undef  lockStaticMutex
#undef  unlockStaticMutex
#define unlockStaticMutex(x)
#define lockStaticMutex(x)
#define hashInit(a,b,c)		((hashtab_t)((u_int)utf8ConstCompare \
					+ (u_int)utf8ConstHashValueInternal))
#define hashAdd(t, x)		(x)
#define hashFind(t, x)		NULL
#define hashRemove(t, x)	(void)NULL
#endif

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
     ? -1								\
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

/* Internal variables */
static hashtab_t	hashTable;
#ifndef KAFFEH				/* Yuk! */
static iLock		utf8Lock;	/* mutex on all intern operations */
#endif

/* Internal functions */
static int		utf8ConstHashValueInternal(const void *v);
static int		utf8ConstCompare(const void *v1, const void *v2);

/*
 * Convert a non-terminated UTF-8 string into an interned Utf8Const.
 */
Utf8Const *
utf8ConstNew(const char *s, int len)
{
	Utf8Const *utf8, *temp;
	int32 hash;

	/* Automatic length finder */
	if (len < 0) {
		len = strlen(s);
	}

#ifdef DEBUG
	assert(utf8ConstIsValidUtf8(s, len);
#endif

	/* Precompute hash value using String.hashCode() algorithm */
	{
		const char *ptr = s;
		const char *const end = s + len;
		int ch;

		for (hash = 0;
		    (ch = UTF8_GET(ptr, end)) != -1;
		    hash = (31 * hash) + ch);
	}

	/* Lock intern table */
	if (!staticLockIsInitialized(&utf8Lock)) {
		initStaticLock(&utf8Lock);
	}
	lockStaticMutex(&utf8Lock);

	/* See if string is already in the table using a "fake" Utf8Const */
	if (hashTable != NULL) {
		Utf8Const *fake;
		char buf[200];

		if (sizeof(Utf8Const) + len + 1 > sizeof(buf)) {
			fake = KMALLOC(sizeof(Utf8Const) + len + 1);
			assert(fake != NULL);		/* XXX */
		} else {
			fake = (Utf8Const*)buf;
		}
		memcpy((char *)fake->data, s, len);
		((char *)fake->data)[len] = '\0';
		fake->hash = hash;
		utf8 = hashFind(hashTable, fake);
		if (fake != (Utf8Const*)buf) {
			KFREE(fake);
		}
		if (utf8 != NULL) {
			assert(utf8->nrefs >= 1);
			utf8->nrefs++;
			unlockStaticMutex(&utf8Lock);
			return(utf8);
		}
	} else {
		hashTable = hashInit(utf8ConstHashValueInternal,
			utf8ConstCompare, HASH_ADD_REFS);
	}

	/* Not in table; create new Utf8Const struct */
	utf8 = KMALLOC(sizeof(Utf8Const) + len + 1);
	memcpy((char *) utf8->data, s, len);
	((char*)utf8->data)[len] = '\0';
	utf8->hash = hash;
	utf8->nrefs = 1;

	/* Add to hash table */
	temp = hashAdd(hashTable, utf8);
	assert(temp == utf8);
	unlockStaticMutex(&utf8Lock);
	return(utf8);
}

/*
 * Add a reference to a Utf8Const.
 */
void
utf8ConstAddRef(Utf8Const *utf8)
{
	assert(staticLockIsInitialized(&utf8Lock));
	lockStaticMutex(&utf8Lock);
	assert(utf8->nrefs >= 1);
	utf8->nrefs++;
	unlockStaticMutex(&utf8Lock);
}

/*
 * Release a Utf8Const.
 */
void
utf8ConstRelease(Utf8Const *utf8)
{
	assert(staticLockIsInitialized(&utf8Lock));
	lockStaticMutex(&utf8Lock);
	assert(utf8->nrefs >= 1);
	if (--utf8->nrefs == 0) {
		hashRemove(hashTable, utf8);
		KFREE(utf8);
	}
	unlockStaticMutex(&utf8Lock);
}

/*
 * Return hash value for the hash table.
 */
static int	
utf8ConstHashValueInternal(const void *v)
{
	const Utf8Const *const utf8 = v;

	return(utf8->hash);
}

/*
 * Compare Utf8Consts for the hash table.
 */
static int
utf8ConstCompare(const void *v1, const void *v2)
{
	const Utf8Const *const utf8_1 = v1;
	const Utf8Const *const utf8_2 = v2;

	return(strcmp(utf8_1->data, utf8_2->data));
}

/*
 * Check if a string is a valid UTF-8 string.
 */
int
utf8ConstIsValidUtf8(const char *ptr, unsigned int len)
{
	const char *const end = ptr + len;

	while (UTF8_GET(ptr, end) != -1);
	return(ptr == end);
}

/*
 * Compute Unicode length of a UTF-8 string.
 */
int
utf8ConstUniLength(const Utf8Const *utf8)
{
	const char *ptr = utf8->data;
	const char *const end = ptr + strlen(utf8->data);
	int uniLen;

	for (uniLen = 0; UTF8_GET(ptr, end) != -1; uniLen++);
	assert(ptr == end);
	return(uniLen);
}

/*
 * Decode a UTF-8 string into Unicode. The buffer must be
 * big enough to hold utf8ConstUniLength(utf8) jchar's.
 */
void
utf8ConstDecode(const Utf8Const *utf8, jchar *buf)
{
	const char *ptr = utf8->data;
	const char *const end = ptr + strlen(utf8->data);
	int ch;

	while ((ch = UTF8_GET(ptr, end)) != -1) {
		*buf++ = ch;
	}
	assert(ptr == end);
}

/*
 * Return true iff the Utf8Const string is equal to the Java String.
 */
int
utf8ConstEqualJavaString(const Utf8Const *utf8, const Hjava_lang_String *string)
{
	const char *uptr = utf8->data;
	const char *const uend = uptr + strlen(utf8->data);
	const jchar *sptr = STRING_DATA(string);
	int ch, slen = STRING_SIZE(string);

#if 0
	/* Question: would this optimization be worthwhile? */
	if (unhand(string)->hash != 0 && unhand(string)->hash != utf8->hash) {
		return(0);
	}
#endif
	for (;;) {
		if ((ch = UTF8_GET(uptr, uend)) == -1) {
			return(slen == 0);
		}
		if (--slen < 0) {
			return(0);
		}
		if (ch != *sptr++) {
			return(0);
		}
	}
}

