#!/usr/bin/perl -w

use strict;
use warnings;
use Data::Dumper;
my $prefix_regex = qr/^([^:\n]+):(\d+): warning: /m;
my $prefix_regex_noparam = qr/^(?:[^:\n]+):(?:\d+): warning: /m;
my $prefix_regex2 = qr/^([^:\n]+):(\d+):(?:\d+): warning: /m;
my $jikes_prefix = qr/([^:\n]+):(\d+):\d+:\d+:\d+: Semantic Warning: /m;

#<robilad> guilhem: ~3000 unique ones with -Wall -W -Wtraditional -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual
#          -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes
#          -Wmissing-declarations -Wmissing-noreturn -Wredundant-decls -Wnested-externs -Winline -Wlong-long

my @warning_types = (
	'java:throws-unchecked'		=> qr/${jikes_prefix}Since type "([^"]+)" is an unchecked exception, it does not need to be listed in the throws clause.$/m,
	'java:modifier-order'		=> qr/${jikes_prefix}The modifier "([^"]+)" did not appear in the recommended order public\/protected\/private, abstract, static, final, synchronized, transient, volatile, strictfp.$/m,
	'java:public-in-interface'	=> qr/${jikes_prefix}The use of the "([^"]+)" modifier in this context is redundant and strongly discouraged as a matter of style.$/m,
	'java:exception-superclass'	=> qr/${jikes_prefix}The listing of type "([^"]+)" in the throws clause is not necessary, since its superclass, "([^"]+)", is also listed.$/m,
	'java:override-default'		=> qr/${jikes_prefix}Method "([^"]+)" in class "([^"]+)" does not override or hide the corresponding method with default access in class "([^"]+)".$/m,
	'java:invalid-zip'		=> qr/${jikes_prefix}The file "([^"]+)" does not exist or else is not a valid zip file.$/m,

	'missing-prototypes-mismatch'	=> qr/${prefix_regex}no previous prototype for `([^']+)'\n${prefix_regex_noparam}type mismatch with previous implicit declaration\n${prefix_regex}previous implicit declaration of `[^']+'\n${prefix_regex_noparam}`[^']+' was previously implicitly declared to return `([^']+)'$/m,

	'-Wformat-nonliteral'		=> qr/${prefix_regex}format not a string literal, argument types not checked$/m,
	'-Wimplicit-func-decl'		=> qr/${prefix_regex}implicit declaration of function `([^']+)'$/m,
	'-Wmissing-braces'		=> qr/${prefix_regex}missing initializer\n${prefix_regex_noparam}\(near initialization for `([^']+)'\)$/m,
	'-Wunused-parameter'		=> qr/${prefix_regex}unused parameter `([^']+)'$/m,

	'-Wfloat-equal'			=> qr/${prefix_regex}comparing floating point with == or != is unsafe$/m,

	'-Wshadow:1'			=> qr/${prefix_regex}declaration of `([^']+)' shadows a global declaration\n${prefix_regex}shadowed declaration is here$/m,
	'-Wshadow:2'			=> qr/${prefix_regex}declaration of `([^']+)' shadows a previous local\n${prefix_regex}shadowed declaration is here$/m,
	'-Wpointer-arith:1'		=> qr/${prefix_regex}pointer of type `([^']+)' used in arithmetic$/m,
	'-Wpointer-arith:2'		=> qr/${prefix_regex}pointer of type `([^']+)' used in subtraction$/m,
	'-Wbad-function-cast'		=> qr/${prefix_regex}cast does not match function type$/m,
	'-Wcast-qual:1'			=> qr/${prefix_regex}cast discards qualifiers from pointer target type$/m,
	'-Wcast-qual:2'			=> qr/${prefix_regex}initialization discards qualifiers from pointer target type$/m,
	'-Wcast-qual:3'			=> qr/${prefix_regex}passing arg (\d+) of `([^']+)' discards qualifiers from pointer target type$/m,
	'-Wcast-qual:4'			=> qr/${prefix_regex}return discards qualifiers from pointer target type$/m,
	'-Wcast-qual:5'			=> qr/${prefix_regex}assignment discards qualifiers from pointer target type$/m,
	'-Wcast-align:1'		=> qr/${prefix_regex}padding struct size to alignment boundary$/m,
	'-Wconversion:1'		=> qr/${prefix_regex}negative integer implicitly converted to unsigned type$/m,
	'-Wconversion:2'		=> qr/${prefix_regex}passing arg (\d+) of `([^']+) makes (pointer) from (integer) without a cast$/m,
	'-Wconversion:3'		=> qr/${prefix_regex}passing arg (\d+) of `([^']+)' makes (pointer) from (integer) without a cast$/m,
	'-W:sign-compare'		=> qr/${prefix_regex}comparison of unsigned expression < 0 is always false$/m,
	'-Wsign-compare:1'		=> qr/${prefix_regex}comparison between signed and unsigned$/m,
	'-Wsign-compare:2'		=> qr/${prefix_regex}signed and unsigned type in conditional expression$/m,
	'-Waggregate-return:1'		=> qr/${prefix_regex}function call has aggregate value$/m,
	'-Waggregate-return:2'		=> qr/${prefix_regex}function returns an aggregate$/m,
	'-Wstrict-prototypes'		=> qr/${prefix_regex}non-static declaration for `([^']+)' follows static$/m,
	'-Wmissing-prototypes'		=> qr/${prefix_regex}no previous prototype for `([^']+)'$/m,
	'-Wmissing-declarations:1'	=> qr/${prefix_regex2}"([^"]+)" is not defined\s*$/m,
	'-Wmissing-declarations:2'	=> qr/${prefix_regex2}`([^']+)' is not defined\s*$/m,
	'-Wmissing-noreturn'		=> qr/${prefix_regex}function might be possible candidate for attribute `(noreturn)'$/m,
	'-Wmissing-format-attribute'	=> qr/${prefix_regex}function might be possible candidate for `printf' format attribute$/m,
	'-Wpadded'			=> qr/${prefix_regex}padding struct to align `([^']+)'$/m,
	'-Wredundant-decls'		=> qr/${prefix_regex}redundant redeclaration of `([^']+)' in same scope\n${prefix_regex}previous declaration of `[^']+'$/m,
	'-Wnested-externs'		=> qr/${prefix_regex}nested extern declaration of `([^']+)'$/m,
	'-Wunreachable-code'		=> qr/${prefix_regex}will never be executed$/m,
	'-Winline'			=> qr/${prefix_regex}inlining failed in call to `([^']+)'\n${prefix_regex}called from here$/m,





	'traditional-1'			=> qr/${prefix_regex}passing arg (\d+) of (?:`([^']+)'|(pointer to function)) with different width due to prototype$/m,
	'traditional-2'			=> qr/${prefix_regex}passing arg (\d+) of (?:`([^']+)'|(pointer to function)) as (unsigned|signed) due to prototype$/m,
	'traditional-3'			=> qr/${prefix_regex}passing arg (\d+) of `([^']+)' as `([^']+)' rather than `([^']+)' due to prototype$/m,
	'traditional-4'			=> qr/${prefix_regex}macro arg `([^']+)' would be stringified with -traditional\.$/m,
	'traditional-5'			=> qr/${prefix_regex}passing arg (\d+) of `([^']+)' as (floating) rather than (integer) due to prototype$/m,


	'implicit-func-decl-mismatch'	=> qr/${prefix_regex}implicit declaration of function `([^']+)'$/m,
	'deprecated-lvalue'		=> qr/${prefix_regex}use of (compound|conditional|cast) expressions as lvalues is deprecated$/m,



);

#print( STDERR join(',', keys( %warning_types ) )."\n" );
my $text;
while (<>) {
#	chomp;
	next if ( m,(^(\(cd \.libs|In file included from| *from|config\.status|checking |for |then |if |rm|mv|mkdir|ar|ranlib|creating|echo|gmake| ?gcc|Making|source|depmode|depfile|../kaffe/kaffeh/kaffeh |/bin/sh|  adding:|\[ (start compilation|compilation ended|parsed|checked body|optimized and generated|checked interfaces)|<GC|Compiling classes from| [0-9][0-9]+\.[0-9]%)|awaiting finalization>$), );
	$text .= $_;
}
print( STDERR "done loading: " . length( $text ) . "\n" );

my %allfiles;
my %error_counts;
my %errors;
my $total_errors = 0;

for ( my $i = 0; $i + 1 < @warning_types; $i += 2 ) {
	my ( $type, $regex ) = @warning_types[ $i .. $i+1 ];

print( STDERR "\t$type\t\t" );
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
		next if ( $file =~ m,^/, );
		push( @{ $errors{ $type }{ $file }{ $line } }, [ grep( { defined( $_ ) } @matches ) ] );
		$allfiles{ $file } = 1;
		$error_counts{ $type }++;
		$count++;
		$total_errors++;
	}
print( STDERR "$count\n" );
	$text = $scanned . $text;
}
#print( STDERR join( "\n", keys( %allfiles ) ) . "\n" );
print( "\nTotal Errors: $total_errors\n\n" );

#print( Dumper( \%errors ) );
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
