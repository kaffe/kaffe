# Regression testing
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SETUP_ENVIRONMENT = env `. $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

TESTS_ENVIRONMENT = $(SETUP_ENVIRONMENT) $(SHELL) TestScript

ALWAYS_TESTS = \
## Simple health check
	HelloWorldApp.class.save \
## Now test the compiler
	HelloWorldApp.java \
## Test ints, longs, floats and doubles
	TestIntLong.java \
	TestFloatDouble.java \
	DoubleCvt.java \
	DoubleNeg.java \
	DoubleConst.java \
	DoublePrint.java \
	DoubleComp.java \
	ModuloTest.java \
	LongNeg.java \
	FPUStack.java \
	divtest.java \
## Test strings
	Str.java \
	Str2.java \
	InternHog.java \
## Test exceptions
	IndexTest.java \
	StackDump.java \
## note that the following can be compiled from CatchLimits.j by Jasmin
	CatchLimits.class.save \
## Test threads
	tname.java \
	ttest.java \
	ThreadInterrupt.java \
	ThreadState.java \
	UncaughtException.java \
	IllegalWait.java \
## Test that preemption works---may not be supported by all threading systems
	Preempt.java 		\
## Serialization tests
	TestSerializable.java \
	TestSerializable2.java \
	SerializationCompatibility.java \
	SerialPersistentFields.java \
	TestSerialVersions.java \
	TestSerialPersistent.java \
	TestSerialFields.java \
	TestObjectStreamField.java \
## Misc tests
	ReflectInterfaces.java \
	InnerTest.java \
	SerialUID.java \
	TestCasts.java \
	Alias.java \
	NullPointerTest.java \
	NullInvoke.java \
	TableSwitch.java \
	LostFrame.java \
	ConstructorTest.java \
	burford.java \
	IllegalInterface.java \
	GetInterfaces.java \
	IntfTest.java \
	SignedShort.java \
	CharCvt.java \
	BadFloatTest.java \
	ProcessTest.java \
	UDPTest.java \
	SoTimeout.java \
	wc.java \
	FileTest.java \
	FileChecks.java \
	finalexc.java \
	finaltest.java \
	finaltest2.java \
	forNameTest.java \
	LoaderTest.java \
	ArrayForName.java \
	KaffeVerifyBug.java \
	Schtum.java \
	Reflect.java \
	MethodBug.java \
	Bean.java \
	SortTest.java \
	HashTest.java \
	MapTest.java \
	URLTest.java \
	PropertiesTest.java \
	ReaderTest.java \
	CharArrayReaderTest.java \
	LineNumberReaderTest.java \
	BufferedReaderTest.java \
	ReaderReadVoidTest.java \
	PipeTest.java \
	GetField.java \
	LostTrampolineFrame.java \
## simple test for Method.invoke
	ReflectInvoke.java \
## simple test for InvocationTargetExceptions
	InvTarExcTest.java \
## tests for ClassLoader
	SystemLoaderTest.java \
	NoClassDefTest.java \
	CLTest.java \
	CLTestConc.java \
	CLTestJLock.java \
	CLTestLie.java \
	CLTestFindLoaded.java \
	ClassDeadLock.java \
	ExceptionTest.java \
	ExceptionTestClassLoader.java \
	ExceptionTestClassLoader2.java \
	TestClassRef.java \
	ClassGC.java \
	GCTest.java \
	ThreadLocalTest.java \
	BeanBug.java \
	BitSetTest.java \
	ExceptionInInitializerTest.java \
	InitTest.java \
	ProcessClassTest.java \
	ProcessClassInst.java \
	ProcessClassStop.java \
	FindSystemClass.java \
	ProcessClassLockTest.java \
	DoubleBug.java \
	TestUnlock.java \
	MarkResetTest.java \
	HashtableTest1.java \
	ReflectMultiArray.java \
	ExecTest.java \
## Test catching our own death
	CatchDeath.java \
## Test various ways of stopping threads
	ThreadStop.java \
	DeadThread.java \
	tthrd1.java \
## Test interrupting I/O
## This is kind of tricky, and Sun doesn't support it.
	SoInterrupt.java \
	sysdepCallMethod.java \
## Test for package java.util.zip
	DosTimeVerify.java \
	ZipVerify.java \
	ZipTest.java \
	TruncatedClass.java \
## This one is skipped unless you --enable-debug.
	TestNative.java

TEST_Overflow = Overflow.java

if HAVE_GCJ_SUPPORT
TESTS = $(ALWAYS_TESTS) $(TEST_Overflow)
else
TESTS = $(ALWAYS_TESTS)
endif

XFAIL_TESTS =

EXTRA_DIST = $(ALWAYS_TESTS) $(TEST_Overflow) \
	CLTestConc_run.java \
	ZipVerifyUtils.java \
	ClassGCTest.java \
	ClassGCTestLater.java \
	LoaderTest_Class0.java \
	LoaderTest_Class1.java \
	LoaderTest_Class1BeanInfo.java \
	LoaderTest_Class2.java \
	LoaderTest_Class3.java \
	LoaderTest_Loader1.java \
	LoaderTest_Loader2.java

CLEANFILES = *.class *.fail *.out core *.core ExceptionTest ziptest1.zip \
	ziptest2.zip frozen_serial

check-TESTS: clean TestScript
