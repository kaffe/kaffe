# Library support
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

KAFFEH=		$(top_builddir)/kaffe/kaffeh/kaffeh@EXEEXT@
KLASSES_JAR=    $(top_srcdir)/libraries/javalib/Klasses.jar

KAFFEH_HDRS = \
		$(srcdir)/java_lang_Throwable.h \
		$(srcdir)/java_lang_Thread.h \
		$(srcdir)/java_lang_ThreadGroup.h

BUILT_SOURCES = jtypes.h

include_HEADERS = $(BUILT_SOURCES) $(KAFFEH_HDRS) \
		native.h \
		jmalloc.h \
		jsyscall.h \
		jni.h \
		Arrays.h \
		java_lang_Object.h \
		java_lang_String.h

$(KAFFEH_HDRS): $(KLASSES_JAR)
	if test -x $(KAFFEH); then \
	  $(KAFFEH) -d @srcdir@ -classpath $(KLASSES_JAR) \
	    `echo $@ | sed -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	fi

