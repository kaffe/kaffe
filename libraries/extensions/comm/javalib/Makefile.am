#
# comm extension library component.
#
# Copyright (c) 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class libraries are not currently part of the integrate Make cycle.
#

CLASSFILE = comm.jar
SRCDIRS = javax

jrelib_DATA = $(CLASSFILE)

comm_jar_SRCS = \
	$(javax_comm_SRCS)
javax_comm_SRCS = \
	javax/comm/CommDriver.java \
	javax/comm/CommPort.java \
	javax/comm/CommPortIdentifier.java \
	javax/comm/CommPortOwnershipListener.java \
	javax/comm/NoSuchPortException.java \
	javax/comm/ParallelPort.java \
	javax/comm/ParallelPortEvent.java \
	javax/comm/ParallelPortEventListener.java \
	javax/comm/PortInUseException.java \
	javax/comm/SerialPort.java \
	javax/comm/SerialPortEvent.java \
	javax/comm/SerialPortEventListener.java \
	javax/comm/UnsupportedCommOperationException.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = $(comm_jar_SRCS)

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
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(comm_jar_SRCS)
	@echo $(SHELL) $(rebuildLib)
	@CPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(comm_jar_SRCS)
	@echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(LIBDIR)/comm.jar: $(LIBDIR)/stamp
	rm -f $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) $(SRCDIRS))

.PHONY: build-classes Klasses
build-classes Klasses: $(LIBDIR)/$(CLASSFILE)
	rm -f $(srcdir)/$(CLASSFILE)
	cp $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
