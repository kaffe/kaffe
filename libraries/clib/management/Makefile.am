# Native support for Management Java library
#
# Copyright (c) 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni

native_LTLIBRARIES = libmanagement.la

libmanagement_la_CFLAGS = \
	$(CFLAGS_PG)

libmanagement_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libmanagement_la_LIBADD = $(top_builddir)/kaffe/kaffevm/libkaffevm.la

libmanagement_la_SOURCES = \
	Classpath.c \
	JIT.c \
	Debug.c \
	XProfiler.c

CLEANFILES = so_locations
