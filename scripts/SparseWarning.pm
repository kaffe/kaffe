#!/usr/bin/perl -w

package SparseWarning;

use strict;
use warnings;

use LogWarning;
use Registry;

use vars qw( @ISA $prefix_regex $prefix_regex_noparam $prefix_regex_nowarn );

BEGIN {
	@ISA = qw( LogWarning );
	$prefix_regex = qr/^([^:\n]+):(\d+):(?:\d+:)? warning: /m;
	$prefix_regex_nowarn = qr/^[^:\n]+:\d+:(?:\d+:)? /m;
	$prefix_regex_noparam = qr/^[^:\n]+:\d+: warning: /m;
}

sub new {
	my $class = shift;
	my ( $name, $regex, $description, $ignore ) = @_;
	my $warning = $class->SUPER::new( 'sparse-cc', $name, qr/${prefix_regex}$regex/, $description, $ignore );
	Registry::add_warning($warning);
	return $warning;
}

sub register_description {
	Registry::register_description( 'sparse-cc', @_ );
}

sub register_warning {
	SparseWarning->new( @_ );
}

BEGIN {
	my $mod_type = qr/\((different type sizes|different modifiers|different base types)\)/;
	register_warning( 'undefined-identifier', qr/undefined identifier '([^']+)'$/m ),
	register_warning( 'too-many-warnings', qr/too many warnings$/m ),
	register_warning( 'not-an-lvalue', qr/not an lvalue$/m ),
	register_warning( 'address-of-non-lvalue', qr/generating address of non-lvalue \(([^)]+)\)$/m ),
	register_warning( 'no-type-call', qr/call with no type!$/m ),
	register_warning( 'NULL-constant', qr/Using plain integer as NULL pointer$/m ),
	register_warning( 'Undefined Preprocessor', qr/undefined preprocessor identifier '([^']+)'$/m ),
	register_warning( 'non-ANSI declaration', qr/non-ANSI function declaration$/m ),
	register_warning( 'incorrect type', qr/incorrect type in argument (\d+) \((different base types)\)\n${prefix_regex_nowarn}   expected ([^\n]+)\n${prefix_regex_nowarn}   got ([^\n]+)$/m ),
	register_warning( 'incorrect type', qr/incorrect type in (assignment|return expression|initializer) $mod_type\n${prefix_regex_nowarn}   expected ([^\n]+)\n${prefix_regex_nowarn}   got ([^\n]+)$/m ),
	register_warning( 'incorrect type', qr/incorrect type in (assignment|return expression|initializer) \(incompatible argument (\d+) $mod_type\)\n${prefix_regex_nowarn}   expected ([^\n]+)\n${prefix_regex_nowarn}   got ([^\n]+)$/m ),
	register_warning( 'incompatible types', qr/incompatible types in conditional expression (\(different types\))$/m ),
}
1;
