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

%.moc.cc: %.h
	$(MOC) -o $@ $<


native_LTLIBRARIES = libqteawt.la

AM_CXXFLAGS := -fno-exceptions -fno-rtti $(CXXFLAGS) $(QT_CXXFLAGS)
AM_CPPFLAGS = -I$(top_builddir)/include

AWT_LIBS = -L$(QTDIR)/lib

libqteawt_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libqteawt_la_LIBADD = $(AWT_LIBS) $(QT_LDADD)

libqteawt_la_MOC = \
	evt.moc.cc
libqteawt_la_SOURCES = \
	tlk.cc \
	wnd.cc \
	evt.cc \
	gra.cc \
	fnt.cc \
	clr.cc \
	img.cc \
	cbd.cc

nodist_libqteawt_la_SOURCES = \
	$(libqteawt_la_MOC)

noinst_HEADERS = \
	keysyms.h \
	toolkit.h \
	tlkprops.h

BUILT_SOURCES = $(libqteawt_la_MOC)

CLEANFILES = so_locations $(BUILT_SOURCES)

EXTRA_DIST = evt.h


