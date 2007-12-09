# Compile time class files.
#
# Copyright (c) 2004
#      Kaffe.org contributors. See ChangeLogs for details. All rights reserved.
#
# This file is licensed under the terms of the GNU Public License.
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.


CLEANFILES=*.class

CPATH =  .:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -classpath $(CPATH)
JAVAROOT = .

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
compilerdir = $(FAKE)

dist_compiler_JAVA = \
	VarInit.java \
	VarInit2.java \
	InnerTest1_A.java \
	InnerTest1_B.java
