# Qt AWT backend for Kaffe
#
# Copyright (c) 2001-2002
#	Murphy Chen, Ken-Hao Liu from Dialogue Technologies, Inc.
#	All rights reserved.
#
# Copyright (c) 2002, 2003, 2004
#	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libqtawt.la

AM_CPPFLAGS = -I$(top_builddir)/include

AWT_LIBS = -L$(QTDIR)/lib

libqtawt_la_CXXFLAGS = \
	$(CFLAGS_PG) \
	-fno-exceptions \
	-fno-rtti \
	$(CXXFLAGS) \
	$(QT_CXXFLAGS)

libqtawt_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libqtawt_la_LIBADD = $(top_builddir)/kaffe/kaffevm/libkaffevm.la $(AWT_LIBS) $(QT_LDADD)

libqtawt_la_MOC = \
	evt.moc.cc

evt.moc.cc: evt.h
	$(MOC) -o evt.moc.cc $(srcdir)/evt.h

libqtawt_la_SOURCES = \
	tlk.cc \
	wnd.cc \
	evt.cc \
	gra.cc \
	fnt.cc \
	clr.cc \
	img.cc \
	cbd.cc

nodist_libqtawt_la_SOURCES = \
	$(libqtawt_la_MOC)

noinst_HEADERS = \
	keysyms.h \
	toolkit.h \
	tlkprops.h

BUILT_SOURCES = $(libqtawt_la_MOC)

CLEANFILES = so_locations $(BUILT_SOURCES)

EXTRA_DIST = evt.h


