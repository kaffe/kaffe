#!/bin/sh
#
# Copyright (c) 1999 University of Utah CSL.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# Written by Godmar Back <gback@cs.utah.edu>
#

if [ -x /usr/bin/perl ]; then
	PERL=/usr/bin/perl
else
	if [ -x /usr/local/bin/perl ]; then
		PERL=/usr/local/bin/perl
	fi
fi

PERL5INC=`echo "$0" | sed 's,/[^/]*$,,'`  # very portable
$PERL -I $PERL5INC -x $0 ${1+"$@"}
exit

#!perl -w
#line 27

#
# Take one of libtool's exp file and the file created by mangleNative
# and create a linker script that contains only those aliases defined
# by a given library.
#
# The intended use is this
#
#	${SRCDIR}/developers/createLdScript.pl \
#       $SRCDIR/developers/allNativeAliases \
#	${BUILDDIR}/libraries/clib/native/.libs/libnative.exp \
#	    > ${SRCDIR}/libraries/clib/native/gcj-native-alias.ld
#
# where allNativeAliases is the output of mangleNative.pl
#

## Parse the command line
my $allalias = shift 
	|| die "Usage: createLdScript.pl aliasfile libexpfile\n";
my $libexpfile = shift
	|| die "Usage: createLdScript.pl aliasfile libexpfile\n";
my %alias = ();

# 1. read allalias file
# 
open(ALIAS, $allalias) || die "could not open $allalias";

while (<ALIAS>) {
    m/([^ ]*) = ([^ ]*);/g;
    $alias{$2} = $1;

    # print "insert $2 -> $alias{$2}\n";
}

open(LIBEXP, $libexpfile) || die "could not open $libexpfile";

while (<LIBEXP>) {
    chop;
    my $kname = $_;
    my $gcjname;

    if (defined($alias{$kname})) {
	$gcjname = $alias{$kname};
	print "${gcjname} = ${kname};\n"
    }
}
