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

# Build a multiboot image from Kaffe plus other stuff. You can either
# copy this script to your object directory and edit the paths below,
# or you can pass it all in as command line args.
#
# The most important arguments are the -dir args, which specify which
# directories to place into the multiboot image. You can specify as
# many -dir args as you like.
#
# Min usage would be:
#	mkimage-test -dir /tmp/foo -dir /tmp/fee
# which will add the contents of the those two directories to the image.
# By default, you alway get the contents of the Kaffe install/share/kaffe
# directory, which includes the standard kaffe classes.
#
# The other important consideration is the default CLASSPATH. Since there
# is no "easy" way to pass a long environment string to an oskit kernel,
# the oskit/kaffe startup code will read a file from /etc that contains the
# default classpath. This file is called /etc/kaffe_classpath, and is the
# usual colon separated list of names. This script will add that file to
# the boot image. Note that the target file resides in /etc, but the file
# that the classpath is taken from can reside anywhere. It defaults to the
# value of CLASSPATHFILE below, but can be specified as a command line
# option to this script using the --classpathfile= option. With the example
# above, you would want to add /tmp/foo and /tmp/fee to the classpath
# list in your classpath file. 
#
# When you start the oskit, you can change the name of file to be loaded
# to any other file in the multiboot image by using the environment variable
# command line option. That is, you can add CLASSPATHFILE=some_file_name to
# the oskit the command line, and that becomes an environment variable that
# the oskit/kaffe startup code will look for before opening the default
# file /etc/kaffe_classpath.
#

# LIBTOOL NOTES:
# When using preloaded (static) libraries, libtool still needs to
# examine the .la files (but not the real archives) at runtime,
# KAFFELIBRARYPATH should contain the directories where all the native
# .la library descriptions can be found.
#
# KAFFELIBRARYPATH's default value in the OSKit is /lib.


# Your Kaffe object dir
OBJDIR=/y/stoller/kaffe-oskit/obj

# Your Kaffe prefix (install) directory
PREFIX=/y/stoller/kaffe-oskit/install

# where your oskit was installed
OSKITDIR=/y/stoller/oskit-real/install

# Default classpath file
CLASSPATHFILE=kaffe_classpath

# A list of directories
DIRS=

# Parse the command-line options to override above.
until [ $# -eq 0 ]
do
	case "$1" in
		--objdir=* )
			OBJDIR=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			 shift;;
		--prefix=* )
			PREFIX=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		--oskitdir=* )
			OSKITDIR=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		--classpathfile=* )
			CLASSPATHFILE=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;
			shift;;
		-dir )
			DIRS="$DIRS $2" ;
			shift;
			shift;;
		* ) echo "Bad args";
			  exit 1 ;;
	esac
done

# The Kaffe "kernel"
KAFFE=$OBJDIR/kaffe/kaffe/Kaffe

# The directory with the minimum necessary class files.
CLASSDIR="$PREFIX/share/kaffe"

# The final list of directories
DIRS="$CLASSDIR $DIRS"

# The place where the default classpath will be created.
CPF="$CLASSPATHFILE:/etc/kaffe_classpath"

for DIR in $DIRS
do
	for FILE in `find $DIR -type f -print`
	do
	    echo "$FILE:$FILE"
	done
done | $OSKITDIR/bin/mkmbimage -o $OBJDIR/Image -stdin $KAFFE $CPF

