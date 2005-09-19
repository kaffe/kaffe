# Regression testing
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

SUBDIRS = compile_time run_time compiler

SETUP_ENVIRONMENT = env `. $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

# Redirect errors to /dev/null to avoid annoying job control
# messages from shell when killing tests due to timeouts
TESTS_ENVIRONMENT = $(SETUP_ENVIRONMENT) $(SHELL) TestScript 2> /dev/null

ALWAYS_TESTS = \
	$(SIMPLE_HEALTH_CHECK) \
	$(TEST_COMPILER) \
	$(TEST_ARRAYS) \
	$(TEST_REFERENCE) \
	$(TEST_INTS_LONGS_FLOATS_DOUBLES) \
	$(TEST_STRINGS) \
	$(TEST_EXCEPTIONS) \
	$(TEST_THREADS) \
	$(TEST_SERIALIZATION) \
	$(TEST_MISC) \
	$(TEST_REFLECTION) \
	$(TEST_CLASS_LOADING) \
	$(TEST_STOPPING_THREADS) \
	$(TEST_ZIP) \
	$(TEST_NATIVE) \
	$(JASMIN_TESTS) \
	$(BCEL_TESTS) \
	$(TEST_GNU_CRYPTO)

## Simple health check
SIMPLE_HEALTH_CHECK = \
	HelloWorldApp.class.save 

## Now test the compiler
TEST_COMPILER = \
	HelloWorldApp.java 

## Test the reference subsystem.
TEST_REFERENCE = \
	RefTest.java

## Test the array creation/handling.
TEST_ARRAYS = \
	MultiArray.java

## Test ints, longs, floats and doubles
TEST_INTS_LONGS_FLOATS_DOUBLES = \
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
	NegativeDivideConst.java \
	divtest.java \
	DoubleIEEE.java

## Test strings
TEST_STRINGS = \
	Str.java \
	Str2.java \
	InternHog.java 

## Test exceptions
## note that CatchLimits can be compiled from CatchLimits.j by Jasmin
TEST_EXCEPTIONS = \
	IndexTest.java \
	StackDump.java \
	CatchLimits.class.save 

## Test threads
## Preempt tests that preemption works---may not be supported by all threading systems
TEST_THREADS = \
	tname.java \
	ttest.java \
	ThreadInterrupt.java \
	ThreadState.java \
	UncaughtException.java \
	IllegalWait.java \
        WaitTest.java \
	Preempt.java 

## Serialization tests
TEST_SERIALIZATION = \
	TestSerializable.java \
	TestSerializable2.java \
	SerializationCompatibility.java \
	SerialPersistentFields.java \
	TestSerialVersions.java \
	TestSerialPersistent.java \
	TestSerialFields.java \
	TestObjectStreamField.java 

## Misc tests
TEST_MISC = \
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
	ArraysTest.java \
	SubListTest.java \
	HashTest.java \
        SecureRandomTest.java \
	MapTest.java \
	URLTest.java \
	PropertiesTest.java \
	ReaderTest.java \
	CharArrayReaderTest.java \
	LineNumberReaderTest.java \
	BufferedReaderTest.java \
	ReaderReadVoidTest.java \
	InputStreamTest.java \
	BufferedInputStreamAvailableTest.java \
	PipeTest.java \
        DateFormatTest.java \
	GetField.java \
	LostTrampolineFrame.java \
        NetworkInterfaceTest.java \
        InetAddressTest.java \
        InetSocketAddressTest.java \
        ShutdownHookTest.java \
	TestMessageFormat.java

TEST_REFLECTION = \
	ReflectInvoke.java \
	InvTarExcTest.java \
	DeleteFile.java

## tests for ClassLoader
TEST_CLASS_LOADING = \
	PrimordialLoaderTest.java \
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
        CLInitThrow.java \
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
	BadClassVersion.java \
	InterfaceSuperClass.java \
	ImplementClass.java \
	FieldErrors.java \
	MethodErrors.java \
	ProhibitedClass.java \
	KaffeInternal.java

## Test catching our own death
## Test various ways of stopping threads
TEST_STOPPING_THREADS = \
	CatchDeath.java \
	DeadThread.java \
	tthrd1.java 

## Test for package java.util.zip
TEST_ZIP = \
	DosTimeVerify.java \
	ZipVerify.java \
	ZipTest.java \
	TruncatedClass.java 

## Test interrupting I/O
## This is kind of tricky, and Sun doesn't support it.
TEST_INTERRUPTING_IO = \
	sysdepCallMethod.java 

## This one is skipped unless you --enable-debug.
TEST_NATIVE = \
	TestNative.java

JASMIN_TESTS_DIST = \
	FloatAlias.j \
	BadCatch1.j \
	BadCatch2.j \
	CircularClass.j \
	CircularInterface.j \
	ClassFlags1.j \
	ClassFlags2.j \
	DuplicateField.j \
	EmptyMethod.j \
	FieldFlags1.j \
	FieldFlags2.j \
	FieldFlags3.j \
	FieldFlags4.j \
	FieldRefSignature1.j \
	FieldSignature1.j \
	FieldSignature2.j \
	FieldSignature3.j \
	FieldSignature4.j \
	InvokedMethodSignature1.j \
	InvokedMethodSignature2.j \
	InvokedMethodSignature3.j \
	MethodFlags1.j \
	MethodFlags2.j \
	MethodFlags3.j \
	MethodFlags4.j \
	MethodFlags5.j \
	MethodSignature1.j \
	MethodSignature2.j \
	MethodSignature3.j \
	MethodSignature4.j \
	MethodSignature5.j \
	MethodSignature6.j \
	MethodSignature7.j \
	MethodSignature8.j \
	MethodSignature9.j

if HAVE_JASMIN
JASMIN_TESTS = $(JASMIN_TESTS_DIST)
else
JASMIN_TESTS =
endif

BCEL_TESTS_DIST = \
	BadClassFileCode.java \
	BadClassFileConstants.java

if HAVE_BCEL
BCEL_TESTS = $(BCEL_TESTS_DIST)
else
BCEL_TESTS =
endif


## Test for GNU Crypto classes
TEST_GNU_CRYPTO = \
	TestGnuCrypto.java

TEST_Overflow = Overflow.java

if HAVE_GCJ_SUPPORT
TESTS = $(ALWAYS_TESTS) $(TEST_Overflow)
else
TESTS = $(ALWAYS_TESTS)
endif

XFAIL_TESTS =

EXTRA_DIST = $(ALWAYS_TESTS) \
	$(TEST_Overflow) \
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
	LoaderTest_Loader2.java \
	$(JASMIN_TESTS_DIST) \
	CatchLimits.j \
	$(BCEL_TESTS_DIST) \
	TestGnuCrypto.java \
	$(TEST_INTERRUPTING_IO)

CLEANFILES = \
	*.class \
	*.fail \
	*.out \
	core \
	*.core \
	ExceptionTest \
	ziptest1.zip \
	ziptest2.zip \
	frozen_serial \
	serializedFoos.bin

all-am: TestScript
