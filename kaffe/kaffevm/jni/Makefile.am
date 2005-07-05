# Makefile.am for the JNI - a Java(tm) compatible virtual machine.
#
# Copyright (c) 2004
#	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

@engine_frag@
AM_CPPFLAGS = \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	$(ENGINE_INCLUDES) \
	-I$(top_srcdir)/libltdl \
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_srcdir)/kaffe/xprof \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/kaffevm/verifier \
	-I$(top_srcdir)/kaffe/kaffevm/jni

noinst_LTLIBRARIES = libkaffejni.la

libkaffejni_la_LDFLAGS = \
	$(KVMLIBFLAGS)

libkaffejni_la_SOURCES = \
	jni.c \
	jni-base.c \
	jni-callmethod.c \
	jni-fields.c \
	jni-arrays.c \
	jni-string.c \
	jni-helpers.c \
	jni-refs.c

noinst_HEADERS= \
	jni_i.h \
	jnirefs.h \
	jni_funcs.h
