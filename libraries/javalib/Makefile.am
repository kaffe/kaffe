#
# Java core library component.
#
# Copyright (c) 1997, 1998, 1999
#      Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Class library's are not currently part of the integrate Make cycle.
#

CLASSFILE = Klasses.jar

class_DATA = $(CLASSFILE) pizza.jar

EXTRA_DIST = `find java kaffe -name \*.java -print`

LIBDIR = lib

clean-hook:
	rm -rf $(LIBDIR)

$(srcdir)/Klasses.jar:
	$(MAKE) classes build-classes

classes:
	$(SHELL) ./rebuildLib

build-classes:
	rm -f $(srcdir)/$(CLASSFILE) $(LIBDIR)/$(CLASSFILE)
	(cd $(LIBDIR) && $(ZIP) -r $(CLASSFILE) java kaffe)
	mv $(LIBDIR)/$(CLASSFILE) $(srcdir)/$(CLASSFILE)
