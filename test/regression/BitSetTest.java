/**
 * A test for java.util.BitSet
 *
 * Written by Edouard Parmelan <Edouard.Parmelan@France.NCR.COM>
 */
import java.util.BitSet;

public class BitSetTest {

    private static void show(String name, BitSet b) {
	System.out.println(name + " = " + b + "\tsize " + b.size() + "\tlength " + b.length());
	//System.out.println(name + " = " + b + "\tsize " + b.size());
    }


    private static void equalTest(String name, BitSet a, BitSet b) {
	System.out.println(name + " " + a.equals(b) + " == " + b.equals(a));
    }

    private static void hashCodeTest() {
	BitSet a = new BitSet(64);
	BitSet b = new BitSet(128);

	a.set(1);
	a.set(13);

	b.set(1);
	b.set(13);

	show("a", a);
	show("b", b);
	equalTest("a == b", a, b);
	System.out.println("hashCode()"
		+ " a " + Integer.toString(a.hashCode(), 16)
		+ " b " + Integer.toString(b.hashCode(), 16)
		);
	System.out.println();
    }

    public static void main(String args[]) {

	hashCodeTest();

	BitSet a = new BitSet(0);
	BitSet b = new BitSet(0);
	BitSet c = new BitSet(0);
	BitSet d = null;

	a.set(0);
	a.set(1);
	a.set(66);
	show("a", a);
	show("a.clone()", (BitSet)a.clone());

	b.set(1);
	b.set(2);
	b.set(63);
	show("b", b);
	show("b.clone()", (BitSet)b.clone());
	System.out.println();

	show("c", c);
	equalTest("a == b", a, b);
	System.out.println();

	d = (BitSet)a.clone();
	d.or(b);
	show("a or b", d);
	show("clone()", (BitSet)d.clone());
	c = new BitSet(0); c.or(d); show("{} or d", c);
	equalTest("d == d.clone()", d, (BitSet)d.clone());
	System.out.println();

	d = (BitSet)a.clone();
	d.and(b);
	show("a and b", d);
	show("clone()", (BitSet)d.clone());
	c = new BitSet(0); c.or(d); show("{} or d", c);
	equalTest("d == d.clone()", d, (BitSet)d.clone());
	System.out.println();

	d = (BitSet)a.clone();
	d.xor(b);
	show("a xor b", d);
	show("clone()", (BitSet)d.clone());
	c = new BitSet(0); c.or(d); show("{} or d", c);
	equalTest("d == d.clone()", d, (BitSet)d.clone());
	System.out.println();

	d = (BitSet)a.clone();
	d.andNot(b);
	show("a andNot b = ", d);
	show("clone()", (BitSet)d.clone());
	c = new BitSet(0); c.or(d); show("{} or d", c);
	equalTest("d == d.clone()", d, (BitSet)d.clone());
	System.out.println();
    }
}

/* Expected Output:
a = {1, 13}	size 64	length 14
b = {1, 13}	size 128	length 14
a == b true == true
hashCode() a 24d0 b 24d0

a = {0, 1, 66}	size 128	length 67
a.clone() = {0, 1, 66}	size 128	length 67
b = {1, 2, 63}	size 64	length 64
b.clone() = {1, 2, 63}	size 64	length 64

c = {}	size 0	length 0
a == b false == false

a or b = {0, 1, 2, 63, 66}	size 128	length 67
clone() = {0, 1, 2, 63, 66}	size 128	length 67
{} or d = {0, 1, 2, 63, 66}	size 128	length 67
d == d.clone() true == true

a and b = {1}	size 128	length 2
clone() = {1}	size 128	length 2
{} or d = {1}	size 64	length 2
d == d.clone() true == true

a xor b = {0, 2, 63, 66}	size 128	length 67
clone() = {0, 2, 63, 66}	size 128	length 67
{} or d = {0, 2, 63, 66}	size 128	length 67
d == d.clone() true == true

a andNot b =  = {0, 66}	size 128	length 67
clone() = {0, 66}	size 128	length 67
{} or d = {0, 66}	size 128	length 67
d == d.clone() true == true

*/
