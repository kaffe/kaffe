# Base level Makefile.in for kaffe.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

if COND_CLASSPATH_TOOLS
MAYBE_CLASSPATH_TOOLS=javatools
endif

SUBDIRS = $(MAYBE_CLASSPATH_TOOLS)

install-exec-local:
	if ! test -f $(DESTDIR)${toolslibdir}/tools.jar ; then \
                $(LN_S) $(CLASSPATH_PREFIX)/share/classpath/tools.zip $(DESTDIR)${toolslibdir}/tools.jar ; \
        fi

uninstall-local:
	$(RM) $(DESTDIR)${toolslibdir}/tools.jar 
