#!/usr/bin/perl -w

use strict;
use warnings;
use Data::Dumper;
my $prefix_regex = qr/^([^:\n]+):(\d+): warning: /m;
my $prefix_regex2 = qr/^([^:\n]+):(\d+):(?:\d+): warning: /m;

#<robilad> guilhem: ~3000 unique ones with -Wall -W -Wtraditional -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual
#          -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes
#          -Wmissing-declarations -Wmissing-noreturn -Wredundant-decls -Wnested-externs -Winline -Wlong-long

my %warning_types = (
	'traditional-1'			=> qr/${prefix_regex}passing arg (\d+) of (?:`([^']+)'|(pointer to function)) with different width due to prototype$/m,
	'traditional-2'			=> qr/${prefix_regex}passing arg (\d+) of (?:`([^']+)'|(pointer to function)) as (unsigned|signed) due to prototype$/m,
	'traditional-3'			=> qr/${prefix_regex}passing arg (\d+) of `([^']+)' as `([^']+)' rather than `([^']+)' due to prototype$/m,
	'traditional-4'			=> qr/${prefix_regex}macro arg `([^']+)' would be stringified with -traditional\.$/m,
	'shadow-1'			=> qr/${prefix_regex}declaration of `([^']+)' shadows a global declaration$/m,
	'shadow-2'			=> qr/${prefix_regex}declaration of `([^']+)' shadows a previous local$/m,
	'pointer-arith-1'		=> qr/${prefix_regex}pointer of type `([^']+)' used in arithmetic$/m,
	'pointer-arith-2'		=> qr/${prefix_regex}pointer of type `([^']+)' used in subtraction$/m,
	'bad-function-cast'		=> qr/${prefix_regex}cast does not match function type$/m,
	'cast-qual-1'			=> qr/${prefix_regex}cast discards qualifiers from pointer target type$/m,
	'cast-qual-2'			=> qr/${prefix_regex}initialization discards qualifiers from pointer target type$/m,
	'cast-qual-3'			=> qr/${prefix_regex}passing arg (\d+) of `([^']+)' discards qualifiers from pointer target type$/m,
	'cast-qual-4'			=> qr/${prefix_regex}return discards qualifiers from pointer target type$/m,
	'cast-qual-5'			=> qr/${prefix_regex}assignment discards qualifiers from pointer target type$/m,
	'cast-align-1'			=> qr/${prefix_regex}padding struct size to alignment boundary$/m,
	'cast-align-2'			=> qr/${prefix_regex}padding struct to align `([^']+)'$/m,
	'conversion'			=> qr/${prefix_regex}negative integer implicitly converted to unsigned type$/m,
	'sign-compare-1'		=> qr/${prefix_regex}comparison of unsigned expression < 0 is always false$/m,
	'sign-compare-2'		=> qr/${prefix_regex}comparison between signed and unsigned$/m,
	'sign-compare-3'		=> qr/${prefix_regex}signed and unsigned type in conditional expression$/m,
	'aggregate-return-1'		=> qr/${prefix_regex}function call has aggregate value$/m,
	'aggregate-return-2'		=> qr/${prefix_regex}function returns an aggregate$/m,
#	'strict-prototypes-1'		=> qr/${prefix_regex}previous declaration of `([^']+)'$/m,
	'strict-prototypes-1'		=> qr/${prefix_regex}redundant redeclaration of `([^']+)' in same scope$/m,
	'strict-prototypes-2'		=> qr/${prefix_regex}non-static declaration for `([^']+)' follows static$/m,
	'missing-prototypes'		=> qr/${prefix_regex}no previous prototype for `([^']+)'$/m,
	'missing-declarations-1'	=> qr/${prefix_regex2}"([^"]+)" is not defined\s*$/m,
	'missing-declarations-2'	=> qr/${prefix_regex2}`([^']+)' is not defined\s*$/m,
	'missing-noreturn'		=> qr/${prefix_regex}function might be possible candidate for attribute `(noreturn)'$/m,
	'attribute-formatf'		=> qr/${prefix_regex}function might be possible candidate for `printf' format attribute$/m,
	'redundant-decls'		=> qr/${prefix_regex}redundant redeclaration of `([^']+)' in same scope$/m,
	'nested-externs'		=> qr/${prefix_regex}nested extern declaration of `([^']+)'$/m,
	'unused'			=> qr/${prefix_regex}unused parameter `([^']+)'$/m,
	'never-run'			=> qr/${prefix_regex}will never be executed$/m,

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

foreach my $type ( keys( %warning_types ) ) {
	my $regex = $warning_types{ $type };

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
		push( @{ $errors{ $type }{ $file }{ $line } }, [ @matches ] );
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
		print( "\tFile: $file\n" );
		my $h2 = $h1->{ $file };
		foreach my $line ( sort( { $a <=> $b } keys( %$h2 ) ) ) {
			my $params = $h2->{ $line };
			foreach my $param ( @$params ) {
				print( "\t\tLine: $line\n" );
				print( "\t\tParams: " . join( ',', @$param ) . "\n\n" ) if ( @$params );
			}
		}
	}
}
print( $text );
#print( join( ',', keys( %errors ) ) . "\n" );
