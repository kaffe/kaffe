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
typedef struct HArrayOfBoolean { Hjava_lang_Object base; jsize length; union { double align; jboolean body[1]; } data[1]; } HArrayOfBoolean;
typedef struct HArrayOfByte { Hjava_lang_Object base; jsize length; union { double align; jbyte body[1]; } data[1]; } HArrayOfByte;
typedef struct HArrayOfChar { Hjava_lang_Object base; jsize length; union { double align; jchar body[1]; } data[1]; } HArrayOfChar;
typedef struct HArrayOfDouble { Hjava_lang_Object base; jsize length; union { double align; jdouble body[1]; } data[1]; } HArrayOfDouble;
typedef struct HArrayOfFloat { Hjava_lang_Object base; jsize length; union { double align; jfloat body[1]; } data[1]; } HArrayOfFloat;
typedef struct HArrayOfInt { Hjava_lang_Object base; jsize length; union { double align; jint body[1]; } data[1]; } HArrayOfInt;
typedef struct HArrayOfShort { Hjava_lang_Object base; jsize length; union { double align; jshort body[1]; } data[1]; } HArrayOfShort;
typedef struct HArrayOfLong { Hjava_lang_Object base; jsize length; union { double align; jlong body[1]; } data[1]; } HArrayOfLong;
typedef struct HArrayOfArray { Hjava_lang_Object base; jsize length; union { double align; Hjava_lang_Object* body[1]; } data[1]; } HArrayOfArray;
typedef struct HArrayOfObject { Hjava_lang_Object base; jsize length; union { double align; Hjava_lang_Object* body[1]; } data[1]; } HArrayOfObject;

/* Get length of arrays */
#define	obj_length(_obj)	((_obj)->length)
#define unhand_array(_arr)	((_arr)->data)
#define unhand_byte_array(_arr)		((jbyte *)((_arr)->data))
#define unhand_char_array(_arr)		((jchar *)((_arr)->data))
#define unhand_double_array(_arr)	((jdouble *)((_arr)->data))
#define unhand_float_array(_arr)	((jfloat *)((_arr)->data))
#define unhand_int_array(_arr)		((jint *)((_arr)->data))
#define unhand_short_array(_arr)	((jshort *)((_arr)->data))
#define unhand_long_array(_arr)		((jlong *)((_arr)->data))
#define unhand_array_array(_arr)	((Hjava_lang_Object **)((_arr)->data))
#define unhand_object_array(_arr)	((Hjava_lang_Object *)((_arr)->data))

#endif
