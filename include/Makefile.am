# Library support
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

KLASSES_JAR = $(top_srcdir)/libraries/javalib/Klasses.jar

KAFFEH_HDRS = \
	java_lang_Throwable.h \
	java_lang_Thread.h \
	java_lang_ThreadGroup.h

noinst_HEADERS = \
	defs.h \
	files.h \
	nets.h \
	system.h

include_HEADERS = $(KAFFEH_HDRS) \
	jtypes.h \
	native.h \
	jmalloc.h \
	jsyscall.h \
	jni.h \
	jni_cpp.h \
	Arrays.h \
	java_lang_Object.h \
	java_lang_String.h

$(KAFFEH_HDRS): $(KLASSES_JAR)
	if test -x $(KAFFEH_FOR_BUILD); then \
	  $(KAFFEH_FOR_BUILD) -d @srcdir@ -classpath $(KLASSES_JAR) \
	    `echo $@ | sed -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	fi

