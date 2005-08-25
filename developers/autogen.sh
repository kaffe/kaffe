#! /bin/sh

set -e

# This script runs all of the various auto* programs in the correct order.
# You should run this from the top-level directory.
# Written by Mo DeJong.

export LC_ALL=C

# Check for versions of various tools to use when regenerating 
# Makefiles and configure scripts - if you want to use different
# versions, use --override.  The purpose of these checks is to just
# make sure that people are using consistent versions of tools
# when checking into CVS so we have predictable regression.

if [ "$1" != "--override" ]; then

WANTED_AUTOMAKE_VERS="1.9.6"
WANTED_AUTOCONF_VERS="2.59"
WANTED_LIBTOOL_VERS="1.5.18"
WANTED_AUTOPOINT_VERS="0.14.5"

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

LIBTOOLIZE_VERS=`libtoolize --version | 
	sed -n 's,^libtoolize (GNU libtool) \(.*\)$,\1,p'`
if [ "$LIBTOOLIZE_VERS" != "$WANTED_LIBTOOL_VERS" ]; then
	echo "Missing or wrong version for libtoolize (from libtool)."
	echo "We want libtool $WANTED_LIBTOOL_VERS"
	if [ -n "$LIBTOOLIZE_VERS" ]; then
		echo "We found libtoolize from libtool $LIBTOOLIZE_VERS"
	fi
	exit 1
fi

AUTOPOINT_VERS=`gettext --version |
        sed -n 's,^gettext (GNU gettext-runtime) \(.*\)$,\1,p'`
if [ "$AUTOPOINT_VERS" != "$WANTED_AUTOPOINT_VERS" ]; then
        echo "Missing or wrong version for autopoint (from gettext)."
        echo "We want autopoint $WANTED_AUTOPOINT_VERS"
        if [ -n "$AUTOPOINT_VERS" ]; then
                echo "We found autopoint from gettext $AUTOPOINT_VERS"
        fi
        exit 1
fi

fi

( cd libraries/javalib && ../../developers/update-class-list )

# Delete old files to make sure we regenerate things
# automake things
rm -f depcomp missing config.guess config.sub install-sh
# libtool things
rm -f aclocal.m4 ltmain.sh libtool.m4 ltconfig
(
 cd libltdl
 rm -f acinclude.m4 config-h.in configure.ac install-sh
 rm -f ltmain.sh missing aclocal.m4 config.sub COPYING.LIB
 rm -f ltdl.c Makefile.am mkinstalldirs config.guess configure
 rm -f ltdl.h Makefile.in README
)

# autoconf things
rm -f aclocal.m4 configure
rm -f config/config.h.in
find . -type f -name 'Makefile.in' | xargs rm -f

# Now regenerate autotools
libtoolize --automake --ltdl --copy --force
# add some libtool patches if necessary here
cp libltdl/acinclude.m4 m4/libtool.m4

# gettextize kaffe
# commented out due to bugs in gettextize
##gettextize -c -f --intl

autopoint -f
aclocal -I m4
autoheader # -Wall
automake --add-missing --force-missing --copy # -Wall || true  # ignore warnings
autoconf # -Wall

(
 cd libltdl
 # Need to regenerate things because patching 	 
 # screws up timestamps 	 
 autoreconf -i # -Wall
 touch config-h.in
) 	 

(
  cd kaffe/kaffevm/boehm-gc/boehm

  autoreconf -i # -Wall
)
