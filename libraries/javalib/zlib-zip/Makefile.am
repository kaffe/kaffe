CLASSFILE = kaffe-zlib.jar
jrelib_DATA = $(CLASSFILE)
JAR_CMD1 = $(FASTJAR) cf $(CLASSFILE) `find . -name "*.class"`

CPATH =  .:$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -bootclasspath $(CPATH)
JAVAROOT = .

$(CLASSFILE): classdist_zlib.stamp
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
zlibdir = $(FAKE)

dist_zlib_JAVA = \
	java/util/zip/ZipFile.java

