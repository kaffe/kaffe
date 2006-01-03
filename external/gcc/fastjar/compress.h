/* $Id: compress.h,v 1.1 2006/01/03 15:27:01 robilad Exp $

   $Log: compress.h,v $
   Revision 1.1  2006/01/03 15:27:01  robilad
   Merged in fastjar

   2006-01-03 Dalibor Topic  <robilad@kaffe.org>

           * Makefile.am: (SUBDIRS) Added external subdir.
           (DIST_SUBDIRS) Added external subdir.

           * THIRDPARTY: Added information on fastjar. Adapted information on
           zlib.

           * configure.ac: (--with-internal-zlib) Replaced by
           (--with-system-zlib), so that kaffe now uses the merged in
           zlib by default, like gcc does.
           (--disable-fastjar) New option.
           Added fastjar subdir for configuration, and the Makefile for output.

           * kaffe/scripts/jar.in: Delegate to fastjar

           * libraries/clib/Makefile.am (SUBDIRS): Removed zlib.

           * libraries/clib/zip/Makefile.am (COND_INTERNAL_ZLIB) Replaced by
           (COND_SYSTEM_ZLIB). Adapted to use internal zlib from external/gcc
           directory.

           * libraries/javalib/Makefile.am: Regenerated.

           * libraries/javalib/vmspecific/org/kaffe/tools/jar/Jar.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFile.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileInputStream.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileOutputStream.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileReader.java
           libraries/javalib/vmspecific/org/kaffe/tools/jar/XPFileWriter.java:
           Removed.

           * libraries/clib/zlib : Moved over to external/gcc/zlib.

           * external/gcc/fastjar: New files. Merged in from gcc 4.0.2.
           Slightly adapted the build system to behave well when merged into Kaffe.

   Revision 1.1  2000/12/09 03:08:23  apbianco
   2000-12-08  Alexandre Petit-Bianco  <apbianco@cygnus.com>

           * fastjar: Imported.

   Revision 1.1.1.1  1999/12/06 03:09:12  toast
   initial checkin..



   Revision 1.3  1999/05/10 08:32:09  burnsbr
   added new function protos.

   Revision 1.2  1999/04/23 12:02:20  burnsbr
   added licence

   Revision 1.1  1999/04/23 11:59:37  burnsbr
   Initial revision


*/

/*
  compress.h - header for compression
  Copyright (C) 1999  Bryan Burns
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* Initializes the compression data structure(s) */
void init_compression(void);

/* Compresses the file specified by in_fd and appends it to out_fd */
int compress_file(int, int, struct zipentry *, struct zipentry *);

/* Frees memory used by compression function */
void end_compression(void);

void init_inflation(void);
int inflate_file(pb_file *, int, struct zipentry *);
void end_inflation(void);
Bytef *inflate_string(pb_file *, ub4 *, ub4 *);
