#! /usr/bin/perl -w
#
# The Kaffe Unicode Database generator.
#
# Creates files unicode.idx and unicode.tbl from the Unicode Character
# Database file UnicodeData.txt.
#
# Latest version of this file should be download from
#   ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt
# previous version should match this:
#   ftp://ftp.unicode.org/Public/*-Update/UnicodeData-*.txt
#
# Format of files unicode.tbl and unicode.idx are explain in file
# FAQ/FAQ.unicode.
#
# I have test this script with UnicodeData-2.1.8.txt and
# UnicodeData-3.0.0.txt.
#
# Edouard G. Parmelan <egp@free.fr>
# March 27, 1999
# Last edit May 2, 2000

use English;
use strict;


# map Unicode general category to Java type
my %java_category = (
# Normative
    'Mn' => 6,	# Mark, Non-Spacing
    'Mc' => 8,	# Mark, Spacing Combining
    'Me' => 7,	# Mark, Enclosing

    'Nd' => 9,	# Number, Decimal Digit
    'Nl' => 10,	# Number, Letter
    'No' => 11,	# Number, Other

    'Zs' => 12,	# Separator, Space
    'Zl' => 13,	# Separator, Line
    'Zp' => 14,	# Separator, Paragraph

    'Cc' => 15,	# Other, Control
    'Cf' => 16,	# Other, Format
    'Cs' => 19,	# Other, Surrogate
    'Co' => 18,	# Other, Private Use
    'Cn' => 0,	# Other, Not Assigned

# Informative
    'Lu' => 1,	# Letter, Uppercase
    'Ll' => 2,	# Letter, Lowercase
    'Lt' => 3,	# Letter, Titlecase
    'Lm' => 4,	# Letter, Modifier
    'Lo' => 5,	# Letter, Other

    'Pc' => 23,	# Punctuation, Connector
    'Pd' => 20,	# Punctuation, Dash
    'Ps' => 21,	# Punctuation, Open
    'Pe' => 22,	# Punctuation, Close
    'Po' => 24,	# Punctuation, Other
    # FIXME: Unicode 2.1.8 mapped to Unicode 2.1
    'Pi' => 21,	# Punctuation, Initial quote (may behave like Ps or Pe depending on usage)
    'Pf' => 22,	# Punctuation, Final quote (may behave like Ps or Pe depending on usage)

    'Sm' => 25,	# Symbol, Math
    'Sc' => 26,	# Symbol, Currency
    'Sk' => 27,	# Symbol, Modifier
    'So' => 28,	# Symbol, Other

);

my $nr_small = 0;
my $nr_big = 0;
my $nr_block = 0;
my @nr_method = (0, 0, 0, 0);

my $last_unicode = -2;
my $unicode = -1;
my $category = 0;
my $numerical = 0;
my $uppercase = 0;
my $lowercase = 0;
my $titlecase = 0;
my $field = 0;
my $nr_fields = 0;
my $generical = 0;


my ($last_last_unicode, $last_category, $last_numerical, $last_field, $last_nr_fields, $last_generical);

my $nr_hole = 0;
my $method = -2;
my $start_unicode = 0;
my $start_offset = 0;
my $offset = 0;				# offset in properties table
my $last_offset;
my $mandatory = 0;			# mandatory block

local *TRACE;
local *INDEX;
local *TABLE;

sub trace {
    my ($unicode, $category, $name, $numerical, $uppercase, $lowercase, $titlecase) = @_;
    if (defined $ARGV[1]) {
	$name = "" unless ($name =~ /First>$/ ||
			   $name =~ /Last>$/ ||
			   $name =~ /noBreak/);
	$numerical = "-2"
	    if ($numerical ne "" && !($numerical =~ /^\d+$/));
	print TRACE "$unicode;$category;$name;$numerical;$uppercase;$lowercase;$titlecase\n";
    }
}

sub write_char {
    $last_offset = $offset;
    if ($method == 3) {
	# Exception table
	print TABLE (pack("C", $category));
	$numerical = 0xFFFF if ($numerical eq "");
	print TABLE (pack("n", $numerical));
	print TABLE (pack("n", $uppercase));
	print TABLE (pack("n", $lowercase));
	print TABLE (pack("n", $titlecase));
	$offset += 9;
	$nr_big++;
    }
    else {
        # Character Properties table
	print TABLE (pack("C", ($field << 5) + $category));
	print TABLE (pack("n", $generical));
	$offset += 3;
	$nr_small++;
    }
}

sub end_block {
    $method = 0 && printf("sanity: negative method in &end_block U+%04X\n", $unicode)
	if ($method < 0);
    print INDEX (pack("n", $start_unicode));
    print INDEX (pack("n", $last_unicode));
    print INDEX (pack("C", ($method << 4) + ($start_offset >> 16)));
    print INDEX (pack("n", $start_offset & 0xFFFF));
    $nr_method[$method]++;
    $nr_block++;
    $method = -2;
}

sub emit_method {
    my ($meth) = @_;

    if ($meth != $method) {
	if ($method == 0 && ($meth == 1 || $meth == 2)) {
	    # switch from uncompressed to compressed, try to peek
	    # the last entry of the previous uncompressed block
	    if ($start_unicode <= $last_last_unicode) {
		# do not empty it
		my $saved = $last_unicode;
		$last_unicode = $last_last_unicode;
		&end_block;
		$last_unicode = $saved;
		$start_unicode = $last_unicode;
		$start_offset = $last_offset;
	    }
	    # else, do not emit empty block
	    $method = $meth;
	}
	else {
	    if ($method != -2) {
		&end_block;
	    }
	    $start_unicode = $unicode;
	    $start_offset = $offset;
	    $method = $meth;
	    &write_char;
	}
    }
    elsif ($meth == 0 || $meth == 3) {
	&write_char;
    }
}


$OUTPUT_AUTOFLUSH = 1;

open (UNICODE, $ARGV[0]) ||
    die "Can't open Unicode Characters Database: $!\n";

open (TRACE, ">$ARGV[1]")
    if (defined $ARGV[1]);

open (INDEX, ">unicode.idx") ||
    die "can't create unicode index: $!\n";
open (TABLE, ">unicode.tbl") ||
    die "Cant' create unicode table: $!\n";

LOOP:
while (<UNICODE>) {
    chomp;
    my ($name, $class, $bidir_category, $decomposition, $decimal, $digit,
	    $mirrored, $old_name, $comment, $eol);

    ($last_last_unicode, $last_unicode, $last_category, $last_numerical, $last_field, $last_nr_fields, $last_generical) =
	($last_unicode, $unicode, $category, $numerical, $field, $nr_fields, $generical);
    
    ($unicode, $name, $category, $class, $bidir_category, $decomposition,
     $decimal, $digit, $numerical, $mirrored, $old_name,
     $comment, $uppercase, $lowercase, $titlecase) = split(/;/);

    &trace ($unicode, $category, $name, $numerical, $uppercase, $lowercase, $titlecase);

    if ($category eq 'Zs' &&
        $decomposition =~ /noBreak/) {
	$category = 31;
    }
    else {
	$category = $java_category{$category} ||
	    die "database error: unknown Unicode category $category\n";
    }

    $unicode = hex($unicode);
    $uppercase = hex($uppercase);
    $lowercase = hex($lowercase);
    $titlecase = hex($titlecase);

    # convert numerical unicode to java one's
    if ($numerical ne "") {
	if ($numerical =~ /^\d+$/) {	# getNumericalValue() returns $numerical.
	    if ($numerical >= 0xFFFE) {
		die "database error: numerical out of range: $numerical\n";
	    }
	}
	else {
	    $numerical = 0xFFFE		# getNumericalValue() returns -2.
	}
    }
    # else getNumericalValue() return -1 aka no numerical



    $nr_fields = 0;
    $field = 0;
    $generical = 0;

    if ($uppercase != 0) {
	$field = 1;
	$generical = $uppercase;
	$nr_fields++;
    }
    if ($lowercase != 0) {
	$field = 2;
	$generical = $lowercase;
	$nr_fields++;
    }
    if ($numerical ne "") {
	$field = 3;
	$generical = $numerical;
	$nr_fields++;
    }
    if ($uppercase != $titlecase) {
	$field = 4;
	$generical = $titlecase;
	$nr_fields++;
    }

    if ($field == 4 && $nr_fields == 1) {
	$nr_fields = 2;			# force exception entry
	printf("%04X only titlecase!\n", $unicode);
    }


    die "sanity: digit value != numeric value\n"
	if (($category == 9) &&	# 'Nd' => 9,	# Number, Decimal Digit
	    ($digit != $numerical));

    if ($unicode == 0) {
	&emit_method (0);
	next LOOP;
    }

    # handle mandatory ranges
    if ($name =~ /First>$/) {
	if ($method != -2) {
	    &end_block;
	}
	&emit_method (1);
	$mandatory = 1;
	next LOOP;
    }
    # end mandatory
    if ($mandatory == 1) {
	$mandatory = 0;
	next LOOP;
    }
    die "sanity: unexpected end of mandatory block\n"
	if ($name =~ /Last>$/);

    if ($unicode != ($last_unicode+1)) {
	$nr_hole++;
	if ($method != -2) {
	    &end_block;
	}
    }

    # check for compression
    if (($unicode == ($last_unicode+1)) &&
	($category == $last_category) &&
	($field == $last_field) &&
	($nr_fields == $last_nr_fields) &&
	($nr_fields < 2)) {
	# check compression method
	if ($generical == $last_generical) {
	    &emit_method (1);
	}
	elsif ($generical == ($last_generical+1)) {
	    &emit_method (2);
	}
	else {
	    &emit_method (0);
	}
    }
    elsif ($nr_fields <= 1) {
	&emit_method (0);
    }
    else {
	&emit_method (3);
    }

}
if ($method != -2) {
    # finish the last opened block
    $last_unicode = $unicode;
    &end_block;
}

print "$nr_block blocks, $nr_small small, $nr_big big\n";
print "$nr_method[0] blocks not compressed\n";
print "$nr_method[1] blocks compressed same value\n";
print "$nr_method[2] blocks compressed one increment\n";
print "$nr_method[3] blocks extended\n";
print "$nr_hole holes\n";
# vim:set cindent sw=4:
