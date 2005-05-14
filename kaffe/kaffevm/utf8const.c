/*
 * utf8const.c
 * Handle UTF-8 constant strings. These are intern'ed into a hash table.
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *  
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "classMethod.h"
#include "jni_md.h"
#include "kaffe/jmalloc.h"
#include "constants.h"
#include "object.h"
#include "itypes.h"
#include "locks.h"
#include "jsyscall.h"
#include "hashtab.h"
#include "stats.h"
#include "debug.h"
#include "utf8const.h"

/* For kaffeh, don't use the hash table. Instead, just make these
   function calls into macros in such a way as to avoid compiler
   warnings.  Yuk! */
#ifdef KAFFEH
#define hashInit(a,b,c,d)	((hashtab_t)((u_int)utf8ConstCompare \
					+ (u_int)utf8ConstHashValueInternal))
#define hashAdd(t, x)		(x)
#define hashFind(t, x)		NULL
#define hashRemove(t, x)	(void)NULL
#define initStaticLock(a)	do { } while (0)
#endif

/* Internal variables */
#ifndef KAFFEH				/* Yuk! */
static hashtab_t	hashTable;
static iStaticLock	utf8Lock;	/* mutex on all intern operations */

/*
 * Used to keep track of the current utf8Lock holder's stack
 * frame on which they first took the lock.  Protected by
 * that lock.  Need to keep track of this so that a thread can
 * drop and re-acquire the lock.
 *
 * Also used by debugging code to assert that the utf8Lock is never
 * recursively acquired.
 */

#define lockUTF() lockStaticMutex(&utf8Lock)
#define unlockUTF() unlockStaticMutex(&utf8Lock)

static inline void *UTFmalloc(size_t size)
{
	void *ret;

	unlockStaticMutex(&utf8Lock);
	ret = gc_malloc(size, KGC_ALLOC_UTF8CONST);
	lockStaticMutex(&utf8Lock);

	return ret;
}

static inline void UTFfree(const void *mem)
{
        unlockStaticMutex(&utf8Lock);
	gc_free((void *)mem);
	lockStaticMutex(&utf8Lock);
}
#else /* KAFFEH replacements: */
static hashtab_t	hashTable = (hashtab_t)1;

#define lockUTF()
#define unlockUTF() 
#define UTFmalloc(size) malloc(size)
#define UTFfree(ptr)   free(ptr)
#endif

/* Internal functions */
static int		utf8ConstHashValueInternal(const void *v);
static int		utf8ConstCompare(const void *v1, const void *v2);

/*
 * Convert a non-terminated UTF-8 string into an interned Utf8Const.
 * Returns 0 if an malloc failed occurred.
 */
Utf8Const *
utf8ConstNew(const char *s, int slen)
{
	size_t len;
	Utf8Const *utf8, *temp;
	int32 hash;
	Utf8Const *fake;
	char buf[200];

	/* Automatic length finder */
	if (slen < 0) {
		len = strlen(s);
	}else{
		len = (unsigned int) slen;
	}

#ifdef KAFFE_VMDEBUG
	assert(utf8ConstIsValidUtf8(s, len));
#endif
	hitCounter(&utf8new, "utf8-new");

	/* Precompute hash value using String.hashCode() algorithm */
	{
		const char *ptr = s;
		const char *const end = s + len;
		int ch;

		for (hash = 0;
		    (ch = UTF8_GET(ptr, end)) != -1;
		    hash = (31 * hash) + ch);
	}

	/* See if string is already in the table using a "fake" Utf8Const */
	assert (hashTable != NULL);
	if (sizeof(Utf8Const) + len + 1 > sizeof(buf)) {
		fake = gc_malloc(sizeof(Utf8Const) + len + 1, KGC_ALLOC_UTF8CONST);
		if (!fake) {
			return NULL;
		}
	} else {
		fake = (Utf8Const*)buf;
	}
	memcpy((char *)fake->data, s, len);
	((char *)fake->data)[len] = '\0';
	fake->hash = hash;
	fake->length = len;
	
	/* Lock intern table */
	lockUTF();
	utf8 = (Utf8Const *) hashFind(hashTable, fake);

	if (utf8 != NULL) {
		assert(utf8->nrefs >= 1);
		utf8->nrefs++;
		unlockUTF();
		if (fake != (Utf8Const*)buf) {
			gc_free(fake);
		}
		return(utf8);
	}
	unlockUTF();

	hitCounter(&utf8newalloc, "utf8-new-alloc");
	/* Not in table; create new Utf8Const struct */
	if ((char *) fake == buf) {
		utf8 = gc_malloc(sizeof(Utf8Const) + len + 1, KGC_ALLOC_UTF8CONST);
		if (!utf8) {
			return NULL;
		}
		memcpy((char *) utf8->data, s, len);
		((char*)utf8->data)[len] = '\0';
		utf8->hash = hash;
		utf8->length = len;
	} else {
		utf8 = fake;
	}
	
	utf8->nrefs = 1;

	/* Add to hash table */
	lockUTF();
	temp = (Utf8Const *) hashAdd(hashTable, utf8);

	/* 
	 * temp == 0    -> hash table couldn't resize, return 0
	 * temp != utf8 -> other thread beat us, drop our utf8
	 *		   add additional ref to other utf8
	 */

	if (temp != NULL && temp != utf8) {
		temp->nrefs++;
	}

	unlockUTF();

	if (temp == NULL || temp != utf8) {
		gc_free(utf8);
	}

	assert(temp == 0 || temp->nrefs > 0);
	return (temp);
}

/*
 * Add a reference to a Utf8Const.
 */
void
utf8ConstAddRef(Utf8Const *utf8)
{
	lockUTF();
	assert(utf8->nrefs >= 1);
	utf8->nrefs++;
	unlockUTF();
}

/*
 * Release a Utf8Const.
 */
void
utf8ConstRelease(Utf8Const *utf8)
{
	/* NB: we ignore zero utf8s here in order to not having to do it at
	 * the call sites, such as when destroying half-processed class 
	 * objects because of error conditions.
	 */
	if (utf8 == 0) {
		return;
	}
	lockUTF();
	assert(utf8->nrefs >= 1);
	if (--utf8->nrefs == 0) {
		hitCounter(&utf8release, "utf8-release");
		hashRemove(hashTable, utf8);
	}
	unlockUTF();
	if (utf8->nrefs == 0)
		gc_free(utf8);
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
 * Encode a jchar[] Array into a zero-terminated C string
 * that contains the array's utf8 encoding.
 *
 * NB.: This function assumes the output array has a sufficient size.
Caller must free via KFREE.
 */
void utf8ConstEncodeTo(const jchar *chars, int clength, char *buf)
{
	int i, pos = 0;

	for (i = 0; i < clength; i++) {
		jchar ch = chars[i];
		if (ch >= 0x0001 && ch <= 0x007f) {
			buf[pos++] = (char) ch;
		} else if (ch <= 0x07ff) {
			buf[pos++] = (char) (0xc0 | (0x3f & (ch >> 6)));
			buf[pos++] = (char) (0x80 | (0x3f &  ch));
		} else {
			buf[pos++] = (char) (0xe0 | (0x0f & (ch >> 12)));
			buf[pos++] = (char) (0x80 | (0x3f & (ch >>  6)));
			buf[pos++] = (char) (0x80 | (0x3f &  ch));
		}
	}
}

/*
 * Encode a jchar[] Array into a zero-terminated C string
 * that contains the array's utf8 encoding.
 *
 * NB.: Caller must free via KFREE.
 */
char*
utf8ConstEncode(const jchar *chars, int clength)
{
        char *buf;
	int i;
	unsigned int size = 0;
	
	/* Size output array */
	for (i = 0; i < clength; i++) {
		jchar ch = chars[i];
		if (ch >= 0x0001 && ch <= 0x007f) {
			size++;
		} else if (ch <= 0x07ff) {
			size += 2;
		} else {
			size += 3;
		}
	}

	/* Now fill it in */
	buf = KMALLOC(size + 1);
	if (buf == NULL) {
		return NULL;
	}

	utf8ConstEncodeTo(chars, clength, buf);
	
	return buf;
}

/*
 * Initialize utf8const support system
 */
void
utf8ConstInit(void)
{
	DBG(INIT, dprintf("utf8ConstInit()\n"); );

	initStaticLock(&utf8Lock);

	lockUTF();
	hashTable = hashInit(utf8ConstHashValueInternal,
		utf8ConstCompare, UTFmalloc, UTFfree);
	assert(hashTable != NULL);
	unlockUTF();

	DBG(INIT, dprintf("utf8ConstInit() done\n"); );
}

bool
utf8ConstEqual(Utf8Const* a, Utf8Const* b)
{
	assert(a != NULL);
	assert(a->nrefs >= 1);
	assert(b != NULL);
	assert(b->nrefs >= 1);

#ifdef KAFFEH
	/* Do the full compare (Kaffeh doesn't intern Utf8s) */
	return (0 == strcmp(a->data, b->data));
#else
#ifdef KAFFE_VMDEBUG
	/* If they're different pointers, double check that they're different strings... */
	if ((a != b) && (a->hash == b->hash))
	{
		assert(strcmp(a->data,b->data) != 0);
	}
#endif
	/* Since we intern all UTF-8 constants, we can do this: */
	return (a == b);
#endif
}
