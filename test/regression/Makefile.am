# Regression testing
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SETUP_ENVIRONMENT = env `. $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

TESTS_ENVIRONMENT = $(SETUP_ENVIRONMENT) $(SHELL) TestScript

if HAVE_GCJ_SUPPORT
TEST_Overflow = #
else
TEST_Overflow = Overflow.java
endif

TESTS = \
## Simple health check
	HelloWorldApp.class.save \
## Now test the compiler
	HelloWorldApp.java \
## Test ints, longs, floats and doubles
	TestIntLong.java \
	TestFloatDouble.java \
	DoubleNeg.java \
	DoublePrint.java \
	DoubleComp.java \
	ModuloTest.java \
	LongNeg.java \
## Test strings
	Str.java \
	Str2.java \
	InternHog.java \
## Test exceptions
	IndexTest.java \
	StackDump.java \
## Test threads
	tname.java \
	ttest.java \
	ThreadInterrupt.java \
	ThreadState.java \
	UncaughtException.java \
	IllegalWait.java \
## Test that preemption works---may not be supported by all threading systems
	Preempt.java 		\
## Misc tests
	ReflectInterfaces.java \
	TestSerializable.java \
	TestCasts.java \
	Alias.java \
	NullPointerTest.java \
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
	finalexc.java \
	finaltest.java \
	finaltest2.java \
	forNameTest.java \
	KaffeVerifyBug.java \
	Schtum.java \
	Reflect.java \
	MethodBug.java \
	Bean.java \
	SortTest.java \
	HashTest.java \
	MapTest.java \
	URLTest.java \
	ReaderTest.java \
	BufferedReaderTest.java \
	ReaderReadVoidTest.java \
## simple test for Method.invoke
	ReflectInvoke.java \
## simple test for InvocationTargetExceptions
	InvTarExcTest.java \
## tests for ClassLoader
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
	$(TEST_Overflow) \
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
## This one is skipped unless you --enable-debug.
	TestNative.java

EXTRA_DIST = $(TESTS)

CLEANFILES = *.class *.fail *.out core *.core ExceptionTest

check-TESTS: clean TestScript
