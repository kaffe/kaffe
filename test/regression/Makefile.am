# Regression testing
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

TESTS = TestScript TestScriptThread

TESTS_ENVIRONMENT = \
	CLASSPATH=$${KAFFE_TEST_CLASSPATH-.}'$(PATHSEP)'$(top_srcdir)/libraries/javalib/Klasses.jar'$(PATHSEP)'$(top_srcdir)/libraries/javalib/pizza.jar \
	KAFFELIBRARYPATH=$${KAFFE_TEST_LIBRARYPATH+"$$KAFFE_TEST_LIBRARY_PATH"'$(PATHSEP)'}`for f in $(JAVA_LIBS); do echo "$$f" | sed 's%/[^/]*$$%%'; done | tr '\012' '$(PATHSEP)' | sed s/.$$//`} \
	JAVA=$${JAVA-$(top_builddir)/kaffe/kaffe/Kaffe} \
	$(SHELL)
# This last line assumes that all tests are shell-scripts

EXTRA_DIST = *.java *.out
CLEANFILES = *.class *.fail

check-TESTS: clean
