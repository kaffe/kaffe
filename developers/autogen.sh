#!/bin/sh

# This script runs all of the various auto* programs in the correct order.
# Written by Mo DeJong.

aclocal -I .
autoheader
automake --add-missing --verbose
autoconf

