#!/bin/sh
#
# Copyright (c) 1998 The University of Utah. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.
#
# Contributed by the Flux Research Group at the University of Utah.
# Authors: Leigh Stoller
#

# use this script to configure kaffe for the oskit in Unix mode. You
# can copy this script and edit the paths below, or you can use the
# command line options.
#
#
# Notes:
#  1) The oskit must have been built with the --enable-unixexamples
#     option, which allows oskit applications to be run as unix processes.
#  2) The Oskit was built with a.out tools, so you must arrange to build
#     Kaffe with a.out tools. Do "setenv OBJFORMAT aout"
#

# Actual src directory.
SRCDIR=/home/stoller/flux/kaffe

# Your Kaffe object dir
OBJDIR=/y/stoller/kaffe-oskit-unix/obj

# Your Kaffe prefix (install) directory
PREFIX=/y/stoller/kaffe-oskit-unix/install

# where your oskit was installed
OSKITDIR=/y/stoller/oskit/install

# Parse the command-line options to override above.
until [ $# -eq 0 ]
do
	case "$1" in
		--srcdir=* )
			SRCDIR=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		--objdir=* )
			OBJDIR=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			 shift;;
		--prefix=* )
			PREFIX=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		--oskitdir=* )
			OSKITDIR=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		* ) echo "Bad args";
			  exit 1 ;;
	esac
done

echo "Kaffe on Oskit/Unix Configuration"
echo "srcdir = $SRCDIR"
echo "objdir = $OBJDIR"
echo "prefix = $PREFIX"
echo "oskit  = $OSKITDIR"

cd $OBJDIR

PATH=$OSKITDIR/bin:$PATH \
CC="$OSKITDIR/bin/i386-oskit-gcc -posix-oskit -pthread" \
$SRCDIR/configure \
	--prefix=$PREFIX \
	--with-threads=oskit-pthreads \
	--with-engine=intrp \
	--with-staticlib \
	--without-x \
	--host=i386-oskit
