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

SUBDIRS = $(ENGINE_NAME) systems $(GC_NAME) gcj
DIST_SUBDIRS = boehm-gc kaffe-gc intrp jit jit3 systems gcj

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
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_srcdir)/kaffe/xprof \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/verifier \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
        -I$(top_srcdir)/config/$(Khost_cpu) \
	-I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os) \
	$(GLIB_CFLAGS)

native_LTLIBRARIES = libkaffevm.la libkaffe.la

@cpu_frag@
@cpu_os_frag@
@threads_frag@
@gc_frag@

if HAVE_GCJ_SUPPORT
libgcj = gcj/libgcj.la
else
libgcj = #
endif

libkaffe_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(CFLAGS_PG) \
	$(ZZIP_CFLAGS) \
	$(PTHREAD_CFLAGS)

libkaffe_la_LDFLAGS = \
	$(PTHREAD_CFLAGS) \
        $(PTHREAD_LIBS) \
        $(SEMAPHORE_LIB) \
	$(KVMLIBFLAGS) \
	-no-undefined \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffe_la_DEPENDENCIES = \
	$(ENGINE_NAME)/libengine.la \
	$(libgcj) \
	systems/$(THREAD_SYSTEM)/libkthread.la \
	$(GC_NAME)/libkaffegc.la \
        $(top_builddir)/replace/libreplace.la 

libkaffe_la_LIBADD = \
	$(libkaffe_la_DEPENDENCIES) \
	$(VM_LIBS) \
	$(M_LIBS) \
	$(ZZIP_LIBS) \
	$(GLIB_LIBS)

libkaffevm_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(CFLAGS_PG) \
	$(PTHREAD_CFLAGS)

libkaffevm_la_LDFLAGS = \
	$(PTHREAD_CFLAGS) \
        $(PTHREAD_LIBS) \
        $(SEMAPHORE_LIB) \
	$(KLIBFLAGS) \
	-module \
	-no-undefined \
	-export-dynamic \
	-release $(PACKAGE_VERSION)

libkaffevm_la_DEPENDENCIES = \
	libkaffe.la

nodist_libkaffe_la_SOURCES = \
	md.c

libkaffe_la_SOURCES = \
	access.c \
	baseClasses.c \
	classMethod.c \
	classPool.c \
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
	gc-refs.c \
	gc-refs.h \
	hashtab.c \
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
	support.c \
	javacall.c \
	thread.c \
	utf8const.c \
	gcFuncs.c \
	reflect.c \
	reference.c \
	access.h \
	baseClasses.h \
	bytecode.h \
	classMethod.h \
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
	stringSupport.h \
	support.h \
	thread.h \
	threadData.h \
	utf8const.h \
	reflect.h \
	reference.h \
	jni/jni.c \
	jni/jni-base.c \
	jni/jni-callmethod.c \
	jni/jni-fields.c \
	jni/jni-arrays.c \
	jni/jni-string.c \
	jni/jni-helpers.c \
	jni/jni-refs.c \
	jni/jni_i.h \
	jni/jnirefs.h \
	jni/jni_funcs.h \
	verifier/verify.c \
	verifier/verify.h \
	verifier/verify2.c \
	verifier/verify3a.c \
	verifier/verify3b.c \
	verifier/verify-block.c \
	verifier/verify-block.h \
	verifier/verify-debug.c \
	verifier/verify-debug.h \
	verifier/verify-errors.h \
	verifier/verify-sigstack.c \
	verifier/verify-sigstack.h \
	verifier/verify-type.c \
	verifier/verify-type.h \
	verifier/verify-uninit.c \
	verifier/verify-uninit.h

libkaffevm_la_SOURCES = \
	$(top_srcdir)/libraries/clib/native/AccessController.c \
	$(top_srcdir)/libraries/clib/native/java_lang_ref_Reference.c \
        $(top_srcdir)/libraries/clib/native/Constructor.c \
        $(top_srcdir)/libraries/clib/native/Field.c \
        $(top_srcdir)/libraries/clib/native/Method.c \
        $(top_srcdir)/libraries/clib/native/Class.c \
        $(top_srcdir)/libraries/clib/native/ClassLoader.c \
        $(top_srcdir)/libraries/clib/native/java_lang_Object.c \
        $(top_srcdir)/libraries/clib/native/Runtime.c \
        $(top_srcdir)/libraries/clib/native/VMRuntime.c \
        $(top_srcdir)/libraries/clib/native/java_lang_String.c \
        $(top_srcdir)/libraries/clib/native/System.c \
        $(top_srcdir)/libraries/clib/native/java_lang_Thread.c \
        $(top_srcdir)/libraries/clib/native/Throwable.c \
	$(top_srcdir)/libraries/clib/native/gnu_classpath_VMStackWalker.c \
	$(top_srcdir)/libraries/clib/native/gnu_classpath_VMSystemProperties.c \
	$(top_srcdir)/libraries/clib/native/Unsafe.c

CLEANFILES = so_locations

## This avoids that dependencies produced by `make dist' in a jit
## configuration prevents intrp builds.
jit/icode.h:
