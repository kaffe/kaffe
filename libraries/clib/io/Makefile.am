# File system (IO) support for Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libio.la

libio_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER)
libio_la_SOURCES = \
	FileDescriptor.c \
	File.c \
	FileInputStream.c \
	FileOutputStream.c \
	RandomAccessFile.c \
	AudioPlayer.c

noinst_HEADERS = \
	File.h \
	FileDescriptor.h \
	FileInputStream.h \
	FileOutputStream.h \
	RandomAccessFile.h
