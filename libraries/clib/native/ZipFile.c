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
#include "errors.h"
#include "exception.h"
#include "gc.h"
#include "gtypes.h"
#include "itypes.h"
#include "jar.h"

#include "java_util_zip_ZipEntry.h"
#include "java_util_zip_ZipFile.h"
#include "java_util_Vector.h"

static Hjava_util_zip_ZipEntry* makeZipEntry(jarEntry*);

struct Hkaffe_util_Ptr*
java_util_zip_ZipFile_openZipFile0(Hjava_lang_String* fname)
{
	jarFile* zip;
	char* str;

	str = checkPtr(stringJava2C(fname));
	zip = openJarFile(str);
	gc_free(str);
	return ((struct Hkaffe_util_Ptr*)zip);
}

void
java_util_zip_ZipFile_closeZipFile0(struct Hkaffe_util_Ptr* zip)
{
	closeJarFile((jarFile*)zip);
}

int
java_util_zip_ZipFile_getZipFileSize0(struct Hkaffe_util_Ptr* zip)
{
	return ((jarFile*)zip)->count;
}

struct Hjava_util_zip_ZipEntry*
java_util_zip_ZipFile_getZipEntry0(struct Hkaffe_util_Ptr* zip, Hjava_lang_String* zname)
{
	jarEntry* entry;
	char* str;
	Hjava_util_zip_ZipEntry* zentry;

	str = checkPtr(stringJava2C(zname));
	entry = lookupJarFile((jarFile*)zip, str);
	gc_free(str);
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
	uint8* buf = 0;
	jlong size;

	size = unhand(zentry)->size;

	if (size < 0) {
                throwException(NegativeArraySizeException);
        }
	if( size > 0 )
	{
		entry.fileName = '\0';
		entry.uncompressedSize = size;
		entry.compressionMethod = unhand(zentry)->method;
		entry.compressedSize = unhand(zentry)->csize;
		entry.localHeaderOffset = unhand(zentry)->offset;
		
		buf = getDataJarFile((jarFile*)zip, &entry);
		if (buf == 0) {
			return (0);
		}
	}
	array = (HArrayOfByte*)AllocArray((size_t)size, TYPE_Byte);
	if( buf )
	{
		memcpy(unhand_array(array)->body, buf, unhand(zentry)->size);
		gc_free(buf);
	}
	return (array);
}

Hjava_util_Vector*
java_util_zip_ZipFile_getZipEntries0(struct Hkaffe_util_Ptr* zip)
{
	Hjava_util_Vector* vec;
	jarFile* zfile;
	jarEntry* entry;
	HObject** elems;
	int i = 0;
	unsigned int j;

	zfile = (jarFile*)zip;
	vec = (Hjava_util_Vector*)execute_java_constructor("java.util.Vector",
	    0, 0, "(I)V", zfile->count);
	elems = unhand_array(unhand(vec)->elementData)->body;
	for (j = 0; j < zfile->tableSize; j++) {
		entry = zfile->table[j];
		while( entry )
		{
			elems[i] = (HObject*)makeZipEntry(entry);
			i++;
			entry = entry->next;
		}
	}
	unhand(vec)->elementCount = zfile->count;

	return (vec);
}

static
Hjava_util_zip_ZipEntry*
makeZipEntry(jarEntry* entry)
{
	Hjava_util_zip_ZipEntry* zentry;

	zentry = (Hjava_util_zip_ZipEntry*)
	    execute_java_constructor("java.util.zip.ZipEntry", 0, 0, "()V");
	unhand(zentry)->name =
	    checkPtr(stringC2Java(entry->fileName));
	unhand(zentry)->crc = 0;
	unhand(zentry)->size = entry->uncompressedSize;
	unhand(zentry)->method = entry->compressionMethod;
	unhand(zentry)->extra = 0;
	unhand(zentry)->comment = 0;
	unhand(zentry)->flag = 0;
	unhand(zentry)->version = 0;
	unhand(zentry)->csize = entry->compressedSize;
	unhand(zentry)->offset = entry->localHeaderOffset;
	unhand(zentry)->dosTime = entry->dosTime;

	return (zentry);
}
