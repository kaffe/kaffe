/* java_io_VMFile.c - Native methods for java.io.File class
   Copyright (C) 1998, 2004, 2006 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

/* do not move; needed here because of some macro definitions */
#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <jni.h>
#include <jcl.h>

#include "java_io_VMFile.h"

/*************************************************************************/

/*
 * These two methods are used to maintain dynamically allocated
 * buffers for getCanonicalPath without the overhead of calling
 * realloc every time a buffer is modified.  Buffers are sized
 * at the smallest multiple of CHUNKSIZ that is greater than or
 * equal to the desired length.  The default CHUNKSIZ is 256,
 * longer than most paths, so in most cases a getCanonicalPath
 * will require only one malloc per buffer.
 */

#define CHUNKLOG 8
#define CHUNKSIZ (1 << CHUNKLOG)

static int
nextChunkSize (int size)
{
  return ((size >> CHUNKLOG) + ((size & (CHUNKSIZ - 1)) ? 1 : 0)) << CHUNKLOG;
}

static char *
maybeGrowBuf (JNIEnv *env, char *buf, int *size, int required)
{
  if (required > *size)
    {
      *size = nextChunkSize (required);
      buf = JCL_realloc (env, buf, *size);
    }
  return buf;
}

/*************************************************************************/

/*
 * This method converts a path to canonical form on GNU/Posix systems.
 * This involves the removal of redundant separators, references to
 * "." and "..", and symbolic links.
 *
 * The conversion proceeds on a component-by-component basis: symbolic
 * links and references to ".."  are resolved as and when they occur.
 * This means that if "/foo/bar" is a symbolic link to "/baz" then the
 * canonical form of "/foo/bar/.." is "/" and not "/foo".
 *
 * In order to mimic the behaviour of proprietary JVMs, non-existant
 * path components are allowed (a departure from the normal GNU system
 * convention).  This means that if "/foo/bar" is a symbolic link to
 * "/baz", the canonical form of "/non-existant-directory/../foo/bar"
 * is "/baz".
 *
 * Class:     java_io_VMFile
 * Method:    toCanonicalForm
 * Signature: (Ljava/lang/String)Ljava/lang/String
 */

JNIEXPORT jstring JNICALL
Java_java_io_VMFile_toCanonicalForm (JNIEnv *env,
				     jclass class __attribute__ ((__unused__)),
				     jstring jpath)
{
#ifndef WITHOUT_FILESYSTEM
  const char *path;
  char *src, *dst;
  int srci, dsti;
  int srcl, dstl;
  int len;
  int fschecks;
#if defined (HAVE_LSTAT) && defined (HAVE_READLINK)
  struct stat sb;
#endif /* HAVE_LSTAT && HAVE_READLINK */

  path = JCL_jstring_to_cstring (env, jpath);
  if (path == NULL)
    return NULL;

  /* It is the caller's responsibility to ensure the path is absolute. */
  if (path[0] == 0 || path[0] != '/')
    {
      JCL_free_cstring (env, jpath, path);
      JCL_ThrowException (env, "java/lang/RuntimeException", "Not absolute");
      return NULL;
    }

  len = strlen (path);
  srcl = nextChunkSize (len + 1);
  src = JCL_malloc (env, srcl);
  if (src == NULL)
    {
      JCL_free_cstring (env, jpath, path);
      return NULL;
    }
  strcpy (src, path);
  JCL_free_cstring (env, jpath, path);
  srci = 1;

  dstl = nextChunkSize (2);  
  dst = JCL_malloc (env, dstl);
  if (dst == NULL)
    {
      JCL_free (env, src);
      return NULL;
    }
  dst[0] = '/';
  dsti = 1;

  fschecks = JNI_TRUE;

  while (src[srci] != '\0')
    {
      int tmpi, dsti_save;

      /* Skip slashes. */
      while (src[srci] == '/')
	srci++;
      tmpi = srci;
      /* Find next slash. */
      while (src[srci] != '/' && src[srci] != '\0')
	srci++;
      if (srci == tmpi)
	/* We hit the end. */
	break;
      len = srci - tmpi;

      /* Handle "." and "..". */
      if (len == 1 && src[tmpi] == '.')
	continue;
      if (len == 2 && src[tmpi] == '.' && src[tmpi + 1] == '.')
	{
	  while (dsti > 1 && dst[dsti - 1] != '/')
	    dsti--;
	  if (dsti != 1)
	    dsti--;
	  /* Reenable filesystem checking if disabled, as we might
	   * have reversed over whatever caused the problem before.
	   * At least one proprietary JVM has inconsistencies because
	   * it does not do this.
	   */
	  fschecks = JNI_TRUE;
	  continue;
	}

      /* Handle real path components. */
      dst = maybeGrowBuf (env,
			  dst, &dstl, dsti + (dsti > 1 ? 1 : 0) + len + 1);
      if (dst == NULL)
	{
	  JCL_free (env, src);
	  return NULL;
	}
      dsti_save = dsti;
      if (dsti > 1)
	dst[dsti++] = '/';
      strncpy (&dst[dsti], &src[tmpi], len);
      dsti += len;
      if (fschecks == JNI_FALSE)
	continue;

#if defined (HAVE_LSTAT) && defined (HAVE_READLINK)
      dst[dsti] = '\0';
      if (lstat (dst, &sb) == 0)
	{
	  if (S_ISLNK (sb.st_mode))
	    {
	      int tmpl = CHUNKSIZ;
	      char *tmp = JCL_malloc (env, tmpl);
	      if (tmp == NULL)
		{
		  JCL_free (env, src);
		  JCL_free (env, dst);
		  return NULL;
		}

	      while (1)
		{
		  tmpi = readlink (dst, tmp, tmpl);
		  if (tmpi < 1)
		    {
		      JCL_free (env, src);
		      JCL_free (env, dst);
		      JCL_free (env, tmp);
		      JCL_ThrowException (env, "java/io/IOException",
					  "readlink failed");
		      return NULL;
		    }
		  if (tmpi < tmpl)
		    break;
		  tmpl += CHUNKSIZ;
		  tmp = JCL_realloc (env, tmp, tmpl);
		}

	      /* Prepend the link's path to src. */
	      tmp = maybeGrowBuf (env,
				  tmp, &tmpl, tmpi + strlen (&src[srci]) + 1);
	      if (tmp == NULL)
		{
		  JCL_free (env, src);
		  JCL_free (env, dst);
		  return NULL;
		}

	      strcpy (&tmp[tmpi], &src[srci]);
	      JCL_free (env, src);
	      src = tmp;
	      srcl = tmpl;
	      srci = 0;

	      /* Either replace or append dst depending on whether the
	       * link is relative or absolute.
	       */
	      dsti = src[0] == '/' ? 1 : dsti_save;
	    }
	}
      else
	{
	  /* Something doesn't exist, or we don't have permission to
	   * read it, or a previous path component is a directory, or
	   * a symlink is looped.  Whatever, we can't check the
	   * filesystem any more.
	   */
	  fschecks = JNI_FALSE;
	}
#endif /* HAVE_LSTAT && HAVE_READLINK */
    }
  dst[dsti] = '\0';

  jpath = (*env)->NewStringUTF (env, dst);
  JCL_free (env, src);
  JCL_free (env, dst);
  return jpath;
#else /* not WITHOUT_FILESYSTEM */
  return NULL;
#endif /* not WITHOUT_FILESYSTEM */
}
