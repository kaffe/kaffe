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
# This script must be run like this:
#
#    echo ...../Object.class | mangleNative.pl
# 	(or find ... -name '*.class' -print | mangleNative.pl)
# It will output stuff that's suitable for inclusion as a linker
# script:
#
#    clone__Q34java4lang6Object = java_lang_Object_clone;
#    getClass__Q34java4lang6Object = java_lang_Object_getClass;
#    hashCode__Q34java4lang6Object = java_lang_Object_hashCode;
#    notify__Q34java4lang6Object = java_lang_Object_notify;
#    notifyAll__Q34java4lang6Object = java_lang_Object_notifyAll;
#    wait0__Q34java4lang6Objectx = java_lang_Object_wait0;
#
# To the left there's a name mangled according to gcj conventions,
# to the right is Kaffe's KNI convention.  Having these aliases will
# partially eliminate the need for gcj trampolines and speed things up
# some.
#

use JavaClass;

## a bunch of subroutines to do gcj name mangling
##

sub number {
    my $n = shift;
    if ($n < 10) {
    	print $n;
    } else {
    	print "_${n}_";
    }
}

sub number2 {
    my $n = shift;
    print $n;
    print "_" if ($n > 9);
}

sub GCJMangleType {
    my @cname = split /\//, shift;

    if ($#cname > 0) {
	print "Q";
	&number($#cname+1);
    }

    foreach $n (@cname) {
	print length($n) . $n;
    }
}

sub GCJMangleSignature {
    local $repeating = 0;
    local $repitem = -1;

    sub finishrep {
	if ($repeating > 0 && $repitem != -1) {
	    if ($repeating == 1) {
		print "T";
	    } else {
		print "N";
		number2($repeating);
	    }
	    number2($repitem + 1);
	    $repeating = 0;
	    $repitem = -1;
	}
    }

    my $av;
    my @ac;
    
    # get EXP out of "(EXP)..." into $1

    my $sig = shift;
    $sig =~ '\(([^\)]*)\).*';
    $sig = $1;

    $ac = 0;
    while (length($sig) > 0) {
	local $len = 1;
	local $adepth = 0;

	sub getnext {
	    my $sig = shift;
	    my $c = substr($sig, 0, 1);
	    my $r = "";

	    $len = 0;
	    $adepth = 0;
	    while ($c eq "[") { 
		$adepth++;
		$len++;
		$r .= "[";
		$sig = substr($sig, 1, length($sig) - 1);
		$c = substr($sig, 0, 1);
	    }

	    if ($c eq "L") {
		my $ll = index($sig, ";");
		$len += $ll + 1;
		$r .= substr($sig, 0, $ll + 1);
	    } else {
		$len += 1;
		$r .= $c;
	    }
	    return ($r);
	}

	$next = &getnext($sig);
	$sig = substr($sig, $len, length($sig) - $len);
	$av[$ac++] = $next;

	# is this a repeat?
	$repeat = -1;
	for ($j = 0; $j < $ac - 1; $j++) {
	    if ($av[$j] eq $next) {
		$repeat = $j;
		last;
	    }
	}

	my $c = substr($next, 0, 1);
	while ($c eq '[') {
	    $next = substr($next, 1, length($next) - 1);
	    $c = substr($next, 0, 1);
	}

	if (($c eq 'L' || $adepth > 0) && $repeat != -1) {
	    if ($repeat != $repitem) {
		&finishrep;
	    }
	    $repitem = $repeat;
	    $repeating++;
	    next;
	}
	&finishrep;

	if ($adepth > 0) {
	    print "Pt6JArray1Z" x $adepth; 	# is the x $adepth correct ???
	}

	if ($c eq "Z") { print 'b'; }
	if ($c eq "C") { print 'w'; }
	if ($c eq "V") { print 'v'; }
	if ($c eq "B") { print 'c'; }
	if ($c eq "S") { print 's'; }
	if ($c eq "I") { print 'i'; }
	if ($c eq "J") { print 'x'; }
	if ($c eq "F") { print 'f'; }
	if ($c eq "D") { print 'd'; }

	if ($c eq "L") {
	    print "P";
	    &GCJMangleType(substr($next, 1, length($next) - 2));
	}
    }
    &finishrep;
}

sub KaffeName {
    my $cname = shift;
    my $mname = shift;
    $cname =~ s/\//_/g;
    return ($cname . "_" . $mname);
}

sub doClass {
    my %ch = %{$_[0]};

    my $thisClName = %{$ch{constantPool}[$ch{thisClass}]}->{nameIndex};
    $thisClName = %{$ch{constantPool}[$thisClName]}->{val};

    my $i = 0;
    while ($i < $ch{methodCt}) {
	my %method = %{$ch{methods}[$i]};
	my $accflags = $method{accessFlags};

	if ($accflags & $JavaClass::ACC_NATIVE) {
	    my $name = $ch{constantPool}[$method{nameIndex}]->{val};
	    my $desc = $ch{constantPool}[$method{descriptorIndex}]->{val};

	    # print ("\t$thisClName $name $desc\n");
	    $kname = &KaffeName($thisClName, $name);

	    if ($name ne "<init>") {
		print $name;
	    }
	    print "__";
	    &GCJMangleType($thisClName);
	    &GCJMangleSignature($desc);

	    print " = ";
	    print $kname;
	    print ";\n";
	}
    } continue {
	$i++;
    }
}

while (<>) {
    my $class = &JavaClass::readClass($_);
    &doClass($class);
}

#eof
