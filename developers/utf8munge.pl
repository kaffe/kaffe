#!/usr/bin/perl -w
#
# Munge the Utf8 constants in a .class file.
#
# Usage: utf8munge.pl [option] <old.class> <new.class> <old str> <new str> [<oldstr> <newstr> ...]
#
# Options:
#	-changeClassName: change the name of the actual class object.
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

use JavaClass;

$changeClassName = 0;

## Parse the required command line
my $classFile = shift || &usage();
if ($classFile eq '-changeClassName') {
  $changeClassName = 1;
  $classFile = shift || &usage();
}
my $newClass  = shift || &usage();
my @oldStr;
my @newStr;

$oldStr[0] = shift || &usage();
$newStr[0] = shift || &usage();

if (!(defined($newClass) && defined($oldStr[0]) && defined($newStr[0]))) {
    print STDOUT ("Usage: utf8munge.pl [option] <old.class> <new.class> <old str> <new str> [<oldstr> <newstr> ...]\n");
    exit 11;
}

## Now parse any additional str mucking parameters
my $i = 1;
while ($#ARGV >= 0) {
  $oldStr[$i] = shift;
  #die "$oldStr[$i] must be followed by a replacement string" if (!$#ARGV);
  $newStr[$i] = shift;
  $i++;
}

## for printing:
#$JavaClass::detailedFields = 0;
#$JavaClass::detailedMethods = 0;

my $class = &JavaClass::readClass($classFile);


if (!$changeClassName) {
  ## We don't want to change the "name" of this class, so to be safe, we
  ## just create an additional Class entry and an additional Utf8 entry
  ## which contain the old info This is only a problem if any of the
  ## $oldStr match 'thisClass->name'.
  $thisClassEntry = $class->{constantPool}[$class->{thisClass}];
  $ni = $thisClassEntry->{nameIndex};
  my $name = $class->{constantPool}[$ni]->{val};
  
  if (grep($name =~ m/$_/, @oldStr)) {
    # Save copies of the thisClass entry and the Utf8String it
    # points to.  These copies won't get munged in the frenzy below.
    
    %classEntry = %{$thisClassEntry};
    %origUtf8   = %{$class->{constantPool}[$ni]};
  } 
}


### transform class
foreach $i (1..(($class->{constantPoolCt})-1)) {
    $cpEntry = $class->{constantPool}[$i];

    ### Change the Utf8Strings
    if ($cpEntry->{tag} eq $JavaClass::CONSTANT_Utf8) {
        $val = $cpEntry->{val};

	## Apply each transform (in order!)
	foreach $s (0..$#oldStr) {
	  $val =~ s,$oldStr[$s],$newStr[$s],g;
        }

        # print "OLD: $cpEntry->{val}; NEW: $val\n";

        $cpEntry->{val} = $val;
        $cpEntry->{len} = length($val);
    }
}

# Insert the saved entries, if they were defined.
if ((!$changeClassName) && defined(%classEntry)) {
    $ct = $class->{constantPoolCt};

    # Insert the saved Utf8 entry at the end of the constant pool
    $class->{constantPool}[$ct] = \%origUtf8;
    
    # Patch up the saved classEntry to point to this utf8
    $classEntry{nameIndex} = $ct;

    $ct++;

    # Insert the classEntry in the constant pool
    $class->{constantPool}[$ct] = \%classEntry;

    # Patch the thisClass pointer
    $class->{thisClass} = $ct;

    $ct++;

    # Patch the constantPoolCt
    $class->{constantPoolCt} = $ct;
}

###
#&JavaClass::printClass($class);
&JavaClass::writeClass($class, "$newClass");

####
####
####

sub usage() {
  print STDOUT "Usage:\n";
  print STDOUT "    utf8munge.pl [option] <old.class> <new.class> <old str> <new str> [<oldstr> <newstr> ...]\n\n";
  print STDOUT "    The only [option] is -changeClassName.\n";
  print STDOUT "    By default the name will not be changed.\n";

  exit 11;
}

# eof
