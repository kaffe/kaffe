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

#define	RCDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "gtypes.h"
#include "file.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "errors.h"
#include "debug.h"
#ifdef KAFFEH
#include <readClassConfig.h>
#else
#include "readClassConfig.h"
#endif
#include "stringSupport.h"
#include "readClass.h"

Hjava_lang_Class*
readClass(Hjava_lang_Class* classThis, classFile* fp, struct Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	u2 minor_version;
	u2 major_version;
	u4 magic;
	u2 access_flags;
	u2 this_class;
	u2 super_class;

	/* Read in class info */
	readu4(&magic, fp);
	if (magic != JAVAMAGIC) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError), 
				    "Bad magic number 0x%x", magic);
		return (0);
	}
	readu2(&minor_version, fp);
	readu2(&major_version, fp);

RCDBG(	dprintf("major=%d, minor=%d\n", major_version, minor_version);	)

	if (major_version != MAJOR_VERSION) {
		dprintf("Warning: Major version number mismatch.\n");
	}
	if (minor_version != MINOR_VERSION) {
		dprintf("Warning: Minor version number mismatch.\n");
	}

	if (readConstantPool(classThis, fp, einfo) == false) {
		return (0);
	}

	readu2(&access_flags, fp);
	readu2(&this_class, fp);
	readu2(&super_class, fp);

	ADDCLASS(this_class, super_class, access_flags, constant_pool);

	if (readInterfaces(fp, classThis, einfo) == false ||
	    readFields(fp, classThis, einfo) == false ||
	    readMethods(fp, classThis, einfo) == false ||
	    readAttributes(fp, classThis, classThis, einfo) == false) {
		return (0);
	}

	return (classThis);
}

/*
 * Read in interfaces.
 */
bool
readInterfaces(classFile* fp, Hjava_lang_Class* this, errorInfo *einfo)
{
	u2 interfaces_count;

	readu2(&interfaces_count, fp);
RCDBG(	dprintf("interfaces_count=%d\n", interfaces_count);	)

#ifdef READINTERFACES
	READINTERFACES(fp, this, interfaces_count);
#else
	seekm(fp, interfaces_count * 2);
#endif
	return (true);
}

/*
 * Read in fields.
 */
bool
readFields(classFile* fp, Hjava_lang_Class* this, errorInfo *einfo)
{
	u2 i;
	u2 fields_count;
	void* fieldThis;

	readu2(&fields_count, fp);
RCDBG(	dprintf("fields_count=%d\n", fields_count);		)
	fieldThis = 0;

#if defined(READFIELD_START)
	READFIELD_START(fields_count, this);
#endif
	for (i = 0; i < fields_count; i++) {
#if defined(READFIELD)
		READFIELD(fp, this);
#else
		fseek(fp, 6, SEEK_CUR);
#endif
#if defined(READFIELD_ATTRIBUTE)
		READFIELD_ATTRIBUTE(fp, this);
#else
		if (readAttributes(fp, this, fieldThis, einfo) == false) {
			return (false);
		}
#endif
	}
#if defined(READFIELD_END)
	READFIELD_END(this);
#endif
	return (true);
}

/*
 * Read in attributes.
 */
bool
readAttributes(classFile* fp, Hjava_lang_Class* this, void* thing, errorInfo *einfo)
{
	u2 i;
	u2 cnt;

	readu2(&cnt, fp);
RCDBG(	dprintf("attributes_count=%d\n", cnt);				)

	/* Skip attributes for the moment */
	for (i = 0; i < cnt; i++) {
#ifdef READATTRIBUTE
		READATTRIBUTE(fp, this, thing);
#else
		u2 idx;
		u4 len;
		readu2(&idx, fp);
		readu4(&len, fp);
		seekm(fp, len);
#endif
	}
	return (true);
}

/*
 * Read in methods.
 */
bool
readMethods(classFile* fp, Hjava_lang_Class* this, errorInfo *einfo)
{
	u2 i;
	u2 methods_count;
	Method* methodThis;

	readu2(&methods_count, fp);
RCDBG(	dprintf("methods_count=%d\n", methods_count);		)
	methodThis = 0;

#ifdef READMETHOD_START
	READMETHOD_START(methods_count, this);
#endif
	for (i = 0; i < methods_count; i++) {
#ifdef READMETHOD
		READMETHOD(methodThis, fp, this, einfo);
		if (READMETHOD_FAILED(methodThis)) {
			return (false);
		}
#else
		fseek(fp, 6, SEEK_CUR);
#endif
		if (readAttributes(fp, this, methodThis, einfo) == false) {
			return (false);
		}
	}
#ifdef READMETHOD_END
	READMETHOD_END();
#endif
	return (true);
}
