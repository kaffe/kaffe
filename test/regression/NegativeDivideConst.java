
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
	return n / 4;
    }
    
    static int div16(int n)
    {
	return n / 4;
    }
    
    static int div32(int n)
    {
	return n / 4;
    }
    
    public static void main(String args[])
    {
	System.out.println("div2 = " + div2(-1));
	System.out.println("div4 = " + div4(-1));
	System.out.println("div8 = " + div8(-1));
	System.out.println("div16 = " + div16(-1));
	System.out.println("div32 = " + div32(-1));
    }
}

/* Expected Output:
div2 = 0
div4 = 0
div8 = 0
div16 = 0
div32 = 0
*/
