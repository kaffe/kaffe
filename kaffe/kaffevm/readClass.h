/*
 * readClass.h
 * Various bits of information in a Java class file.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __readclass_h
#define __readclass_h

#define	JAVAMAGIC	0xCafeBabe
#define	MAJOR_VERSION	45
#define	MINOR_VERSION	3

typedef struct _field_info {
	u2		access_flags;
	u2		name_index;
	u2		signature_index;
} field_info;

typedef struct _method_info {
	u2		access_flags;
	u2		name_index;
	u2		signature_index;
} method_info;

struct Hjava_lang_Class;
struct Hjava_lang_ClassLoader;
struct _methods;

struct Hjava_lang_Class* readClass(struct Hjava_lang_Class*, classFile*, struct Hjava_lang_ClassLoader*);
void readInterfaces(classFile*, struct Hjava_lang_Class*);
void readMethods(classFile*, struct Hjava_lang_Class*);
void readFields(classFile*, struct Hjava_lang_Class*);
void readAttributes(classFile*, struct Hjava_lang_Class*, void*);

#endif
