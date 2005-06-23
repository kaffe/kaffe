# Makefile.am for kaffevm - a Java(tm) compatible virtual machine.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2004
#	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SUBDIRS = $(ENGINE_NAME) systems $(GC_NAME) gcj verifier jni
DIST_SUBDIRS = boehm-gc kaffe-gc intrp jit jit3 systems gcj verifier jni

EXTRA_DIST = \
	kaffe.def \
	systems/win32/winio.c \
	systems/win32/winthread.c \
	systems/win32/winthread.h

@engine_frag@
AM_CPPFLAGS = \
	-I$(top_srcdir)/kaffe/kaffevm/$(THREAD_DIR) \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
	$(ENGINE_INCLUDES) \
	-I$(top_srcdir)/libltdl \
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_srcdir)/kaffe/xprof \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/verifier \
	-I$(top_srcdir)/kaffe/kaffevm/jni

native_LTLIBRARIES = libkaffevm.la

@cpu_frag@
@cpu_os_frag@
@threads_frag@
@gc_frag@

if HAVE_GCJ_SUPPORT
libgcj = gcj/libgcj.la
else
libgcj = #
endif

libkaffevm_la_CFLAGS = \
	$(CFLAGS_PG) \
	$(PTHREAD_CFLAGS)

libkaffevm_la_LDFLAGS = \
	$(PTHREAD_CFLAGS) \
        $(PTHREAD_LIBS) \
        $(SEMAPHORE_LIB) \
	$(KVMLIBFLAGS) \
	-no-undefined \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffevm_la_DEPENDENCIES = \
	$(LIBLTDL) \
	$(ENGINE_NAME)/libengine.la \
	$(libgcj) \
	systems/$(THREAD_SYSTEM)/libkthread.la \
	verifier/libkaffeverifier.la \
	$(GC_NAME)/libkaffegc.la \
	jni/libkaffejni.la \
        $(top_builddir)/replace/libreplace.la 

libkaffevm_la_LIBADD = \
	$(libkaffevm_la_DEPENDENCIES) \
	$(VM_LIBS) \
	$(M_LIBS)

libkaffevm_la_GEN_SRCS = \
	md.c

nodist_libkaffevm_la_SOURCES = \
	$(libkaffevm_la_GEN_SRCS)

if ENABLE_BINRELOC
BINRELOC_SOURCES=\
	../../binreloc/prefix.c
endif

libkaffevm_la_SOURCES = \
	access.c \
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
	file.c \
	findInJar.c \
	fp.c \
	gc.c \
	hashtab.c \
	inflate.c \
	itypes.c \
	jar.c \
	ksem.c \
	locks.c \
	lookup.c \
	object.c \
	readClass.c \
	soft.c \
	stackTrace.c \
	stats.c \
	string.c \
	stringParsing.c \
	support.c \
	javacall.c \
	thread.c \
	utf8const.c \
	gcFuncs.c \
	reflect.c \
	reference.c \
	$(BINRELOC_SOURCES)

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
	ksem.h \
	lerrno.h \
	locks.h \
	lookup.h \
	object.h \
	readClass.h \
	slib.h \
	soft.h \
	stackTrace.h \
	stats.h \
	stringParsing.h \
	stringSupport.h \
	support.h \
	thread.h \
	threadData.h \
	utf8const.h \
	reflect.h \
	reference.h \
	../../binreloc/prefix.h

md.c: stamp-h01
	@if test ! -f $@; then rm -f stamp-h01; $(MAKE) stamp-h01; \
	else :; fi
stamp-h01: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(KAFFEVM_MD_C) md.c $@

CLEANFILES = so_locations

DISTCLEANFILES = md.c stamp-h01

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done

## This avoids that dependencies produced by `make dist' in a jit
## configuration prevents intrp builds.
jit/icode.h:
