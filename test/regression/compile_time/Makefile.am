# Compile time class files.
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

## We have to list the dependencies explicitely for those make programs
## that don't support GNU make's %.suffix target extension.

ClassToInterface.class: ClassToInterface.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/ClassToInterface.java

DefinedToAbstract.class: DefinedToAbstract.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/DefinedToAbstract.java

DefinedToUndefined.class: DefinedToUndefined.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/DefinedToUndefined.java

InterfaceToClass.class: InterfaceToClass.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/InterfaceToClass.java

IntToFloat.class: IntToFloat.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/IntToFloat.java

InstToStatic.class: InstToStatic.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/InstToStatic.java

a/ProtectedToPrivate.class: a/ProtectedToPrivate.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/a/ProtectedToPrivate.java

a/PublicToPackage.class: a/PublicToPackage.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/a/PublicToPackage.java

a/PublicToProtected.class: a/PublicToProtected.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/a/PublicToProtected.java

PublicToPrivate.class: PublicToPrivate.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/PublicToPrivate.java

StaticToInst.class: StaticToInst.java
	@$(SETUP_ENVIRONMENT) $(SHELL) CompileScript $(srcdir)/StaticToInst.java

all-am: 

check-am: CompileScript $(CLASS_FILES)

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
