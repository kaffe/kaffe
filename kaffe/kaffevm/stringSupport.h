/*
 * stringSupport.h
 * String related functionality
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __stringSupport_h
#define __stringSupport_h

#include "object.h"
#include "native.h"
#include "java_lang_String.h"

/**** String routines ****/

#define STRING_SIZE(STR)	(unhand(STR)->count)
#define STRING_DATA(STR)	\
		(&(unhand(unhand(STR)->value)->body[unhand(STR)->offset]))

/* Create a String object from the supplied C (not UTF-8!) string */
extern Hjava_lang_String* stringC2Java(const char*);

/* Return the interned String object having the char array value.
   Create and intern a new String if necessary. */
extern Hjava_lang_String* stringCharArray2Java(const jchar*, int);

/* Convert a String object into a C string (truncating the higher 8 bits).
   Returns the string in an KMALLOC()'d buffer. Caller is responsible for
   calling KFREE() on the buffer when finished. */
extern char*		  stringJava2C(const Hjava_lang_String*);

/* Like stringJava2C() but use a caller-supplied buffer instead of KMALLOC() */
extern char*		  stringJava2CBuf(const Hjava_lang_String*, char*, int);

/* Create an array of Unicode characters from the supplied C string */
extern HArrayOfChar*	  stringC2CharArray(const char*);

/* Create a String object from a Utf8Const */
extern Hjava_lang_String* stringUtf82Java(const Utf8Const*);

/* Create a String object from a Utf8Const, and replace all occurrences
   of the first character with the second. */
extern Hjava_lang_String* stringUtf82JavaReplace(const Utf8Const *,
				jchar, jchar);

/* Return the interned version of the String object */
extern Hjava_lang_String* stringInternString(Hjava_lang_String*);

/* Remove the String object from the intern table. This should only be
   called during finalization of the corresponding String object. */
extern void		  stringUninternString(Hjava_lang_String*);

/**** UTF-8 routines ****/

/* Create a Utf8Const from a UTF-8 encoded array. The returned Utf8Const
   is subject to garbage collection when no pointers to it remain. */
extern Utf8Const*	utf8ConstNew(const char*, int);

/* Create a permanent Utf8Const from a UTF-8 encoded string. */
extern Utf8Const*	utf8ConstNewFixed(const char*, int);

/* Remove a Utf8Const from the intern table. This should only be called
   by the garbage collector finalization routine. */
extern void		utf8ConstDestroy(const Utf8Const*);

/* Return the length of the corresponding Unicode string */
extern int		utf8ConstUniLength(const Utf8Const*);

/* Decode a Utf8Const (to Unicode) into the buffer (which must be big enough) */
extern void		utf8ConstDecode(const Utf8Const*, jchar*);

/* Compare a Utf8Const and a String object */
extern int		utf8ConstEqualJavaString(const Utf8Const*,
				const Hjava_lang_String*);

/* Since we intern all UTF-8 constants, we can do this: */
#define utf8ConstEqual(A,B)	((A) == (B))

/* Since we compute hash values at creation time, we can do this: */
#define utf8ConstHashValue(A)	((A)->hash)

#endif
