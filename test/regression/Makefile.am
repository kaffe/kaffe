# Regression testing
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

TESTS = TestScript TestScriptThread

SETUP_ENVIRONMENT = . $(top_builddir)/BUILD_ENVIRONMENT

TESTS_ENVIRONMENT = $(SETUP_ENVIRONMENT); $(SHELL)
# The $(SHELL) above assumes that all tests are shell-scripts
# which saves us from having to ensure that they are executable

EXTRA_DIST = \
	BadFloatTest.java \
	BadFloatTest.out \
	Bean.java \
	Bean.out \
	BeanBug.java \
	BeanBug.out \
	BitSetTest.java \
	BitSetTest.out \
	CLTest.java \
	CLTest.out \
	CLTestConc.java \
	CLTestConc.out \
	CLTestConc_run.java \
	CLTestFindLoaded.java \
	CLTestFindLoaded.out \
	CLTestJLock.java \
	CLTestJLock.out \
	CLTestLie.java \
	CLTestLie.out \
	CatchDeath.java \
	CatchDeath.out \
	CharCvt.java \
	CharCvt.out \
	ClassGC.java \
	ClassGC.out \
	ClassGCTest.java \
	ClassGCTestLater.java \
	ConstructorTest.java \
	ConstructorTest.out \
	DeadThread.java \
	DeadThread.out \
	DoubleBug.java \
	DoubleBug.out \
	ExceptionInInitializerTest.java \
	ExceptionInInitializerTest.out \
	ExceptionTest.java \
	ExceptionTestClassLoader.java \
	ExceptionTestClassLoader.out \
	ExceptionTestClassLoader2.java \
	ExceptionTestClassLoader2.out \
	ExecTest.java \
	ExecTest.out \
	FindSystemClass.java \
	FindSystemClass.out \
	GCTest.java \
	GCTest.out \
	GetInterfaces.java \
	GetInterfaces.out \
	HashtableTest1.java \
	HashtableTest1.out \
	Hello.java \
	HelloWorldApp.java \
	HelloWorldApp.out \
	IllegalInterface.java \
	IllegalInterface.out \
	IndexTest.java \
	IndexTest.out \
	InitTest.java \
	InitTest.out \
	InternHog.java \
	InternHog.out \
	InvTarExcTest.java \
	InvTarExcTest.out \
	KaffeVerifyBug.java \
	KaffeVerifyBug.out \
	LostFrame.java \
	LostFrame.out \
	MarkResetTest.java \
	MarkResetTest.out \
	MethodBug.java \
	MethodBug.out \
	ModuloTest.java \
	ModuloTest.out \
	NullPointerTest.java \
	NullPointerTest.out \
	Overflow.java \
	Overflow.out \
	ProcessClassInst.java \
	ProcessClassInst.out \
	ProcessClassLockTest.java \
	ProcessClassLockTest.out \
	ProcessClassStop.java \
	ProcessClassStop.out \
	ProcessClassTest.java \
	ProcessClassTest.out \
	ProcessTest.java \
	ProcessTest.out \
	Reflect.java \
	Reflect.out \
	ReflectInterfaces.java \
	ReflectInterfaces.out \
	ReflectInvoke.java \
	ReflectInvoke.out \
	Schtum.java \
	Schtum.out \
	SignedShort.java \
	SignedShort.out \
	StackDump.java \
	StackDump.out \
	Str.java \
	Str.out \
	Str2.java \
	Str2.out \
	TestFloatDouble.java \
	TestFloatDouble.out \
	TestIntLong.java \
	TestIntLong.out \
	TestNative.java \
	TestSerializable.java \
	TestSerializable.out \
	TestUnlock.java \
	TestUnlock.out \
	ThreadInterrupt.java \
	ThreadInterrupt.out \
	ThreadLocalTest.java \
	ThreadLocalTest.out \
	ThreadStop.java \
	ThreadStop.out \
	UDPTest.java \
	UDPTest.out \
	UncaughtException.java \
	UncaughtException.out \
	burford.java \
	burford.out \
	finaltest.java \
	finaltest.out \
	finaltest2.java \
	forNameTest.java \
	forNameTest.out \
	tname.java \
	tname.out \
	ttest.java \
	ttest.out \
	tthrd1.java \
	tthrd1.out \
	wc.java \
	wc.out

CLEANFILES = *.class *.fail

check-TESTS: clean
