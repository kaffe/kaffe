/* java_lang_String.h
 * Java's Strings.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __java_lang_string_h
#define __java_lang_string_h

typedef struct Hjava_lang_String {
	Hjava_lang_Object	base;
	struct {
		HArrayOfChar*	value;
		jint		offset;
		jint		count;
	} data[1];
} Hjava_lang_String;

/* Get length of string */
#define	javaStringLength(_str)	(unhand(_str)->count)

#endif
