# Makefile.in for kaffevm - a Java(tm) compatible virtual machine.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SUBDIRS = $(ENGINE_NAME) systems
DIST_SUBDIRS = intrp jit systems
EXTRA_DIST = mem/gc-*.[ch]

@engine_frag@

INCLUDES = $(ENGINE_INCLUDES) -I$(top_srcdir)/libltdl $(ENGINE_DEFS)

lib_LTLIBRARIES = libkaffevm.la

@cpu_frag@
@cpu_os_frag@
@threads_frag@

libkaffevm_la_LDFLAGS = $(KVMLIBFLAGS) -export-dynamic -release $(KVER)
libkaffevm_la_LIBADD = \
	$(top_builddir)/libltdl/libltdl.la \
	$(ENGINE_NAME)/libengine.la \
	systems/$(THREAD_SYSTEM)/libkthread.la $(VM_LIBS)
libkaffevm_la_SOURCES = \
	baseClasses.c \
	classMethod.c \
	code-analyse.c \
	code.c \
	constants.c \
	debug.c \
	exception.c \
	external.c \
	findInJar.c \
	gc-mem.c \
	gc.c \
	hashtab.c \
	inflate.c \
	itypes.c \
	jar.c \
	jni.c \
	locks.c \
	lookup.c \
	md.c \
	object.c \
	readClass.c \
	soft.c \
	stackTrace.c \
	string.c \
	support.c \
	thread.c \
	utf8const.c \
	verify.c

BUILT_SOURCES = \
	java_lang_Cloneable.h \
	gc-mem.c \
	md.c \
	trampolines.c

java_lang_Cloneable.h: $(top_srcdir)/libraries/javalib/Klasses.jar
	if test -x $(KAFFEH_FOR_HOST); then \
	    $(KAFFEH_FOR_HOST) \
		-classpath $(top_srcdir)/libraries/javalib/Klasses.jar \
		java.lang.Cloneable; \
	fi

gc-mem.c: stamp-h01
	@if test ! -f $@; then rm -f stamp-h01; $(MAKE) stamp-h01; fi
stamp-h01: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/mem/gc-mem.c gc-mem.c $@

md.c: stamp-h02
	@if test ! -f $@; then rm -f stamp-h02; $(MAKE) stamp-h02; fi
stamp-h02: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_MD_C) md.c $@

trampolines.c: stamp-h03
	@if test ! -f $@; then rm -f stamp-h03; $(MAKE) stamp-h03; fi
stamp-h03: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_TRAMPOLINES_C) trampolines.c $@
