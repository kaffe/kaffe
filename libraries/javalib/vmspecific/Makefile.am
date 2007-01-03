CLASSFILE = kaffe-vminterface.jar
SRCDIRS = org
jrelib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS))
JAR_CMD2 = ($(FASTJAR) uf $(CLASSFILE) -C $(LIBDIR) gnu)
JAR_CMD3 = ($(FASTJAR) uf $(CLASSFILE) -C $(LIBDIR) java)

CPATH =  .:$(srcdir):$(LIBDIR):$(GLIBJ_ZIP)
AM_JAVACFLAGS = -bootclasspath $(CPATH)
JAVAC = $(JAVA_COMPILER)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_vminterface.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)
	$(JAR_CMD2)
	$(JAR_CMD3)

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

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
	gnu/java/net/InetAddressImpl.java \
	gnu/java/net/SysInetAddressImpl.java \
	java/lang/Runtime.java \
	java/lang/VMClassLoader.java \
	java/lang/VMProcess.java \
	java/lang/VMRuntime.java \
	java/lang/VMString.java \
	java/lang/VMSystem.java \
	java/lang/VMThread.java \
	java/lang/VMThrowable.java \
	java/lang/ref/Reference.java \
	java/lang/reflect/Constructor.java \
	java/lang/reflect/Field.java \
	java/lang/reflect/Method.java \
	java/lang/reflect/VMArray.java \
	java/net/VMInetAddress.java \
	java/security/VMAccessController.java \
	java/security/VMSecureRandom.java \
	org/kaffe/jar/ExecJar.java \
	org/kaffe/jar/ExecJarName.java \
	org/kaffe/management/Classpath.java \
	org/kaffe/management/Debug.java \
	org/kaffe/management/JIT.java \
	org/kaffe/management/XProfiler.java \
	org/kaffe/security/provider/Kaffe.java \
	org/kaffe/security/provider/MD2.java \
	org/kaffe/security/provider/MD4.java \
	org/kaffe/security/provider/MD5.java \
	org/kaffe/security/provider/SHA.java \
	org/kaffe/security/provider/SHA1PRNG.java \
	org/kaffe/security/provider/UpdateDigest.java \
	org/kaffe/security/LameRandomness.java \
	org/kaffe/security/Randomness.java \
	org/kaffe/security/UnixRandomness.java \
	org/kaffe/util/NotImplemented.java \
	org/kaffe/util/Ptr.java \
	org/kaffe/util/SupportDisabled.java 
