# Native support for Management Java library
#
# Copyright (c) 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include

native_LTLIBRARIES = libmanagement.la

libmanagement_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(KVER) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libmanagement_la_SOURCES = \
	Classpath.c \
	JIT.c \
	Debug.c \
	XProfiler.c

CLEANFILES = so_locations
