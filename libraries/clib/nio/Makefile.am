# New IO subsystem (NIO) support for Java libraries
#
# Copyright (c) 2003 Kaffe's team
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
	$(CLASSPATH_INCLUDES) \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_srcdir)/libraries/javalib/external/classpath/native/jni/classpath

native_LTLIBRARIES = libjavanio.la

libjavanio_la_CFLAGS = \
	$(CFLAGS_PG)

libjavanio_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|kaffe|gnu)_"

libjavanio_la_SOURCES = \
	java_nio_MappedByteBufferImpl.c \
	java_nio_VMDirectByteBuffer.c \
	gnu_java_nio_charset_iconv_IconvDecoder.c \
	gnu_java_nio_charset_iconv_IconvEncoder.c \
	gnu_java_nio_channels_FileChannelImpl.c \
	gnu_java_nio_VMSelector.c \
	gnu_java_nio_VMPipe.c 

libjavanio_la_LIBADD = \
        $(LTLIBICONV) \
	$(top_builddir)/kaffe/kaffevm/libkaffevm.la \
	$(top_builddir)/libraries/javalib/external/classpath/native/jni/classpath/libclasspath.la

CLEANFILES = so_locations
