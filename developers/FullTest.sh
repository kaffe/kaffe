#!/bin/sh
#
# FullTest.sh
#
# Copyright (c) 2002 Pat Tullmann <pat@tullmann.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#


#
# Usage:
#	FullTest.sh <full path to kaffe source> [prefix]
# The [prefix] argument is optional, defaults to the last element
# of the full path.
#
# Build/test/install a full complement of Kaffe variations.  Stores
# full log of the build/test/install.  Prints a summary of steps and
# any failures to stdout.  The commands are all run under 'nice' so
# your machine should still be usable while you run this script.
#
# BEFORE RUNNING: Edit the BASE_OBJ_DIR and BASE_INSTALL_DIR
# variables.  Set them to someplace sensible on your system.  Edit
# GMAKE if necessary to point to GNU make (or drop the MAKE_OPTS?).
# To add a new build combination, give it a name in the BUILDS line,
# and then below in the case statement that interprets the names, set
# the configuration you'd like to test.
#
# TODO:
#

# About 350Mb of object trees
BASE_OBJ_DIR=/home/pat/kaffe-core/obj

# About 70Mb of installed stuff 
BASE_INSTALL_DIR=/home/pat/kaffe-core/install

GMAKE=${GMAKE:-make}
# -s means quiet; -j for multiple jobs
MAKE_OPTS="-s -j 2"

### Control the variations of Kaffe that are tested:  See the case statements
### below to see what these mean (or to add your own).
#BUILDS="debug optimized stats xprof"
BUILDS="debug optimized stats default" #xprof
ENGINE="jit3 intrp"
STATICBUILD="no yes"
CONFIG_COMMON_OPTIONS="--disable-gcj"


SRC="$1"
PREFIX="$2"   # Optional, will be deduced from $SRC if not provided.

if test ! -d "$SRC"; then
    echo "Usage: $0 <full path to Kaffe src> [output prefix]"
    exit 11;
fi

if test ! -x "$SRC/configure"; then
    echo "No configure script in $SRC"
    exit 11;
fi

if test -z "$PREFIX"; then
    PREFIX=`basename $SRC`
fi


status() {
    echo "$@"
}

ok=1

doCmd() {
    # Only do the command if a previous one didn't fail
    if test $ok -eq 1; then
	MSG="$1"
	shift;
	if test ! -z "$MSG"; then
	    status "$MSG"
	fi

	# run the command at a nice level, and put all output in TRACEF
	nice "$@" >> "$TRACEF" 2>&1
	rc=$?
	if test $rc -ne 0; then
	    status "    Command FAILED: $@"
	    status "    tail -5 $TRACEF"
	    tail -5 "$TRACEF" | sed -e 's/^/    /'
	    ok=0;
	fi
    fi
}

for s in $STATICBUILD; do
    for b in $BUILDS; do
        for e in $ENGINE; do
	    doBuild=1

	    # Map the static build yes/no into configuration information
	    case "$s" in 
		yes)
		    STATDIR="-static"
		    CONFIG_STATIC="--with-staticvm --with-staticlib --with-staticbin"
		    ;;
		no)
		    STATDIR=
		    CONFIG_STATIC=
		    ;;
	    esac

	    # Map the build style into configuration information
	    case "$b" in
		debug)
		    CFLAGS="-O0 -g"
		    CONFIG_BUILD="--enable-debug --enable-xdebugging"
		    ;;
		default)
		    CFLAGS=""
		    CONFIG_BUILD=""
		    ;;
		optimized)
		    CFLAGS="-O4"
		    CONFIG_BUILD="--disable-debug"
		    ;;
		stats)
		    CFLAGS=
		    CONFIG_BUILD="--with-stats"
		    ;;
		xprof)
		    CFLAGS=
		    CONFIG_BUILD="--enable-xprofiling --with-profiling"
		    ;;
	    esac

	    export CFLAGS

	    if test $doBuild -eq 1; then
		OBJ_DIR="${BASE_OBJ_DIR}/${PREFIX}-${e}-${b}${STATDIR}"
		INST_DIR="${BASE_INSTALL_DIR}/${PREFIX}-${e}-${b}${STATDIR}"
		ok=1
		
		TRACEF="${BASE_OBJ_DIR}/Trace-${PREFIX}-$e-$b${STATDIR}.txt"
		rm -f "$TRACEF"
		touch "$TRACEF"
		
		date
		status "${PREFIX}-${e}-${b}${STATDIR}:"
		
		# Clean out whatever cruft may exist
		status "  Cleaning ..."
		doCmd '' rm -rf "${OBJ_DIR}"
		doCmd '' rm -rf "${INST_DIR}"
		
		# Create the object and install directories
		doCmd '' mkdir "${OBJ_DIR}"
		doCmd '' mkdir "${INST_DIR}"
		
		# In the object dir, do the required steps
		(
		    cd "${OBJ_DIR}" || ok=0
		
		    doCmd "  Configuring ..." \
			"${SRC}/configure" \
			--prefix="$INST_DIR" \
			$CONFIG_COMMON_OPTIONS \
			--with-engine=${e} \
			$CONFIG_BUILD \
			$CONFIG_STATIC
		    
		    # Build Kaffe, need VM to run kjc..
		    doCmd "  Building all ..." \
			${GMAKE} ${MAKE_OPTS} -C ${OBJ_DIR}/ all
		    
		    doCmd "  Building Klasses ..." \
			${GMAKE} ${MAKE_OPTS} -C ${OBJ_DIR}/ compile-classes
		    
		    doCmd "  Building bootstrap ..." \
			${GMAKE} ${MAKE_OPTS} -C ${OBJ_DIR}/libraries/javalib bootstrap
		    
		    # Re-build Kaffe, in case bootstrap changed...
		    doCmd "  Building all ..." \
			${GMAKE} ${MAKE_OPTS} -C ${OBJ_DIR}/ all
		    
		    doCmd "  Installing ..." \
			${GMAKE} ${MAKE_OPTS} -C ${OBJ_DIR}/ install
		    
		    doCmd "  Checking ..." \
			${GMAKE} ${MAKE_OPTS} -C ${OBJ_DIR}/ check
		    
		    # Show any test failures, if any:
		    grep "^FAIL:" "$TRACEF" | sed -e 's/^/    /'
		    
		    status " "
		);
	    fi
        done
    done
done

#eof
