import java.text.MessageFormat;
import java.text.ChoiceFormat;
import java.lang.String;
import java.util.Locale;

public class TestMessageFormat {

    public static void test1() {
	MessageFormat fmt = new MessageFormat("{0,number}");
	Object[] data;

	data = new Object[] { new Integer(1120445093) };
	System.out.println(fmt.format(data));
	data = new Object[] { new Double(12.4) };
	System.out.println(fmt.format(data));
    }
    
    public static void test2() {
	MessageFormat fmt = new MessageFormat("This is {0}");
	double[] limits = {0,1,2};
	String[] string_choices = {"bad","great","unknown ({0,number})"};
	ChoiceFormat choices = new ChoiceFormat(limits, string_choices);
	Object[] data;

	fmt.setFormat(0, choices);

	data = new Object[] { new Integer(1) };
	System.out.println(fmt.format(data));
	data = new Object[] { new Integer(0) };
	System.out.println(fmt.format(data));
	data = new Object[] { new Integer(10) };
	System.out.println(fmt.format(data));
    }

    public static void test3() {
	MessageFormat fmt = new MessageFormat("{0,number} {1,number,##.#}");
	Object[] data;

	data = new Object[] { new Integer(130385093), new Double(12.4) };
	System.out.println(fmt.format(data));
	data = new Object[] { new Integer(0), new Double(100) };
	System.out.println(fmt.format(data));
    }

    public static void test4() {
        MessageFormat fmt = new MessageFormat("{0,number,#.#E00}");
	Object[] data;

	data = new Object[] { new Integer(130385093) };
	System.out.println(fmt.format(data));
	data = new Object[] { new Double(12.4) };
	System.out.println(fmt.format(data));
    }

    public static void main(String args[]) {
	Locale.setDefault(Locale.US);

	test1();
	test2();
	test3();
	test4();
    }
}
/* Expected output:
1,120,445,093
12.4
This is great
This is bad
This is unknown (10)
130,385,093 12.4
0 100
1.3E08
1.2E01
*/
