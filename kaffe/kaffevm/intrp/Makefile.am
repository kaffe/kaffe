# Makefile.in for kaffevm JIT compiler
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

@engine_frag@

AM_CPPFLAGS = \
	      -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	      -I$(top_srcdir)/kaffe/kaffevm/jni \
	      -I.. \
	      -I$(srcdir)/.. \
	      $(ENGINE_DEFS) \
	      -I$(top_builddir)/include

noinst_LTLIBRARIES = libengine.la

libengine_la_LDFLAGS = \
	$(KVMLIBFLAGS)

libengine_la_SOURCES = \
	machine.c \
	native-wrapper.c \
	methodcalls.c

EXTRA_DIST = \
	Makefile.frag \
	checks.h \
	icode.h \
	machine.h \
	slots.h \
	stackTrace-impl.h \
	native-wrapper.h \
	methodcalls.h
