# Math support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include

native_LTLIBRARIES = libmath.la

libmath_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libmath_la_LIBADD = $(top_builddir)/kaffe/kaffevm/libkaffevm.la $(LIBADD_GMP) $(M_LIBS)

libmath_la_SOURCES = BigInteger.c

CLEANFILES = so_locations
