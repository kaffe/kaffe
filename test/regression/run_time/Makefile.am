# Run time class files.
#
# Copyright (c) 2002, 2003 University of Utah and the Flux Group.
# All rights reserved.
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# Contributed by the Flux Research Group, Department of Computer Science,
# University of Utah, http://www.cs.utah.edu/flux/

CLASS_FILES = \
	ClassToInterface.class \
	DefinedToAbstract.class \
	DefinedToUndefined.class \
	InterfaceToClass.class \
	IntToFloat.class \
	InstToStatic.class \
	a/ProtectedToPrivate.class \
	a/PublicToPackage.class \
	a/PublicToProtected.class \
	PublicToPrivate.class \
	StaticToInst.class

SETUP_ENVIRONMENT = env `. $(top_builddir)/BUILD_ENVIRONMENT; sed 's/.*export \(.*\)/echo \1=$$\1/' < $(top_builddir)/BUILD_ENVIRONMENT | sh`

%.class: %.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $<

all-am: CompileScript $(CLASS_FILES)

check-am: all-am

CLEANFILES=$(CLASS_FILES)

EXTRA_DIST = \
	ClassToInterface.java \
	DefinedToAbstract.java \
	DefinedToUndefined.java \
	InterfaceToClass.java \
	IntToFloat.java \
	InstToStatic.java \
	a/ProtectedToPrivate.java \
	a/PublicToPackage.java \
	a/PublicToProtected.java \
	PublicToPrivate.java \
	StaticToInst.java
