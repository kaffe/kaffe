#!/usr/bin/perl -w
package LogWarning;

use strict;
use warnings;

use WarningDescription;

sub new {
	my $class = shift;
	my %data = (
		'compiler'	=> $_[ 0 ],
		'simple-name'	=> $_[ 1 ],
		'regex'		=> $_[ 2 ],
		'ignore'	=> $_[ 4 ],
	);

	my $self = bless( \%data, $class );
	$data{ 'description' } = ref( $_[ 3 ] ) ? $_[ 3 ] : new WarningDescription( $self, $_[ 3 ] );
	return $self;
}

sub ignore {
	my $self = shift;
	if ( $_[ 0 ] ) {
		$self->{ 'ignore' } = $_[ 0 ];
	} else {
		return $self->{ 'ignore' };
	}
}

sub compiler {
	my $self = shift;
	if ( $_[ 0 ] ) {
		$self->{ 'compiler' } = $_[ 0 ];
	} else {
		return $self->{ 'compiler' };
	}
}

sub name {
	my $self = shift;
	return $self->shortName() . $self->index();
}

sub shortName {
	my $self = shift;
	my $compiler = $self->compiler();
	return ( $compiler ? $compiler . "/" : "" ) . $self->{ 'simple-name' };
}

sub index {
	my $self = shift;
	if ( $_[ 0 ] ) {
		$self->{ 'index' } = $_[ 0 ];
	} else {
		my $index = $self->{ 'index' };
		return $index ? ":$index" : "";
	}
}

sub regex {
	my $self = shift;
	return $self->{ 'regex' };
}

sub description_text {
	my $self = shift;
	return $self->{ 'description' }->description();
}

sub get_description {
	my $self = shift;
	my $description = $self->description_text();
	if ( !$description ) {
		$description = Registry::lookup_description( $self->compiler(), $self->shortName() );
		$description = $description->description() if ( $description );
	}
	return $description;
}
1;
