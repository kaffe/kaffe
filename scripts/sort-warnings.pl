#!/usr/bin/perl -w

use strict;
use warnings;
use Data::Dumper;
BEGIN {
    my $prog = $0;
    $prog =~ s,/[^/]*,,;
    unshift( @INC, $prog );
}
use LogWarning;
use JikesWarning;
use GCCWarning;

#<robilad> guilhem: ~3000 unique ones with -Wall -W -Wtraditional -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual
#          -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes
#          -Wmissing-declarations -Wmissing-noreturn -Wredundant-decls -Wnested-externs -Winline -Wlong-long

my $path_prefix_regex = qr/\/?([^\/]*\/)*?/;
my $prog_regex = qr/ ?\(?(${path_prefix_regex})\b(kaffeh|config\.status|rm|mv|mkdir|ar|ranlib|echo|gmake|sh|sed|cd|grep|cat|gcc|touch|test|make)\b/;
my $kjc_regex = qr/\[ (?:start compilation|compilation ended|parsed|checked body|optimized and generated|checked interfaces)/;
my $shell_keywords = qr/(?:for|then|if) /;
my $make_regex = qr/make\[\d+\]: (?:(?:Entering|Leaving) directory|Nothing to be done for)/;
my $libtool_regex = qr/generating symbol list for/;

my $skip_line_regex = qr,
    (
        ^(
            \(cd\ \.libs|
            In\ file\ included\ from|
            \ *from|
            checking\ |
            $shell_keywords|
            $prog_regex|
            creating|
            Making|
            source|
            $make_regex|
            depmode|
            depfile|
            \ \ adding:|
            $kjc_regex|
            $libtool_regex|
            <GC|
            \(dstdir=|
            Compiling\ classes\ from|
            \ [0-9][0-9]+\.[0-9]%
        )|
        awaiting\ finalization>$
    ),x;

my @warning_types = (
	new JikesWarning( 'throws-uncheck', qr/Since type "([^"]+)" is an unchecked exception, it does not need to be listed in the throws clause.$/m, '' ),
	new JikesWarning( 'throws-unchecked', qr/Since type "([^"]+)" is an unchecked exception, it does not need to be listed in the throws clause.$/m ),
	new JikesWarning( 'modifier-order', qr/The modifier "([^"]+)" did not appear in the recommended order public\/protected\/private, abstract, static, final, synchronized, transient, volatile, strictfp.$/m ),
	new JikesWarning( 'public-in-interface', qr/The use of the "([^"]+)" modifier in this context is redundant and strongly discouraged as a matter of style.$/m ),
	new JikesWarning( 'exception-superclass', qr/The listing of type "([^"]+)" in the throws clause is not necessary, since its superclass, "([^"]+)", is also listed.$/m ),
	new JikesWarning( 'override-default', qr/Method "([^"]+)" in class "([^"]+)" does not override or hide the corresponding method with default access in class "([^"]+)".$/m ),
	new JikesWarning( 'invalid-zip', qr/The file "([^"]+)" does not exist or else is not a valid zip file.$/m ),
	new JikesWarning( 'invalid-zip', qr/I\/O warning: "No such file or directory" while trying to open (.*)\.$/m ),
	new JikesWarning( 'negative-shift-count', qr/The shift count (-\d+) is negative; it will be masked to the appropriate width and behave as a positive shift count.$/m ),
	new JikesWarning( 'large-shift-count', qr/The shift count of (\d+) is >= the (\d+-bit) width of the type.$/m ),
	new JikesWarning( 'method-is-constructor', qr/The name of this method "([^"]+)" matches the name of the containing class. However, the method is not a constructor since its declarator is qualified with a type.$/m ),
	new JikesWarning( 'use-parens', qr/Suggest parentheses around assignment used as truth value.$/m ),
	new JikesWarning( 'instance-static-access', qr/Invoking the class method "([^"]+)" via an instance is discouraged because the method invoked will be the one in the variable's declared type, not the instance's dynamic type.$/m ),
	new JikesWarning( 'instance-static-access', qr/Accessing the class field "([^"]+)" via an instance is discouraged because the field accessed will be the one in the variable's declared type, not the instance's dynamic type.$/m ),
	new JikesWarning( 'static-variable-init', qr/Final field "([^"]+)" is initialized with a constant expression and could be made static to save space.$/m ),
	new JikesWarning( 'lexical:invalid-char', qr/The use of "([^"]+)" in an identifier, while legal, is strongly discouraged, since it can conflict with compiler-generated names. If you are trying to access a nested type, use "([^"]+)" instead of "(?:[^"]+)".$/m ),

	new GCCWarning( 'missing-prototypes-mismatch', qr/no previous prototype for `([^']+)'\n${GCCWarning::prefix_regex_noparam}type mismatch with previous implicit declaration\n${GCCWarning::prefix_regex}previous implicit declaration of `[^']+'\n${GCCWarning::prefix_regex_noparam}`[^']+' was previously implicitly declared to return `([^']+)'$/m ),

	new GCCWarning( '-Wformat-nonliteral', qr/format not a string literal, argument types not checked$/m ),
	new GCCWarning( '-Wformat-nonliteral', qr/format not a string literal and no format arguments$/m ),
	new GCCWarning( '-Wimplicit-func-decl', qr/implicit declaration of function `([^']+)'$/m ),
	new GCCWarning( '-Wmissing-braces', qr/missing initializer\n${GCCWarning::prefix_regex_noparam}\(near initialization for `([^']+)'\)$/m ),
	new GCCWarning( '-Wunused-parameter', qr/unused parameter `([^']+)'$/m ),
	new GCCWarning( '-Wunused-variable', qr/unused variable `([^']+)'$/m ),

	new GCCWarning( '-Wfloat-equal', qr/comparing floating point with == or != is unsafe$/m ),

	new GCCWarning( '-Wshadow', qr/declaration of `([^']+)' shadows a global declaration\n${GCCWarning::prefix_regex}shadowed declaration is here$/m ),
	new GCCWarning( '-Wshadow', qr/declaration of `([^']+)' shadows a previous local\n${GCCWarning::prefix_regex}shadowed declaration is here$/m ),
	new GCCWarning( '-Wpointer-arith', qr/pointer of type `([^']+)' used in arithmetic$/m ),
	new GCCWarning( '-Wpointer-arith', qr/pointer of type `([^']+)' used in subtraction$/m ),
	new GCCWarning( '-Wbad-function-cast', qr/cast does not match function type$/m ),
	new GCCWarning( '-Wcast-qual', qr/cast discards qualifiers from pointer target type$/m ),
	new GCCWarning( '-Wcast-qual', qr/initialization discards qualifiers from pointer target type$/m ),
	new GCCWarning( '-Wcast-qual', qr/passing arg (\d+) of `([^']+)' discards qualifiers from pointer target type$/m ),
	new GCCWarning( '-Wcast-qual', qr/return discards qualifiers from pointer target type$/m ),
	new GCCWarning( '-Wcast-qual', qr/assignment discards qualifiers from pointer target type$/m ),
	new GCCWarning( '-Wcast-qual', qr/assignment makes qualified function pointer from unqualified$/m ),
	new GCCWarning( '-Wcast-align', qr/padding struct size to alignment boundary$/m ),
	new GCCWarning( '-Wconversion', qr/negative integer implicitly converted to unsigned type$/m ),
	new GCCWarning( '-Wconversion', qr/passing arg (\d+) of `([^']+)' makes (integer|pointer) from (integer|pointer) without a cast$/m ),
	new GCCWarning( '-Wsign-compare', qr/comparison of unsigned expression < 0 is always false$/m ),
	new GCCWarning( '-Wsign-compare', qr/comparison between signed and unsigned$/m ),
	new GCCWarning( '-Wsign-compare', qr/signed and unsigned type in conditional expression$/m ),
	new GCCWarning( '-Waggregate-return', qr/function call has aggregate value$/m ),
	new GCCWarning( '-Waggregate-return', qr/function returns an aggregate$/m ),
	new GCCWarning( '-Wstrict-prototypes', qr/non-static declaration for `([^']+)' follows static$/m ),
	new GCCWarning( '-Wstrict-prototypes', qr/function declaration isn't a prototype$/m ),
	new GCCWarning( '-Wmissing-prototypes', qr/no previous prototype for `([^']+)'$/m ),
	new GCCWarning( '-Wmissing-declarations'	=> qr/"([^"]+)" is not defined\s*$/m ),
	new GCCWarning( '-Wmissing-declarations'	=> qr/`([^']+)' is not defined\s*$/m ),
	new GCCWarning( '-Wmissing-noreturn', qr/function might be possible candidate for attribute `(noreturn)'$/m ),
	new GCCWarning( '-Wmissing-noreturn', qr/`([^']+)' function does return$/m ),
	new GCCWarning( '-Wmissing-format-attribute', qr/function might be possible candidate for `printf' format attribute$/m ),
	new GCCWarning( '-Wpadded', qr/padding struct to align `([^']+)'$/m ),
	new GCCWarning( '-Wredundant-decls', qr/redundant redeclaration of `([^']+)' in same scope\n${GCCWarning::prefix_regex}previous declaration of `[^']+'$/m ),
	new GCCWarning( '-Wnested-externs', qr/nested extern declaration of `([^']+)'$/m ),
	new GCCWarning( '-Wunreachable-code', qr/will never be executed$/m ),
	new GCCWarning( '-Winline', qr/inlining failed in call to `([^']+)'\n${GCCWarning::prefix_regex}called from here$/m ),





	new GCCWarning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) with different width due to prototype$/m ),
	new GCCWarning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) as (unsigned|signed) due to prototype$/m ),
	new GCCWarning( 'traditional', qr/passing arg (\d+) of `([^']+)' as `([^']+)' rather than `([^']+)' due to prototype$/m ),
	new GCCWarning( 'traditional', qr/macro arg `([^']+)' would be stringified with -traditional\.$/m ),
	new GCCWarning( 'traditional', qr/passing arg (\d+) of `([^']+)' as (floating|integer) rather than (floating|integer) due to prototype$/m ),
	new GCCWarning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) as `([^']+)' rather than `([^']+)' due to prototype$/m ),
	new GCCWarning( 'traditional', qr/passing arg (\d+) of (?:`([^']+)'|(pointer to function)) from incompatible pointer type$/m ),


	new GCCWarning( 'deprecated-lvalue', qr/use of (compound|conditional|cast) expressions as lvalues is deprecated$/m ),

	new GCCWarning( 'foo', qr/`([^']+)' declared inside parameter list$/m ),
	new GCCWarning( 'foo', qr/(assignment|initialization) from incompatible pointer type$/m ),
	new GCCWarning( 'foo', qr/integer constant is too large for "([^"]+)" type$/m ),

);

# No User Servicable Parts Below

my %disabled;

# <auto-number duplicate entries>
my %duplicated_warnings;
for ( my $i = 0; $i < @warning_types; $i++ ) {
	my $warning = $warning_types[ $i ];
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
# </auto-number duplicate entries>

#print( STDERR join(',', keys( %warning_types ) )."\n" );
my $text;
my $removeNext = 0;
while (<>) {
#	chomp;
	if ( $removeNext ) {
        $removeNext = 0 if ( !m/\\$/ );
        next;
    }
    if ( m/$skip_line_regex/ ) {
        if ( m/\\$/ ) {
            $removeNext = 1;
        } else {
            $removeNext = 0;
        }
        next;
    }
	$text .= $_;
}
print( STDERR "done loading: " . length( $text ) . "\n" );

my %file_errors;
my %error_counts;
my %errors;
my $total_errors = 0;

for ( my $i = 0; $i < @warning_types; $i++ ) {
	my $warning = $warning_types[ $i ];
    my $compiler = $warning->compiler();
    my $type = $warning->name();
    my $regex = $warning->regex();

print( STDERR "\t$type\t\t" ) if ( !$disabled{ $compiler } );
	my @matches;
	my $scanned = '';
	my $count = 0;
	while ( @matches = ( $text =~ m/$regex/m ) ) {
#print STDERR join(', ', map( { length($_) } @matches ))."\n";
		$scanned .= $`;
		$text = $';
#print STDERR length( $text ) . "\t$type => $full_regex\n";
#print( STDERR "1='$1' [@matches]\n" );
		my ( $file, $line, ) = ( shift( @matches ), shift( @matches ) );
		$file =~ s,(?:(?:\.\./)+|/tmp/topic/)(include|kaffe|libraries|config)/,$1/,;
        # skip files if they are absolute, on the assumption they are system files.
		next if ( $file =~ m,^/, || $disabled{ $compiler } );
		push( @{ $errors{ $type }{ $file }{ $line } }, [ grep( { defined( $_ ) } @matches ) ] );
		$file_errors{ $file }++;
		$error_counts{ $type }++;
		$count++;
		$total_errors++;
	}
print( STDERR "$count\n" ) if ( !$disabled{ $compiler } );
	$text = $scanned . $text;
}
#print( STDERR join( "\n", keys( %file_errors ) ) . "\n" );
print( "\nTotal Errors: $total_errors\n\n" );

#print( Dumper( \%errors ) );
foreach my $file ( sort( { $file_errors{ $b } <=> $file_errors{ $a } } keys( %file_errors ) ) ) {
	my $count = $file_errors{ $file };
	print( "File: $count\t$file\n" );
}
print( "\n" );
foreach my $type ( sort( { $error_counts{ $b } <=> $error_counts{ $a } } keys( %errors ) ) ) {
	my $h1 = $errors{ $type };
	print( "Type: $type\nCount: $error_counts{ $type }\n" );
	foreach my $file ( sort( keys( %$h1 ) ) ) {
		my @text = ();
		my $file_warning_count = 0;
		my $h2 = $h1->{ $file };
		foreach my $line ( sort( { $a <=> $b } keys( %$h2 ) ) ) {
			my $params = $h2->{ $line };
			foreach my $param ( @$params ) {
				my $text .= "\t\tLine: $line\n";
				$text .= "\t\tParams: " . join( ',', @$param ) . "\n" if ( @$param );
				push( @text, $text );
				$file_warning_count++;
			}
		}
		print( "\tFile: $file ($file_warning_count)\n" );
		print( join( "", @text ) . "\n" );
	}
}
print( $text );
#print( join( ',', keys( %errors ) ) . "\n" );
