# Makefile.in for kaffevm JIT compiler
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

@engine_frag@

INCLUDES = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) -I.. -I$(srcdir)/.. $(ENGINE_DEFS)

noinst_LTLIBRARIES = libengine.la

libengine_la_SOURCES = \
	machine.c

EXTRA_DIST = \
	Makefile.am \
	checks.h \
	icode.h \
	machine.h \
	slots.h
