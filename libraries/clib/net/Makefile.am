# Net support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libnet.la

libnet_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER) -export-symbols-regex "^([Jj]ava|kaffe)_"
libnet_la_LIBADD = $(NET_LIBS)
libnet_la_SOURCES = \
	InetAddressImpl.c \
	PlainDatagramSocketImpl.c \
	PlainSocketImpl.c

CLEANFILES = so_locations
