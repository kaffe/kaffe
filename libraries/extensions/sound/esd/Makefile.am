# Esd files for Tritonus javax.sound implementation
#
# Copyright (c) 2002
#	Dalibor Topic <robilad@yahoo.com>
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libtritonusesd.la

libtritonusesd_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER) -export-symbols-regex "^([Jj]ava|org)_"
libtritonusesd_la_LIBADD = -lesd -L../common -ltritonuscommon
libtritonusesd_la_SOURCES = \
	org_tritonus_lowlevel_esd_EsdRecordingStream.c \
	org_tritonus_lowlevel_esd_EsdStream.c

CLEANFILES = so_locations
