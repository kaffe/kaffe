/*
 * readClass.c
 * Read in a new class.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "debug.h"
#include "gtypes.h"
#include "file.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "errors.h"
#include "debug.h"
#include "readClass.h"
#include "classMethod.h"
#include "code.h"
#include "utf8const.h"

Hjava_lang_Class*
readClass(Hjava_lang_Class* classThis, classFile* fp, struct Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	u2 minor_version;
	u2 major_version;
	u4 magic;
	u2 access_flags;
	u2 this_class;
	u2 super_class;

	/* CLASS_CNAME(classThis) won't work until after 'setupClass', below */
	const char* className = NULL;

	if (! checkBufSize(fp, 4+2+2, className, einfo))
		return NULL;

	/* Read in class info */
	readu4(&magic, fp);
	if (magic != JAVAMAGIC) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError), 
				    "Bad magic number 0x%x", magic);
		return NULL;
	}
	readu2(&minor_version, fp);
	readu2(&major_version, fp);

	/* Note, can't print CLASS_CNAME(classThis), as name isn't initialized yet... */

	DBG(READCLASS,
	    dprintf("major=%d, minor=%d\n",
		    major_version, minor_version);
		);

	if (! ((major_version == MAJOR_VERSION_V1_1 && minor_version == MINOR_VERSION_V1_1) ||
	       (major_version == MAJOR_VERSION_V1_2 && minor_version == MINOR_VERSION_V1_2) ||
	       (major_version == MAJOR_VERSION_V1_3 && minor_version == MINOR_VERSION_V1_3) ||
	       (major_version == MAJOR_VERSION_V1_4 && minor_version == MINOR_VERSION_V1_4))) {
		postExceptionMessage(einfo,
				     JAVA_LANG(UnsupportedClassVersionError),
				     "%d.%d",
				     major_version,
				     minor_version);
	}

	if (readConstantPool(classThis, fp, einfo) == false) {
		return NULL;
	}

	if (! checkBufSize(fp, 2+2+2, className, einfo))
		return NULL;

	readu2(&access_flags, fp);
	readu2(&this_class, fp);
	readu2(&super_class, fp);

	if (! setupClass(classThis,
			 this_class, super_class, access_flags,
			 loader, einfo)) {
		return (NULL);
	}

	/* CLASS_CNAME(classThis) is now defined. */

	if (readInterfaces(fp, classThis, einfo) == false ||
	    readFields(fp, classThis, einfo) == false ||
	    readMethods(fp, classThis, einfo) == false ||
	    readAttributes(fp, classThis, READATTR_CLASS, classThis, einfo) == false) {
		return NULL;
	}

	return (classThis);
}

/*
 * Read in interfaces.
 */
bool
readInterfaces(classFile* fp, Hjava_lang_Class* this, errorInfo *einfo)
{
	Hjava_lang_Class** interfaces;
	u2 interfaces_count;
	u2 i;				

	if (! checkBufSize(fp, 2, CLASS_CNAME(this), einfo))
		return false;

	readu2(&interfaces_count, fp);
	DBG(READCLASS,
	    dprintf("%s: interfaces_count=%d\n",
		    CLASS_CNAME(this), interfaces_count);
		);

	if (interfaces_count == 0) {		
		return true;
	}			

	if (! checkBufSize(fp, (u2)(interfaces_count * 2), CLASS_CNAME(this), einfo))
		return false;

	interfaces = (Hjava_lang_Class**)
		gc_malloc(sizeof(Hjava_lang_Class**) * interfaces_count, KGC_ALLOC_INTERFACE);
	if (interfaces == NULL) {
		postOutOfMemory(einfo);
		return false;	
	}				

	for (i = 0; i < interfaces_count; i++)
	{
		u2 iface;
		readu2(&iface, fp);
		/* Will be converted from idx to Class* in processClass() */
		interfaces[i] = (Hjava_lang_Class*) (size_t) iface;
	}

	if (!gc_add_ref(interfaces)) {
		postOutOfMemory(einfo);
		return false;
	}

	addInterfaces(this, interfaces_count, interfaces);
		
	return true;
}

/*
 * Read in fields.
 */
bool
readFields(classFile* fp, Hjava_lang_Class* this, errorInfo *einfo)
{
	u2 i;
	u2 fields_count;

	if (! checkBufSize(fp, 2, CLASS_CNAME(this), einfo))
		return false;

	readu2(&fields_count, fp);
	DBG(READCLASS,
	    dprintf("%s: fields_count=%d\n", CLASS_CNAME(this), fields_count);
		);

	if( !startFields(this, fields_count, einfo) )
		return false;

	for (i = 0; i < fields_count; i++) {
		Field* fieldThis;
		u2 access_flags;
		u2 name_index;
		u2 signature_index;
		
		if (! checkBufSize(fp, 2+2+2, CLASS_CNAME(this), einfo))
			return false;

		readu2(&access_flags, fp);
		readu2(&name_index, fp);
		readu2(&signature_index, fp);
		fieldThis = addField(this, access_flags, name_index, signature_index, einfo);

		if (fieldThis == NULL)
			return false;

		if (! readAttributes(fp, this, READATTR_FIELD, fieldThis, einfo))
			return false;
	}

	finishFields(this);
	
	return (true);
}

/*
 * Read in attributes.
 */
bool
readAttributes(classFile* fp, Hjava_lang_Class* this,
	       ReadAttrType thingType, void* thing, errorInfo *einfo)
{
	u2 i;
	u2 cnt;

	if (! checkBufSize(fp, 2, CLASS_CNAME(this), einfo))
		return false;

	readu2(&cnt, fp);
	DBG(READCLASS,
	    dprintf("%s: attributes_count=%d\n",
		    CLASS_CNAME(this),
		    cnt);
		);

	for (i = 0; i < cnt; i++) {
		u2 idx;
		u4 len;

		if (! checkBufSize(fp, 2+4, CLASS_CNAME(this), einfo))
			return false;

		readu2(&idx, fp);
		readu4(&len, fp);

		if (! checkBufSize(fp, len, CLASS_CNAME(this), einfo))
			return false;

		if (CLASS_CONST_TAG(this, idx) == CONSTANT_Utf8) {
			Utf8Const* name;
			name = WORD2UTF(CLASS_CONST_DATA (this, idx));
			DBG(READCLASS,
			    dprintf("%s: parsing attr %s on %s\n", CLASS_CNAME(this), name->data,
				    (thingType == READATTR_METHOD) ? "Method"
				    : ((thingType == READATTR_CLASS) ? "Class"
				       : ((thingType == READATTR_FIELD) ? "Field"
					  : "unknown enum element")));
				);
			if (utf8ConstEqual(name, Code_name)
			    && (thingType == READATTR_METHOD)) {
				if (! addCode((Method*)thing, (size_t) len, fp, einfo)) {
					return false;
				}
			}
			else if (utf8ConstEqual(name, LineNumberTable_name)
				 && (thingType == READATTR_METHOD)) {
				if (!addLineNumbers((Method*)thing,
						    (size_t) len, fp, einfo)) {
					return false;
				}
			}
			else if (utf8ConstEqual(name, LocalVariableTable_name)
				 && (thingType == READATTR_METHOD)) {
				if (!addLocalVariables((Method*)thing,
						      (size_t) len, fp, einfo)) {
					return false;
				}
			}
			else if (utf8ConstEqual(name, ConstantValue_name)
				 && (thingType == READATTR_FIELD)) {
				readu2(&idx, fp);
				setFieldValue(this, (Field*)thing, idx);
			}
			else if (utf8ConstEqual(name, Exceptions_name)
				&& (thingType == READATTR_METHOD)) {
				if (!addCheckedExceptions((Method*)thing,
							 (size_t) len, fp, einfo)) {
					return false;
				}
			}
			else if (utf8ConstEqual(name, SourceFile_name)
				&& (thingType == READATTR_CLASS)) {
				readu2(&idx, fp);
				if (! addSourceFile((Hjava_lang_Class*)thing, idx, einfo)) {
					return false;
				}
			}
			else if (utf8ConstEqual(name, InnerClasses_name)
				 && (thingType == READATTR_CLASS)) {
				if(! addInnerClasses((Hjava_lang_Class*)thing,
						    (size_t) len, fp, einfo)) {
					return false;
				}
			}
			else {
				DBG(READCLASS,
				    dprintf("%s: don't know how to parse %s on %s\n",
					    CLASS_CNAME(this), name->data,
					    (thingType == READATTR_METHOD) ? "Method"
					    : ((thingType == READATTR_CLASS) ? "Class"
					       : ((thingType == READATTR_FIELD) ? "Field"
						  : "unknown enum element")));
				    );

		seekm(fp, len);
	}
		}
		else {
			/* XXX should this throw an exception? */
			DBG(READCLASS,
			    dprintf("%s: WARNING! Skipping broken(?) attribute (name is not a Utf8 constant).\n",
				    CLASS_CNAME(this)));
			
			seekm(fp, len);
		}
	}
	return true;
}

/*
 * Read in methods.
 */
bool
readMethods(classFile* fp, Hjava_lang_Class* this, errorInfo *einfo)
{
	u2 i;
	u2 methods_count;

	if (! checkBufSize(fp, 2, CLASS_CNAME(this), einfo))
		return false;

	readu2(&methods_count, fp);

	DBG(READCLASS,
	    dprintf("%s: methods_count=%d\n", CLASS_CNAME(this), methods_count);
		);

	if( !startMethods(this, methods_count, einfo) )
		return false;

	for (i = 0; i < methods_count; i++) {
		Method* methodThis;
		u2 access_flags;
		u2 name_index;
		u2 signature_index;

		if (! checkBufSize(fp, 2+2+2, CLASS_CNAME(this), einfo))
			return false;

		readu2(&access_flags, fp);
		readu2(&name_index, fp);
		readu2(&signature_index, fp);

		methodThis = addMethod(this, access_flags, name_index, signature_index,
				       einfo);

		if (methodThis == NULL) {
			return false;
		}

		if (readAttributes(fp, this, READATTR_METHOD, methodThis, einfo) == false) {
			return false;
		}
	}
#ifdef KAFFEH	
	finishMethods(this);
#endif
	return (true);
}
