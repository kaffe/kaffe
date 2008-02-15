/*
 * jar.c
 * Handle JAR input files.
 *
 * Copyright (c) 2008
 *   Dalibor Topic  <robilad@kaffe.org>
 *
 * This file is licensed under the terms of the GNU Public License.
 * (v2 or any later version)
 * See the file "license.terms" for restrictions on redistribution 
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "kaffe/jmalloc.h"
#include "jar.h"

void *getDataJarFile(ZZIP_FILE *je)
{
  zzip_ssize_t size = getUncompressedSize(je);

  if (-1 != size) {
    void * buf = KMALLOC(size);
    if (NULL != buf)
      if (-1 != zzip_read(je, buf, size))
	return buf;
  }

  return NULL;
}

zzip_ssize_t getUncompressedSize(ZZIP_FILE * je) 
{
  /* Stores information about the file. */
  ZZIP_STAT stat;

  if (-1 != zzip_fstat(je, &stat))
    return stat.st_size;

  /* If entry can't be found, return -1. */
  return -1;
}


