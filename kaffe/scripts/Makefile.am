# Misc. scripts
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

DIST_SUBDIRS = bat compat

EXTRA_DIST = nm2as.awk

SCRIPTFILES_SUN=\
		jar javah javap \
		native2ascii rmic serialver

if SUN_COMPAT
SUBDIRS = compat
endif

if COND_GJDOC
KAFFE_JAVADOC = javadoc
endif

if COND_NATIVE_AWT
KAFFE_APPLETVIEWER = appletviewer
endif

SCRIPTFILES_COMPAT= \
	$(KAFFE_APPLETVIEWER) \
	$(KAFFE_JAVADOC) 

if ENABLE_XPROF
SCRIPTFILES_XPROF=kaffexprof nm2as.awk
else
SCRIPTFILES_XPROF=
endif

PSCRIPTFILES=	install-jar

jrebin_SCRIPTS = kaffe rmiregistry

SCRIPTFILES_JDB = jdb

bin_SCRIPTS = \
	$(SCRIPTFILES_KJC) \
	$(SCRIPTFILES_SUN) \
	$(SCRIPTFILES_COMPAT) \
	$(PSCRIPTFILES) \
	$(SCRIPTFILES_XPROF) \
	$(SCRIPTFILES_JDB)

#include $(top_srcdir)/include/distclean-hack

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(jrebin_SCRIPTS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_SCRIPT) $$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`"; \
	    $(INSTALL_SCRIPT) $$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	  else if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_SCRIPT) $(srcdir)/$$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`"; \
	    $(INSTALL_SCRIPT) $(srcdir)/$$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	  else :; fi; fi; \
	done

uninstall-local:
	@$(NORMAL_UNINSTALL)
	list='$(jrebin_SCRIPTS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	done


