# Native support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libnative.la

IO_SRCS = \
		ObjectInputStream.c \
		ObjectOutputStream.c \
		ObjectStreamClass.c

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
		ZipFile.c

REFLECT_SRCS = \
		Array.c \
		Constructor.c \
		Field.c \
		Method.c

TEXT_SRCS = \
		DateFormat.c

TEST_SRCS = \
		TestNative.c

libnative_la_LDFLAGS = $(KLIBFLAGS) -module -release $(KVER)
libnative_la_LIBADD = $(M_LIBS)
libnative_la_SOURCES = $(IO_SRCS) $(LANG_SRCS) $(REFLECT_SRCS) $(TEXT_SRCS) $(TEST_SRCS)

noinst_HEADERS = \
	Array.h \
	Boolean.h \
	Byte.h \
	Character.h \
	Compiler.h \
	Constructor.h \
	Double.h \
	Field.h \
	Float.h \
	InputStream.h \
	Integer.h \
	Long.h \
	Math.h \
	Method.h \
	ObjectInputStream.h \
	ObjectOutputStream.h \
	ObjectStreamClass.h \
	ObjectStreamField.h \
	PrintStream.h \
	Runtime.h \
	SecurityManager.h \
	Short.h \
	System.h \
	Throwable.h \
	UNIXProcess.h \
	Vector.h \
	ZipEntry.h
