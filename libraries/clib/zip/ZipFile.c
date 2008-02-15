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
#include "native.h"

#include <zzip/zzip.h>

#include "java_util_zip_ZipEntry.h"
#include "java_util_zip_ZipFile.h"
#include "java_util_Vector.h"

static
Hjava_util_zip_ZipEntry*
makeZipEntry(ZZIP_FILE * entry, Hjava_lang_String* zname)
{
	Hjava_util_zip_ZipEntry* zentry;
	ZZIP_STAT stat;
	zzip_fstat(entry, &stat);

	zentry = (Hjava_util_zip_ZipEntry*)
	    execute_java_constructor("java.util.zip.ZipEntry", NULL, NULL, 
				     "(Ljava/lang/String;)V", zname);
	unhand(zentry)->size = stat.st_size;
	unhand(zentry)->known |= java_util_zip_ZipEntry_KNOWN_SIZE;
	unhand(zentry)->method = stat.d_compr;
	unhand(zentry)->compressedSize = stat.d_csize;
	unhand(zentry)->dostime = -1;
	
	return (zentry);
}

struct Horg_kaffe_util_Ptr*
java_util_zip_ZipFile_openZipFile0(Hjava_lang_String* fname)
{
	ZZIP_DIR* zip;
	char* str;

	str = checkPtr(stringJava2C(fname));
	zip = zzip_opendir(str);
	gc_free(str);
	return ((struct Horg_kaffe_util_Ptr*)zip);
}

void
java_util_zip_ZipFile_closeZipFile0(struct Horg_kaffe_util_Ptr* zip)
{
	zzip_closedir((ZZIP_DIR*)zip);
}

jint
java_util_zip_ZipFile_getZipFileSize0(struct Horg_kaffe_util_Ptr* zip)
{
  jint size = 0;

  zzip_rewinddir((ZZIP_DIR *) zip);

  while(zzip_readdir((ZZIP_DIR *) zip) != NULL)
    ++size;

  return size;
}

static
ZZIP_FILE* getZipEntry(struct Horg_kaffe_util_Ptr* zip, Hjava_lang_String* zname)
{
	ZZIP_FILE* entry;
	char* str;

	str = checkPtr(stringJava2C(zname));
	entry = zzip_file_open((ZZIP_DIR*)zip, str, 0);
	gc_free(str);

	return entry;
}

struct Hjava_util_zip_ZipEntry*
java_util_zip_ZipFile_getZipEntry0(struct Horg_kaffe_util_Ptr* zip, Hjava_lang_String* zname)
{
  ZZIP_FILE* entry;
  Hjava_util_zip_ZipEntry* zentry = NULL;

  entry = getZipEntry(zip, zname);
  if (entry != NULL) {
    zentry = makeZipEntry(entry, zname);
    zzip_file_close(entry);
  }
  
  return (zentry);
}

HArrayOfByte*
java_util_zip_ZipFile_getZipData0(struct Horg_kaffe_util_Ptr* zip, struct Hjava_util_zip_ZipEntry* zentry)
{
	HArrayOfByte* array;
	uint8* buf = NULL;
	jlong size;

	size = unhand(zentry)->size;

	if (size < 0) {
                throwException(NegativeArraySizeException);
        }
	if( size > 0 )
	{
	  ZZIP_FILE *entry = getZipEntry( zip, unhand(zentry)->name);
	  if (entry != NULL) {
	    buf = getDataJarFile(entry);
	    zzip_file_close(entry);
	  }
	  if (buf == 0) {
	    return (NULL);
	  }
	}
	array = (HArrayOfByte*)AllocArray((jsize)size, TYPE_Byte);
	if( buf )
	{
		memcpy(unhand_array(array)->body, buf, (size_t)size);
		gc_free(buf);
	}
	return (array);
}

Hjava_util_Vector*
java_util_zip_ZipFile_getZipEntries0(struct Horg_kaffe_util_Ptr* zip)
{
	Hjava_util_Vector* vec;
	ZZIP_DIR* zfile;
	ZZIP_DIRENT * zip_entry = NULL; 
	HObject** elems;
	int i = 0;
	jint num_entries = java_util_zip_ZipFile_getZipFileSize0(zip);
	zfile = (ZZIP_DIR*)zip;
	
	vec = (Hjava_util_Vector*)
	  execute_java_constructor("java.util.Vector",
				   NULL, NULL, "(I)V", 
				   num_entries);
	elems = unhand_array(unhand(vec)->elementData)->body;

	zzip_rewinddir(zfile);
	do {
	  ZZIP_FILE* entry;

	  zip_entry = zzip_readdir(zfile);
	  if (NULL != zip_entry) {
	    entry = zzip_file_open(zfile, zip_entry->d_name, 0);
	    if (NULL != entry) {
	      elems[i] = (HObject*)makeZipEntry(entry, stringC2Java(zip_entry->d_name));
	      zzip_file_close(entry);
	    }
	    i++;
	  }
	}while (zip_entry != NULL);

	unhand(vec)->elementCount = num_entries;

	return (vec);
}
