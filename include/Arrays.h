/*
 * array.h
 * What do our internal Java arrays look like.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __array_h
#define __array_h

#define ARRAY_FIELDS \
	unsigned int		length;	\
	double			align[0]

struct Array {
	Hjava_lang_Object	base;
	ARRAY_FIELDS;
};

/*
 * Define various array types
 */
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jbyte body[1]; } data[1]; } HArrayOfBoolean;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jbyte body[1]; } data[1]; } HArrayOfByte;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jchar body[1]; } data[1]; } HArrayOfChar;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jdouble body[1]; } data[1]; } HArrayOfDouble;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jfloat body[1]; } data[1]; } HArrayOfFloat;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jint body[1]; } data[1]; } HArrayOfInt;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jshort body[1]; } data[1]; } HArrayOfShort;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jlong body[1]; } data[1]; } HArrayOfLong;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { Hjava_lang_Object* body[1]; } data[1]; } HArrayOfArray;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { Hjava_lang_Object* body[1]; } data[1]; } HArrayOfObject;

/* Get length of arrays */
#define	obj_length(_obj)	((_obj)->length)

#endif
