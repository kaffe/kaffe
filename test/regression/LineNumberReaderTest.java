// LineNumberReader tests
// submitted by Dalibor Topic

import java.io.*;

class LineNumberReader_Test {

  // Print a failure message
  public static void failure(Class test, Exception e) {
    System.out.print(test.toString());
    System.out.println(" failed");
    if (e != null) {
	System.out.println(e);
	// e.printStackTrace();
    }
  }

  // Print a success message
  public static void success(Class test) {
    System.out.print(test.toString());
    System.out.println(" succeeded");
  }

}

class LineNumberReader_Test1 extends LineNumberReader_Test {
  // Construct from null
  public LineNumberReader_Test1() {
    super();

    try {
      LineNumberReader lnr = new LineNumberReader(null);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test2 extends LineNumberReader_Test {

  // Construct with size 0
  public LineNumberReader_Test2() {
    super();

    try {
      StringReader sr = new StringReader(" ");
      LineNumberReader lnr = new LineNumberReader(sr, 0);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test3 extends LineNumberReader_Test {

  // Construct with negative size
  public LineNumberReader_Test3() {
    super();

    try {
      StringReader sr = new StringReader("X");
      LineNumberReader lnr = new LineNumberReader(sr, -5);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class NewLNR extends LineNumberReader_Test {

  // Create a new line number reader to experiment with
  protected StringReader sr;
  protected LineNumberReader lnr;
  protected int bufferlength;

  public NewLNR () {
    this("X");
  }

  public NewLNR (String buffer) {
    sr = new StringReader(buffer);
    lnr = new LineNumberReader(sr);
    bufferlength = buffer.length();
  }
}

class LineNumberReader_Test4 extends NewLNR {

  // Initial line number
  public LineNumberReader_Test4() {
    super();

    try {
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test5 extends NewLNR {

  // read ahead limit = 0
  public LineNumberReader_Test5() {
    super();

    try {
      lnr.mark(0);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test6 extends NewLNR {

  // read ahead limit < 0
  public LineNumberReader_Test6() {
    super();

    try {
      lnr.mark(-5);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test7 extends NewLNR {

  // read into non-existent character array
  public LineNumberReader_Test7() {
    super();

    try {
      int r = lnr.read(null, 0, 0);
      System.out.println(r);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test8 extends NewLNR {

  // read past end of character array
  public LineNumberReader_Test8() {
    super();

    try {
      int r = lnr.read(new char[1], 2, 1);
      System.out.println(r);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test9 extends NewLNR {

  // read past end of character array
  public LineNumberReader_Test9() {
    super();

    try {
      int r = lnr.read(new char[1], 0, 2);
      System.out.println(r);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test10 extends NewLNR {

  // read at negative offset
  public LineNumberReader_Test10() {
    super();

    try {
      int r = lnr.read(new char[1], -5, 0);
      System.out.println(r);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test11 extends NewLNR {

  // read negative amount of characters
  public LineNumberReader_Test11() {

    try {
      int r = lnr.read(new char[1], 0, -5);
      System.out.println(r);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test12 extends NewLNR {

  // read correct amount of characters
  public LineNumberReader_Test12() {

    try {
      System.out.println(bufferlength);
      int r = lnr.read(new char[bufferlength], 0, bufferlength );
      System.out.println(r);
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test13 extends NewLNR {

  // set line number to 0
  public LineNumberReader_Test13() {
    try {
      lnr.setLineNumber(0);
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test14 extends NewLNR {

  // set line number to < 0
  public LineNumberReader_Test14() {
    try {
      lnr.setLineNumber(-5);
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test15 extends NewLNR {

  // read \n

  public LineNumberReader_Test15() {
    super("\n");

    try {
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test16 extends NewLNR {

    // read \r (compression test)

  public LineNumberReader_Test16() {
    super("\r");

    try {
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test17 extends NewLNR {

  // read \r\n (compression test)

  public LineNumberReader_Test17() {
    super("\r\n");

    try {
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test18 extends NewLNR {

  // read \r\r\n (compression test)

  public LineNumberReader_Test18() {
    super("\r\r\n");

    try {
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test19 extends NewLNR {

  // read \n

  public LineNumberReader_Test19() {
    super("\n");

    try {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test20 extends NewLNR {

  // read \r (compression test)

  public LineNumberReader_Test20() {
    super("\r");

    try {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test21 extends NewLNR {

  // read \r\n (compression test)

  public LineNumberReader_Test21() {
    super("\r\n");

    try {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test22 extends NewLNR {

  // read \r\r\n (compression test)

  public LineNumberReader_Test22() {
    super("\r\r\n");

    try {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test23 extends NewLNR {

  // read \r\r\n (compression test)
  // read(,,) *doesn't* compress characters

  public LineNumberReader_Test23() {
    super("\r\n");

    try {
      char [] ch = new char[2];
      int r = lnr.read(ch,0,2);
      System.out.print(new Integer(r).toString());
      for (int i = 0; i < r; i++) {
	System.out.print(" , " + new Integer(ch[i]).toString());
      }
      System.out.println("");
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test24 extends NewLNR {

  // read \r\r\n (compression test)

  public LineNumberReader_Test24() {
    super("\r\n\r");

    try {
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      System.out.println(lnr.read());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test25 extends NewLNR {

  // read \r\r\n (compression test)

  public LineNumberReader_Test25() {
    super("\r\r\n");

    try {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test26 extends NewLNR {

  // read \r\n\r (compression test)
  // with interleaving read() and read (,,)
  public LineNumberReader_Test26() {
    super("\r\n\r");

    try {
      char [] ch = new char[1];
      System.out.println(lnr.read());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.read());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test27 extends NewLNR {

  // read \r\n\r (compression test)
  // with interleaving read() and read (,,)

  public LineNumberReader_Test27() {
    super("\r\n\r");

    try {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.read());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test28 extends NewLNR {

    // read \r\n\r (compression test)
    // with interleaving read() and read (,,)
    // and as an extra meanness, a mark is set on \n

    public void prepare() throws IOException {
      System.out.println(lnr.read());
      lnr.mark(5);
    }

    public void test() throws IOException {
      char [] ch = new char[1];
 
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
      System.out.println(lnr.read());
    }

  public LineNumberReader_Test28() {
    super("\r\n\r");

    try {
	prepare();
	test();
	lnr.reset();
	test();
	System.out.println(lnr.getLineNumber());
	success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test29 extends NewLNR {

    // read \r\n\r (compression test)
    // with interleaving read() and read (,,)
    // and as an extra meanness, a mark is set on \n

    public void prepare() throws IOException {
      char [] ch = new char[1];
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	  + " , " + new Integer(ch[0]).toString());
      lnr.mark(5);
    }

    public void test() throws IOException {
      char [] ch = new char[1];
      System.out.println(lnr.read());
      System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
	+ " , " + new Integer(ch[0]).toString());
    }

  public LineNumberReader_Test29() {
    super("\r\n\r");

    try {
	prepare();
	test();
	lnr.reset();
	test();
	System.out.println(lnr.getLineNumber());
	success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test30 extends NewLNR {

    // reset without a mark
    public LineNumberReader_Test30 () {
	try {
	    lnr.reset();
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test31 extends NewLNR {

    // getLineNumber, mark, setLineNumber, reset, and getLineNumber.
    // is it the line number before the mark was set, or the set line number?
    // the spec is not clear on this one.

    public LineNumberReader_Test31 () {
	try {
	    System.out.println(lnr.getLineNumber());
	    lnr.mark(5);
	    lnr.setLineNumber(10);
	    lnr.reset();
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test32 extends NewLNR {

    // readLine() after end of buffer
    // what's the line number ?

    // It should be one for the default NewLNR.

    public LineNumberReader_Test32 () {
	try {
	    System.out.println(lnr.readLine());
	    lnr.readLine();
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test33 extends NewLNR {

  // read \r\n (compression test)

  public LineNumberReader_Test33() {
    super("\r\n");

    try {
      System.out.println(lnr.readLine());
      lnr.readLine();
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test34 extends NewLNR {

  // read \r\r\n (compression test)

  public LineNumberReader_Test34() {
    super("\r\r\n");

    try {
      System.out.println(lnr.readLine());
      System.out.println(lnr.readLine());
      lnr.readLine();
      System.out.println(lnr.getLineNumber());
      success(this.getClass());
    }
    catch (Exception e) {
      failure(this.getClass(), e);
    }
  }
}

class LineNumberReader_Test35 extends NewLNR {

    // read \r\nX\n
    // readLine should return "X"

    public LineNumberReader_Test35 () {
	super("\r\nX\n");

	try {
	    char [] ch = new char[1];
	    // read over \r
	    System.out.println(new Integer(lnr.read(ch,0,1)).toString() 
		+ " , " + new Integer(ch[0]).toString());

	    // The line feed should be skipped over. It belongs to
	    // last carriage return.
	    // This command should print "X" on standard out.
	    System.out.println(lnr.readLine());

	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test36 extends NewLNR {

    // readLine() gets EOF before a line terminator
    // what's the line number ?
    public LineNumberReader_Test36 () {
	try {
	    System.out.println(lnr.readLine());
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test37 extends NewLNR {

    // negative skip

    public LineNumberReader_Test37 () {
	try {
	    lnr.skip(-5);
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test38 extends NewLNR {

    // skip at EOF
    // what does it return? Spec says number of characters read.
    // so it should be 0.

    public LineNumberReader_Test38 () {
	super("");
	try {
	    System.out.println(lnr.skip(1));
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test39 extends NewLNR {

    // skip: as how many characters does \r\n count?

    public LineNumberReader_Test39 () {
	super("\r\nXY");
	try {
	    System.out.println(lnr.skip(2));
	    System.out.println(lnr.read());
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test40 extends NewLNR {

    // skip from \n part of \r\n
    // how many characters skipped?

    public LineNumberReader_Test40 () {
	super("\r\nXY");
	try {
	    System.out.println(lnr.read(new char[1], 0, 1));
	    System.out.println(lnr.skip(1));
	    System.out.println(lnr.read());
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class LineNumberReader_Test41 extends NewLNR {
    // skip \r\n

    public LineNumberReader_Test41 () {
	super("\r\nX\r\nY");
	try {
	    System.out.println(lnr.skip(2));
	    System.out.println(lnr.read());
	    System.out.println(lnr.getLineNumber());
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

public class LineNumberReaderTest {

  // LineNumberReader_Test driver
  public static void main (String[] args) {
    new LineNumberReader_Test1();
    new LineNumberReader_Test2();
    new LineNumberReader_Test3();
    new LineNumberReader_Test4();
    new LineNumberReader_Test5();
    new LineNumberReader_Test6();
    new LineNumberReader_Test7();
    new LineNumberReader_Test8();
    new LineNumberReader_Test9();
    new LineNumberReader_Test10();
    new LineNumberReader_Test11();
    new LineNumberReader_Test12();
    new LineNumberReader_Test13();
    new LineNumberReader_Test14();
    new LineNumberReader_Test15();
    new LineNumberReader_Test16();
    new LineNumberReader_Test17();
    new LineNumberReader_Test18();
    new LineNumberReader_Test19();
    new LineNumberReader_Test20();
    new LineNumberReader_Test21();
    new LineNumberReader_Test22();
    new LineNumberReader_Test23();
    new LineNumberReader_Test24();
    new LineNumberReader_Test25();
    new LineNumberReader_Test26();
    new LineNumberReader_Test27();
    new LineNumberReader_Test28();
    new LineNumberReader_Test29();
    new LineNumberReader_Test30();
    new LineNumberReader_Test31();
    new LineNumberReader_Test32();
    new LineNumberReader_Test33();
    new LineNumberReader_Test34();
    new LineNumberReader_Test35();
    new LineNumberReader_Test36();
    new LineNumberReader_Test37();
    new LineNumberReader_Test38();
    new LineNumberReader_Test39();
    new LineNumberReader_Test40();
    new LineNumberReader_Test41();
  }

}
/* Expected Output:
class LineNumberReader_Test1 failed
java.lang.NullPointerException
class LineNumberReader_Test2 failed
java.lang.IllegalArgumentException: Illegal buffer size: 0
class LineNumberReader_Test3 failed
java.lang.IllegalArgumentException: Illegal buffer size: -5
0
class LineNumberReader_Test4 succeeded
class LineNumberReader_Test5 succeeded
class LineNumberReader_Test6 failed
java.lang.IllegalArgumentException: Read-ahead limit is negative
class LineNumberReader_Test7 failed
java.lang.NullPointerException
class LineNumberReader_Test8 failed
java.lang.IndexOutOfBoundsException
class LineNumberReader_Test9 failed
java.lang.IndexOutOfBoundsException
class LineNumberReader_Test10 failed
java.lang.IndexOutOfBoundsException
class LineNumberReader_Test11 failed
java.lang.IndexOutOfBoundsException
1
1
class LineNumberReader_Test12 succeeded
0
class LineNumberReader_Test13 succeeded
-5
class LineNumberReader_Test14 succeeded
10
-1
class LineNumberReader_Test15 succeeded
10
-1
1
class LineNumberReader_Test16 succeeded
10
-1
1
class LineNumberReader_Test17 succeeded
10
10
-1
2
class LineNumberReader_Test18 succeeded
1 , 10
-1 , 10
1
class LineNumberReader_Test19 succeeded
1 , 13
-1 , 13
1
class LineNumberReader_Test20 succeeded
1 , 13
1 , 10
1
class LineNumberReader_Test21 succeeded
1 , 13
1 , 13
1 , 10
2
class LineNumberReader_Test22 succeeded
2 , 13 , 10
1
class LineNumberReader_Test23 succeeded
10
10
-1
2
class LineNumberReader_Test24 succeeded
1 , 13
1 , 13
1 , 10
2
class LineNumberReader_Test25 succeeded
10
1 , 10
10
2
class LineNumberReader_Test26 succeeded
1 , 13
10
-1 , 13
2
class LineNumberReader_Test27 succeeded
10
1 , 10
10
1 , 10
10
2
class LineNumberReader_Test28 succeeded
1 , 13
10
-1 , 0
10
-1 , 0
2
class LineNumberReader_Test29 succeeded
class LineNumberReader_Test30 failed
java.io.IOException: mark never set or invalidated
0
0
class LineNumberReader_Test31 succeeded
X
1
class LineNumberReader_Test32 succeeded

1
class LineNumberReader_Test33 succeeded


2
class LineNumberReader_Test34 succeeded
1 , 13
X
class LineNumberReader_Test35 succeeded
X
1
class LineNumberReader_Test36 succeeded
class LineNumberReader_Test37 failed
java.lang.IllegalArgumentException: skip() value is negative
0
0
class LineNumberReader_Test38 succeeded
2
88
1
class LineNumberReader_Test39 succeeded
1
1
88
1
class LineNumberReader_Test40 succeeded
2
88
1
class LineNumberReader_Test41 succeeded
*/
