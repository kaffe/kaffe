#!/usr/bin/perl -w 
#
# run as: mauve-html-gen.pl <extra-title-text> <extra-blurb> < mauve-output
#
# chris toshok
#

use strict;

my (%test_hash, $prev_line, $cur_class, $cur_testlet);
my ($cls, $first_testlet, $testlet, @test_results_array, @test_result_array);
my ($test_count_string, $i);
my ($tests_failed, $total_tests) = (0, 0);

my $extra_text = $ARGV[0];
my $extra_blurb = $ARGV[1];

@ARGV = ();

while (<>) {

  if (/([0-9]+) of ([0-9]+) tests failed/) {
    $tests_failed = $1;
    $total_tests = $2;
  }

  if (/PASS: ([^: ]+): (.+)? ?\(number ([0-9]+)\)$/) {
    my $trimmed_class;
    my $blurb = $2;
    my $num = $3;
    $cur_class = $1;

    $blurb  = "" if (!defined $blurb);
    $cur_class =~ /gnu.testlet.(.*)\.([^.:]+)/;
    $trimmed_class = $1;
    $cur_testlet = $2;
    $test_hash{$trimmed_class}{$cur_testlet}{$blurb}[$num] = 1;
  }
  elsif (/PASS: ([^: ]+): (.*)/) {
    my $trimmed_class;
    my $blurb = $2;
    $cur_class = $1;

    $blurb = "" if (!defined $blurb);
    $cur_class =~ /gnu.testlet.(.*)\.([^.]+)/;
    $trimmed_class = $1;
    $cur_testlet = $2;
    $test_hash{$trimmed_class}{$cur_testlet}{$blurb}[1] = 1;
  }

  if (/FAIL: ([^: ]+): (.+)? ?\(number ([0-9]+)\)$/) {
    my $trimmed_class;
    my $blurb = $2;
    my $num = $3;
    $cur_class = $1;

    $blurb  = "" if (!defined $blurb);
    $cur_class =~ /gnu.testlet.(.*)\.([^.]+)/;
    $trimmed_class = $1;
    $cur_testlet = $2;
    $test_hash{$trimmed_class}{$cur_testlet}{$blurb}[$num] = 0;
  }
  elsif (/FAIL: ([^: ]+): (.*)/) {
    my $trimmed_class;
    my $blurb = $2;
    $cur_class = $1;

    $blurb = "" if (!defined $blurb);
    $cur_class =~ /gnu.testlet.(.*)\.([^.]+)/;
    $trimmed_class = $1;
    $cur_testlet = $2;
    $test_hash{$trimmed_class}{$cur_testlet}{$blurb}[1] = 0;
  }

  $prev_line = $_;
}

print << "_EOF_"
<body bgcolor="#ffffff" text="#000000">
<center><b>Mauve Test Results $extra_text</b></center>
<p>
$extra_blurb
</p>
<p>
<font color="#ff0000">Tests failed: $tests_failed</font> &nbsp;&nbsp;&nbsp;&nbsp;
Total tests run: $total_tests
</p>
<table cellspacing=1 border=0 bgcolor="#dddddd">
_EOF_
;

foreach $cls (sort(keys %test_hash)) {
  print << "_EOF_"
  <tr><th align=left>$cls</th><td colspan=\"4\">&nbsp;</td></tr>
_EOF_
;

  $first_testlet = 1;
  foreach $testlet (sort(keys %{ $test_hash{$cls} } )) {

    my ($failure, $num_failures, $num_tests, $test_name);

    $num_failures = 0;
    $failure = 0;
    $num_tests = 0;

    foreach $test_name (keys %{ $test_hash{$cls}{$testlet} }) {
      for $i (1 .. ($#{ $test_hash{$cls}{$testlet}{$test_name} })) {
        $num_failures += 1 if ($test_hash{$cls}{$testlet}{$test_name}[$i] == 0);
        $num_tests +=1;
      }
    }

    print << "_EOF_"
        <tr><td align="right">$testlet</td><td align="left"><em>$num_tests tests</em></td>
_EOF_
;

    if ($num_failures == 0) {
      printf "<th colspan=\"3\" bgcolor=\"#00ff00\">all passed</th>";
    }
    else {
      if ($num_failures == $num_tests) {
        printf "<th colspan=\"3\" bgcolor=\"#ff0000\">all failed</th>";
      }
      else {
        printf "<th colspan=\"3\" bgcolor=\"#ffff00\">$num_failures tests failed</th>";
      }
    }

    if ($num_failures > 0 && $num_failures < $num_tests) {
      foreach $test_name (keys %{ $test_hash{$cls}{$testlet} }) {
        for $i (1 .. ($#{ $test_hash{$cls}{$testlet}{$test_name} })) {
          $failure = 0;
	  $failure = 1 if ($test_hash{$cls}{$testlet}{$test_name}[$i] == 0);
 
          if ($failure) {
	    my $bgcolor;
  
	    $bgcolor = "bgcolor=\"#ff0000\"";
    
	    printf "              <tr><td>&nbsp;</td><td>&nbsp;</td><th $bgcolor>$i</th>";

	    printf "<td align=\"right\" $bgcolor>";
	    printf "PASS</td>" if ($test_hash{$cls}{$testlet}{$test_name}[$i] == 1);
	    printf "FAIL</td>" if ($test_hash{$cls}{$testlet}{$test_name}[$i] == 0);
	    printf "<td $bgcolor>&nbsp;$test_name</td>";
	    printf "</tr>\n";
	  }
        }
      }
    }

    print << "_EOF_"
_EOF_
;
  }

  print << "_EOF_"
    </td>
  </tr>
_EOF_
;
}
  print << "_EOF_"
</table>
_EOF_
;

my $date = `date`;
printf "<em>(generated $date)</em>\n";
