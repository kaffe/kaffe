
class ObjectFields
{
    public static final boolean test_true__boolean_get_ref = true;
    
    public static final boolean boolean_get_ref(boolean z)
    {
	ObjectFields of = new ObjectFields(z);

	return of.z;
    }
    
    public static final byte test_1__byte_get_ref = 1;
    
    public static byte byte_get_ref(byte b)
    {
	ObjectFields of = new ObjectFields(b);

	return of.b;
    }
    
    public static final char test_1__char_get_ref = '1';
    
    public static char char_get_ref(char c)
    {
	ObjectFields of = new ObjectFields(c);

	return of.c;
    }
    
    public static final short test_1__short_get_ref = 1;
    
    public static short short_get_ref(short s)
    {
	ObjectFields of = new ObjectFields(s);

	return of.s;
    }
    
    public static final int test_1__int_get_ref = 1;
    
    public static int int_get_ref(int i)
    {
	ObjectFields of = new ObjectFields(i);

	return of.i;
    }
    
    public static final float test_1__float_get_ref = 1.0F;
    
    public static float float_get_ref(float f)
    {
	ObjectFields of = new ObjectFields(f);

	return of.f;
    }
    
    public static final double test_1__double_get_ref = 1.0;
    
    public static double double_get_ref(double d)
    {
	ObjectFields of = new ObjectFields(d);

	return of.d;
    }

    public static final int test_1__object_get_ref = 1;
    
    public static int object_get_ref(int i)
    {
	ObjectFields of = new ObjectFields(new ObjectFields(i));

	return of.o.i;
    }

    public static final int test__six_ints = 6;
    
    public static int six_ints()
    {
	ObjectFields of = new ObjectFields(1, 2, 3, 4, 5, 6);
	
	return of.i6;
    }
    
    private boolean z;
    private byte b;
    private char c;
    private short s;
    private int i;
    private float f;
    private double d;
    private ObjectFields o;
	
    private int i1;
    private int i2;
    private int i3;
    private int i4;
    private int i5;
    private int i6;

    private ObjectFields(boolean z)
    {
	this.z = z;
    }
    
    private ObjectFields(byte b)
    {
	this.b = b;
    }
    
    private ObjectFields(char c)
    {
	this.c = c;
    }
    
    private ObjectFields(short s)
    {
	this.s = s;
    }
    
    private ObjectFields(int i)
    {
	this.i = i;
    }
    
    private ObjectFields(float f)
    {
	this.f = f;
    }
    
    private ObjectFields(double d)
    {
	this.d = d;
    }
    
    private ObjectFields(ObjectFields o)
    {
	this.o = o;
    }

    private ObjectFields(int i1, int i2, int i3, int i4, int i5, int i6)
    {
	this.i1 = i1;
	this.i2 = i2;
	this.i3 = i3;
	this.i4 = i4;
	this.i5 = i5;
	this.i6 = i6;
    }
}
