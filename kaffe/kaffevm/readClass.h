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
#define	MAJOR_VERSION_V1_1	45
#define	MINOR_VERSION_V1_1	3
#define	MAJOR_VERSION_V1_4	46
#define	MINOR_VERSION_V1_4	0

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
struct _errorInfo;

struct Hjava_lang_Class* readClass(struct Hjava_lang_Class*, classFile*, struct Hjava_lang_ClassLoader*, struct _errorInfo*);
bool readInterfaces(classFile*, struct Hjava_lang_Class*, struct _errorInfo*);
bool readMethods(classFile*, struct Hjava_lang_Class*, struct _errorInfo*);
bool readFields(classFile*, struct Hjava_lang_Class*, struct _errorInfo*);
bool readAttributes(classFile*, struct Hjava_lang_Class*, void*, struct _errorInfo*);

#endif
