# Esd files for Tritonus javax.sound implementation
#
# Copyright (c) 2002, 2003, 2004
#	Dalibor Topic <robilad@kaffe.org>
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include

native_LTLIBRARIES = libtritonusesd.la

libtritonusesd_la_CFLAGS =\
	$(CFLAGS_PG)

libtritonusesd_la_LDFLAGS =\
	$(KLIBFLAGS) \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|org)_"

libtritonusesd_la_LIBADD = -lesd
libtritonusesd_la_SOURCES = \
	common.c \
	org_tritonus_lowlevel_esd_EsdRecordingStream.c \
	org_tritonus_lowlevel_esd_EsdStream.c


noinst_HEADERS = \
	common.h 

CLEANFILES = so_locations
