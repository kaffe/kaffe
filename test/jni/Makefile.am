# Makefile to handle JNI regression tests.
#
# Copyright (c) 2003
#       Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

check_PROGRAMS= jniBase jniExecClass jniReflect jniWeakTest

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

HelloWorldApp.class: $(srcdir)/../regression/HelloWorldApp.java
	$(JIKES) -g -classpath $(CPATH) -d . $(srcdir)/../regression/HelloWorldApp.java

jniBase_SOURCES= jniBase.c
jniBase_LDADD= \
	$(DLOPEN_JAVA_LIBS) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@
jniBase_DEPENDENCIES= $(LIBKAFFEVM)

jniExecClass_SOURCES= jniExecClass.c
jniExecClass_LDADD= \
	$(DLOPEN_JAVA_LIBS) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@
jniExecClass_DEPENDENCIES= \
	$(LIBKAFFEVM) \
	HelloWorldApp.class

jniReflect_SOURCES= jniReflect.c
jniReflect_LDADD= \
	$(DLOPEN_JAVA_LIBS) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@
jniReflect_DEPENDENCIES= \
	$(LIBKAFFEVM) \
	HelloWorldApp.class

# Okay, the following is a bit convulted and hackish, and makes me feel dizzy.
# But as I found no way to do it better, here it goes:
#
# We need to have a JNI library built just for make check purpose, i.e
# a) it may not be installed
# b) it may only build on check
# c) in must be dlopen-able for JNI library loading to work
# d) it must actually load the library and run it
# 
# Solution for a and b: Fortunately, automake lets us build libtool libraries for 
# make check purpose only using check_LTLIBRARIES.
# Solution for c: Unfortunately, it builds convenience libraries by default, which
# can not be dlopened. Adding -module or -static to LDFLAGS does not help, one 
# needs to add -rpath with a fake absolute directory to it to make libtool build
# a dlopenable library.
# Solution for d: Adding . to KAFFELIBRARYPATH allows the resulting test to find the
# so created library.
# 
# Amazingly enough, this actually seems to work.

check_LTLIBRARIES = libjniweaklib.la

libjniweaklib_la_SOURCES = jniweaklib.c

libjniweaklib_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-no-undefined \
	-module \
	-rpath $(nativedir) \
	-release $(PACKAGE_VERSION)

JAVA_CLASSES = \
	JNIWeakTest.class

CPATH = .:$(top_builddir)/libraries/javalib/rt.jar

JNIWeakTest.class:  $(srcdir)/JNIWeakTest.java
	$(JIKES) -g -classpath $(CPATH) -d . $(srcdir)/JNIWeakTest.java

jniWeakTest_SOURCES = jniWeakTest.c

jniWeakTest_LDADD= \
	-dlopen $(top_builddir)/test/jni/libjniweaklib.la \
	$(DLOPEN_JAVA_LIBS) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
        $(LTLIBINTL) \
	@LIBLTDL@

jniWeakTest_DEPENDENCIES = $(LIBKAFFEVM) libjniweaklib.la

jniWeakTest.o: JNIWeakTest.class

EXTRA_DIST = \
	JNIWeakTest.java

TESTS_ENVIRONMENT = env `BOOTCLASSPATH="."; export BOOTCLASSPATH; KAFFELIBRARYPATH=".libs/" ; export KAFFELIBRARYPATH ; .  $(top_builddir)/BUILD_ENVIRONMENT; sed  's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`
TESTS = $(check_PROGRAMS)

CLEANFILES = *.fail *.out core *.core *.class
