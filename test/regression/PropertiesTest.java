import java.io.*;
import java.util.*;

public class PropertiesTest {

	private static final String INPUT_PREFIX = "//INPUT-";

	public static void main(String[] args) throws Exception {

		// Generate test input file into data1
		BufferedReader r = new BufferedReader(
		    new InputStreamReader(
		    new FileInputStream(
		    new File(args[0]))));
		ByteArrayOutputStream data1 = new ByteArrayOutputStream();
		PrintWriter p = new PrintWriter(
		    new OutputStreamWriter(data1, "ISO_8859-1"));
		for (String line; (line = r.readLine()) != null; ) {
			if (!line.startsWith(INPUT_PREFIX))
				continue;
			p.println(line.substring(INPUT_PREFIX.length()));
		}
		p.flush();
		p.close();

		// Load expected properties
		Properties expect = new Properties();
		for (int i = 0; i < EXPECT.length / 2; i++)
			expect.setProperty(EXPECT[i * 2], EXPECT[i * 2 + 1]);

		// Read in input into p1
		Properties p1 = new Properties();
		p1.load(new ByteArrayInputStream(data1.toByteArray()));

		// Verify contents of p1
		verify(p1, expect);

		// Write out to data2
		ByteArrayOutputStream data2 = new ByteArrayOutputStream();
		p1.store(data2, new Date().toString());

		// Read in again into p2
		Properties p2 = new Properties();
		p2.load(new ByteArrayInputStream(data2.toByteArray()));
		verify(p2, expect);

		// OK
		System.out.println("Done.");
	}

	private static void fail(String s) throws Exception {
		System.out.println("FAILURE: " + s);
	}

	private static void verify(Properties p, Properties q)
			throws Exception {
		if (p.size() != q.size())
			fail("size " + p.size() + " != " + q.size());
		for (Iterator i = p.keySet().iterator(); i.hasNext(); ) {
			String key = (String)i.next();
			String val, val2;
			if ((val = p.getProperty(key)) == null)
				fail("key \"" + key + "\" missing?");
			if ((val2 = q.getProperty(key)) == null)
				fail("key \"" + key + "\" unexpected");
			if (val2 != null && !val2.equals(val))
				fail("key \"" + key + "\": wrong value: "
				    + "\"" + val + "\" != \"" + val2 + "\"");
		}
	}

	private static final String[] EXPECT = new String[] {
		"fooKey",			"fooValue",
		"barKey",			"override previous bar value",
		"noEqualsSign",			"value",
		"spacesAroundEqualsSign",	"this is the value",
		"useColon",			"colon's value",
		"key=contains=equals",		"value3",
		"key:contains:colons",		"value4",
		"key contains spaces",		"value5",
		"backslashes",			"\r\n\t\\\"'",
		"lineContinuation",
			"this is a line continuation times two",
		"unicodeString",
			new String(new char[] { (char)0x1234, (char)0x5678 } ),
		"controlKey",
			new String(new char[] { (char)1, } ),
		"keyWithEmptyStringAsValue",	""
	};
}

// java args: PropertiesTest $1
/* Expected Output:
Done.
*/

//INPUT-fooKey=fooValue
//INPUT-controlKey:\u0001
//INPUT-barKey=bar value
//INPUT-barKey=override previous bar value
//INPUT-noEqualsSign value
//INPUT-spacesAroundEqualsSign   =  this is the value
//INPUT-useColon : colon's value
//INPUT-key\=contains\=equals=value3
//INPUT-key\:contains\:colons=value4
//INPUT-key\ contains\ spaces=value5
//INPUT-backslashes = \r\n\t\\\"\'
//INPUT-lineContinuation=this is a \
//INPUT-   line continuation \
//INPUT-   times two
//INPUT-unicodeString=\u1234\u5678
//INPUT-keyWithEmptyStringAsValue
//INPUT-# a few blank lines follow
//INPUT-
//INPUT-
//INPUT-# comment line
//INPUT-      ## another comment line
//INPUT-! another comment line
//INPUT-      !! another comment line

