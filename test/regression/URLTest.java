
import java.util.*;
import java.net.*;

class URLTest {

  private static class TestCase {
    public String context;
    public String input;
    public String result;
    TestCase(String context, String input, String result) {
      this.context = context;
      this.input = input;
      this.result = result;
    }
  };

  static final TestCase[] cases = {

    new TestCase(
	null,
	"jar:http://www.kaffe.org/foo/bar.jar!/float/boat",
	"jar:http://www.kaffe.org/foo/bar.jar!/float/boat"
    ),

    new TestCase(
	null,
	"http://www.kaffe.org",
	"http://www.kaffe.org/"
    ),

    new TestCase(
	null,
	"http://www.kaffe.org:8080#ref",
	"http://www.kaffe.org:8080/#ref"
    ),

    new TestCase(
	"http://www.kaffe.org",
	"foo/bar",
	"http://www.kaffe.org/foo/bar"
    ),

    new TestCase(
	"http://www.kaffe.org/foo/bar#baz",
	"jan/far",
	"http://www.kaffe.org/foo/jan/far"
    ),

    new TestCase(
	"http://www.kaffe.org/foo/bar",
	"/jan/far",
	"http://www.kaffe.org/jan/far"
    ),

    new TestCase(
	"http://www.kaffe.org/foo/bar",
	"",
	"http://www.kaffe.org/foo/bar"
    ),

    new TestCase(
	null,
	"foo/bar",
	"java.net.MalformedURLException: no protocol: foo/bar"
    ),

    new TestCase(
	"file:/foo/bar",
	"barf#jow",
	"file:/foo/barf#jow"
    ),

    new TestCase(
	"file:/foo/bar#fly",
	"jabawaba",
	"file:/foo/jabawaba"
    ),

    new TestCase(
	null,
	"jar:file:/usr/local/share/kaffe/Klasses.jar!/kaffe/lang/unicode.tbl",
	"jar:file:/usr/local/share/kaffe/Klasses.jar!/kaffe/lang/unicode.tbl"
    ),

    new TestCase(
	null,
	"jar:http://www.kaffe.org/foo/bar.jar",
	"java.net.MalformedURLException: no !/ in spec"
    ),

    new TestCase(
	"jar:http://www.kaffe.org/foo/bar.jar!/path/name",
	"float/boat",
	"jar:http://www.kaffe.org/foo/bar.jar!/path/float/boat"
    ),

    new TestCase(
	"jar:http://www.kaffe.org/foo/bar.jar!/",
	"float/boat",
	"jar:http://www.kaffe.org/foo/bar.jar!/float/boat"
    ),

    new TestCase(
	"jar:http://www.kaffe.org/foo/bar.jar!/path/name",
	"/float/boat",
	"jar:http://www.kaffe.org/foo/bar.jar!/float/boat"
    ),

    new TestCase(
	"jar:http://www.kaffe.org/foo/bar.jar!/",
	"/float/boat",
	"jar:http://www.kaffe.org/foo/bar.jar!/float/boat"
    ),

    new TestCase(
	null,
	"foo/bar",
	"java.net.MalformedURLException: no protocol: foo/bar"
    ),

    new TestCase(
	null,
	"http://www.kaffe.org:99999/foo/bar",
	"http://www.kaffe.org:99999/foo/bar"
    ),

    new TestCase(
	null,
	"jar:abc!/eat/me",
	"java.net.MalformedURLException: invalid inner URL: no protocol: abc"
    ),

  };

  public static void main (String[] args) throws Exception {

    if (args.length > 0) {
      URL context = null;
      String input, result;

      if (args.length > 1) {
	context = new URL(args[0]);
	input = args[1];
      } else {
	input = args[0];
      }

      try {
	result = new URL(context, input).toString();
      } catch (MalformedURLException e) {
	result = e.toString();
      }

      System.out.println("  CONTEXT: " + context);
      System.out.println("    INPUT: " + input);
      System.out.println("   RESULT: " + result);
      System.exit(0);
    }

    boolean failed = false;
    for (int i = 0; i < cases.length; i++) {
      TestCase t = cases[i];
      URL context = null;
      String result;

      // Create context URL
      try {
	context = (t.context == null) ? null : new URL(t.context);
      } catch (MalformedURLException e) {
	System.out.println("Oops: " +t.context+ ": " + e);
	System.exit(1);
      }

      // Create new URL in context
      try {
	result = new URL(context, t.input).toString();
      } catch (MalformedURLException e) {
	result = e.toString();
      }

      // Check result
      if (!result.equals(t.result)) {
	System.out.println("Test failure (case " +i+ ")");
	System.out.println("  CONTEXT: " + t.context);
	System.out.println("    INPUT: " + t.input);
	System.out.println("   WANTED: " + t.result);
	System.out.println("  BUT GOT: " + result);
	failed = true;
      }
    }
    if (!failed) {
      System.out.println("Success.");
    }
  }
}

/* Expected Output:
Success.
*/

