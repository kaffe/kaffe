# Makefile.frag for kaffevm pthread.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2004
#	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

noinst_LTLIBRARIES = libkthread.la

libkthread_la_LIBADD = \
	$(NET_LIBS)

libkthread_la_LDFLAGS = \
	$(KVMLIBFLAGS) \
	$(PTHREAD_CFLAGS) \
	$(PTHREAD_LIBS) \
	$(SEMAPHORE_LIB)

libkthread_la_CFLAGS = \
	$(PTHREAD_CFLAGS)

libkthread_la_SOURCES = \
	lock-impl.c \
	signal.c \
	syscalls.c \
	thread-impl.c \
	lock-impl.h \
	thread-impl.h \
	thread-internal.h \
	jsignal.h \
	jthread.h

EXTRA_DIST = \
	Makefile.frag 

@engine_frag@
@gc_frag@

AM_CPPFLAGS = \
	-I. \
	-I$(srcdir) \
	-I../.. \
	-I$(srcdir)/../.. \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	-I$(top_builddir)/config \
	-I$(top_srcdir)/config \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(ENGINE_INCUDES) \
	$(ENGINE_DEFS) \
	$(GC_DEFS)
