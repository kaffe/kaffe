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
#include "utf8const.h"
#include "java_lang_String.h"

/**** String routines ****/

#define STRING_SIZE(STR)	(unhand(STR)->count)
#define STRING_DATA(STR)	\
		(&(unhand_array(unhand(STR)->value)->body[unhand(STR)->offset]))

/* Return the interned String object having the char array value.
   Create and intern a new String if necessary. */
extern Hjava_lang_String* stringCharArray2Java(const jchar*, int);

/* Create a Utf8Const object from a String object after replacing all 
   occurrences of the first character with the second. 
   NB.: unlike a conversion via stringJava2C -> utf8ConstNew, this will
   preserve unicode characters properly, including the '\u0000' character.
 */
extern Utf8Const* stringJava2Utf8ConstReplace(Hjava_lang_String *,
				jchar, jchar);

/* Return the length of the corresponding Unicode string */
/* Create an array of Unicode characters from the supplied C string */
extern HArrayOfChar*	  stringC2CharArray(const char*);

/* Return the interned version of the String object */
extern const Hjava_lang_String* stringInternString(Hjava_lang_String*);

/* Remove the String object from the intern table. This should only be
   called during destruction of the corresponding String object. */
extern void		  stringUninternString(Hjava_lang_String*);

/* Have the collector walk/destroy a string */
struct _Collector;
extern void    		  stringWalk(struct _Collector*, void*, void*, uint32);
extern void    		  stringDestroy(struct _Collector*, void*);

/* Initialize string support system */
extern void		  stringInit(void);

/**** UTF-8 routines (see also utf8const.h) ****/

/* Create a String object from a Utf8Const */
extern Hjava_lang_String* utf8Const2Java(const Utf8Const*);

/* Create a String object from a Utf8Const, and replace all occurrences
   of the first character with the second. */
extern Hjava_lang_String* utf8Const2JavaReplace(const Utf8Const *,
				jchar, jchar);

#endif
