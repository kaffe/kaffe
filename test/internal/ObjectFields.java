
class ObjectFields
{
    public static final byte test_1__byte_get_ref = 1;
    
    public static byte byte_get_ref(byte b)
    {
	ObjectFields of = new ObjectFields();

	of.b = b;
	return of.b;
    }
    
    public static final char test_1__char_get_ref = '1';
    
    public static char char_get_ref(char c)
    {
	ObjectFields of = new ObjectFields();

	of.c = c;
	return of.c;
    }
    
    public static final short test_1__short_get_ref = 1;
    
    public static short short_get_ref(short s)
    {
	ObjectFields of = new ObjectFields();

	of.s = s;
	return of.s;
    }
    
    public static final int test_1__int_get_ref = 1;
    
    public static int int_get_ref(int i)
    {
	ObjectFields of = new ObjectFields();

	of.i = i;
	return of.i;
    }
    
    public static final float test_1__float_get_ref = 1.0F;
    
    public static float float_get_ref(float f)
    {
	ObjectFields of = new ObjectFields();

	of.f = f;
	return of.f;
    }
    
    public static final double test_1__double_get_ref = 1.0;
    
    public static double double_get_ref(double d)
    {
	ObjectFields of = new ObjectFields();

	of.d = d;
	return of.d;
    }
    
    private byte b;
    private char c;
    private short s;
    private int i;
    private float f;
    private double d;
    
    private ObjectFields()
    {
    }
}
