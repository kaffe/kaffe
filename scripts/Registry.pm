#!/usr/bin/perl -w

package Registry;

use strict;
use warnings;

use Data::Dumper;
use vars qw( %description_registry @warnings %warning_map );

sub register_description {
	my ( $compiler, $shortName, $description );
	if ( ref( $_[ 0 ] ) ) {
		my $compiler = $_[ 0 ]->compiler();
		my $shortName = $_[ 0 ]->shortName();
	} else {
		$compiler = shift;
		$shortName = shift;
		$description = shift;
	}
	my $existing = $description_registry{ $compiler }{ $shortName };
	print STDERR "Duplicate description for: compiler[$compiler] name[$shortName]!\n" if ( $existing );
	$description_registry{ $compiler }{ $shortName } = $description;
}

sub lookup_description($$) {
	return $description_registry{ $_[ 0 ] }{ $_[ 0 ] };
}

sub add_warning {
	push( @warnings, @_ );
}

sub analyze_warnings {
	my %duplicated_warnings;
	for ( my $i = 0; $i < @warnings; $i++ ) {
		my $warning = $warnings[ $i ];
		my $warning_group = $duplicated_warnings{ $warning->compiler() } ||= {};
		my $warning_name = $warning->name();
		if ( !$warning_group->{ $warning_name } ) {
			$warning_group->{ $warning_name } = $warning;
		} else {
			if ( ref( $warning_group->{ $warning_name } ) ) {
				$warning_group->{ $warning_name }->index( 1 );
				$warning_group->{ $warning_name } = 2;
			}
			$warning->index( $warning_group->{ $warning_name }++ );
		}
	}

	for ( my $i = 0; $i < @warnings; $i++ ) {
		$warning_map{ $warnings[ $i ]->name() } = $warnings[ $i ];
	}
}

sub get_warning {
	return $warning_map{ $_[ 0 ] };
}

1;

