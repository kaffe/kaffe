# Native support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libnative.la

IO_SRCS = \
		ObjectStreamClassImpl.c \
		sha-1.c

LANG_SRCS = \
		ClassLoader.c \
		Class.c \
		Compiler.c \
		Double.c \
		Float.c \
		Math.c \
		Object.c \
		Runtime.c \
		SecurityManager.c \
		String.c \
		System.c \
		Thread.c \
		Throwable.c \
		UNIXProcess.c \
		CharToByteDefault.c \
		ByteToCharDefault.c \
		ZipFile.c \
		MemoryAdvice.c

REFLECT_SRCS = \
		Array.c \
		Constructor.c \
		Field.c \
		Method.c

TEXT_SRCS = \
		DateFormat.c

TEST_SRCS = \
		TestNative.c

noinst_HEADERS = \
		sha-1.h

libnative_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER) -export-symbols-regex "^([Jj]ava|kaffe)_"
libnative_la_LIBADD = $(M_LIBS)
libnative_la_SOURCES = $(IO_SRCS) $(LANG_SRCS) $(REFLECT_SRCS) $(TEXT_SRCS) $(TEST_SRCS)

CLEANFILES = so_locations
