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

/*
 * Define various array types
 */
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jbyte body[1]; } data[1]; } HArrayOfBoolean;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jbyte body[1]; } data[1]; } HArrayOfByte;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jchar body[1]; } data[1]; } HArrayOfChar;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jdouble body[1]; } data[1]; } HArrayOfDouble;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jfloat body[1]; } data[1]; } HArrayOfFloat;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jint body[1]; } data[1]; } HArrayOfInt;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jshort body[1]; } data[1]; } HArrayOfShort;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; jlong body[1]; } data[1]; } HArrayOfLong;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; Hjava_lang_Object* body[1]; } data[1]; } HArrayOfArray;
typedef struct { Hjava_lang_Object base; unsigned int length; union { double align; Hjava_lang_Object* body[1]; } data[1]; } HArrayOfObject;

/* Get length of arrays */
#define	obj_length(_obj)	((_obj)->length)

#endif
