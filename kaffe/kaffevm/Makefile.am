# Makefile.in for kaffevm - a Java(tm) compatible virtual machine.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SUBDIRS = $(ENGINE_NAME) systems gcj
DIST_SUBDIRS = intrp jit jit3 systems gcj

EXTRA_DIST = \
	kaffe.def \
	mem/gc-incremental.c \
	mem/gc-incremental.h \
	mem/gc-mem.c \
	mem/gc-mem.h \
	systems/win32/winio.c \
	systems/win32/winthread.c \
	systems/win32/winthread.h

@engine_frag@

INCLUDES = -I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) $(ENGINE_INCLUDES) -I$(top_srcdir)/libltdl $(ENGINE_DEFS)

lib_LTLIBRARIES = libkaffevm.la

@cpu_frag@
@cpu_os_frag@
@threads_frag@

if HAVE_GCJ_SUPPORT
libgcj = gcj/libgcj.la
endif
libkaffevm_la_LDFLAGS = $(KVMLIBFLAGS) -export-dynamic -release $(KVER)
libkaffevm_la_DEPENDENCIES = \
	$(LIBLTDL) \
	$(ENGINE_NAME)/libengine.la \
	$(libgcj) \
	systems/$(THREAD_SYSTEM)/libkthread.la
libkaffevm_la_LIBADD = $(libkaffevm_la_DEPENDENCIES) $(VM_LIBS) $(M_LIBS)

libkaffevm_la_GEN_SRCS = \
	gc-mem.c \
	md.c

libkaffevm_la_SOURCES = \
	$(libkaffevm_la_GEN_SRCS) \
	baseClasses.c \
	classMethod.c \
	classPool.c \
	methodCache.c \
	code-analyse.c \
	code.c \
	constants.c \
	debug.c \
	exception.c \
	external.c \
	findInJar.c \
	fp.c \
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
	sha-1.c \
	soft.c \
	stackTrace.c \
	stats.c \
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
	methodCache.h \
	classpath.h \
	code-analyse.h \
	code.h \
	constants.h \
	debug.h \
	exception.h \
	external.h \
	file.h \
	fp.h \
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
	sha-1.h \
	slib.h \
	soft.h \
	stackTrace.h \
	stats.h \
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

## This avoids that dependencies produced by `make dist' in a jit
## configuration prevents intrp builds.
jit/icode.h:

include $(top_srcdir)/include/distclean-hack
