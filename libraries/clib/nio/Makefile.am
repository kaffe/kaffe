# New IO subsystem (NIO) support for Java libraries
#
# Copyright (c) 2003 Kaffe's team
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/kaffe/kaffevm/jni
native_LTLIBRARIES = libnio.la

libnio_la_CFLAGS = \
	$(CFLAGS_PG)

libnio_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe|gnu)_"

libnio_la_SOURCES = \
	DirectByteBufferImpl.c \
	SelectorImpl.c \
	NIOServerSocket.c \
	FileChannelImpl.c \
	Channels.c

CLEANFILES = so_locations
