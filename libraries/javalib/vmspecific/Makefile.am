CLASSFILE = kaffe-vminterface.jar
jrelib_DATA = $(CLASSFILE)
JAR_CMD1 = $(FASTJAR) cf $(CLASSFILE) `find . -name "*.class"`

CPATH =  $(GLIBJ_ZIP):.:$(srcdir)
JAVAROOT = .
AM_JAVACFLAGS = -bootclasspath $(CPATH)

$(CLASSFILE): classdist_vminterface.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)

clean-local:
	rm -rf $(CLASSFILE) `find . -name "*.class"`

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
vminterfacedir = $(FAKE)

dist_vminterface_JAVA = \
	gnu/classpath/VMStackWalker.java \
	gnu/classpath/VMSystemProperties.java \
	java/lang/Runtime.java \
	java/lang/VMRuntime.java \
	java/lang/VMString.java \
	java/lang/VMSystem.java \
	java/lang/VMThread.java \
	java/lang/VMThrowable.java \
	java/lang/ref/Reference.java \
	java/lang/reflect/Constructor.java \
	java/lang/reflect/Field.java \
	java/lang/reflect/Method.java \
	java/security/VMAccessController.java \
	java/security/VMSecureRandom.java \
	org/kaffe/jar/ExecJar.java \
	org/kaffe/jar/ExecJarName.java \
	org/kaffe/security/LameRandomness.java \
	org/kaffe/security/Randomness.java \
	org/kaffe/security/UnixRandomness.java \
	org/kaffe/util/NotImplemented.java \
	org/kaffe/util/Ptr.java \
	org/kaffe/util/SupportDisabled.java 
