# nano-X AWT backend for Kaffe
#
# Copyright (c) 2001
#	Exor International Inc.  All rights reserved.
# Copyright (c) 2001
#	Sinby Corporation,  All rights reserved.
#
# Copyright (c) 2005
#	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

native_LTLIBRARIES = libnanoxawt.la

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni
		
libnanoxawt_la_CFLAGS = \
	$(CFLAGS_PG) \
	$(NANOX_CFLAGS)

libnanoxawt_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libnanoxawt_la_LIBADD = $(top_builddir)/kaffe/kaffevm/libkaffevm.la $(AWT_LIBS)

libnanoxawt_la_SOURCES = \
	awt_const.h  \
	keymap.h \
	event.c \
	handler.c \
	color.c \
	font.c \
	window.c \
	graphics.c \
	image.c \
	clipboard.c \
	toolkit.c 

noinst_HEADERS = \
	toolkit.h 

CLEANFILES = so_locations
