# Math support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni

native_LTLIBRARIES = libsecurity.la

SECURITY_SRCS =	\
	digest.c \
	kaffe_md5.c \
	kaffe_md5.h \
	sha-1.c \
	sha-1.h 

security_DATA = java.security

EXTRA_DIST = \
	$(security_DATA)

libsecurity_la_CFLAGS = \
	$(CFLAGS_PG)

libsecurity_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libsecurity_la_LIBADD = $(top_builddir)/kaffe/kaffevm/libkaffevm.la $(SECURITY_LIBS)

libsecurity_la_SOURCES = $(SECURITY_SRCS)

CLEANFILES = so_locations
