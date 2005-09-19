# Makefile.am for the garbage collector - original Kaffe's GC.
#
# Copyright (c) 2004, 2005
#	Kaffe.org contributors, see ChangeLogs for details.  
#       All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = \
        -I$(top_srcdir)/include \
        -I$(top_builddir)/config \
        -I$(top_srcdir)/config \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/jvmpi \
	$(ENGINE_INCLUDES) \
	$(ENGINE_DEFS)


noinst_LTLIBRARIES = libkaffegc.la

libkaffegc_la_LDFLAGS = \
	$(KVMLIBFLAGS)

libkaffegc_la_SOURCES= \
	gc-incremental.c \
	gc-incremental.h \
	gc-mem.c \
	gc-mem.h \
	gc-refs.c \
	gc-refs.h 

libkaffegc_la_DEPENDENCIES = \
        $(top_builddir)/kaffe/kaffevm/systems/$(THREAD_SYSTEM)/libkthread.la
