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

CLEANFILES=`find . -name "*.class"`

CPATH =  .:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAROOT = .

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
runtimedir = $(FAKE)

dist_runtime_JAVA = \
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
	PublicToPrivateField.java \
	StaticToInst.java
