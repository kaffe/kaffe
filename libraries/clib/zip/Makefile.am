# Native support for zip Java library
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
        -I$(top_srcdir)/config/$(Khost_cpu) \
        -I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os)

native_LTLIBRARIES = libzip.la

libzip_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(CFLAGS_PG) \
	$(ZZIP_CFLAGS)

libzip_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libzip_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffe.la \
	$(ZZIP_LIBS)

libzip_la_SOURCES = \
	ZipFile.c

CLEANFILES = so_locations
