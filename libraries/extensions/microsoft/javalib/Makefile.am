#
# Microsoft extension library component.
#
# Copyright (c) 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are not currently part of the integrate Make cycle.
#

CLASSFILE = microsoft.jar
SRCDIRS = com

jrelib_DATA = $(CLASSFILE)

microsoft_jar_SRCS = \
	$(com_ms_lang_SRCS)
com_ms_lang_SRCS = \
	com/ms/lang/Delegate.java \
	com/ms/lang/MulticastNotSupportedException.java \
	com/ms/lang/MulticastDelegate.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = $(microsoft_jar_SRCS)

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
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(microsoft_jar_SRCS)
	@echo $(SHELL) $(rebuildLib)
	@CPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(microsoft_jar_SRCS)
	@echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(LIBDIR)/microsoft.jar: $(LIBDIR)/stamp
	rm -f $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) $(SRCDIRS))

.PHONY: build-classes Klasses
build-classes Klasses: $(LIBDIR)/$(CLASSFILE)
	rm -f $(srcdir)/$(CLASSFILE)
	cp $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
