# Microsoft extensions support
#
# Copyright (c) 1999
#       Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
        -I$(top_srcdir)/kaffe/kaffevm/jni

native_LTLIBRARIES = libmicrosoft.la

libmicrosoft_la_CFLAGS = \
	($CFLAGS_PG)

libmicrosoft_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libmicrosoft_la_SOURCES = Delegate.c

CLEANFILES = so_locations
