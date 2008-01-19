/*
 * string.c
 * Manage strings.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "jni_md.h"
#include "gtypes.h"
#include "constants.h"
#include "object.h"
#include "itypes.h"
#include "locks.h"
#include "baseClasses.h"
#include "hashtab.h"
#include "stringSupport.h"
#include "exception.h"

/* Internal variables */
static hashtab_t	hashTable;	/* intern hash table */
static iStaticLock	stringLock; /* mutex on all intern operations */

/* Internal functions */
static int		stringHashValue(const void *ptr);
static int		stringCompare(const void *s1, const void *s2);

/*
 * Convert a Java string into a KMALLOC()'d C string buffer.
 */
char*
stringJava2C(const Hjava_lang_String* js)
{
	char* str;
	const size_t size = (size_t)(STRING_SIZE(js) + 1);

	str = gc_malloc(size, KGC_ALLOC_FIXED);
	if (str != 0) {
		stringJava2CBuf(js, str, size);
	}
	return(str);
}

/*
 * Convert an Java string to a C string in the buffer.
 * We simply truncate the Java characters to 8 bits.
 */
char*
stringJava2CBuf(const Hjava_lang_String* js, char* cs, int len)
{
	jchar* chrs;

	if (len <= 0) {
		return(NULL);
	}
	if (js == NULL) {
		cs[0] = 0;
		return(cs);
	}
	chrs = STRING_DATA(js);
	len--;
	if (len > STRING_SIZE(js)) {
		len = STRING_SIZE(js);
	}

	while (--len >= 0) {
	  if (*chrs >= 0x0001 && *chrs <= 0x007F) {
	    *cs++ = *chrs++ & 0x7F;
	  }
	  else if (*chrs >= 0x0080 && *chrs <= 0x07FF) {
	    *cs++ = 0xC0 | ((*chrs >> 6) & 0x1F);
	    *cs++ = 0x80 | (*chrs++ & 0x3F);
	  }
	  else {
	    *cs++ = 0xE0 | ((*chrs >> 12) & 0x0F);
	    *cs++ = 0x80 | ((*chrs >> 6) & 0x3F);
	    *cs++ = 0x80 | (*chrs++ & 0x3F);
	  }
	}
	
	*cs = 0;

	return (cs);
}

/*
 * Convert a C string into a Java String.
 */
Hjava_lang_String*
stringC2Java(const char* cs)
{
	const int len = strlen(cs);
	Hjava_lang_String *string;
	jchar *ary, buf[200];
	int k;

	/* Get buffer */
	if (len * sizeof(*ary) > sizeof(buf)) {
		ary = gc_malloc(len * sizeof(*ary), KGC_ALLOC_FIXED);
		if (!ary) return NULL;
	} else {
		ary = buf;
	}

	/* Convert C chars to Java chars */
	for (k = 0; k < len; k++) {
		ary[k] = (unsigned char) cs[k];
	}

	/* Return a string containing those chars */
	string = stringCharArray2Java(ary, len);
	if (ary != buf) {
		gc_free(ary);
	}
	return(string);
}

/*
 * Convert a C string into a Java char array.
 */
HArrayOfChar*
stringC2CharArray(const char* cs)
{
	const int len = strlen(cs);
	HArrayOfChar* ary;
	int k;
	errorInfo info;

	/* Get new array object */
	ary = (HArrayOfChar*)newArrayChecked(TYPE_CLASS(TYPE_Char),
					     (jsize)len, &info);
	if (!ary) {
		discardErrorInfo(&info);
		return NULL;
	}
	
	/* Convert C chars to Java chars */
	for (k = 0; k < len; k++) {
		unhand_array(ary)->body[k] = (unsigned char) cs[k];
	}
	return(ary);
}

/*
 * Create a String object from a UTF-8 string.
 *
 * This function is here instead of in utf8const.c to
 * avoid kaffeh linkage problems.
 */
Hjava_lang_String*
utf8Const2Java(const Utf8Const *utf8)
{
	Hjava_lang_String* string;

	string = utf8Const2JavaReplace(utf8, 0, 0);
	unhand(string)->cachedHashCode = utf8->hash;	/* why not, they're equal */
	return(string);
}

/*
 * Create a String object from a UTF-8 string, replacing every
 * occurrence of "from_ch" with "to_ch". If "from_ch" is zero,
 * then no substitution is done.
 *
 * This function is here instead of in utf8const.c to
 * avoid kaffeh linkage problems.
 */
Hjava_lang_String*
utf8Const2JavaReplace(const Utf8Const *utf8, jchar from_ch, jchar to_ch)
{
	const int uniLen = utf8ConstUniLength(utf8);
	Hjava_lang_String* string;
	jchar *jc, buf[200];
	int k;

	/* Get buffer */
	if (uniLen * sizeof(jchar) > sizeof(buf)) {
		jc = gc_malloc(uniLen * sizeof(*jc), KGC_ALLOC_FIXED);
		if (!jc) return NULL;
	} else {
		jc = buf;
	}

	/* Convert UTF-8 to Unicode */
	utf8ConstDecode(utf8, jc);

	/* Substitute chars */
	if (from_ch != 0) {
		for (k = 0; k < uniLen; k++) {
			if (jc[k] == from_ch) {
				jc[k] = to_ch;
			}
		}
	}

	/* Convert to a String object */
	string = stringCharArray2Java(jc, uniLen);
	if (jc != buf) {
		gc_free(jc);
	}
	return(string);
}

/* Create a Utf8Const object from a String object after replacing all
 * occurrences of the first character with the second.
 *
 * NB.: unlike a conversion via stringJava2C -> utf8ConstNew, this will   
 * preserve unicode characters properly, including the '\u0000' character.
 */
Utf8Const* 
stringJava2Utf8ConstReplace(Hjava_lang_String *str, jchar from, jchar to) 
{
	int slength = STRING_SIZE(str);
	jchar * chars = STRING_DATA(str);
	char *utf8buf;
	Utf8Const* utf8;

	/* convert characters only if necessary */
	if (slength != 0 && from != to) {
		int i;
		chars = gc_malloc(sizeof(jchar) * slength, KGC_ALLOC_FIXED);

		for (i = 0; i < slength; i++) {
			jchar ci = ((jchar *)STRING_DATA(str))[i];

			if (from == ci) {
				ci = to;
			} 
			chars[i] = ci;
		}
	}

	utf8buf = utf8ConstEncode(chars, slength);

	if (chars != STRING_DATA(str)) {
		gc_free(chars);
	}

	if (utf8buf == 0) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}

	utf8 = utf8ConstFromString(utf8buf);
	KFREE(utf8buf);
	return (utf8);
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

/*
 * Define functions used by the string hashtable to resize itself.
 * The problem is that we may block in gc_malloc/gc_free and the gc may kick
 * in.  The collector, however, must be able to call stringUninternString
 * while destroying strings.  If we held the lock while this is happening,
 * we would deadlock.
 */
static void*
stringAlloc(size_t sz)
{
	void* p;

	/* XXX assumes stringLock isn't acquired recursively (which it isn't) */
	unlockStaticMutex(&stringLock);
	p = gc_malloc(sz, KGC_ALLOC_FIXED);
	lockStaticMutex(&stringLock);
	return p;
}

static void
stringFree(const void *ptr)
{
	/* XXX assumes stringLock isn't acquired recursively (which it isn't) */
        unlockStaticMutex(&stringLock);	
	gc_free((void *) ptr);
	lockStaticMutex(&stringLock);
}

/**
 * Return the interned version of the String if
 * it has been already interned, or NULL otherwise.
 */
static Hjava_lang_String *
string_isInterned(Hjava_lang_String *string)
{
  Hjava_lang_String * result = NULL;

  /* Lock intern table */
  lockStaticMutex(&stringLock);

  /* See if string is already in the table */
  if (hashTable != NULL)
    result = hashFind(hashTable, string);

  unlockStaticMutex(&stringLock);

  return(result);
}

/*
 * Return the interned version of a String object.
 * May or may not be the same String.
 */
Hjava_lang_String *
stringInternString(Hjava_lang_String *string)
{
	Hjava_lang_String *temp;

	temp = string_isInterned(string);

	if(temp != NULL)
	  return temp;

	/* Lock intern table */
	lockStaticMutex(&stringLock);

	/* See if string is already in the table */
	if (hashTable == NULL) {
		hashTable = hashInit(stringHashValue, stringCompare, stringAlloc, stringFree);
		assert(hashTable != NULL);
	}

	/* Not in table, so add it */
	temp = hashAdd(hashTable, string);
	if (!temp) {
		unlockStaticMutex(&stringLock);
		return temp;
	}
	assert(temp == string);

	/* Unlock table and return new string */
	unlockStaticMutex(&stringLock);
	return(string);
}

/*
 * Called by String destructor to remove an interned string
 * from the hash table.
 */
void
stringUninternString(Hjava_lang_String* string)
{
	if (!string_isInterned(string))
	  return;

	lockStaticMutex(&stringLock);
	hashRemove(hashTable, string);
	unlockStaticMutex(&stringLock);
}

/*
 * This does exactly what java.lang.String.hashCode() does, only faster.
 * Note: there is a race condition, but this is OK because the same value
 * is getting written no matter who writes it (strings are immutable).
 */
static int
stringHashValue(const void *ptr)
{
	Hjava_lang_String *string = (Hjava_lang_String*) ptr;
	jint hash;
	int k;

	if (unhand(string)->cachedHashCode != 0) {
		return(unhand(string)->cachedHashCode);
	}
	for (k = hash = 0; k < STRING_SIZE(string); k++) {
	       hash = (31 * hash) + STRING_DATA(string)[k];
	}
	unhand(string)->cachedHashCode = hash;
	return(hash);
}

/*
 * Comparison function for the hash table.
 */
static int
stringCompare(const void *v1, const void *v2)
{
	const Hjava_lang_String *const s1 = v1;
	const Hjava_lang_String *const s2 = v2;
	const int len = STRING_SIZE(s1);
	int k;

	if (STRING_SIZE(s1) != STRING_SIZE(s2)) {
		return(1);
	}
	if (unhand(s1)->cachedHashCode != 0
	    && unhand(s2)->cachedHashCode != 0
	    && unhand(s1)->cachedHashCode != unhand(s2)->cachedHashCode) {
		return(1);
	}
	for (k = 0; k < len; k++) {
		if (STRING_DATA(s1)[k] != STRING_DATA(s2)[k]) {
			return(1);
		}
	}
	return(0);
}

/*
 * This returns a String object containing the supplied characters.
 * If there is already such a String in the intern table, return it.
 * Otherwise, create a new String object, intern it, and return it.
 *
 * The idea with "fake" is that you avoid creating an extra String object
 * only to find out there's already an interned version, and then forget it,
 * which causes extra GC work.
 */
Hjava_lang_String*
stringCharArray2Java(const jchar *data, int len)
{
        Hjava_lang_String *string;
	HArrayOfChar *ary;
	errorInfo info;

	/* Lock intern table 
	 * NB: we must not hold stringLock when we call gc_malloc/gc_free!
	 */

	/* Look for it already in the intern hash table */
	if (hashTable != NULL) {
		Hjava_lang_String fakeString;
		HArrayOfChar *fakeAry;
		unsigned char buf[200];

		/* Construct a temporary and fake String object */
		if (sizeof(*fakeAry) + (len * sizeof(*data)) > sizeof(buf)) {
			fakeAry = gc_malloc(sizeof(*fakeAry) + len * sizeof(*data),
					    KGC_ALLOC_FIXED);
		} else {
			fakeAry = (HArrayOfChar*)buf;
		}
		if (!fakeAry) return NULL;
		
		memset(fakeAry, 0, sizeof(*fakeAry));
		memcpy(unhand_array(fakeAry)->body, data, len * sizeof(*data));
		obj_length(fakeAry) = len;

		/* Attach fake array to fake string */
		memset(&fakeString, 0, sizeof(fakeString));
		unhand(&fakeString)->value = fakeAry;
		unhand(&fakeString)->count = len;

		/* Return existing copy of this string, if any */
		lockStaticMutex(&stringLock);
		string = (Hjava_lang_String *) hashFind(hashTable, &fakeString);
		unlockStaticMutex(&stringLock);

		if (fakeAry != (HArrayOfChar*)buf) {
			gc_free(fakeAry);
		}
		if (string != NULL) {
			return(string);
		}
	}

	/* Create a new String object */
	ary = (HArrayOfChar*)newArrayChecked(charClass, (jsize)len,
					     &info);
	if (!ary) {
		discardErrorInfo(&info);
		return NULL;
	}
	
	memcpy(ARRAY_DATA(ary), data, len * sizeof(jchar));
	string = (Hjava_lang_String*)newObjectChecked(getStringClass(), &info);
	if (!string) {
		discardErrorInfo(&info);
		return NULL;
	}
	unhand(string)->value = ary;
	unhand(string)->count = len;

	/* Intern and return string */
	/* NB: the string returned might not be the string we created,
	 * but we don't care if we lose the race.  The string created by the
	 * loser will be picked up by the gc.
	 */
	string = stringInternString(string);
	return (string);
}

/*                      
 * Walk a java.lang.String object
 */     
void
stringWalk(Collector* collector, void *gc_info, void* str, uint32 size UNUSED)
{
	Hjava_lang_String *objstr = (Hjava_lang_String *)str;
	iLock *lk;

        /* That's all we have to do here */
	KGC_markObject(collector, gc_info, unhand(objstr)->value);

        lk = GET_HEAVYLOCK(unhand(objstr)->base.lock);
	if (lk != NULL && KGC_getObjectIndex(collector, lk) == KGC_ALLOC_LOCK)
	  KGC_markObject(collector, gc_info, lk);
}

/*       
 * Destroy a string object.
 */      
void
/* ARGSUSED */
stringDestroy(Collector* collector UNUSED, void* obj)
{
        Hjava_lang_String* str = (Hjava_lang_String*)obj;

        /* unintern this string if necessary */
        if (string_isInterned(str)) {
                stringUninternString(str);
        }       
}

/*
 * Initialize string support system
 */
void
stringInit(void)
{
  initStaticLock(&stringLock);
}
