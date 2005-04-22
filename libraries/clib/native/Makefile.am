# Native support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2004
#	Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libnative.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/kaffe/jvmpi -I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
        -I$(top_srcdir)/libraries/clib/classpath \
        -I$(top_srcdir)/libraries/clib/fdlibm

IO_SRCS = \
		ByteToCharDefault.c \
		ByteToCharIconv.c \
		CharToByteDefault.c \
		CharToByteIconv.c

LANG_SRCS = \
		Class.c \
		ClassLoader.c \
		java_lang_VMDouble.c \
		java_lang_VMFloat.c \
		Math.c \
		MemoryAdvice.c \
		Object.c \
		PrimordialClassLoader.c \
		Runtime.c \
		String.c \
		System.c \
		Thread.c \
		ThreadStack.c \
		Throwable.c \
		UNIXProcess.c \
		gnu_classpath_VMSystemProperties.c \
		ZipFile.c

REF_SRCS= \
	  java_lang_ref_Reference.c

REFLECT_SRCS = \
		Array.c \
		Constructor.c \
		Field.c \
		Method.c

SECURITY_SRCS = \
		AccessController.c

TEST_SRCS = \
		TestNative.c

UTIL_SRCS = \
		TimeZone.c

libnative_la_CFLAGS = \
	$(CFLAGS_PG)

libnative_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe)_"

libnative_la_DEPENDENCIES = \
	../../../kaffe/kaffevm/libkaffevm.la \
	$(top_builddir)/libraries/clib/fdlibm/libfdlibm.la \
        $(top_builddir)/libraries/clib/classpath/libclasspath.la

libnative_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(M_LIBS) \
	$(LTLIBICONV) \
	$(libnative_la_DEPENDENCIES)

libnative_la_SOURCES = \
	$(IO_SRCS) \
	$(LANG_SRCS) \
	$(REFLECT_SRCS) \
	$(SECURITY_SRCS) \
	$(TEST_SRCS) \
	$(UTIL_SRCS) \
	$(REF_SRCS)

CLEANFILES = so_locations
