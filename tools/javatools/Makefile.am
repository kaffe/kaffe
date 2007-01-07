CLASSFILE = kaffe-classpath-tools.jar
SRCDIRS = gnu 
toolslib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS))
JAR_CMD2 = ($(FASTJAR) uf $(CLASSFILE) -C $(srcdir) org)

CPATH =  .:$(LIBDIR):$(GLIBJ_ZIP)
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAC = $(JAVA_COMPILER)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_tools.stamp
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
toolsdir = $(FAKE)

dist_tools_JAVA = \
	gnu/classpath/tools/javap/GnuByteCodeJavap.java \
	gnu/classpath/tools/javap/Javap.java \
	gnu/classpath/tools/javap/JavapMain.java \
	gnu/classpath/tools/Util.java \
	gnu/bytecodecvssnap/Access.java \
	gnu/bytecodecvssnap/ArrayClassLoader.java \
	gnu/bytecodecvssnap/ArrayType.java \
	gnu/bytecodecvssnap/AttrContainer.java \
	gnu/bytecodecvssnap/Attribute.java \
	gnu/bytecodecvssnap/ClassFileInput.java \
	gnu/bytecodecvssnap/ClassType.java \
	gnu/bytecodecvssnap/ClassTypeWriter.java \
	gnu/bytecodecvssnap/CodeAttr.java \
	gnu/bytecodecvssnap/ConstantPool.java \
	gnu/bytecodecvssnap/ConstantValueAttr.java \
	gnu/bytecodecvssnap/CpoolClass.java \
	gnu/bytecodecvssnap/CpoolEntry.java \
	gnu/bytecodecvssnap/CpoolFloat.java \
	gnu/bytecodecvssnap/CpoolNameAndType.java \
	gnu/bytecodecvssnap/CpoolRef.java \
	gnu/bytecodecvssnap/CpoolString.java \
	gnu/bytecodecvssnap/CpoolUtf8.java \
	gnu/bytecodecvssnap/CpoolValue1.java \
	gnu/bytecodecvssnap/CpoolValue2.java \
	gnu/bytecodecvssnap/ExceptionsAttr.java \
	gnu/bytecodecvssnap/Field.java \
	gnu/bytecodecvssnap/Filter.java \
	gnu/bytecodecvssnap/IfState.java \
	gnu/bytecodecvssnap/InnerClassesAttr.java \
	gnu/bytecodecvssnap/Label.java \
	gnu/bytecodecvssnap/LineNumbersAttr.java \
	gnu/bytecodecvssnap/LocalVarsAttr.java \
	gnu/bytecodecvssnap/Location.java \
	gnu/bytecodecvssnap/Method.java \
	gnu/bytecodecvssnap/MiscAttr.java \
	gnu/bytecodecvssnap/ObjectType.java \
	gnu/bytecodecvssnap/PrimType.java \
	gnu/bytecodecvssnap/Scope.java \
	gnu/bytecodecvssnap/SourceDebugExtAttr.java \
	gnu/bytecodecvssnap/SourceFileAttr.java \
	gnu/bytecodecvssnap/SwitchState.java \
	gnu/bytecodecvssnap/TryState.java \
	gnu/bytecodecvssnap/Type.java \
	gnu/bytecodecvssnap/VarEnumerator.java \
	gnu/bytecodecvssnap/Variable.java \
	gnu/bytecodecvssnap/ZipArchive.java \
	gnu/bytecodecvssnap/ZipLoader.java \
	gnu/bytecodecvssnap/dump.java 
