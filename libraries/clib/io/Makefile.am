# File system (IO) support for Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_srcdir)/libraries/clib/classpath

native_LTLIBRARIES = libio.la

libio_la_CFLAGS = \
	$(CFLAGS_PG)

libio_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libio_la_LIBADD = $(top_builddir)/kaffe/kaffevm/libkaffevm.la

libio_la_SOURCES = \
	File.c \
	java_io_VMObjectInputStream.c \
	java_io_VMObjectStreamClass.c

CLEANFILES = so_locations
