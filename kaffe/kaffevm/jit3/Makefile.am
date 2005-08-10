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
	-I$(top_builddir)/include

AM_CCASFLAGS = \
	$(DEFS) \
	$(ENGINE_DEFS) \
	$(DEFAULT_INCLUDES) \
	$(INCLUDES) \
	-I$(top_srcdir)/include

noinst_LTLIBRARIES = libengine.la

libengine_la_LDFLAGS = \
	$(KVMLIBFLAGS)

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

BUILT_SOURCES = \
	trampolines.S

nodist_libengine_la_SOURCES = \
	trampolines.S

EXTRA_DIST = \
	Makefile.frag

DISTCLEANFILES = icode.h jit.def trampolines.S stamp-h01 stamp-h02 stamp-h03

basecode.lo icode.lo machine.lo registers.lo: icode.h
funcs.lo: jit.def trampolines.S

icode.h: stamp-h01
	@if test ! -f $@; then rm -f stamp-h01; $(MAKE) stamp-h01; \
	else :; fi
stamp-h01: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_ICODE_H) icode.h $@

jit.def: stamp-h02
	@if test ! -f $@; then rm -f stamp-h02; $(MAKE) stamp-h02; \
	else :; fi
stamp-h02: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_JIT_DEF) jit.def $@

trampolines.S: stamp-h03
	@if test ! -f $@; then rm -f stamp-h03; $(MAKE) stamp-h03; \
	else :; fi
stamp-h03: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_TRAMPOLINES_S) trampolines.S $@

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
