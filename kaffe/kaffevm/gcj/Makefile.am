# Makefile.in for kaffevm JIT compiler
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

@engine_frag@

AM_CPPFLAGS = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) -I.. -I$(srcdir)/.. $(ENGINE_DEFS) -I$(top_srcdir)/compat-include -I$(top_builddir)/include

if HAVE_GCJ_SUPPORT
libgcj = libgcj.la
else
libgcj = #
endif

noinst_LTLIBRARIES = $(libgcj)

libgcj_la_SOURCES = \
	gcj-class.cc \
	gcj-soft.cc \
	gcj-except.c \
	gcj-glue.c \
	eh.c

EXTRA_DIST = \
	gcj-except.h \
	gcj.h

#
#eh.lo: $(srcdir)/eh.c
#	@AMDEP@ source='eh.c' object='eh.lo' @AMDEPBACKSLASH@
#	@AMDEP@ depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' @AMDEPBACKSLASH@
#	@AMDEP@ $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
#	$(LTCOMPILE) -fexceptions -c -o eh.lo $(srcdir)/eh.c
#
