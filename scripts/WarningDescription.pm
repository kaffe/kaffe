#!/usr/bin/perl -w

package WarningDescription;

use strict;
use warnings;

sub new {
	my $class = shift;
	my %data;
	if ( ref( $_[ 0 ] ) ) {
		$data{ 'parent' } = shift;
	} else {
		$data{ 'compiler' } = shift;
		$data{ 'simple-name' } = shift;
	}
	$data{ 'description' } = shift;
	return bless( \%data, $class );
}

sub compiler {
	my $self = shift;
	if ( $_[ 0 ] ) {
		$self->{ 'compiler' } = $_[ 0 ];
	} else {
		return $self->{ 'parent' } ? $self->{ 'parent' }->compiler() : $self->{ 'compiler' };
	}
}

sub name {
	my $self = shift;
	my $compiler = $self->compiler();
	return ( $compiler ? $compiler . "/" : "" ) . $self->{ 'simple-name' };
}

sub description {
	my $self = shift;
	return $self->{ 'description' };
}

1;
