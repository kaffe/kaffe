# Internal testing
#
# Copyright (c) 2003 University of Utah and the Flux Group.
# All rights reserved..
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# Contributed by the Flux Research Group, Department of Computer Science,
# University of Utah, http://www.cs.utah.edu/flux/

@engine_frag@

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

nodist_GEN_SRCS = \
	version.c

$(nodist_GEN_SRCS):
	$(SHELL) $(REGEN_FORWARD) $(top_srcdir)/kaffe/kaffe/$@ $@

AM_CPPFLAGS = -I$(top_srcdir)/kaffe \
	-I$(top_builddir)/kaffe/kaffe \
	$(ENGINE_INCLUDES) \
	-I../../kaffe/kaffevm \
	$(ENGINE_DEFS) \
	-I$(top_srcdir)/kaffe/kaffevm \
	-I$(top_srcdir)/kaffe/jvmpi \
	-I$(top_builddir)/include

LIBNATIVE = $(top_builddir)/libraries/clib/native/libnative.la
LIBKAFFEVM = ../../kaffe/kaffevm/libkaffevm.la $(libjvmpi)

jitBasic_LDFLAGS = -export-dynamic
jitBasic_LDADD = "-dlopen" $(LIBNATIVE) $(LIBKAFFEVM) $(libxprof) @LIBLTDL@
jitBasic_DEPENDENCIES = $(LIBNATIVE) $(LIBKAFFEVM) $(libxprof)

STUB = main_stub.c
JIT_STUB = jit_stub.c

jitBasic_SOURCES = jitBasic.c $(JIT_STUB)
nodist_jitBasic_SOURCES = $(nodist_GEN_SRCS)

# Order matters here!
JAVA_CLASSES = \
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
	MethodOptimizations.class

TESTS_ENVIRONMENT = env TEST_CLASSES="$(JAVA_CLASSES)" `BOOTCLASSPATH="."; export BOOTCLASSPATH; . $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

%.class: %.java
	$(JIKES) -classpath $(top_builddir)/libraries/javalib/rt.jar -d . $<

jit_stub.o: $(JAVA_CLASSES)

TESTS = $(check_PROGRAMS)

XFAIL_TESTS =

EXTRA_DIST = \
	ConstMethods.java \
	ConstMathMethods.java \
	ParameterizedMethods.java \
	ParameterizedMathMethods.java \
	ParameterizedLogicalMethods.java \
	ParameterizedBitwiseMethods.java \
	ControlFlowMethods.java \
	StaticMethodCall.java \
	TypeConversion.java \
	StaticFields.java \
	PrimitiveArrays.java \
	ObjectFields.java \
	MethodOptimizations.java

CLEANFILES = *.fail *.out core *.core
