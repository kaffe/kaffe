
final class MethodOptimizations
{
    private int value = 0;
    
    int getValue()
    {
	return this.value;
    }

    public static final boolean test__extraFakeCalls = true;

    public static boolean extraFakeCalls()
    {
	MethodOptimizations mo = new MethodOptimizations();

	mo.getValue();
	mo.getValue();
	return true;
    }
}
