#!/usr/bin/perl -w

package JikesWarning;

use strict;
use warnings;

use LogWarning;
use Registry;
use vars qw( @ISA $jikes_prefix );

BEGIN {
	@ISA = qw( LogWarning );
	$jikes_prefix = qr/([^:\n]+):(\d+):\d+:\d+:\d+: (?:Lexical|Semantic) (?:Caution|Warning): /m;
}

sub new {
	my $class = shift;
	my ( $name, $regex, $description, $ignore ) = @_;
	my $warning = $class->SUPER::new( 'jikes', $name, qr/${jikes_prefix}$regex/, $description, $ignore );
	Registry::add_warning($warning);
	return $warning;
}

sub register_description {
	Registry::register_description( 'jikes', @_ );
}

sub register_warning {
	JikesWarning->new( @_ );
}

BEGIN {
	register_warning( 'throws-uncheck', qr/Since type "([^"]+)" is an unchecked exception, it does not need to be listed in the throws clause.$/m, '' ),
	register_warning( 'throws-unchecked', qr/Since type "([^"]+)" is an unchecked exception, it does not need to be listed in the throws clause.$/m ),
	register_warning( 'modifier-order', qr/The modifier "([^"]+)" did not appear in the recommended order public\/protected\/private, abstract, static, final, synchronized, transient, volatile, strictfp.$/m ),
	register_warning( 'public-in-interface', qr/The use of the "([^"]+)" modifier in this context is redundant and strongly discouraged as a matter of style.$/m ),
	register_warning( 'exception-superclass', qr/The listing of type "([^"]+)" in the throws clause is not necessary, since its superclass, "([^"]+)", is also listed.$/m ),
	register_warning( 'override-default', qr/Method "([^"]+)" in class "([^"]+)" does not override or hide the corresponding method with default access in class "([^"]+)".$/m ),
	register_warning( 'invalid-zip', qr/The file "([^"]+)" does not exist or else is not a valid zip file.$/m ),
	register_warning( 'invalid-zip', qr/I\/O warning: "No such file or directory" while trying to open (.*)\.$/m ),
	register_warning( 'negative-shift-count', qr/The shift count (-\d+) is negative; it will be masked to the appropriate width and behave as a positive shift count.$/m ),
	register_warning( 'large-shift-count', qr/The shift count of (\d+) is >= the (\d+-bit) width of the type.$/m ),
	register_warning( 'method-is-constructor', qr/The name of this method "([^"]+)" matches the name of the containing class. However, the method is not a constructor since its declarator is qualified with a type.$/m ),
	register_warning( 'use-parens', qr/Suggest parentheses around assignment used as truth value.$/m ),
	register_warning( 'instance-static-access', qr/Invoking the class method "([^"]+)" via an instance is discouraged because the method invoked will be the one in the variable's declared type, not the instance's dynamic type.$/m ),
	register_warning( 'instance-static-access', qr/Accessing the class field "([^"]+)" via an instance is discouraged because the field accessed will be the one in the variable's declared type, not the instance's dynamic type.$/m ),
	register_warning( 'static-variable-init', qr/Final field "([^"]+)" is initialized with a constant expression and could be made static to save space.$/m ),
	register_warning( 'lexical:invalid-char', qr/The use of "([^"]+)" in an identifier, while legal, is strongly discouraged, since it can conflict with compiler-generated names. If you are trying to access a nested type, use "([^"]+)" instead of "(?:[^"]+)".$/m ),
}

1;
