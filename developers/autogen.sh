#! /bin/sh

set -e

# This script runs all of the various auto* programs in the correct order.
# You should run this from the top-level directory.
# Written by Mo DeJong.

## update class list breaks on Mandrake 9.0
## generating a ton of double entries
##

# Check for versions of various tools to use when regenerating 
# Makefiles and configure scripts - if you want to use different
# versions, use --override.  The purpose of these checks is to just
# make sure that people are using consistent versions of tools
# when checking into CVS so we have predictable regression.

if [ "$1" != "--override" ]; then

WANTED_AUTOMAKE_VERS="1.7.5"
WANTED_AUTOCONF_VERS="2.57"

ACLOCAL_VERS=`aclocal --version | 
	sed -n 's,^aclocal (GNU automake) \(.*\)$,\1,p'`
if [ "$ACLOCAL_VERS" != "$WANTED_AUTOMAKE_VERS" ]; then
	echo "Missing or wrong version for aclocal (from automake)."
	echo "We want automake $WANTED_AUTOMAKE_VERS"
	if [ -n "$ACLOCAL_VERS" ]; then
		echo "We found aclocal from automake $ACLOCAL_VERS"
	fi
	exit 1
fi
 
AUTOHEADER_VERS=`autoheader --version | 
	sed -n 's,^autoheader (GNU Autoconf) \(.*\)$,\1,p'`
if [ "$AUTOHEADER_VERS" != "$WANTED_AUTOCONF_VERS" ]; then
	echo "Missing or wrong version for autoheader (from autoconf)."
	echo "We want autoconf $WANTED_AUTOCONF_VERS"
	if [ -n "$AUTOHEADER_VERS" ]; then
		echo "We found autoheader from autoconf $AUTOHEADER_VERS"
	fi
	exit 1
fi

AUTOMAKE_VERS=`automake --version | 
	sed -n 's,^automake (GNU automake) \(.*\)$,\1,p'`
if [ "$AUTOMAKE_VERS" != "$WANTED_AUTOMAKE_VERS" ]; then
	echo "Missing or wrong version for automake."
	echo "We want automake $WANTED_AUTOMAKE_VERS"
	if [ -n "$AUTOMAKE_VERS" ]; then
		echo "We found automake $AUTOMAKE_VERS"
	fi
	exit 1
fi

AUTOCONF_VERS=`autoconf --version | 
	sed -n 's,^autoconf (GNU Autoconf) \(.*\)$,\1,p'`
if [ "$AUTOCONF_VERS" != "$WANTED_AUTOCONF_VERS" ]; then
	echo "Missing or wrong version for autoconf."
	echo "We want autoconf $WANTED_AUTOCONF_VERS"
	if [ -n "$AUTOCONF_VERS" ]; then
		echo "We found autoconf $AUTOCONF_VERS"
	fi
	exit 1
fi

fi

( cd libraries/javalib && ../../developers/update-class-list )
aclocal -I .
autoheader -Wall
automake --add-missing --copy -Wall
autoconf -Wall

