/*
 * itypes.h
 * Internal types.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __itypes_h
#define __itypes_h

#include "gtypes.h"

#define	TYPE_Boolean	4
#define	TYPE_Char	5
#define	TYPE_Float	6
#define	TYPE_Double	7
#define	TYPE_Byte	8
#define	TYPE_Short	9
#define	TYPE_Int	10
#define	TYPE_Long	11
#define	TYPE_Ref	15
#define	TYPE_Bad	17

#define	MAXTYPES	16

extern struct Hjava_lang_Class* types[MAXTYPES];

extern struct Hjava_lang_Class* _Jv_intClass;
extern struct Hjava_lang_Class* _Jv_longClass;
extern struct Hjava_lang_Class* _Jv_booleanClass;
extern struct Hjava_lang_Class* _Jv_charClass;
extern struct Hjava_lang_Class* _Jv_floatClass; 
extern struct Hjava_lang_Class* _Jv_doubleClass;
extern struct Hjava_lang_Class* _Jv_byteClass; 
extern struct Hjava_lang_Class* _Jv_shortClass;     
extern struct Hjava_lang_Class* _Jv_voidClass;

#define	TYPE_CLASS(t)		types[t]

extern void finishTypes(void);
extern void initTypes(void);

#endif
