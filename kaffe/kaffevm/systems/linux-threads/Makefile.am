# Makefile.frag for kaffevm pthreads.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

noinst_LTLIBRARIES = libkthread.la
libkthread_la_LDFLAGS = $(KVMLIBFLAGS)
libkthread_la_SOURCES = signal.c pjthread.c syscalls.c
EXTRA_DIST = Makefile.frag jsignal.h jthread.h

@engine_frag@

INCLUDES = \
	-I$(srcdir) -I../.. -I$(srcdir)/../.. \
	-I$(top_builddir)/config -I$(top_srcdir)/config \
	-I$(top_builddir)/include -I$(top_srcdir)/include

DEFS = $(ENGINE_DEFS) -DKVER=\"$(KVER)\" -D_REENTRANT
