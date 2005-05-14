#
# Kaffe GNU MP-based BigInteger and BigDecimal implementation.
#
# Copyright (c) 2004, 2005
# 	Kaffe.org contributors. All rights reserved. See ChangeLog for details.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

CLASSFILE = gmpjavamath.jar
SRCDIRS = java

jrelib_DATA = $(CLASSFILE)

gmpjavamath_jar_SRCS = \
        $(java_math_SRCS)
java_math_SRCS = \
        java/math/BigDecimal.java \
        java/math/BigInteger.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = \
	$(gmpjavamath_jar_SRCS)

JAR_CMD1 = (dstdir=`pwd`; cd $(LIBDIR) && $(ZIP) -r $$dstdir/$(CLASSFILE) $(SRCDIRS))

LIBDIR = lib

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

.PHONY: new-classes
new-classes: $(top_srcdir)/developers/update-class-list
	cd $(srcdir) && $(SHELL) $(top_srcdir)/developers/update-class-list

rebuildLib = $(top_builddir)/libraries/javalib/rebuildLib
Klasses_jar = $(srcdir):$(top_builddir)/libraries/javalib/rt.jar

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(gmpjavamath_jar_SRCS)
	rm -rf $(LIBDIR)
	mkdir $(LIBDIR)
	echo $(SHELL) $(rebuildLib)
	LIBDIR=$(LIBDIR) CLASSPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(gmpjavamath_jar_SRCS)
	echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(CLASSFILE): $(LIBDIR)/stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)
