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
