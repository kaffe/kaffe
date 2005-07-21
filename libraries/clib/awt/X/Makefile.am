# X-based AWT support for standard Java libraries
#
# Copyright (c) 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libxawt.la

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_srcdir)/libraries/clib/classpath

libxawt_la_CFLAGS = \
	$(CFLAGS_PG) \
	$(X_CFLAGS)

libxawt_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libxawt_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la $(AWT_LIBS) \
	$(top_builddir)/libraries/clib/classpath/libclasspath.la

libxawt_la_SOURCES = \
	tlk.c \
	wnd.c \
	evt.c \
	gra.c \
	fnt.c \
	clr.c \
	img.c \
	imggif.c \
	imgjpeg.c \
	imgpng.c \
	cbd.c

noinst_HEADERS = \
	keysyms.h \
	toolkit.h \
	tlkprops.h

CLEANFILES = so_locations
