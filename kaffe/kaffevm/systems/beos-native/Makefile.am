# Makefile.frag for kaffevm with BeOS native threads.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

noinst_LTLIBRARIES = libkthread.la
libkthread_la_LDFLAGS = \
	$(KVMLIBFLAGS)

libkthread_la_SOURCES = signal.c bjthread.c syscalls.c
EXTRA_DIST = Makefile.frag jsignal.h jthread.h lock-impl.h thread-impl.h

@engine_frag@

AM_CPPFLAGS = \
	-I$(srcdir) -I../.. -I$(srcdir)/../.. \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_builddir)/config -I$(top_srcdir)/config \
	-I$(top_builddir)/include -I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(ENGINE_DEFS) \
	-D_REENTRANT
