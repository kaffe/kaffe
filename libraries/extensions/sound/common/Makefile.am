# Common files for Tritonus javax.sound implementation
#
# Copyright (c) 2002
#	Dalibor Topic <robilad@yahoo.com>
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libtritonuscommon.la

libtritonuscommon_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER) -export-symbols-regex "^([Jj]ava|kaffe|org)_"
libtritonuscommon_la_SOURCES = common.c

noinst_HEADERS = \
	common.h \
	debug.h \
	HandleFieldHandler.h

CLEANFILES = so_locations
