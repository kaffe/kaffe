#!/usr/bin/perl -w

package JikesWarning;

use strict;
use warnings;

use base qw( LogWarning );
use vars qw( $jikes_prefix );
$jikes_prefix = qr/([^:\n]+):(\d+):\d+:\d+:\d+: (?:Lexical|Semantic) (?:Caution|Warning): /m;

sub new {
    my $class = shift;
	my ( $name, $regex, $description ) = @_;
    return $class->SUPER::new( 'jikes', $name, qr/${jikes_prefix}$regex/, $description );
}

1;
