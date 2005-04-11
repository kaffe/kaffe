
class NegativeDivideConst
{
    static int div2(int n)
    {
	return n / 2;
    }
    
    static int div4(int n)
    {
	return n / 4;
    }
    
    static int div8(int n)
    {
	return n / 8;
    }
    
    static int div16(int n)
    {
	return n / 16;
    }
    
    static int div32(int n)
    {
	return n / 32;
    }
    
    public static void main(String args[])
    {
	System.out.println("div2(-1) = " + div2(-1));
	System.out.println("div4(-1) = " + div4(-1));
	System.out.println("div8(-1) = " + div8(-1));
	System.out.println("div16(-1) = " + div16(-1));
	System.out.println("div32(-1) = " + div32(-1));
	System.out.println("div4(-3) = " + div4(-3));
	System.out.println("div8(-3) = " + div8(-3));
	System.out.println("div16(-3) = " + div16(-3));
	System.out.println("div32(-3) = " + div32(-3));
    }
}

/* Expected Output:
div2(-1) = 0
div4(-1) = 0
div8(-1) = 0
div16(-1) = 0
div32(-1) = 0
div4(-3) = 0
div8(-3) = 0
div16(-3) = 0
div32(-3) = 0
*/
