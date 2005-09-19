# Makefile.am for the garbage collector - original Kaffe's GC.
#
# Copyright (c) 2004
#	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SUBDIRS= boehm

AM_CPPFLAGS = \
        -I$(top_srcdir)/include \
        -I$(top_builddir)/config \
        -I$(top_srcdir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_srcdir)/kaffe/kaffevm/boehm-gc/boehm/include \
	$(BOEHMGC_SPECIFIC_FLAGS)

native_LTLIBRARIES = libkaffegc.la

libkaffegc_la_LDFLAGS = \
	$(KVMLIBFLAGS) \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffegc_la_LIBADD= \
	boehm/libgc.la

libkaffegc_la_SOURCES= \
	gc2.c \
	gc2.h \
	gc-refs.c \
	gc-refs.h \
	gc-kaffe.c \
	gc-kaffe.h

