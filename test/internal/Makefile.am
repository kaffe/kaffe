# Internal testing
#
# Copyright (c) 2003, 2004 University of Utah and the Flux Group.
# All rights reserved..
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# Contributed by the Flux Research Group, Department of Computer Science,
# University of Utah, http://www.cs.utah.edu/flux/

@engine_frag@

AM_CFLAGS= $(KAFFE_CFLAGS)

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

@threads_frag@

check_PROGRAMS = jitBasic

AM_CPPFLAGS = -I$(top_srcdir)/kaffe \
	-I$(top_builddir)/kaffe/kaffe \
	$(ENGINE_INCLUDES) \
	-I../../kaffe/kaffevm \
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni \
        -I$(top_srcdir)/config/$(Khost_cpu) \
        -I$(top_srcdir)/config/$(Khost_cpu)/$(Khost_os)

LIBNATIVE = $(top_builddir)/libraries/clib/native/libjavalang.la
LIBREPLACE = $(top_builddir)/replace/libreplace.la
LIBKAFFEVM = ../../kaffe/kaffevm/libkaffevm.la $(libjvmpi)

jitBasic_LDFLAGS = -export-dynamic
jitBasic_LDADD = \
	"-dlopen" $(LIBNATIVE) \
	$(LIBKAFFEVM) \
	$(LIBREPLACE) \
	$(libxprof) \
        $(LTLIBINTL) \
	@LIBLTDL@

jitBasic_DEPENDENCIES = $(LIBNATIVE) $(LIBKAFFEVM) $(libxprof)

STUB = main_stub.c
JIT_STUB = jit_stub.c

jitBasic_SOURCES = \
	jitBasic.c \
	stringParsing.c \
	stringParsing.h \
	$(JIT_STUB)

# Order matters here!
TEST_CLASSES = \
	ConstMethods.class \
	ConstMathMethods.class \
	ParameterizedMethods.class \
	ParameterizedMathMethods.class \
	ParameterizedLogicalMethods.class \
	ParameterizedBitwiseMethods.class \
	ControlFlowMethods.class \
	StaticMethodCall.class \
	TypeConversion.class \
	StaticFields.class \
	PrimitiveArrays.class \
	ObjectFields.class \
	MethodOptimizations.class \
	NativeMethodCall.class \
	Exceptions.class \
	VirtualMethod.class

JAVA_CLASSES = \
	$(TEST_CLASSES) \
	java/lang/System.class \
	java/lang/VMSystem.class

JAVA_SOURCE_FILES = \
	$(srcdir)/ConstMethods.java \
	$(srcdir)/ConstMathMethods.java \
	$(srcdir)/ParameterizedMethods.java \
	$(srcdir)/ParameterizedMathMethods.java \
	$(srcdir)/ParameterizedLogicalMethods.java \
	$(srcdir)/ParameterizedBitwiseMethods.java \
	$(srcdir)/ControlFlowMethods.java \
	$(srcdir)/StaticMethodCall.java \
	$(srcdir)/TypeConversion.java \
	$(srcdir)/StaticFields.java \
	$(srcdir)/PrimitiveArrays.java \
	$(srcdir)/ObjectFields.java \
	$(srcdir)/MethodOptimizations.java \
	$(srcdir)/NativeMethodCall.java \
	$(srcdir)/Exceptions.java \
	$(srcdir)/VirtualMethod.java \
	$(srcdir)/java/lang/System.java \
	$(srcdir)/java/lang/VMSystem.java

TESTS_ENVIRONMENT = env TEST_CLASSES="$(TEST_CLASSES)" `. $(top_builddir)/BUILD_ENVIRONMENT; BOOTCLASSPATH=.:\$${BOOTCLASSPATH}; export BOOTCLASSPATH; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

CPATH = .:$(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip

$(JAVA_CLASSES): $(JAVA_SOURCE_FILES)
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(JAVA_SOURCE_FILES)

jit_stub.o: $(JAVA_CLASSES)

TESTS = $(check_PROGRAMS)

XFAIL_TESTS =

EXTRA_DIST = \
	$(JAVA_SOURCE_FILES)

CLEANFILES = \
	*.fail \
	*.out \
	core \
	*.core \
	$(JAVA_CLASSES)



