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

LIBREPLACE = $(top_builddir)/replace/libreplace.la
LIBKAFFEVM = ../../kaffe/kaffevm/libkaffe.la $(libjvmpi)

jitBasic_LDFLAGS = -export-dynamic
jitBasic_LDADD = \
        $(LTLIBINTL) \
	$(LIBREPLACE) \
	$(LIBKAFFEVM) \
	$(libxprof) \
	-dlopen ../../kaffe/kaffevm/libkaffevm.la

jitBasic_DEPENDENCIES = $(LIBKAFFEVM) $(libxprof)

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

jitBasicdir = $(srcdir)
dist_jitBasic_JAVA = \
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
	MethodOptimizations.java \
	NativeMethodCall.java \
	Exceptions.java \
	VirtualMethod.java \
	java/lang/System.java \
	java/lang/VMSystem.java

TESTS_ENVIRONMENT = env TEST_CLASSES="$(TEST_CLASSES)" `. $(top_builddir)/BUILD_ENVIRONMENT; BOOTCLASSPATH=.:\$${BOOTCLASSPATH}; export BOOTCLASSPATH; $(SED) 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

CPATH =  .:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAROOT = .

TESTS = $(check_PROGRAMS)

XFAIL_TESTS =

CLEANFILES = \
	*.fail \
	*.out \
	core \
	*.core \
	`find . -name "*.class"`


