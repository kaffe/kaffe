
/*
 * java.lang.NativeLibrary.c
 *
 * Copyright (c) 2000
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 2000
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "defs.h"
#include "gtypes.h"
#include "object.h"
#include "constants.h"
#include "support.h"
#include "external.h"
#include "java_lang_String.h"
#include "java_lang_NativeLibrary.h"
#include <native.h>

#ifndef LIBRARYPREFIX
#define	LIBRARYPREFIX	"lib"
#endif

#ifndef LIBRARYSUFFIX
#define LIBRARYSUFFIX	""
#endif

/*
 * Return prefix to prepend to library names
 */
struct Hjava_lang_String*
java_lang_NativeLibrary_getLibPrefix()
{
	return checkPtr(stringC2Java(LIBRARYPREFIX));
}

/*
 * Return suffix to append to library names
 */
struct Hjava_lang_String*
java_lang_NativeLibrary_getLibSuffix()
{
	return checkPtr(stringC2Java(LIBRARYSUFFIX));
}

/*
 * Attempt to link in a shared library. Throws an UnsatisfiedLinkError
 * if the attempt fails.
 */
int
java_lang_NativeLibrary_linkLibrary(struct Hjava_lang_String *jpath)
{
	char path[MAXPATHLEN];
	char errbuf[128];
	errorInfo einfo;
	int i = -1;

	stringJava2CBuf(jpath, path, sizeof(path));
	if ((i = loadNativeLibrary(path, errbuf, sizeof(errbuf))) < 0) {
 		if( strstr(errbuf, "File not found") ) {
 			postExceptionMessage(&einfo,
 					     JAVA_IO(FileNotFoundException),
 					     "%s",
 					     path);
 		} else {
			postExceptionMessage(&einfo, 
					     JAVA_LANG(UnsatisfiedLinkError),
					     "%s",
					     errbuf);
			throwError(&einfo);
		}
	}
 	if( i == -1 )
 		throwError(&einfo);

	return i;
}

/*
 * Unlink a shared library.
 */
void
java_lang_NativeLibrary_unlinkLibrary(int i)
{
	unloadNativeLibrary(i);
}

