# Compile time class files.
#
# Copyright (c) 2004
#      Kaffe.org contributors. See ChangeLogs for details. All rights reserved.
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.

CLASS_FILES = \
	VarInit.class \
	VarInit2.class

SETUP_ENVIRONMENT = env `. $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

## We have to list the dependencies explicitely for those make programs
## that don't support GNU make's %.suffix target extension.

VarInit.class VarInit2.class: VarInit.java VarInit2.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/VarInit.java $(srcdir)/VarInit2.java

check-am: CompileScript $(CLASS_FILES)

CLEANFILES=$(CLASS_FILES)

EXTRA_DIST = \
	VarInit.java \
	VarInit2.java
