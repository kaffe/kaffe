# File system (IO) support for Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include
native_LTLIBRARIES = libio.la

libio_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(KVER) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libio_la_SOURCES = \
	FileDescriptor.c \
	File.c \
	AudioPlayer.c

CLEANFILES = so_locations
