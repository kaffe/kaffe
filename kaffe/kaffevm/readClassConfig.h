/*
 * readClassConfig.h
 * Configure the class reader.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __readclassconfig_h
#define __readclassconfig_h

#include "classMethod.h"
#include "errors.h"
#include "lookup.h"
#include "exception.h"
#include "code.h"
#include "slots.h"

/*
 * Add a class to the system.
 */
#define	ADDCLASS(this, super, access, constants)			\
	classThis = setupClass(classThis, this, super, access, loader);	\
	if (classThis == 0) {						\
                throwException(ClassFormatError);			\
	}

/*
 * Add the interfaces.
 */
#define	READINTERFACES(fp, this, count)					\
	do {								\
		Hjava_lang_Class** interfaces;				\
		u2 iface;						\
		u2 i;							\
		if (count == 0) {					\
			return;						\
		}							\
		interfaces = (Hjava_lang_Class**)			\
			gc_malloc(sizeof(Hjava_lang_Class**) * count, GC_ALLOC_INTERFACE);\
		for (i = 0; i < count; i++) {				\
			readu2(&iface, fp);				\
			interfaces[i] = (Hjava_lang_Class*) (size_t) iface; \
		}							\
		GC_WRITE(this, interfaces);				\
		addInterfaces(this, count, interfaces);			\
	} while(0)

#define	READFIELD_START(count, this) \
	do {								\
		CLASS_NFIELDS(this) = count;				\
		CLASS_FSIZE(this) = count;				\
		CLASS_FIELDS(this) = count == 0 ? (Field*)0		\
			: (Field*) gc_malloc(sizeof(Field) * count, GC_ALLOC_FIELD);\
 	} while (0)

/*
 * Read in a field.
 */
#define	READFIELD(fp, this)						\
	do {								\
		field_info f;						\
		readu2(&f.access_flags, fp);				\
		readu2(&f.name_index, fp);				\
		readu2(&f.signature_index, fp);				\
		fieldThis = (void*)addField(this, &f);			\
	} while (0)

#define	READFIELD_END(this) finishFields(this)

#define READMETHOD_START(METHODS_COUNT, THIS_CLASS)			\
	do {								\
		THIS_CLASS->methods =					\
			gc_malloc(sizeof(Method)*(METHODS_COUNT), GC_ALLOC_METHOD);\
		GC_WRITE(THIS_CLASS, THIS_CLASS->methods);		\
		THIS_CLASS->nmethods = 0;				\
	} while (0)

/*
 * Read in a method.
 */
#define	READMETHOD(fp, this)						\
	do {								\
		method_info m;						\
		readu2(&m.access_flags, fp);				\
		readu2(&m.name_index, fp);				\
		readu2(&m.signature_index, fp);				\
		methodThis = addMethod(this, &m);			\
	} while(0)

/*
 * Finished reading in methods.
 */
#define	READMETHOD_END() /* nothing */

/*
 * Process the attributes.
 */
#define	READATTRIBUTE(fp, this, thing)					\
	do {								\
		u2 idx;							\
		u4 len;							\
		Utf8Const* name;					\
		readu2(&idx, fp);					\
		readu4(&len, fp);					\
		if (CLASS_CONST_TAG(this, idx) == CONSTANT_Utf8) {	\
			name = WORD2UTF(CLASS_CONST_DATA (this, idx));	\
			if (equalUtf8Consts(name, Code_name)) {		\
				addCode((Method*)thing, len, fp);	\
			}						\
			else if (equalUtf8Consts(name, LineNumberTable_name)){ \
				addLineNumbers((Method*)thing, len, fp);\
			}						\
			else if (equalUtf8Consts(name, ConstantValue_name)){ \
				readu2(&idx, fp);			\
				setFieldValue((Field*)thing, idx);	\
			}						\
			else if (equalUtf8Consts(name, Exceptions_name)){ \
				addCheckedExceptions((Method*)thing, len, fp);\
			}						\
			else {						\
				seekm(fp, len);				\
			}						\
		}							\
		else {							\
			seekm(fp, len);					\
		}							\
	} while(0)

#endif
