/*
 * java.util.zip.ZipFile.c
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include <native.h>
#include "../../../kaffe/kaffevm/jar.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "ZipEntry.h"
#include "Vector.h"

static Hjava_util_zip_ZipEntry* makeZipEntry(jarEntry*);

struct Hkaffe_util_Ptr*
java_util_zip_ZipFile_openZipFile0(Hjava_lang_String* fname)
{
	jarFile* zip;
	char* str;

	str = makeCString(fname);
	zip = openJarFile(str);
	free(str);
	return ((struct Hkaffe_util_Ptr*)zip);
}

void
java_util_zip_ZipFile_closeZipFile0(struct Hkaffe_util_Ptr* zip)
{
	closeJarFile((jarFile*)zip);
}

struct Hjava_util_zip_ZipEntry*
java_util_zip_ZipFile_getZipEntry0(struct Hkaffe_util_Ptr* zip, Hjava_lang_String* zname)
{
	jarEntry* entry;
	char* str;
	Hjava_util_zip_ZipEntry* zentry;

	str = makeCString(zname);
	entry = lookupJarFile((jarFile*)zip, str);
	free(str);
	if (entry == 0) {
		return (0);
	}
	zentry = makeZipEntry(entry);
	return (zentry);
}

HArrayOfByte*
java_util_zip_ZipFile_getZipData0(struct Hkaffe_util_Ptr* zip, struct Hjava_util_zip_ZipEntry* zentry)
{
	jarEntry entry;
	HArrayOfByte* array;
	uint8* buf;

	entry.fileName = "";
	entry.uncompressedSize = unhand(zentry)->size;
	entry.compressionMethod = unhand(zentry)->method;
	entry.compressedSize = unhand(zentry)->csize;
	entry.dataPos = unhand(zentry)->offset;

	buf = getDataJarFile((jarFile*)zip, &entry);
	if (buf == 0) {
		return (0);
	}
	array = (HArrayOfByte*)AllocArray(unhand(zentry)->size, TYPE_Byte);
	memcpy(unhand(array)->body, buf, unhand(zentry)->size);
	return (array);
}

Hjava_util_Vector*
java_util_zip_ZipFile_getZipEntries0(struct Hkaffe_util_Ptr* zip)
{
	Hjava_util_Vector* vec;
	jarFile* zfile;
	jarEntry* entry;
	HObject** elems;
	int i;

	zfile = (jarFile*)zip;
	vec = (Hjava_util_Vector*)execute_java_constructor("java.util.Vector", 0, "(I)V", zfile->count);
	elems = unhand(unhand(vec)->elementData)->body;
	for (i = 0, entry = zfile->head; i < zfile->count; i++, entry = entry->next) {
		elems[i] = (HObject*)makeZipEntry(entry);
	}
	unhand(vec)->elementCount = zfile->count;

	return (vec);
}

static
Hjava_util_zip_ZipEntry*
makeZipEntry(jarEntry* entry)
{
	Hjava_util_zip_ZipEntry* zentry;

	zentry = (Hjava_util_zip_ZipEntry*)execute_java_constructor("java.util.zip.ZipEntry", 0, "()V");
	unhand(zentry)->name = makeJavaString(entry->fileName, strlen(entry->fileName));
	unhand(zentry)->crc = 0;
	unhand(zentry)->size = entry->uncompressedSize;
	unhand(zentry)->method = entry->compressionMethod;
	unhand(zentry)->extra = 0;
	unhand(zentry)->comment = 0;
	unhand(zentry)->flag = 0;
	unhand(zentry)->version = 0;
	unhand(zentry)->csize = entry->compressedSize;
	unhand(zentry)->offset = entry->dataPos;

	return (zentry);
}
