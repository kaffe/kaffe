# Makefile.in for xprof - A library to support cross language profiling
#
# Copyright (c) 2000, 2003, 2004 University of Utah and the Flux Group.
# All rights reserved.
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# Contributed by the Flux Research Group, Department of Computer Science,
# University of Utah, http://www.cs.utah.edu/flux/

@engine_frag@

AM_CPPFLAGS = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) $(ENGINE_INCLUDES) -I$(top_srcdir)/libltdl $(ENGINE_DEFS) -I$(top_srcdir)/kaffe/kaffevm -I$(top_builddir)/include

native_LTLIBRARIES = libkaffexprof.la

libkaffexprof_la_LDFLAGS = \
	-no-undefined \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffexprof_la_DEPENDENCIES = \
	$(LIBLTDL)

libkaffexprof_la_LIBADD = $(libkaffexprof_la_DEPENDENCIES) $(VM_LIBS) $(M_LIBS)

libkaffexprof_la_SOURCES = \
	callGraph.c \
	debugFile.c \
	feedback.c \
	fileSections.c \
	gmonFile.c \
	mangle.c \
	memorySamples.c \
	sectionFile.c \
	xprofiler.c

noinst_HEADERS = \
	callGraph.h \
	debugFile.h \
	feedback.h \
	fileSections.h \
	gmon_out.h \
	gmonFile.h \
	mangle.h \
	memorySamples.h \
	sectionFile.h \
	xprofiler.h
