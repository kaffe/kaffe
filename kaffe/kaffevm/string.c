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
#include "jtypes.h"
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
static iLock*		stringLock;	/* mutex on all intern operations */
static int *            stringLockRoot;	/* the string lock is not a monitor */

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

	str = KMALLOC(STRING_SIZE(js) + 1);
	if (str != 0) {
		stringJava2CBuf(js, str, STRING_SIZE(js) + 1);
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
	cs[len] = 0;
	while (--len >= 0) {
		*cs++ = (char)*chrs++;
	}
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
		ary = KMALLOC(len * sizeof(*ary));
		if (!ary) return 0;
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
		KFREE(ary);
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
					     len, &info);
	if (!ary) return 0;
	
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
	unhand(string)->hash = utf8->hash;	/* why not, they're equal */
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
		jc = KMALLOC(uniLen * sizeof(*jc));
		if (!jc) return 0;
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
		KFREE(jc);
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
		chars = KMALLOC(sizeof(jchar) * slength);

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
		KFREE(chars);
	}

	if (utf8buf == 0) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}

	utf8 = utf8ConstNew(utf8buf, -1);
	KFREE(utf8buf);
	return (utf8);
}

/*
 * Define functions used by the string hashtable to resize itself.
 * The problem is that we may block in KCALLOC/KFREE and the gc may kick
 * in.  The collector, however, must be able to call stringUninternString
 * while destroying strings.  If we held the lock while this is happening,
 * we would deadlock.
 */
static void*
stringAlloc(size_t sz)
{
	void* p;
	int *myRoot = stringLockRoot;

	_unlockMutex(&stringLock, myRoot);
	p = KCALLOC(1, sz);
	_lockMutex(&stringLock, myRoot);
	stringLockRoot = myRoot;
	return (p);
}

static void
stringFree(const void *ptr)
{
	int *myRoot = stringLockRoot;

	_unlockMutex(&stringLock, myRoot);
	KFREE(ptr);
	_lockMutex(&stringLock, myRoot);
	stringLockRoot = myRoot;
}

/*
 * Return the interned version of a String object.
 * May or may not be the same String.
 */
Hjava_lang_String *
stringInternString(Hjava_lang_String *string)
{
	int iLockRoot;
	Hjava_lang_String *temp;

	/* Lock intern table */
	lockStaticMutex(&stringLock);
	stringLockRoot = &iLockRoot;

	/* See if string is already in the table */
	if (hashTable != NULL) {
		Hjava_lang_String *string2;

		if ((string2 = hashFind(hashTable, string)) != NULL) {
			unlockStaticMutex(&stringLock);
			return(string2);
		}
	} else {
		hashTable = hashInit(stringHashValue, stringCompare, stringAlloc, stringFree);
		assert(hashTable);
	}

	/* Not in table, so add it */
	temp = hashAdd(hashTable, string);
	if (!temp) {
		unlockStaticMutex(&stringLock);
		return temp;
	}
	assert(temp == string);
	unhand(string)->interned = true;

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
	int iLockRoot;

	lockStaticMutex(&stringLock);
	stringLockRoot = &iLockRoot;
	hashRemove(hashTable, string);
	unhand(string)->interned = false;
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
	Hjava_lang_String *const string = (Hjava_lang_String*) ptr;
	jint hash;
	int k;

	if (unhand(string)->hash != 0) {
		return(unhand(string)->hash);
	}
	for (k = hash = 0; k < STRING_SIZE(string); k++) {
	       hash = (31 * hash) + STRING_DATA(string)[k];
	}
	unhand(string)->hash = hash;
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
	if (unhand(s1)->hash != 0
	    && unhand(s2)->hash != 0
	    && unhand(s1)->hash != unhand(s2)->hash) {
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
	int iLockRoot;

	/* Lock intern table 
	 * NB: we must not hold stringLock when we call KMALLOC/KFREE!
	 */

	/* Look for it already in the intern hash table */
	if (hashTable != NULL) {
		Hjava_lang_String *string, fakeString;
		HArrayOfChar *fakeAry;
		unsigned char buf[200];

		/* Construct a temporary and fake String object */
		if (sizeof(*fakeAry) + (len * sizeof(*data)) > sizeof(buf)) {
			fakeAry = KMALLOC(sizeof(*fakeAry)
					+ (len * sizeof(*data)));
		} else {
			fakeAry = (HArrayOfChar*)buf;
		}
		if (!fakeAry) return 0;
		
		memset(fakeAry, 0, sizeof(*fakeAry));
		memcpy(unhand_array(fakeAry)->body, data, len * sizeof(*data));
		obj_length(fakeAry) = len;

		/* Attach fake array to fake string */
		memset(&fakeString, 0, sizeof(fakeString));
		unhand(&fakeString)->value = fakeAry;
		unhand(&fakeString)->count = len;

		/* Return existing copy of this string, if any */
		lockStaticMutex(&stringLock);
		stringLockRoot = &iLockRoot;
		string = hashFind(hashTable, &fakeString);
		unlockStaticMutex(&stringLock);

		if (fakeAry != (HArrayOfChar*)buf) {
			KFREE(fakeAry);
		}
		if (string != NULL) {
			return(string);
		}
	}

	/* Create a new String object */
	ary = (HArrayOfChar*)newArrayChecked(charClass, len,
					     &info);
	if (!ary) return 0;
	
	memcpy(ARRAY_DATA(ary), data, len * sizeof(jchar));
	string = (Hjava_lang_String*)newObjectChecked(StringClass, &info);
	if (!string) return 0;
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
stringWalk(Collector* collector, void* str, uint32 size)
{
        /* That's all we have to do here */
        GC_markObject(collector, unhand((Hjava_lang_String*)str)->value);
}

/*       
 * Destroy a string object.
 */      
void
/* ARGSUSED */
stringDestroy(Collector* collector, void* obj)
{
        Hjava_lang_String* str = (Hjava_lang_String*)obj;

        /* unintern this string if necessary */
        if (unhand(str)->interned == true) {
                stringUninternString(str);
        }       
}

/*
 * Initialize string support system
 */
void
stringInit(void)
{
}
