CLASSFILE = kaffe-zlib.jar
SRCDIRS = org java
jrelib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) org)
JAR_CMD2 = ($(FASTJAR) uf $(CLASSFILE) -C $(LIBDIR) java)

CPATH =  .:$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar:$(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAC = $(JAVA_COMPILER)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_zlib.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)
	$(JAR_CMD2)

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
zlibdir = $(FAKE)

dist_zlib_JAVA = \
	java/util/zip/Adler32.java \
	java/util/zip/CRC32.java \
	java/util/zip/Deflater.java \
	java/util/zip/DeflaterOutputStream.java \
	java/util/zip/Inflater.java \
	java/util/zip/ZipEntry.java \
	java/util/zip/ZipFile.java \
	java/util/zip/ZipInputStream.java \
	java/util/zip/ZipOutputStream.java \
	org/kaffe/util/Assert.java \
	org/kaffe/util/UTF8.java \
	org/kaffe/util/zip/SwitchInflater.java 
