# Makefile.in for kaffevm JIT compiler
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

INCLUDES = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) -I.. -I$(srcdir)/.. $(ENGINE_DEFS)

noinst_LTLIBRARIES = libgcj.la

libgcj_la_SOURCES = \
	gcj-class.c \
	gcj-except.c \
	gcj-soft.c

EXTRA_DIST = \
	Makefile.am \
	gcj-except.h
