CLASSFILE = gmpjavamath.jar
SRCDIRS = java
jrelib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS))

CPATH =  .:$(srcdir):$(LIBDIR):$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -bootclasspath $(CPATH)
JAVAC = $(JAVA_COMPILER)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_gmpjavamath.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
gmpjavamathdir = $(FAKE)

dist_gmpjavamath_JAVA = \
	java/math/BigDecimal.java \
	java/math/BigInteger.java

