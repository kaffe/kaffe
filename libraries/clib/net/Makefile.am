# Net support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libnet.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/kaffe \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni

libnet_la_CFLAGS = \
	$(CFLAGS_PG)

libnet_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe|gnu)_"

libnet_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(NET_LIBS)  \
	$(top_builddir)/replace/libreplace.la \
	$(LTLIBINTL)

libnet_la_SOURCES = \
	InetAddressImpl.c \
	NetworkInterface.c \
	PlainDatagramSocketImpl.c \
	PlainSocketImpl.c

CLEANFILES = so_locations

