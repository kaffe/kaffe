#!/usr/bin/perl -w

package GCCWarning;

use strict;
use warnings;

use LogWarning;
use Registry;

use vars qw( @ISA $prefix_regex $prefix_regex_noparam $prefix_regex2 );

BEGIN {
	@ISA = qw( LogWarning );
	$prefix_regex = qr/^([^:\n]+):(\d+):(?:\d+:)? warning: /m;
	$prefix_regex_noparam = qr/^(?:[^:\n]+):(?:\d+): warning: /m;
}

sub new {
	my $class = shift;
	my ( $name, $regex, $description, $ignore ) = @_;
	my $warning = $class->SUPER::new( 'gcc', $name, qr/${prefix_regex}$regex/, $description, $ignore );
	Registry::add_warning($warning);
	return $warning;
}

sub register_description {
	Registry::register_description( 'gcc', @_ );
}

sub register_warning {
	GCCWarning->new( @_ );
}

BEGIN {
	#--
	register_description( '-Wmissing-braces', "Warn if an aggregate or union initializer is not fully bracketed.
In the following example, the initializer for `a' is not fully
bracketed, but that for `b' is fully bracketed.

int a[2][2] = { 0, 1, 2, 3 };
int b[2][2] = { { 0, 1 }, { 2, 3 } };" );
	register_warning( '-Wmissing-braces', qr/missing initializer\n${GCCWarning::prefix_regex_noparam}\(near initialization for `([^']+)'\)$/m ),
	#--
	register_description( '-Wfloat-equal', "Warn if floating point values are used in equality comparisons.

The idea behind this is that sometimes it is convenient (for the
programmer) to consider floating-point values as approximations to
infinitely precise real numbers.  If you are doing this, then you
need to compute (by analysing the code, or in some other way) the
maximum or likely maximum error that the computation introduces,
and allow for it when performing comparisons (and when producing
output, but that's a different problem).  In particular, instead
of testing for equality, you would check to see whether the two
values have ranges that overlap; and this is done with the
relational operators, so equality comparisons are probably
mistaken." ),
	register_warning( '-Wfloat-equal', qr/comparing floating point with == or != is unsafe$/m ),
	#--
	register_description( '-Wbad-function-cast', "Warn whenever a function call is cast to a non-matching type.  For
example, warn if `int malloc()' is cast to `anything *'" );
	register_warning( '-Wbad-function-cast', qr/cast does not match function type$/m ),
	#--
	register_description( '-Wmissing-prototypes', "Warn if a global function is defined without a previous prototype
declaration.  This warning is issued even if the definition itself
provides a prototype.  The aim is to detect global functions that
fail to be declared in header files." );
	register_warning( '-Wmissing-prototypes', qr/no previous prototype for [`']([^']+)'$/m ),
	#--
	register_description( '-Wredundant-decls', "Warn if anything is declared more than once in the same scope,
even in cases where multiple declaration is valid and changes
nothing." );
	register_warning( '-Wredundant-decls', qr/redundant redeclaration of [`']([^']+)'(?: in same scope)?\n${GCCWarning::prefix_regex}previous(?: implicit)? declaration of [`'][^']+'(?: was here)?$/m ),
	#--
	register_description( '-Winline', "Warn if a function can not be inlined and it was declared as
inline." );
	register_warning( '-Winline', qr/inlining failed in call to `([^']+)'\n${GCCWarning::prefix_regex}called from here$/m ),
	#--
	register_warning( 'missing-prototypes-mismatch', qr/no previous prototype for `([^']+)'\n${GCCWarning::prefix_regex_noparam}type mismatch with previous implicit declaration\n${GCCWarning::prefix_regex}previous implicit declaration of `[^']+'\n${GCCWarning::prefix_regex_noparam}`[^']+' was previously implicitly declared to return `([^']+)'$/m ),
	register_warning( '-Wformat-nonliteral', qr/format not a string literal, argument types not checked$/m ),
	register_warning( '-Wformat-nonliteral', qr/format not a string literal and no format arguments$/m ),
	register_warning( '-Wimplicit-func-decl', qr/implicit declaration of function `([^']+)'$/m ),
	register_warning( '-Wunused-function', qr/`([^']+)' defined but not used$/m ),
	register_warning( '-Wunused-parameter', qr/unused parameter `([^']+)'$/m ),
	register_warning( '-Wunused-variable', qr/unused variable `([^']+)'$/m ),
	register_warning( '-Wshadow', qr/declaration of [`']([^']+)' shadows a global declaration\n${GCCWarning::prefix_regex}shadowed declaration is here$/m ),
	register_warning( '-Wshadow', qr/declaration of `([^']+)' shadows a previous local\n${GCCWarning::prefix_regex}shadowed declaration is here$/m ),
	register_warning( '-Wpointer-arith', qr/pointer of type `([^']+)' used in arithmetic$/m ),
	register_warning( '-Wpointer-arith', qr/pointer of type `([^']+)' used in subtraction$/m ),
	register_warning( '-Wcast-qual', qr/cast discards qualifiers from pointer target type$/m ),
	register_warning( '-Wcast-qual', qr/initialization discards qualifiers from pointer target type$/m ),
	register_warning( '-Wcast-qual', qr/passing arg (\d+) of `([^']+)' discards qualifiers from pointer target type$/m ),
	register_warning( '-Wcast-qual', qr/return discards qualifiers from pointer target type$/m ),
	register_warning( '-Wcast-qual', qr/assignment discards qualifiers from pointer target type$/m ),
	register_warning( '-Wcast-qual', qr/assignment makes qualified function pointer from unqualified$/m ),
	register_warning( '-Wcast-align', qr/padding struct size to alignment boundary$/m ),
	register_warning( '-Wconversion', qr/negative integer implicitly converted to unsigned type$/m ),
	register_warning( '-Wconversion', qr/passing arg (\d+) of `([^']+)' makes (integer|pointer) from (integer|pointer) without a cast$/m ),
	register_warning( '-Wsign-compare', qr/comparison of unsigned expression < 0 is always false$/m ),
	register_warning( '-Wsign-compare', qr/comparison between signed and unsigned$/m ),
	register_warning( '-Wsign-compare', qr/signed and unsigned type in conditional expression$/m ),
	register_warning( '-Waggregate-return', qr/function call has aggregate value$/m ),
	register_warning( '-Waggregate-return', qr/function returns an aggregate$/m ),
	register_warning( '-Wstrict-prototypes', qr/non-static declaration for `([^']+)' follows static$/m ),
	register_warning( '-Wstrict-prototypes', qr/function declaration isn't a prototype$/m ),
	register_warning( '-Wmissing-declarations',  qr/"([^"]+)" is not defined\s*$/m ),
	register_warning( '-Wmissing-declarations', qr/`([^']+)' is not defined\s*$/m ),
	register_warning( '-Wmissing-noreturn', qr/function might be possible candidate for attribute `(noreturn)'$/m ),
	register_warning( '-Wmissing-noreturn', qr/`([^']+)' function does return$/m ),
	register_warning( '-Wmissing-format-attribute', qr/function might be possible candidate for `(printf)' format attribute$/m ),
	register_warning( '-Wpadded', qr/padding struct to align `([^']+)'$/m ),
	register_warning( '-Wnested-externs', qr/nested extern declaration of `([^']+)'$/m ),
	register_warning( '-Wunreachable-code', qr/will never be executed$/m ),
	register_warning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) with different width due to prototype$/m ),
	register_warning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) as (unsigned|signed) due to prototype$/m ),
	register_warning( 'traditional', qr/passing arg (\d+) of `([^']+)' as `([^']+)' rather than `([^']+)' due to prototype$/m ),
	register_warning( 'traditional', qr/macro arg `([^']+)' would be stringified with -traditional\.$/m ),
	register_warning( 'traditional', qr/passing arg (\d+) of `([^']+)' as (floating|integer) rather than (floating|integer) due to prototype$/m ),
	register_warning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) as `([^']+)' rather than `([^']+)' due to prototype$/m ),
	register_warning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) from incompatible pointer type$/m ),
	register_warning( 'deprecated-lvalue', qr/use of (compound|conditional|cast) expressions as lvalues is deprecated$/m ),
	register_warning( 'foo', qr/`([^']+)' declared inside parameter list$/m ),
	register_warning( 'foo', qr/"([^"]+)" declared inside parameter list(?:\n${GCCWarning::prefix_regex}its scope is only this definition or declaration, which is probably not what you want)?$/m ),
	register_warning( 'foo', qr/(assignment|initialization) from incompatible pointer type$/m ),
	register_warning( 'foo', qr/integer constant is too large for "([^"]+)" type$/m ),

	register_warning( '-Wold-style-definition', qr/old-style parameter declaration$/m ),
	register_warning( '-Wswitch-default', qr/switch missing default case$/m ),
	register_warning( '-Wswitch-enum', qr/enumeration value `([^']+)' not handled in switch$/m ),
	register_warning( '-Wuninitialized', qr/`([^']+)' might be used uninitialized in this function$/m ),
	register_warning( '-Wformat', qr/format argument is not a (pointer) \(arg (3)\)$/m ),
}
1;
