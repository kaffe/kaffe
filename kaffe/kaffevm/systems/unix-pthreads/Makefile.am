# Makefile.frag for kaffevm pthread.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

noinst_LTLIBRARIES = libkthread.la
libkthread_la_LDFLAGS = \
	$(KVMLIBFLAGS)

libkthread_la_SOURCES = lock-impl.c signal.c syscalls.c thread-impl.c
EXTRA_DIST = Makefile.frag lock-impl.h thread-impl.h thread-internal.h \
		jsignal.h jthread.h

@engine_frag@

AM_CPPFLAGS = \
	-I. -I$(srcdir) -I../.. -I$(srcdir)/../.. \
	-I$(top_builddir)/config -I$(top_srcdir)/config \
	-I$(top_builddir)/include -I$(top_srcdir)/include \
	-I$(top_builddir)/include

DEFS = $(ENGINE_DEFS) -DKVER=\"$(KVER)\"
