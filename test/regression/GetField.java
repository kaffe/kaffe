
import java.lang.reflect.Field;

class SomeFields {
	public int SOME = 10;
	protected int PSOME = 20;
}

class GetField {
	public int TEST = 10;
	public static int STEST = 20;
	private static int PSTEST = 30;
	public static void main(String args[]) {
		Field fld;
		Class cls = GetField.class;
		try {
			fld = cls.getField("TEST");
			System.out.println("public: "+fld.getInt(cls));
		} catch (Exception e) {
			System.out.println(e);
		}

		try {
			fld = cls.getField("STEST");
			System.out.println("public static: "+fld.getInt(cls));
		} catch (Exception e) {
			System.out.println(e);
		}

		try {
			fld = cls.getField("PSTEST");
			System.out.println("private: "+fld.getInt(cls));
		} catch (Exception e) {
			System.out.println(e);
		}

		try {
			fld = cls.getField("TEST");
			System.out.println("public from null: "+fld.getInt(null));
		} catch (Exception e) {
			System.out.println(e);
		}

		try {
			fld = cls.getField("STEST");
			System.out.println("public static from null: "+fld.getInt(null));
		} catch (Exception e) {
			System.out.println(e);
		}

		SomeFields sFields = new SomeFields();
		cls = SomeFields.class;
		try {
			fld = cls.getField("SOME");
			System.out.println("SomeFields public: "+fld.getInt(sFields));
		} catch (Exception e) {
			System.out.println(e);
		}

		try {
			fld = cls.getField("PSOME");
			System.out.println("SomeFields protected: "+fld.getInt(sFields));
		} catch (Exception e) {
			System.out.println(e);
		}
	}
}

/* Expected Output:
java.lang.IllegalArgumentException
public static: 20
java.lang.NoSuchFieldException: PSTEST
java.lang.NullPointerException
public static from null: 20
SomeFields public: 10
java.lang.NoSuchFieldException: PSOME
*/
