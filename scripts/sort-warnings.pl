#!/usr/bin/perl -w

use strict;
use warnings;

use Data::Dumper;

BEGIN {
	my $prog = $0;
	$prog =~ s,/[^/]*$,,;
	unshift( @INC, $prog );
}

use LogWarning;
use JikesWarning;
use GCCWarning;
use SparseWarning;
use Registry;

Registry::analyze_warnings();

#<robilad> guilhem: ~3000 unique ones with -Wall -W -Wtraditional -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual
#	   -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes
#	   -Wmissing-declarations -Wmissing-noreturn -Wredundant-decls -Wnested-externs -Winline -Wlong-long

my $path_prefix_regex = qr/\/?([^\/]*\/)*?/;
my $prog_regex = qr/ ?\(?(${path_prefix_regex})\b(kaffeh|config\.status|rm|mv|mkdir|ar|ranlib|echo|gmake|bash|sh|nm|sed|cd|grep|cat|gcc|touch|test|make|cgcc)\b/;
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
			extracting\ global\ C\ symbols\ from|
			\ \ adding:|
			$kjc_regex|
			$libtool_regex|
			<GC|
			\(dstdir=|
			Compiling\ classes\ from|
			\ [0-9][0-9]+\.[0-9]%
		)|
		(
			is\ already\ up-to-date|
			awaiting\ finalization>
		)$
	),x;

my %ignore = (
	'gcc/traditional:1' => 1,
	'sparse-cc/too-many-warnings' => 1,
);

# No User Servicable Parts Below

my %disabled;

#print( STDERR join(',', keys( %warning_types ) )."\n" );
# <read lines into a single text var>
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
# </read lines into a single text var>

my %file_errors;
my %error_counts;
my %errors;
my $total_errors = 0;
my %compiler_errors;

for ( my $i = 0; $i < @Registry::warnings; $i++ ) {
	my $warning = $Registry::warnings[ $i ];
	my $compiler = $warning->compiler();
	my $type = $warning->name();
	my $regex = $warning->regex();
	my $ignore = $warning->ignore() || $ignore{ $type };

	if ( !$disabled{ $compiler } ) {
		print( STDERR "\t$type" );
		print( STDERR '*' ) if ( $warning->get_description() );
		print( STDERR '!' ) if ( $ignore );
		print( STDERR "\t\t" );
	}
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
		if ( !$ignore ) {
			push( @{ $errors{ $type }{ $file }{ $line } }, [ grep( { defined( $_ ) } @matches ) ] );
			$file_errors{ $file }++;
			$error_counts{ $type }++;
		}
		$count++;
	}
	if ( !$ignore ) {
		$total_errors += $count;
		$compiler_errors{ $compiler } += $count;
	}
print( STDERR "$count\n" ) if ( !$disabled{ $compiler } );
	$text = $scanned . $text;
}
#print( STDERR join( "\n", keys( %file_errors ) ) . "\n" );
print( "\nTotal Errors: $total_errors\n" );
while ( my ( $compiler, $count ) = each( %compiler_errors ) ) {
	print( "Compiler($compiler) Errors: $count\n" ) if ( !$disabled{ $compiler } );
}
print( "\n" );
#print( Dumper( \%errors ) );
foreach my $file ( sort( { $file_errors{ $b } <=> $file_errors{ $a } } keys( %file_errors ) ) ) {
	my $count = $file_errors{ $file };
	print( "File: $count\t$file\n" );
}
print( "\n" );
foreach my $type ( sort( { $error_counts{ $b } <=> $error_counts{ $a } } keys( %errors ) ) ) {
	my $h1 = $errors{ $type };
	my $warning = Registry::get_warning( $type );
	next if ( $warning->ignore() || $ignore{ $type } );
	print( "Type: $type\n" );
	my $description = $warning->get_description();
	print( "Description:\n" . join( "", map( { "\t$_\n" } split( "\n", $description ) ) ) ) if ( $description );
	print( "Count: $error_counts{ $type }\n" );
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
