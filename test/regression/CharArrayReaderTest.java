// CharArrayReader test
// submitted by Dalibor Topic

import java.io.*;

class CharArrayReaderInspector extends CharArrayReader {
    public CharArrayReaderInspector(char [] buf) {
	super(buf);
    }

    public CharArrayReaderInspector(char [] buf, int offset, int length) {
	super(buf, offset, length);
    }

    public char [] getBuf() {
	return buf;
    }

    public int getCount() {
	return count;
    }

    public int getMarkedPos() {
	return markedPos;
    }

    public int getPos() {
	return pos;
    }

    public void printState() {
	System.out.println("CharArrayReader state:");
	System.out.println("buf "       + new String(getBuf()));
	System.out.println("count "     + Integer.toString(getCount()));
	System.out.println("markedPos " + Integer.toString(getMarkedPos()));
	System.out.println("pos "       + Integer.toString(getPos()));
    }

    public void setBuf(char [] bf) {
	buf = bf;
    }

    public void setCount(int cnt) {
	count = cnt;
    }

    public void setMarkedPos(int mp) {
	markedPos = mp;
    }

    public void setPos(int p) {
	pos = p;
    }
}

class CharArrayReaderTest {

  // Print a failure message
  public static void failure(Exception e) {
    System.out.println("FAIL");
    if (e != null) {
	System.out.println(e);
	// e.printStackTrace();
    }
  }

    // Test driver
    public static void main (String[] args) {
	System.out.println("Test1");
	Test1();
	System.out.println("Test2");
	Test2();
	System.out.println("Test3");
	Test3();
	System.out.println("Test4");
	Test4();
	System.out.println("Test5");
	Test5();
	System.out.println("Test6");
	Test6();
	System.out.println("Test7");
	Test7();
	System.out.println("Test8");
	Test8();
	System.out.println("Test9");
	Test9();
	System.out.println("Test10");
	Test10();
	System.out.println("Test11");
	Test11();
	System.out.println("Test12");
	Test12();
	System.out.println("Test13");
	Test13();
	System.out.println("Test14");
	Test14();
	System.out.println("Test15");
	Test15();
	System.out.println("Test16");
	Test16();
	System.out.println("Test17");
	Test17();
	System.out.println("Test18");
	Test18();
	System.out.println("Test19");
	Test19();
	System.out.println("Test20");
	Test20();
	System.out.println("Test21");
	Test21();
	System.out.println("Test22");
	Test22();
	System.out.println("Test23");
	Test23();
	System.out.println("Test24");
	Test24();
	System.out.println("Test25");
	Test25();
	System.out.println("Test26");
	Test26();
	System.out.println("Test27");
	Test27();
	System.out.println("Test28");
	Test28();
	System.out.println("Test29");
	Test29();
	System.out.println("Test30");
	Test30();
	System.out.println("Test31");
	Test31();
    }

    public static char [] NewCharArray () {
	return NewCharArray(1);
    }

    public static char [] NewCharArray (int i) {
	char [] x = new char[i];

	char c = 'A';
	for (int j = 0; j < i; j ++) {
	  x[j] = c++;
	}

	return x;
    }

    public static CharArrayReaderInspector NewCharArrayReaderInspector() {
	return new CharArrayReaderInspector(NewCharArray());
    }

    public static char [] NewEmptyCharArray () {
	return new char[0];
    }

  // Print a success message
  public static void success() {
    System.out.println("PASS");
  }

  // Construct from null
  public static void Test1() {

    try {
      CharArrayReaderInspector cari = new CharArrayReaderInspector(null);
      success();
      cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct from null
  public static void Test2() {

    try {
      CharArrayReaderInspector cari = new CharArrayReaderInspector(null, 0, 0);
      success();
      cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct with negative offset
  public static void Test3() {

    try {
	char [] buf = NewCharArray();
	CharArrayReaderInspector cari = new CharArrayReaderInspector(buf, -5, 0);
	success();
	cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct with offset out of range
  public static void Test4() {

    try {
	char [] buf = NewCharArray();
      CharArrayReaderInspector cari = new CharArrayReaderInspector(buf, 1000, 0);
      success();
      cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct with negative length
  public static void Test5() {

    try {
	char [] buf = NewCharArray();
	CharArrayReaderInspector cari = new CharArrayReaderInspector(buf, 0, -5);
	success();
	cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct with length out of range
  public static void Test6() {

    try {
	char [] buf = NewCharArray();
      CharArrayReaderInspector cari = new CharArrayReaderInspector(buf, 0, 1000);
      success();
      cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct with empty buffer (buffer length == 0)
  public static void Test7() {

    try {
	char [] buf = NewEmptyCharArray();
      CharArrayReaderInspector cari = new CharArrayReaderInspector(buf);
      success();
      cari.printState();
    }
    catch (Exception e) {
      failure(e);
    }
  }

  // Construct a char array reader
  public static void Test8() {

    try {
	CharArrayReaderInspector cari = NewCharArrayReaderInspector();
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test value of count
  public static void Test9() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 4, 0);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test value of count after read
  public static void Test10() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	System.out.println(cari.read());
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test value of count after read
  public static void Test11() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	System.out.println(cari.read(new char [1]));
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read
  public static void Test12() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	int i;
	do {
	    System.out.println(i = cari.read());
	} while (i != -1);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read(char[], int , int)
  public static void Test13() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	int i;
	do {
	    System.out.println(i = cari.read(new char[1], 0, 1));
	} while (i != -1);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read on null
  public static void Test14() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.setBuf(null);
	int i;
	do {
	    System.out.println(i = cari.read());
	} while (i != -1);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read on bad pos
  public static void Test15() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.setPos(-5);
	int i;
	do {
	    System.out.println(i = cari.read());
	} while (i != -1);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read on bad count
  public static void Test16() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.setCount(-5);
	int i;
	do {
	    System.out.println(i = cari.read());
	} while (i != -1);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read(char [] bf, int off, int len) with bf == null
  public static void Test17() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	int i;
	do {
	    System.out.println(i = cari.read(null, 0, 1));
	} while (i != -1);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read(char [] bf, int off, int len) with off < 0
  public static void Test18() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	int i;
	do {
	    System.out.println(i = cari.read(new char[1], -5, 1));
	} while (i != -1 && i != 0);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read(char [] bf, int off, int len) with len < 0
  public static void Test19() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	int i;
	do {
	    System.out.println(i = cari.read(new char[1], 0, -5));
	} while (i != -1 && i != 0);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read(char [] bf, int off, int len) with off + len > bf.length
  public static void Test20() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	int i;
	do {
	    System.out.println(i = cari.read(new char[1], 1, 1));
	} while (i != -1 && i != 0);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test skip with negative length
  public static void Test21() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	long i;
	do {
	    System.out.println(i = cari.skip(-5));
	} while (i != -1 && i != 0);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test close
  public static void Test22() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test close after close
  public static void Test23() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	cari.close();
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test mark after close
  public static void Test24() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	cari.mark(2);
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test markSupported after close
  public static void Test25() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	System.out.println(cari.markSupported());
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read after close
  public static void Test26() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	System.out.println(cari.read());
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test read after close
  public static void Test27() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	System.out.println(cari.read(new char[1], 0, 1));
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test ready after close
  public static void Test28() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	System.out.println(cari.ready());
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test reset  after close
  public static void Test29() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	cari.reset();
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test skip after close
  public static void Test30() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(5), 0, 5);
	cari.close();
	System.out.println(cari.skip(2));
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }

  // Test ready at EOF
  public static void Test31() {

    try {
	CharArrayReaderInspector cari = new CharArrayReaderInspector(NewCharArray(1), 0, 1);
	System.out.println(cari.read());
	System.out.println(cari.ready());
	success();
	cari.printState();
    }
    catch (Exception e) {
	failure(e);
    }
  }
}
/* Expected Output:
Test1
FAIL
java.lang.NullPointerException
Test2
FAIL
java.lang.NullPointerException
Test3
FAIL
java.lang.IllegalArgumentException
Test4
FAIL
java.lang.IllegalArgumentException
Test5
FAIL
java.lang.IllegalArgumentException
Test6
PASS
CharArrayReader state:
buf A
count 1
markedPos 0
pos 0
Test7
PASS
CharArrayReader state:
buf 
count 0
markedPos 0
pos 0
Test8
PASS
CharArrayReader state:
buf A
count 1
markedPos 0
pos 0
Test9
PASS
CharArrayReader state:
buf ABCDE
count 4
markedPos 4
pos 4
Test10
65
PASS
CharArrayReader state:
buf ABCDE
count 5
markedPos 0
pos 1
Test11
1
PASS
CharArrayReader state:
buf ABCDE
count 5
markedPos 0
pos 1
Test12
65
66
67
68
69
-1
PASS
CharArrayReader state:
buf ABCDE
count 5
markedPos 0
pos 5
Test13
1
1
1
1
1
-1
PASS
CharArrayReader state:
buf ABCDE
count 5
markedPos 0
pos 5
Test14
FAIL
java.io.IOException: Stream closed
Test15
FAIL
java.lang.ArrayIndexOutOfBoundsException
Test16
-1
PASS
CharArrayReader state:
buf ABCDE
count -5
markedPos 0
pos 0
Test17
FAIL
java.lang.NullPointerException
Test18
FAIL
java.lang.IndexOutOfBoundsException
Test19
FAIL
java.lang.IndexOutOfBoundsException
Test20
FAIL
java.lang.IndexOutOfBoundsException
Test21
0
PASS
CharArrayReader state:
buf ABCDE
count 5
markedPos 0
pos 0
Test22
PASS
CharArrayReader state:
FAIL
java.lang.NullPointerException
Test23
PASS
CharArrayReader state:
FAIL
java.lang.NullPointerException
Test24
FAIL
java.io.IOException: Stream closed
Test25
true
PASS
CharArrayReader state:
FAIL
java.lang.NullPointerException
Test26
FAIL
java.io.IOException: Stream closed
Test27
FAIL
java.io.IOException: Stream closed
Test28
FAIL
java.io.IOException: Stream closed
Test29
FAIL
java.io.IOException: Stream closed
Test30
FAIL
java.io.IOException: Stream closed
Test31
65
false
PASS
CharArrayReader state:
buf A
count 1
markedPos 0
pos 1
*/





