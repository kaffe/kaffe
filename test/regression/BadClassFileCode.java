
import java.io.IOException;

import org.apache.bcel.classfile.Code;
import org.apache.bcel.classfile.Method;
import org.apache.bcel.classfile.JavaClass;
import org.apache.bcel.classfile.ClassParser;
import org.apache.bcel.classfile.ConstantUtf8;
import org.apache.bcel.classfile.ConstantClass;

class BadClassFileCode
{
    private static final String CLASS_FILE = "BadClassFileCode.class";
    
    private static final String DAMAGED_CLASS = "DamagedClass";
    private static final String DAMAGED_CLASS_FILE = DAMAGED_CLASS + ".class";

    private static final boolean VERBOSE = false;
    
    private static int damage = 0;
    private static int errors = 0;
    
    private static void tryClass(JavaClass jc)
	throws IOException, ClassNotFoundException
    {
	final ClassLoader cl = ClassLoader.getSystemClassLoader();
	
	jc.dump(DAMAGED_CLASS_FILE);
	try
	{
	    Class c = cl.loadClass(DAMAGED_CLASS);

	    c.newInstance();
	    throw new Error("Able to load damaged class?");
	}
	catch(VerifyError e)
	{
	    errors += 1;
	    if( VERBOSE )
	    {
		System.out.println(e);
	    }
	}
	catch(ClassFormatError e)
	{
	    errors += 1;
	    if( VERBOSE )
	    {
		System.out.println(e);
	    }
	}
	catch(Throwable th)
	{
	    th.printStackTrace();
	}
    }

    public static void main(String args[])
	throws Throwable
    {
	Method init = null, methods[];
	ClassParser classParser;
	byte bytecode[];
	byte old_code;
	JavaClass jc;
	Code code;
	int lpc;
	
	classParser = new ClassParser(CLASS_FILE);
	jc = classParser.parse();
	{
		ConstantClass cc;
		
		jc.setClassName(DAMAGED_CLASS);
		cc = ((ConstantClass)jc.getConstantPool().
		      getConstant(jc.getClassNameIndex()));
		((ConstantUtf8)jc.getConstantPool().
		 getConstant(cc.getNameIndex())).setBytes(DAMAGED_CLASS);
	}

	methods = jc.getMethods();
	for( lpc = 0; lpc < methods.length; lpc++ )
	{
	    if( methods[lpc].getName().equals("<init>") )
		init = methods[lpc];
	}
	
	code = init.getCode();
	bytecode = code.getCode();

	/*
	 * Order matters here, once we damage the method we will not be able
	 * to load the class again.
	 */
	
	if( VERBOSE )
	{
	    System.out.println("Empty code");
	}
	code.setCode(new byte[0]);
	tryClass(jc);
	
	damage += 1;
	
	if( VERBOSE )
	{
	    System.out.println("Code too large");
	}
	code.setCode(new byte[65536 * 2]);
	tryClass(jc);
	
	damage += 1;
	
	if( VERBOSE )
	{
	    System.out.println("Damaging byte code");
	}
	old_code = bytecode[0];
	bytecode[0] = (byte)225;
	code.setCode(bytecode);
	tryClass(jc);
	bytecode[0] = old_code;
	
	damage += 1;

	System.out.println("Damage = " + damage);
	System.out.println("Errors = " + errors);
    }
}

/* Expected Output:
Damage = 3
Errors = 3
*/
