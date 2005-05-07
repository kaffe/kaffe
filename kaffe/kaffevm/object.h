/*
 * object.h
 * Object representation.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __object_h
#define __object_h

#include <stddef.h>
#include "gtypes.h"
#include "gc.h"

#include "java_lang_Object.h"
#include "Arrays.h"

#define	OBJECT_DATA(OBJ)	((void*)((Hjava_lang_Object*)(OBJ)+1))

/* Number of elements. */
#define	ARRAY_SIZE(ARRAY)	(((HArrayOfObject*)(ARRAY))->length)
#define	ARRAY_DATA(ARRAY)	((void*)&(((HArrayOfObject*)(ARRAY))->data))
#define	OBJARRAY_DATA(ARRAY)	((Hjava_lang_Object**)&(((HArrayOfObject*)(ARRAY))->data))

/*
 * These bizzare casts provide various offset into the object structure.
 */
#define	OBJECT_DTABLE_OFFSET	((int)&(*(Hjava_lang_Object*)0).vtable)
#define	ARRAY_SIZE_OFFSET	((int)&ARRAY_SIZE(0))
#define	ARRAY_DATA_OFFSET	((int)ARRAY_DATA(0))

struct Hjava_lang_Class;
struct _errorInfo;

Hjava_lang_Object*	newObjectChecked(struct Hjava_lang_Class*,
					 struct _errorInfo *);
Hjava_lang_Object*	newObject(struct Hjava_lang_Class*);
struct Hjava_lang_Class* newClass(void);
Hjava_lang_Object*	newArrayChecked(struct Hjava_lang_Class*, jsize,
					struct _errorInfo *);
Hjava_lang_Object*	newArray(struct Hjava_lang_Class*, jsize);
Hjava_lang_Object*	newMultiArrayChecked(struct Hjava_lang_Class*, int*,
					     struct _errorInfo *);
Hjava_lang_Object*	newMultiArray(struct Hjava_lang_Class*, int*);

#endif
