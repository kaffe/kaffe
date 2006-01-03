/* $Id: zipfile.h,v 1.1 2006/01/03 15:27:02 robilad Exp $

   $Log: zipfile.h,v $
   Revision 1.1  2006/01/03 15:27:02  robilad
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

   Revision 1.1.1.1  1999/12/06 03:09:11  toast
   initial checkin..



   Revision 1.6  1999/05/10 08:33:08  burnsbr
   added UNPACK_UB4 and UNPACK_UB2

   Revision 1.5  1999/04/26 02:36:34  burnsbr
   added LOC_EXTRA macro

   Revision 1.4  1999/04/23 12:01:19  burnsbr
   added more defines


*/

/*
  zipfile.h - defines for indexing zipfile headers
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

#define LOC_EXTRA   6  /* extra bytes */
#define LOC_COMP    8  /* compression method */
#define LOC_MODTIME 10 /* last modification time */
#define LOC_MODDATE 12 /* last modification date */
#define LOC_CRC     14 /* CRC */
#define LOC_CSIZE   18 /* compressed size */
#define LOC_USIZE   22 /* uncompressed size */
#define LOC_FNLEN   26 /* filename length */
#define LOC_EFLEN   28 /* extra-field length */

#define CEN_FLAGS    8
#define CEN_COMP    10 /* compression method */
#define CEN_MODTIME 12
#define CEN_MODDATE 14
#define CEN_CRC     16
#define CEN_CSIZE   20
#define CEN_USIZE   24
#define CEN_FNLEN   28
#define CEN_EFLEN   30
#define CEN_COMLEN  32
#define CEN_OFFSET  42


/* macros */
#define PACK_UB4(d, o, v) d[o] = (ub1)((v) & 0x000000ff); \
                          d[o + 1] = (ub1)(((v) & 0x0000ff00) >> 8); \
                          d[o + 2] = (ub1)(((v) & 0x00ff0000) >> 16); \
                          d[o + 3] = (ub1)(((v) & 0xff000000) >> 24)

#define PACK_UB2(d, o, v) d[o] = (ub1)((v) & 0x00ff); \
                          d[o + 1] = (ub1)(((v) & 0xff00) >> 8)

#define UNPACK_UB4(s, o) (ub4)s[o] + (((ub4)s[o + 1]) << 8) +\
                         (((ub4)s[o + 2]) << 16) + (((ub4)s[o + 3]) << 24)

#define UNPACK_UB2(s, o)  (ub2)s[o] + (((ub2)s[o + 1]) << 8)
