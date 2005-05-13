# Makefile.am for the verifier - a Java(tm) compatible virtual machine.
#
# Copyright (c) 2004
#	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
        -I$(top_srcdir)/include \
        -I$(top_srcdir)/kaffe/kaffevm/jni \
        -I$(top_builddir)/config \
        -I$(top_srcdir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	$(ENGINE_INCLUDES) \
	-I$(top_srcdir)/libltdl \
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_srcdir)/kaffe/xprof \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/kaffevm/verifier

noinst_LTLIBRARIES = libkaffeverifier.la

libkaffeverifier_la_LDFLAGS = \
	$(KVMLIBFLAGS)

libkaffeverifier_la_SOURCES = \
	verify.c \
	verify.h \
	verify2.c \
	verify3a.c \
	verify3b.c \
	verify-block.c \
	verify-block.h \
	verify-debug.c \
	verify-debug.h \
	verify-errors.h \
	verify-sigstack.c \
	verify-sigstack.h \
	verify-type.c \
	verify-type.h \
	verify-uninit.c \
	verify-uninit.h

