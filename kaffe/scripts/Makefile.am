# Misc. scripts
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

DIST_SUBDIRS = bat compat

SCRIPTFILES_KJC=\
		kjc

SCRIPTFILES_SUN=\
		jar javakey javap jdb \
		native2ascii rmic rmiregistry serialver

if SUN_COMPAT
SUBDIRS = compat
SCRIPTFILES_COMPAT=
else
SCRIPTFILES_COMPAT=appletviewer javac javadoc
endif

PSCRIPTFILES=	install-jar kaffe

bin_SCRIPTS = $(SCRIPTFILES_KJC) $(SCRIPTFILES_SUN) \
	$(SCRIPTFILES_COMPAT) $(PSCRIPTFILES)

include $(top_srcdir)/include/distclean-hack
