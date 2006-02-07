#
# Javadoc library component.
#
# Copyright (c) 2004
# 	Kaffe.org contributors. All rights reserved. See ChageLog for details.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

CLASSFILE = tools.jar
SRCDIRS = gnu

toolslib_DATA = $(CLASSFILE)

tools_jar_SRCS = \
	gnu/classpath/tools/Util.java \
	gnu/classpath/tools/javap/GnuByteCodeJavap.java \
	gnu/classpath/tools/javap/Javap.java \
	gnu/classpath/tools/javap/JavapMain.java \
	gnu/classpath/tools/native2ascii/Native2ASCII.java \
	gnu/classpath/tools/rmi/rmic/RMIC.java \
	gnu/classpath/tools/rmi/rmic/RMICException.java \
	gnu/classpath/tools/rmi/rmic/Variables.java \
	gnu/classpath/tools/serialver/SerialVer.java
		 

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = \
	$(tools_jar_SRCS)

JAR_CMD = ($(FASTJAR) cf $(CURDIR)/$@.new -C $(LIBDIR) $(SRCDIRS))

LIBDIR = lib

clean-local:
	rm -rf $(LIBDIR)
	rm -f $(toolslib_DATA)

rebuildLib = $(top_builddir)/libraries/javalib/rebuildLib
real_builddir = $(shell cd $(top_builddir) && pwd )
Klasses_jar = $(real_builddir)/libraries/javalib/external/classpath/lib/glibj.zip

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(antlr_SRCS) $(gjdoc_jar_SRCS) $(gjdoc_jar_GENSOURCES)
	rm -rf $(LIBDIR)
	mkdir $(LIBDIR)
	echo $(SHELL) $(rebuildLib)
	BOOTCLASSPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(tools_jar_SRCS)
	echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes: $(CLASSFILE)
$(CLASSFILE): $(LIBDIR)/stamp
	rm -f $@ $@.new
	$(JAR_CMD)
	mv $@.new $@

export JAVA_COMPILER top_builddir LIBDIR srcdir
