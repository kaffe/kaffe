# QT-embedded-based AWT support for standard Java libraries
#
# Copyright (c) 2001-2002
#	Dialogue Technologies, Inc.  All rights reserved.
# Author: Murphy Chen, Ken-Hao Liu
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

%.moc.cc: %.h
	$(MOC) -o $@ $<


native_LTLIBRARIES = libawt.la

CXXFLAGS := -fno-exceptions -fno-rtti $(CXXFLAGS) $(QT_CXXFLAGS)

AWT_LIBS = -L$(QTDIR)/lib

libawt_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER) -export-symbols-regex "^([Jj]ava|kaffe)_"
libawt_la_LIBADD = $(AWT_LIBS) $(QT_LDADD)
libawt_la_MOC = \
	evt.moc.cc
libawt_la_SOURCES = \
	$(libawt_la_MOC) \
	tlk.cc \
	wnd.cc \
	evt.cc \
	gra.cc \
	fnt.cc \
	clr.cc \
	img.cc \
	cbd.cc

noinst_HEADERS = \
	keysyms.h \
	toolkit.h \
	tlkprops.h

BUILT_SOURCES = $(libawt_la_MOC)

CLEANFILES = so_locations $(BUILT_SOURCES)



