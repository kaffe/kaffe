#!/usr/bin/perl -w
package GCCWarning;

use strict;
use warnings;

use base qw( LogWarning );
use vars qw( $prefix_regex $prefix_regex_noparam $prefix_regex2 );

$prefix_regex = qr/^([^:\n]+):(\d+):(?:\d+:)? warning: /m;
$prefix_regex_noparam = qr/^(?:[^:\n]+):(?:\d+): warning: /m;

sub new {
    my $class = shift;
	my ( $name, $regex, $description ) = @_;
    return $class->SUPER::new( 'gcc', $name, qr/${prefix_regex}$regex/, $description );
}

1;
