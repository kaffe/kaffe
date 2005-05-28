#!/bin/sh
#
# Dump a java .class file to stdout
#
#
# Copyright (c) 1999 University of Utah CSL.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# Written by Patrick Tullmann <tullmann@cs.utah.edu>
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

#!perl

use JavaClass;

# Control the verbosity of &printClass()
$JavaClass::detailedFields = 0;
$JavaClass::detailedMethods = 0;

## Parse the command line
my $classFile = "";

foreach (@ARGV){
  if (/--verbose-fields/) {
    $JavaClass::detailedFields = 1;
  }
  elsif (/--verbose-methods/) {
    $JavaClass::detailedMethods = 1;
  }
  else {
    $classFile = $_;
  }
}

print "$classFile\n";

#$classFile!="" || &usage();

## Read/parse the class file
my $class = &JavaClass::readClass($classFile);

## Print the class filea
&JavaClass::printClass($class);

###
###
###

sub usage() {
  print STDOUT "Usage:\n";
  print STDOUT "    dumpClass.pl [--verbose-methods] [--verbose-fields] <ClassFile>\n\n";
  print STDOUT "    Note that '.class' will be appended if it is not specified in the file name.\n";

  exit 11;
}

#eof
