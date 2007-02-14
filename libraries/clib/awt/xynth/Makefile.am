# Xynth-based AWT support for standard Java libraries
#
# Copyright (c) 2006 - 2007
#       Alper Akcan <alper@kaffe.org>
#

native_LTLIBRARIES = libxynthawt.la

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_srcdir)/libraries/javalib/external/classpath/native/jni/classpath

libxynthawt_la_CFLAGS = \
	$(CFLAGS_PG) \
	$(XYNTH_CFLAGS)

libxynthawt_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libxynthawt_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(top_builddir)/libraries/javalib/external/classpath/native/jni/classpath/libclasspath.la \
	$(AWT_LIBS)

libxynthawt_la_SOURCES = \
	color.c \
	event.c \
	image.c \
	font.c \
	graphic.c \
	toolkit.c \
	window.c

noinst_HEADERS = \
	toolkit.h

CLEANFILES = so_locations
