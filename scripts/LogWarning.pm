#!/usr/bin/perl -w
package LogWarning;

use strict;
use warnings;

sub new {
	my $class = shift;
	return bless( {
		'compiler'	    => $_[ 0 ],
		'simple-name'	=> $_[ 1 ],
		'regex'		    => $_[ 2 ],
		'description'	=> $_[ 3 ]
	}, $class );
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
    my $compiler = $self->compiler();
	return ( $compiler ? $compiler . "/" : "" ) . $self->{ 'simple-name' } . $self->index();
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

sub description {
    my $self = shift;
	return $self->{ 'description' };
}

1;
