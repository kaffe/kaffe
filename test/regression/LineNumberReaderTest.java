// LineNumberReader tests
// submitted by Dalibor Topic

import java.io.*;

class Test {

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

class Test1 extends Test {
  // Construct from null
  public Test1() {
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

class Test2 extends Test {

  // Construct with size 0
  public Test2() {
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

class Test3 extends Test {

  // Construct with negative size
  public Test3() {
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

class NewLNR extends Test {

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

class Test4 extends NewLNR {

  // Initial line number
  public Test4() {
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

class Test5 extends NewLNR {

  // read ahead limit = 0
  public Test5() {
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

class Test6 extends NewLNR {

  // read ahead limit < 0
  public Test6() {
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

class Test7 extends NewLNR {

  // read into non-existent character array
  public Test7() {
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

class Test8 extends NewLNR {

  // read past end of character array
  public Test8() {
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

class Test9 extends NewLNR {

  // read past end of character array
  public Test9() {
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

class Test10 extends NewLNR {

  // read at negative offset
  public Test10() {
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

class Test11 extends NewLNR {

  // read negative amount of characters
  public Test11() {

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

class Test12 extends NewLNR {

  // read correct amount of characters
  public Test12() {

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

class Test13 extends NewLNR {

  // set line number to 0
  public Test13() {
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

class Test14 extends NewLNR {

  // set line number to < 0
  public Test14() {
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

class Test15 extends NewLNR {

  // read \n

  public Test15() {
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

class Test16 extends NewLNR {

    // read \r (compression test)

  public Test16() {
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

class Test17 extends NewLNR {

  // read \r\n (compression test)

  public Test17() {
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

class Test18 extends NewLNR {

  // read \r\r\n (compression test)

  public Test18() {
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

class Test19 extends NewLNR {

  // read \n

  public Test19() {
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

class Test20 extends NewLNR {

  // read \r (compression test)

  public Test20() {
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

class Test21 extends NewLNR {

  // read \r\n (compression test)

  public Test21() {
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

class Test22 extends NewLNR {

  // read \r\r\n (compression test)

  public Test22() {
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

class Test23 extends NewLNR {

  // read \r\r\n (compression test)
  // read(,,) *doesn't* compress characters

  public Test23() {
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

class Test24 extends NewLNR {

  // read \r\r\n (compression test)

  public Test24() {
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

class Test25 extends NewLNR {

  // read \r\r\n (compression test)

  public Test25() {
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

class Test26 extends NewLNR {

  // read \r\n\r (compression test)
  // with interleaving read() and read (,,)
  public Test26() {
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

class Test27 extends NewLNR {

  // read \r\n\r (compression test)
  // with interleaving read() and read (,,)

  public Test27() {
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

class Test28 extends NewLNR {

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

  public Test28() {
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

class Test29 extends NewLNR {

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

  public Test29() {
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

class Test30 extends NewLNR {

    // reset without a mark
    public Test30 () {
	try {
	    lnr.reset();
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class Test31 extends NewLNR {

    // getLineNumber, mark, setLineNumber, reset, and getLineNumber.
    // is it the line number before the mark was set, or the set line number?
    // the spec is not clear on this one.

    public Test31 () {
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

class Test32 extends NewLNR {

    // readLine() after end of buffer
    // what's the line number ?

    // It should be one for the default NewLNR.

    public Test32 () {
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

class Test33 extends NewLNR {

  // read \r\n (compression test)

  public Test33() {
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

class Test34 extends NewLNR {

  // read \r\r\n (compression test)

  public Test34() {
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

class Test35 extends NewLNR {

    // read \r\nX\n
    // readLine should return "X"

    public Test35 () {
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

class Test36 extends NewLNR {

    // readLine() gets EOF before a line terminator
    // what's the line number ?
    public Test36 () {
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

class Test37 extends NewLNR {

    // negative skip

    public Test37 () {
	try {
	    lnr.skip(-5);
	    success(this.getClass());
	}
	catch (Exception e) {
	    failure(this.getClass(), e);
	}
    }
}

class Test38 extends NewLNR {

    // skip at EOF
    // what does it return? Spec says number of characters read.
    // so it should be 0.

    public Test38 () {
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

class Test39 extends NewLNR {

    // skip: as how many characters does \r\n count?

    public Test39 () {
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

class Test40 extends NewLNR {

    // skip from \n part of \r\n
    // how many characters skipped?

    public Test40 () {
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

class Test41 extends NewLNR {
    // skip \r\n

    public Test41 () {
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

  // Test driver
  public static void main (String[] args) {
    new Test1();
    new Test2();
    new Test3();
    new Test4();
    new Test5();
    new Test6();
    new Test7();
    new Test8();
    new Test9();
    new Test10();
    new Test11();
    new Test12();
    new Test13();
    new Test14();
    new Test15();
    new Test16();
    new Test17();
    new Test18();
    new Test19();
    new Test20();
    new Test21();
    new Test22();
    new Test23();
    new Test24();
    new Test25();
    new Test26();
    new Test27();
    new Test28();
    new Test29();
    new Test30();
    new Test31();
    new Test32();
    new Test33();
    new Test34();
    new Test35();
    new Test36();
    new Test37();
    new Test38();
    new Test39();
    new Test40();
    new Test41();
  }

}
// Results from IBM JRE 1.3
/* Expected Output:
class Test1 failed
java.lang.NullPointerException
class Test2 failed
java.lang.IllegalArgumentException: Buffer size <= 0
class Test3 failed
java.lang.IllegalArgumentException: Buffer size <= 0
0
class Test4 succeeded
class Test5 succeeded
class Test6 failed
java.lang.IllegalArgumentException: Read-ahead limit < 0
class Test7 failed
java.lang.NullPointerException
class Test8 failed
java.lang.IndexOutOfBoundsException
class Test9 failed
java.lang.IndexOutOfBoundsException
class Test10 failed
java.lang.IndexOutOfBoundsException
class Test11 failed
java.lang.IndexOutOfBoundsException
1
1
class Test12 succeeded
0
class Test13 succeeded
-5
class Test14 succeeded
10
-1
class Test15 succeeded
10
-1
1
class Test16 succeeded
10
-1
1
class Test17 succeeded
10
10
-1
2
class Test18 succeeded
1 , 10
-1 , 10
1
class Test19 succeeded
1 , 13
-1 , 13
1
class Test20 succeeded
1 , 13
1 , 10
1
class Test21 succeeded
1 , 13
1 , 13
1 , 10
2
class Test22 succeeded
2 , 13 , 10
1
class Test23 succeeded
10
10
-1
2
class Test24 succeeded
1 , 13
1 , 13
1 , 10
2
class Test25 succeeded
10
1 , 10
10
2
class Test26 succeeded
1 , 13
10
-1 , 13
2
class Test27 succeeded
10
1 , 10
10
1 , 10
10
2
class Test28 succeeded
1 , 13
10
-1 , 0
10
-1 , 0
2
class Test29 succeeded
class Test30 failed
java.io.IOException: Stream not marked
0
0
class Test31 succeeded
X
1
class Test32 succeeded

1
class Test33 succeeded


2
class Test34 succeeded
1 , 13
X
class Test35 succeeded
X
1
class Test36 succeeded
class Test37 failed
java.lang.IllegalArgumentException: skip() value is negative
0
0
class Test38 succeeded
2
88
1
class Test39 succeeded
1
1
88
1
class Test40 succeeded
2
88
1
class Test41 succeeded
*/
