/*
 * stringSupport.h
 * String related functionality
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.	 All rights reserved.
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
		(&(unhand_array(unhand(STR)->value)->body[unhand(STR)->offset]))

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

/* Return the interned version of the String object */
extern Hjava_lang_String* stringInternString(Hjava_lang_String*);

/* Remove the String object from the intern table. This should only be
   called during destruction of the corresponding String object. */
extern void		  stringUninternString(Hjava_lang_String*);

/* Have the collector walk/destroy a string */
struct _Collector;
extern void    		  stringWalk(struct _Collector*, void*, uint32);
extern void    		  stringDestroy(struct _Collector*, void*);

/* Initialize string support system */
extern void		  stringInit(void);

/**** UTF-8 routines ****/

/* Create a Utf8Const from a UTF-8 encoded array. The returned pointer
   remains valid until Utf8ConstRelease() is called. */
extern Utf8Const*	  utf8ConstNew(const char*, int);

/* Add a reference to a Utf8Const */
extern void		  utf8ConstAddRef(Utf8Const*);

/* Release a Utf8Const previously created via utf8ConstNew(). */
extern void		  utf8ConstRelease(Utf8Const*);

/* Assign a Utf8Const B to A and do proper reference counting */
#define utf8ConstAssign(A, B) 			\
	do {					\
		if ((A) != 0) {			\
			utf8ConstRelease((A));	\
		}				\
		utf8ConstAddRef((B));		\
		(A) = (B);			\
	} while (0)

/* Create a String object from a Utf8Const */
extern Hjava_lang_String* utf8Const2Java(const Utf8Const*);

/* Create a String object from a Utf8Const, and replace all occurrences
   of the first character with the second. */
extern Hjava_lang_String* utf8Const2JavaReplace(const Utf8Const *,
				jchar, jchar);

/* Return the length of the corresponding Unicode string */
extern int		  utf8ConstUniLength(const Utf8Const*);

/* Check if a string is a valid UTF-8 string */
extern int		  utf8ConstIsValidUtf8(const char *, unsigned int);

/* Decode a Utf8Const (to Unicode) into the buffer (which must be big enough) */
extern void		  utf8ConstDecode(const Utf8Const*, jchar*);

/* Compare a Utf8Const and a String object */
extern int		  utf8ConstEqualJavaString(const Utf8Const*,
				const Hjava_lang_String*);

/* Since we intern all UTF-8 constants, we can do this: */
#define utf8ConstEqual(A,B)	((A) == (B))

/* Since we compute hash values at creation time, we can do this: */
#define utf8ConstHashValue(A)	((A)->hash)

/* Initialize utf8const support system */
extern void		  utf8ConstInit(void);

#endif
