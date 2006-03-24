# Native support for zip Java library
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

if COND_SYSTEM_ZLIB
LINKED_ZIP_LIBS=$(ZIP_LIBS)
else
LINKED_ZIP_LIBS=../../../external/gcc/zlib/libz.la
MAYBE_INTERNAL_ZIP_INCLUDES=-I$(srcdir)/../../../external/gcc/zlib/
endif

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	$(MAYBE_INTERNAL_ZIP_INCLUDES)

native_LTLIBRARIES = libzip.la

libzip_la_CFLAGS = \
	$(CFLAGS_PG)

libzip_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libzip_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(LINKED_ZIP_LIBS)

libzip_la_SOURCES = \
	Adler32.c \
	CRC32.c \
	Deflater.c \
	Inflater.c

CLEANFILES = so_locations
