#! /bin/sh

# This script runs all of the various auto* programs in the correct order.
# You should run this from the top-level directory.
# Written by Mo DeJong.

## update class list breaks on Mandrake 9.0
## generating a ton of double entries
##
## ( cd libraries/javalib && ../../developers/update-class-list )
aclocal -I .
autoheader -l config
automake --add-missing --verbose
autoconf

