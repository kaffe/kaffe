# Native support for zip Java library
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libzip.la

libzip_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER)
libzip_la_LIBADD = $(ZIP_LIBS)
libzip_la_SOURCES = \
	Adler32.c \
	CRC32.c \
	Deflater.c \
	Inflater.c

