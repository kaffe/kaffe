/* ???.h - ???
   Copyright (C) 1998 Free Software Foundation, Inc.

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

/*
Description: Linux target defintions of file functions
Systems    : all
*/

#ifndef __TARGET_NATIVE_FILE__
#define __TARGET_NATIVE_FILE__

/****************************** Includes *******************************/
/* do not move; needed here because of some macro definitions */
#include <config.h>

#include <stdlib.h>

#include "jsyscall.h"

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/****************************** Macros *********************************/

/***************************** Functions *******************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_OPEN
* Purpose    : open a file
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : file is created if it does not exist
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_OPEN
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #define TARGET_NATIVE_FILE_OPEN(filename,filedescriptor,flags,permissions,result) \
    do { \
      int kopen_result; \
      kopen_result=KOPEN(filename, \
			 flags, \
			 permissions, \
			 &filedescriptor); \
      if (kopen_result == 0) \
        fcntl (filedescriptor,F_SETFD,FD_CLOEXEC); \
      result=(kopen_result == 0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
   } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_CLOSE
* Purpose    : close a file
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_CLOSE
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_CLOSE(filedescriptor,result) \
    do  { \
      result=(KCLOSE(filedescriptor)==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
   } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_AVAILABLE
* Purpose    : get available bytes for read
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_AVAILABLE
  #if defined(HAVE_FSTAT)
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #define TARGET_NATIVE_FILE_AVAILABLE(filedescriptor,length,result) \
      do { \
        struct stat __statBuffer; \
        off_t       __n; \
        \
        length=0; \
        \
        if (KFSTAT(filedescriptor,&__statBuffer)==0) \
        { \
          if (S_ISREG(__statBuffer.st_mode)) \
          { \
            int klseek_result; \
            klseek_result=(KLSEEK(filedescriptor,0,SEEK_CUR, &__n));	\
            if (klseek_result == 0) \
            { \
              length=TARGET_NATIVE_MATH_INT_INT32_TO_INT64(__statBuffer.st_size-__n); \
              result=TARGET_NATIVE_OK; \
            } \
            else \
            { \
              result=TARGET_NATIVE_ERROR; \
            } \
          } \
          else \
          { \
              length=TARGET_NATIVE_MATH_INT_INT32_TO_INT64(0); \
              result=TARGET_NATIVE_OK; \
          } \
        } \
        else \
        { \
          result=TARGET_NATIVE_ERROR; \
        } \
      } while (0)
  #elif defined(HAVE_SELECT)
    #include <string.h>
    #include <sys/select.h>
    #define TARGET_NATIVE_FILE_AVAILABLE(filedescriptor,length,result) \
      do { \
        fd_set         __filedescriptset; \
        struct timeval __timeval; \
	int kselect_result; \
	int num_available; \
        \
        length=0; \
        \
        FD_ZERO(&__filedescriptset); \
        FD_SET(filedescriptor,&__filedescriptset); \
        memset(&__timeval,0,sizeof(__timeval)); \
	kselect_result = KSELECT(filedescriptor+1,&__filedescriptset,NULL,NULL,&__timeval, &num_available); \
	if (kselect_result == 0) \
	{ \
	  switch (num_available) \
	  { \
            case  0: length=TARGET_NATIVE_MATH_INT_INT64_CONST_0; result=TARGET_NATIVE_OK; break; \
            default: length=TARGET_NATIVE_MATH_INT_INT64_CONST_1; result=TARGET_NATIVE_OK; break; \
          } \
	} \
	else \
	{ \
	  result=TARGET_NATIVE_ERROR; break; \
	} \
      } while (0)
  #else
    #define TARGET_NATIVE_FILE_AVAILABLE(filedescriptor,length,result) \
      do { \
        errno=TARGET_NATIVE_ERROR_OPERATION_NOT_PERMITTED; \
        length=0; \
        result=TARGET_NATIVE_ERROR; \
      } while (0)
  #endif
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_SIZE
* Purpose    : get size of file (in bytes)
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_SIZE
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_SIZE(filedescriptor,length,result) \
    do { \
      struct stat __statBuffer; \
      \
      result=(KFSTAT(filedescriptor,&__statBuffer)==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
      length=TARGET_NATIVE_MATH_INT_INT32_TO_INT64(__statBuffer.st_size); \
    } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_TELL
* Purpose    : get current file position
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_TELL
  #include <sys/types.h>
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_TELL(filedescriptor,offset,result) \
    do { \
      int klseek_result; \
      klseek_result=KLSEEK(filedescriptor,TARGET_NATIVE_MATH_INT_INT64_CONST_0,SEEK_CUR, &offset); \
      result=(klseek_result==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_SEEK_BEGIN|CURRENT|END
* Purpose    : set file position relativ to begin/current/end
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_SEEK_BEGIN
  #include <sys/types.h>
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_SEEK_BEGIN(filedescriptor,offset,newoffset,result) \
    do { \
      int klseek_result; \
      klseek_result=KLSEEK(filedescriptor,offset,SEEK_SET, &newoffset);			\
      result=(klseek_result==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif
#ifndef TARGET_NATIVE_FILE_SEEK_CURRENT
  #include <sys/types.h>
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_SEEK_CURRENT(filedescriptor,offset,newoffset,result) \
    do { \
      int klseek_result; \
      klseek_result=KLSEEK(filedescriptor,offset,SEEK_CUR, &newoffset);	      \
      result=(klseek_result==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif
#ifndef TARGET_NATIVE_FILE_SEEK_END
  #include <sys/types.h>
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_SEEK_END(filedescriptor,offset,newoffset,result) \
    do { \
      int klseek_result; \
      klseek_result=KLSEEK(filedescriptor,offset,SEEK_END, &newoffset);		\
      result=(klseek_result==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_READ|WRITE
* Purpose    : read/write from/to frile
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_READ
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_READ(filedescriptor,buffer,length,bytesRead,result) \
    do { \
      int kread_result = KREAD(filedescriptor,buffer,length, &bytesRead); \
      result=(kread_result==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif
#ifndef TARGET_NATIVE_FILE_WRITE
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_WRITE(filedescriptor,buffer,length,bytesWritten,result) \
    do { \
      int kwrite_result=KWRITE(filedescriptor,buffer,length, &bytesWritten);	      \
      result=(kwrite_result==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_TRUNCATE
* Purpose    : truncate a file
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_TRUNCATE
  #include <unistd.h>
  #define TARGET_NATIVE_FILE_TRUNCATE(filedescriptor,offset,result) \
    do { \
      result=(KFTRUNCATE(filedescriptor,offset)==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while (0)
#endif

/***********************************************************************\
* Name       : TARGET_NATIVE_FILE_FSYNC
* Purpose    : do filesystem sync
* Input      : -
* Output     : -
* Return     : -
* Side-effect: unknown
* Notes      : -
\***********************************************************************/

#ifndef TARGET_NATIVE_FILE_FSYNC
  #define TARGET_NATIVE_FILE_FSYNC(filedescriptor,result) \
    do { \
      result=(KFSYNC(filedescriptor)==0)?TARGET_NATIVE_OK:TARGET_NATIVE_ERROR; \
    } while(0)
#endif

/* include rest of definitions from generic file (do not move it to 
   another position!) */
#include "target_generic_file.h"

#endif /* __TARGET_NATIVE_FILE__ */

/* end of file */

