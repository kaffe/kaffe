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

#ifndef kaffevm_readclass_h
#define kaffevm_readclass_h

/*
 * Used in kaffeh and kaffevm.
 */

#define	JAVAMAGIC	0xCafeBabe
#define	MAJOR_VERSION_V1_1	45
#define	MINOR_VERSION_V1_1	3
#define	MAJOR_VERSION_V1_2	46
#define	MINOR_VERSION_V1_2	0
#define	MAJOR_VERSION_V1_3	47
#define	MINOR_VERSION_V1_3	0
#define	MAJOR_VERSION_V1_4	48
#define	MINOR_VERSION_V1_4	0

typedef enum ReadAttrType {
	READATTR_METHOD = 11,
	READATTR_CLASS  = 12,
 	READATTR_FIELD  = 13
} ReadAttrType;

struct Hjava_lang_Class;
struct Hjava_lang_ClassLoader;
struct _errorInfo;

struct Hjava_lang_Class* readClass(struct Hjava_lang_Class*, classFile*, struct Hjava_lang_ClassLoader*, struct _errorInfo*);
bool readInterfaces(classFile*, struct Hjava_lang_Class*, struct _errorInfo*);
bool readMethods(classFile*, struct Hjava_lang_Class*, struct _errorInfo*);
bool readFields(classFile*, struct Hjava_lang_Class*, struct _errorInfo*);
bool readAttributes(classFile*, struct Hjava_lang_Class*, ReadAttrType, void*, struct _errorInfo*);

#if defined KAFFEH
void finishMethods (struct Hjava_lang_Class *this);
#endif
  
#endif /* kaffevm_readclass_h */

