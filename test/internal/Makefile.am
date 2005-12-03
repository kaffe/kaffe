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
	-I$(top_builddir)/include \
	-I$(top_srcdir)/kaffe/kaffevm/jni

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
	MethodOptimizations.class \
	NativeMethodCall.class \
	Exceptions.class \
	VirtualMethod.class

TESTS_ENVIRONMENT = env TEST_CLASSES="$(JAVA_CLASSES)" `. $(top_builddir)/BUILD_ENVIRONMENT; BOOTCLASSPATH=.:\$${BOOTCLASSPATH}; export BOOTCLASSPATH; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

##CPATH = $(top_builddir)/libraries/javalib/rt.jar
CPATH = .:$(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip

ConstMethods.class: $(srcdir)/ConstMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ConstMethods.java

ConstMathMethods.class: $(srcdir)/ConstMathMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ConstMathMethods.java

ParameterizedMethods.class: $(srcdir)/ParameterizedMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ParameterizedMethods.java

ParameterizedMathMethods.class: $(srcdir)/ParameterizedMathMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ParameterizedMathMethods.java

ParameterizedLogicalMethods.class: $(srcdir)/ParameterizedLogicalMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ParameterizedLogicalMethods.java

ParameterizedBitwiseMethods.class: $(srcdir)/ParameterizedBitwiseMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ParameterizedBitwiseMethods.java

ControlFlowMethods.class: $(srcdir)/ControlFlowMethods.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ControlFlowMethods.java

StaticMethodCall.class: $(srcdir)/StaticMethodCall.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/StaticMethodCall.java

TypeConversion.class: $(srcdir)/TypeConversion.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/TypeConversion.java

StaticFields.class: $(srcdir)/StaticFields.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/StaticFields.java

PrimitiveArrays.class: $(srcdir)/PrimitiveArrays.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/PrimitiveArrays.java

ObjectFields.class: $(srcdir)/ObjectFields.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/ObjectFields.java

MethodOptimizations.class: $(srcdir)/MethodOptimizations.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/MethodOptimizations.java

NativeMethodCall.class java/lang/System.class java/lang/VMSystem.class: $(srcdir)/NativeMethodCall.java   $(srcdir)/java/lang/VMSystem.java $(srcdir)/java/lang/System.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/java/lang/VMSystem.java $(srcdir)/java/lang/System.java $(srcdir)/NativeMethodCall.java

Exceptions.class: $(srcdir)/Exceptions.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/Exceptions.java

VirtualMethod.class: $(srcdir)/VirtualMethod.java
	$(JAVA_COMPILER) -g -classpath $(CPATH) -d . $(srcdir)/VirtualMethod.java

jit_stub.o: java/lang/System.class java/lang/VMSystem.class $(JAVA_CLASSES)

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
	MethodOptimizations.java \
	NativeMethodCall.java \
	Exceptions.java \
	VirtualMethod.java \
	java/lang/System.java \
	java/lang/VMSystem.java

CLEANFILES = \
	*.fail \
	*.out \
	core \
	*.core \
	*.class \
	$(nodist_GEN_SRCS) \
	java/lang/System.class  \
	java/lang/VMSystem.class

