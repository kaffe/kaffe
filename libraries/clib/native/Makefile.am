# Native support for standard Java libraries
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2004, 2005
#	Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libnative.la

AM_CPPFLAGS = \
        $(CLASSPATH_INCLUDES) \
	-I$(top_srcdir)/kaffe/jvmpi -I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
        -I$(top_srcdir)/libraries/clib/classpath \
        -I$(top_srcdir)/libraries/clib/fdlibm

LANG_SRCS = \
		gnu_classpath_VMStackWalker.c \
		gnu_classpath_VMSystemProperties.c \
		java_lang_Math.c \
		java_lang_VMFloat.c \
		java_lang_VMDouble.c \
		java_lang_VMProcess.c \
		java_lang_VMSystem.c \
		Class.c \
		ClassLoader.c \
		Object.c \
		Runtime.c \
		VMRuntime.c \
		String.c \
		System.c \
		Thread.c \
		Throwable.c \
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
	-export-symbols-regex "^([Jj]ava|kaffe|gnu)_"

libnative_la_DEPENDENCIES = \
	../../../kaffe/kaffevm/libkaffevm.la \
	$(top_builddir)/libraries/clib/fdlibm/libfdlibm.la \
        $(top_builddir)/libraries/clib/classpath/libclasspath.la

libnative_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(M_LIBS) \
	$(libnative_la_DEPENDENCIES)

libnative_la_SOURCES = \
	$(LANG_SRCS) \
	$(REFLECT_SRCS) \
	$(SECURITY_SRCS) \
	$(TEST_SRCS) \
	$(UTIL_SRCS) \
	$(REF_SRCS)

CLEANFILES = so_locations
