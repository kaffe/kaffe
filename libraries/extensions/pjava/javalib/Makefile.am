#
# pjava extension library component.
#
# Copyright (c) 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are not currently part of the integrate Make cycle.
#

CLASSFILE = pjava.jar
SRCDIRS = com

jrelib_DATA = $(CLASSFILE)

pjava_jar_SRCS = \
	$(com_sun_awt_SRCS) \
	$(com_sun_util_SRCS)
com_sun_awt_SRCS = \
	com/sun/awt/ActionInputPreferred.java \
	com/sun/awt/KeyboardInputPreferred.java \
	com/sun/awt/NoInputPreferred.java \
	com/sun/awt/PositionalInputPreferred.java
com_sun_util_SRCS = \
	com/sun/util/PTimer.java \
	com/sun/util/PTimerImpl.java \
	com/sun/util/PTimerSpec.java \
	com/sun/util/PTimerWentOffEvent.java \
	com/sun/util/PTimerWentOffListener.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = $(pjava_jar_SRCS)

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
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(pjava_jar_SRCS)
	@echo $(SHELL) $(rebuildLib)
	@CPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(pjava_jar_SRCS)
	@echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(LIBDIR)/pjava.jar: $(LIBDIR)/stamp
	rm -f $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) $(SRCDIRS))

.PHONY: build-classes Klasses
build-classes Klasses: $(LIBDIR)/$(CLASSFILE)
	rm -f $(srcdir)/$(CLASSFILE)
	cp $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
