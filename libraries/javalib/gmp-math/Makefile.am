CLASSFILE = gmpjavamath.jar
jrelib_DATA = $(CLASSFILE)
JAR_CMD1 = $(FASTJAR) cf $(CLASSFILE) `find . -name "*.class"`

CPATH =  .:$(srcdir):$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -bootclasspath $(CPATH)
JAVAROOT = .

$(CLASSFILE): classdist_gmpjavamath.stamp
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
gmpjavamathdir = $(FAKE)

dist_gmpjavamath_JAVA = \
	java/math/BigDecimal.java \
	java/math/BigInteger.java

