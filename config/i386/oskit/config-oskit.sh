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

# use this script to configure kaffe for the oskit. You can copy this
# script and edit the paths below, or you can use the command line
# options.
#
# There are some default configuration options passed to Kaffe's
# configuration script. See the bottom of this file where configure
# is invoked
#
# Notes:
#   1) Real oskit kernels are built using ELF tools, so unset OBJFORMAT.
#   2) You must have the oskit build tools in your path when you build kaffe.
#

# Actual src directory.
SRCDIR=/home/stoller/flux/kaffe

# Your kaffe object dir
OBJDIR=/y/stoller/kaffe-oskit/obj

# Your kaffe prefix (install) directory
PREFIX=/y/stoller/kaffe-oskit/install

# Where your oskit was installed
OSKITDIR=/y/stoller/oskit-real/install

# Path to the oskit build tools.
TOOLDIR=/usr/flux/bin

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
		--tooldir=* )
			TOOLDIR=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		* ) echo "Bad args";
			  exit 1 ;;
	esac
done

echo "Kaffe on Oskit Configuration"
echo "srcdir = $SRCDIR"
echo "objdir = $OBJDIR"
echo "prefix = $PREFIX"
echo "oskit  = $OSKITDIR"
echo "tools  = $TOOLDIR"

cd $OBJDIR

PATH=$OSKITDIR/bin:$TOOLDIR:$PATH \
CC="$OSKITDIR/bin/i486-oskit-gcc -posix-oskit -pthread" \
$SRCDIR/configure \
	--prefix=$PREFIX \
	--with-threads=oskit-pthreads \
	--with-gcblock=mmap \
	--with-staticlib \
	--without-x \
	--host=i386-oskit
