#! /bin/sh

# This script runs all of the various auto* programs in the correct order.
# Written by Mo DeJong.

aclocal -I .
autoheader -l config
automake --add-missing --verbose
autoconf

