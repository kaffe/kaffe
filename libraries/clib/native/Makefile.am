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

AM_CFLAGS= $(KAFFE_CFLAGS)

native_LTLIBRARIES = libjavalang.la

AM_CPPFLAGS = \
        $(CLASSPATH_INCLUDES) \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_srcdir)/libraries/javalib/external/classpath/native/fdlibm \
	-I$(top_srcdir)/libraries/javalib/external/classpath/native/jni/classpath \
        -I$(top_srcdir)/config/$(Khost_cpu) \
        -I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os) 


LANG_SRCS = \
		java_lang_VMMath.c \
		java_lang_VMFloat.c 

libjavalang_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(CFLAGS_PG)

libjavalang_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe|gnu|TestNative)_"

libjavalang_la_DEPENDENCIES = \
	../../../kaffe/kaffevm/libkaffevm.la \
	$(top_builddir)/libraries/javalib/external/classpath/native/fdlibm/libfdlibm.la

libjavalang_la_LIBADD = \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(top_builddir)/libraries/javalib/external/classpath/native/jni/classpath/libclasspath.la \
	$(M_LIBS) \
	$(libjavalang_la_DEPENDENCIES)

libjavalang_la_SOURCES = \
	$(LANG_SRCS) 

CLEANFILES = so_locations
