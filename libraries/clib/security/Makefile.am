# Math support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include

native_LTLIBRARIES = libsecurity.la

EXTRA_DIST = \
		kaffe_md5.h \
		java.security

SECURITY_SRCS =	\
		digest.c \
		kaffe_md5.c

security_DATA = java.security

libsecurity_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libsecurity_la_LIBADD = $(SECURITY_LIBS)

libsecurity_la_SOURCES = $(SECURITY_SRCS)

CLEANFILES = so_locations
