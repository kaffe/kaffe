/*
 * readClass.c
 * Read in a new class.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include <gtypes.h>			/* Ugh! */
#include <file.h>			/* Ugh! */
#include <access.h>			/* Ugh! */
#include "object.h"
#include <constants.h>			/* Ugh! */
#include <readClassConfig.h>		/* Ugh! */
#include "readClass.h"

Hjava_lang_Class*
readClass(Hjava_lang_Class* classThis, classFile* fp, struct Hjava_lang_ClassLoader* loader)
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
		fprintf(stderr, "Bad magic %x in class\n", magic);
		EXIT(1);
	}
	readu2(&minor_version, fp);
	readu2(&major_version, fp);

DBG(	printf("major=%d, minor=%d\n", major_version, minor_version);	)

	if (major_version != MAJOR_VERSION) {
		fprintf(stderr, "Warning: Major version number mismatch.\n");
	}
	if (minor_version != MINOR_VERSION) {
		fprintf(stderr, "Warning: Minor version number mismatch.\n");
	}

	readConstantPool(classThis, fp);

	readu2(&access_flags, fp);
	readu2(&this_class, fp);
	readu2(&super_class, fp);

	ADDCLASS(this_class, super_class, access_flags, constant_pool);

	readInterfaces(fp, classThis);
	readFields(fp, classThis);
	readMethods(fp, classThis);
	readAttributes(fp, classThis, classThis);

	return (classThis);
}

/*
 * Read in interfaces.
 */
void
readInterfaces(classFile* fp, Hjava_lang_Class* this)
{
	u2 interfaces_count;

	readu2(&interfaces_count, fp);
DBG(	printf("interfaces_count=%d\n", interfaces_count);	)

#ifdef READINTERFACES
	READINTERFACES(fp, this, interfaces_count);
#else
	seekm(fp, interfaces_count * 2);
#endif
}

/*
 * Read in fields.
 */
void
readFields(classFile* fp, Hjava_lang_Class* this)
{
	u2 i;
	u2 fields_count;
	void* fieldThis;

	readu2(&fields_count, fp);
DBG(	printf("fields_count=%d\n", fields_count);		)
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
		readAttributes(fp, this, fieldThis);
#endif
	}
#if defined(READFIELD_END)
	READFIELD_END(this);
#endif
}

/*
 * Read in attributes.
 */
void
readAttributes(classFile* fp, Hjava_lang_Class* this, void* thing)
{
	u2 i;
	u2 cnt;

	readu2(&cnt, fp);
DBG(	printf("attributes_count=%d\n", cnt);				)

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
}

/*
 * Read in methods.
 */
void
readMethods(classFile* fp, Hjava_lang_Class* this)
{
	u2 i;
	u2 methods_count;
	Method* methodThis;

	readu2(&methods_count, fp);
DBG(	printf("methods_count=%d\n", methods_count);		)
	methodThis = 0;

#ifdef READMETHOD_START
	READMETHOD_START(methods_count, this);
#endif
	for (i = 0; i < methods_count; i++) {
#ifdef READMETHOD
		READMETHOD(fp, this);
#else
		fseek(fp, 6, SEEK_CUR);
#endif
		readAttributes(fp, this, methodThis);
	}
#ifdef READMETHOD_END
	READMETHOD_END();
#endif
}
