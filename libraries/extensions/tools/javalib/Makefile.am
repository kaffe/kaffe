#
# tools extension library component.
#
# Copyright (c) 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are not currently part of the integrate Make cycle.
#

CLASSFILE = tools.jar
SRCDIRS = kaffe

class_DATA = $(CLASSFILE)

tools_jar_SRCS = \
	$(kaffe_tools_compiler_SRCS) \
	$(kaffe_tools_jar_SRCS) \
	$(kaffe_tools_javap_SRCS) \
	$(kaffe_tools_serialver_SRCS)
kaffe_tools_compiler_SRCS = \
	kaffe/tools/compiler/Compiler.java \
	kaffe/tools/compiler/Compiler_default.java \
	kaffe/tools/compiler/Compiler_jikes.java \
	kaffe/tools/compiler/Compiler_kjc.java \
	kaffe/tools/compiler/Compiler_pizza.java
kaffe_tools_jar_SRCS = \
	kaffe/tools/jar/Jar.java \
	kaffe/tools/jar/XPFile.java \
	kaffe/tools/jar/XPFileInputStream.java \
	kaffe/tools/jar/XPFileOutputStream.java \
	kaffe/tools/jar/XPFileReader.java \
	kaffe/tools/jar/XPFileWriter.java
kaffe_tools_javap_SRCS = \
	kaffe/tools/javap/JavaP.java
kaffe_tools_serialver_SRCS = \
	kaffe/tools/serialver/SerialVer.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = $(class_DATA) $(tools_jar_SRCS)

LIBDIR = lib

clean-hook:
	rm -rf $(LIBDIR)

.PHONY: new-classes
new-classes: $(top_srcdir)/developers/update-class-list
	cd $(srcdir) && $(SHELL) $(top_srcdir)/developers/update-class-list

rebuildLib = $(top_builddir)/libraries/javalib/rebuildLib
Klasses_jar = $(top_srcdir)/libraries/javalib/Klasses.jar

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(tools_jar_SRCS)
	@echo $(SHELL) $(rebuildLib)
	@CPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(tools_jar_SRCS)
	@echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(LIBDIR)/tools.jar: $(LIBDIR)/stamp
	rm -f $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) $(SRCDIRS))

.PHONY: build-classes Klasses
build-classes Klasses: $(LIBDIR)/$(CLASSFILE)
	rm -f $(srcdir)/$(CLASSFILE)
	cp $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
