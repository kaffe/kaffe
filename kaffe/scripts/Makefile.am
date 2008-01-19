# Misc. scripts
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2006, 2007
#       Kaffe.org contributors. See ChangeLog for details.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

DIST_SUBDIRS = bat compat

EXTRA_DIST = nm2as.awk

SCRIPTFILES_SUN= javah

if SUN_COMPAT
SUBDIRS = compat
endif

if ENABLE_XPROF
SCRIPTFILES_XPROF=kaffexprof nm2as.awk
else
SCRIPTFILES_XPROF=
endif

PSCRIPTFILES=	install-jar

jrebin_SCRIPTS = kaffe

SCRIPTFILES_JDB = jdb

bin_SCRIPTS = \
	$(jrebin_SCRIPTS) \
	$(SCRIPTFILES_SUN) \
	$(SCRIPTFILES_COMPAT) \
	$(PSCRIPTFILES) \
	$(SCRIPTFILES_XPROF) \
	$(SCRIPTFILES_JDB)

CLASSPATH_APPS = appletviewer jar jarsigner keytool orbd native2ascii rmiregistry rmic rmid serialver tnameserv

install-exec-local:
	for app in $(CLASSPATH_APPS); do \
		$(LN_S) -f $(CLASSPATH_PREFIX)/bin/g$$app $(DESTDIR)$(bindir)/$$app ; \
	done
	if test x"@JAVADOC@" != x"" ; then \
		$(LN_S) -f @JAVADOC@  $(DESTDIR)$(bindir)/javadoc$(EXEEXT) ; \
	fi
	if test x"@JAVAP@" != x"" ; then \
		$(LN_S) -f @JAVAP@  $(DESTDIR)$(bindir)/javap$(EXEEXT) ; \
	fi

uninstall-local:
	for app in $(CLASSPATH_APPS); do \
		$(RM) $(DESTDIR)$(bindir)/$$app ; \
	done
	if  test x"@JAVADOC@" != x"" ; then \
		$(RM) $(DESTDIR)$(bindir)/javadoc$(EXEEXT) ; \
	fi
	if  test x"@JAVAP@" != x"" ; then \
		$(RM) $(DESTDIR)$(bindir)/javap$(EXEEXT) ; \
	fi
