# Makefile.in for kaffevm - a Java(tm) compatible virtual machine.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SUBDIRS = $(ENGINE_NAME) systems gcj
DIST_SUBDIRS = intrp jit systems gcj

EXTRA_DIST = \
	kaffe.def \
	mem/gc-incremental.c \
	mem/gc-incremental.h \
	mem/gc-mem.c \
	mem/gc-mem.h

@engine_frag@

INCLUDES = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) $(ENGINE_INCLUDES) -I$(top_srcdir)/libltdl $(ENGINE_DEFS)

lib_LTLIBRARIES = libkaffevm.la

@cpu_frag@
@cpu_os_frag@
@threads_frag@

libkaffevm_la_LDFLAGS = $(KVMLIBFLAGS) -export-dynamic -release $(KVER)
libkaffevm_la_LIBADD = \
	$(top_builddir)/libltdl/libltdl.la \
	$(ENGINE_NAME)/libengine.la \
	gcj/libgcj.la \
	systems/$(THREAD_SYSTEM)/libkthread.la $(VM_LIBS) $(M_LIBS)

libkaffevm_la_GEN_SRCS = \
	gc-mem.c \
	md.c

libkaffevm_la_SOURCES = \
	$(libkaffevm_la_GEN_SRCS) \
	baseClasses.c \
	classMethod.c \
	code-analyse.c \
	code.c \
	constants.c \
	debug.c \
	exception.c \
	external.c \
	findInJar.c \
	gc.c \
	hashtab.c \
	inflate.c \
	itypes.c \
	jar.c \
	jni.c \
	locks.c \
	lookup.c \
	object.c \
	readClass.c \
	soft.c \
	stackTrace.c \
	string.c \
	support.c \
	thread.c \
	utf8const.c \
	gcFuncs.c \
	gcRefs.c \
	verify.c

noinst_HEADERS = \
	access.h \
	baseClasses.h \
	bytecode.h \
	classMethod.h \
	classpath.h \
	code-analyse.h \
	code.h \
	constants.h \
	debug.h \
	errors.h \
	exception.h \
	external.h \
	file.h \
	gc.h \
	gtypes.h \
	hashtab.h \
	inflate.h \
	itypes.h \
	jar.h \
	jnirefs.h \
	lerrno.h \
	locks.h \
	lookup.h \
	object.h \
	readClass.h \
	readClassConfig.h \
	slib.h \
	soft.h \
	stackTrace.h \
	stringSupport.h \
	support.h \
	gcRefs.h \
	thread.h

gc-mem.c: stamp-h01
	@if test ! -f $@; then rm -f stamp-h01; $(MAKE) stamp-h01; \
	else :; fi
stamp-h01: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(srcdir)/mem/gc-mem.c gc-mem.c $@

md.c: stamp-h02
	@if test ! -f $@; then rm -f stamp-h02; $(MAKE) stamp-h02; \
	else :; fi
stamp-h02: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_MD_C) md.c $@

CLEANFILES = so_locations
DISTCLEANFILES = $(libkaffevm_la_GEN_SRCS)

include $(top_srcdir)/include/distclean-hack
