# X-based AWT support for standard Java libraries
#
# Copyright (c) 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libawt.la

CFLAGS = @CFLAGS@ @X_CFLAGS@

libawt_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER)
libawt_la_LIBADD = $(AWT_LIBS)
libawt_la_SOURCES = \
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
	toolkit.h
