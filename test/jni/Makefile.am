# Makefile to handle JNI regression tests.
#
# Copyright (c) 2003
#       Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

check_PROGRAMS= jniBase jniExecClass jniReflect

AM_CPPFLAGS= \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include -I$(top_srcdir)/include/kaffe \
	-I$(top_srcdir)/libltdl \
	-DCLASSPATH_SOURCE_DIR=\"$(top_srcdir)/test/jni\"

if NEED_LIBXPROF
libxprof = ../../kaffe/xprof/libkaffexprof.la
else
libxprof = #
endif

if ENABLE_JVMPI
libjvmpi = ../../kaffe/jvmpi/libkaffejvmpi.la
else
libjvmpi = #
endif

LIBNATIVE = $(top_builddir)/libraries/clib/native/libnative.la
LIBREPLACE = $(top_builddir)/replace/libreplace.la
LIBKAFFEVM = $(top_builddir)/kaffe/kaffevm/libkaffevm.la $(libjvmpi) $(libxprof)

jniBase_SOURCES= jniBase.c
jniBase_LDADD= \
	"-dlopen" $(LIBNATIVE) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@
jniBase_DEPENDENCIES= $(LIBKAFFEVM)

jniExecClass_SOURCES= jniExecClass.c
jniExecClass_LDADD= \
	"-dlopen" $(LIBNATIVE) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@
jniExecClass_DEPENDENCIES= $(LIBKAFFEVM)

jniReflect_SOURCES= jniReflect.c
jniReflect_LDADD= \
	"-dlopen" $(LIBNATIVE) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@
jniReflect_DEPENDENCIES= $(LIBKAFFEVM)

EXTRA_DIST = HelloWorldApp.class

TESTS_ENVIRONMENT = env `. $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`
TESTS = $(check_PROGRAMS)

CLEANFILES = *.fail *.out core *.core
