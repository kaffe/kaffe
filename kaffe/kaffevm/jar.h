/*
 * jar.h
 * Handle JAR input files.
 *
 * Copyright (c) 2000, 2001, 2002 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 *
 * Copyright (c) 2008
 *   Dalibor Topic  <robilad@kaffe.org>
 *
 * This file is licensed under the terms of the GNU Public License.  
 * See the file "license.terms" for restrictions on redistribution 
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef __jar_h
#define __jar_h

#include <zzip/zzip.h>

/*
 * Read in and, if necessary, decompress the contents of a file stored in a
 * JAR file.
 *
 * @param je file to load from the JAR file.
 * @return contents of the file or NULL if there was an error.
 */
void *getDataJarFile(ZZIP_FILE *je);

/**
 * Get the uncompressed size of a JAR file entry.
 *
 * @param je JAR file entry
 * @return its uncompresses length in bytes, or -1 in case of an error.
 */
zzip_ssize_t getUncompressedSize(ZZIP_FILE * je);

#endif
