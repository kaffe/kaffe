# Makefile.in for kaffevm JIT3 compiler
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
	-I$(top_srcdir)/kaffe/xprof \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/config/$(Khost_cpu) \
	-I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os) \
	$(GLIB_CFLAGS)


AM_CCASFLAGS = \
	$(DEFS) \
	$(ENGINE_DEFS) \
	$(DEFAULT_INCLUDES) \
	$(INCLUDES) \
	-I$(top_srcdir)/include \
	$(KAFFE_CCASFLAGS)

noinst_LTLIBRARIES = libengine.la

libengine_la_LDFLAGS = \
	$(KVMLIBFLAGS) \
	$(GLIB_LIBS)

libengine_la_SOURCES = \
	basecode.c \
	basecode.h \
	checks.h \
	codeproto.h \
	constpool.c \
	constpool.h \
	funcs.c \
	funcs.h \
	global-regs.c  \
	global-regs.h \
	icode.c \
	labels.c \
	labels.h \
	machine.c \
	machine.h \
	registers.c \
	registers.h \
	seq.c \
	seq.h \
	slots.c \
	slots.h \
	stackTrace-impl.h \
	native-wrapper.c \
	native-wrapper.h \
	methodcalls.c \
	methodcalls.h

nodist_libengine_la_SOURCES = \
	trampolines.S

EXTRA_DIST = \
	Makefile.frag
