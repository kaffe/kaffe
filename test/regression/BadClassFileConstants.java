
import java.io.IOException;

import org.apache.bcel.classfile.Constant;
import org.apache.bcel.classfile.JavaClass;
import org.apache.bcel.classfile.ConstantCP;
import org.apache.bcel.classfile.ClassParser;
import org.apache.bcel.classfile.ConstantPool;
import org.apache.bcel.classfile.ConstantUtf8;
import org.apache.bcel.classfile.ConstantClass;
import org.apache.bcel.classfile.ConstantString;
import org.apache.bcel.classfile.ConstantNameAndType;

class BadClassFileConstants
{
    private static final String CLASS_FILE = "BadClassFileConstants.class";
    
    private static final String DAMAGED_CLASS = "DamagedClass";
    private static final String DAMAGED_CLASS_FILE = DAMAGED_CLASS + ".class";

    private static final boolean VERBOSE = true;

    private static final int ACCEPT_COUNTS[] = {
	668, /* Jikes */
	684, /* KJC */
    };
    
    private static int damage = 0;
    private static int errors = 0;
    
    private static void tryClass(JavaClass jc)
	throws IOException, ClassNotFoundException
    {
	final ClassLoader cl = ClassLoader.getSystemClassLoader();
	
	jc.dump(DAMAGED_CLASS_FILE);
	try
	{
	    cl.loadClass(DAMAGED_CLASS);
	    throw new Error("Able to load damaged class?");
	}
	catch(ClassFormatError e)
	{
	    errors += 1;
	    if( VERBOSE )
	    {
		System.out.println(e);
	    }
	}
    }

    private static int findOppositeType(Constant constants[], byte type)
    {
	int lpc;

	for( lpc = 0; lpc < constants.length; lpc++ )
	{
	    if( (constants[lpc] != null) && (constants[lpc].getTag() != type) )
		return lpc;
	}
	return -1;
    }
    
    public static void main(String args[])
	throws Throwable
    {
	boolean success = false;
	ClassParser classParser;
	Constant constants[];
	int lpc, old_index;
	ConstantPool cp;
	JavaClass jc;
	
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
	cp = jc.getConstantPool();
	constants = cp.getConstantPool();
	
	for( lpc = 0; lpc < constants.length; lpc++ )
	{
	    if( constants[lpc] instanceof ConstantClass )
	    {
		ConstantClass cc;

		cc = (ConstantClass)constants[lpc];

		old_index = cc.getNameIndex();
		/* Out of range. */
		{
		    cc.setNameIndex(cp.getLength());
		    tryClass(jc);
		    cc.setNameIndex(cp.getLength() + 1);
		    tryClass(jc);
		    cc.setNameIndex(cp.getLength() + 100);
		    tryClass(jc);
		    
		    damage += 3;
		}
		
		/* Wrong type. */
		{
		    cc.setNameIndex(findOppositeType(constants,
						     constants[old_index].
						     getTag()));
		    tryClass(jc);
		    
		    damage += 1;
		}
		cc.setNameIndex(old_index);
	    }
	    else if( constants[lpc] instanceof ConstantString )
	    {
		ConstantString cs;

		cs = (ConstantString)constants[lpc];

		old_index = cs.getStringIndex();
		/* Out of range. */
		{
		    cs.setStringIndex(cp.getLength());
		    tryClass(jc);
		    cs.setStringIndex(cp.getLength() + 1);
		    tryClass(jc);
		    cs.setStringIndex(cp.getLength() + 100);
		    tryClass(jc);
		    
		    damage += 3;
		}
		
		/* Wrong type. */
		{
		    cs.setStringIndex(findOppositeType(constants,
						       constants[old_index].
						       getTag()));
		    tryClass(jc);
		    
		    damage += 1;
		}
		cs.setStringIndex(old_index);
	    }
	    else if( constants[lpc] instanceof ConstantNameAndType )
	    {
		ConstantNameAndType cnat;

		cnat = (ConstantNameAndType)constants[lpc];

		old_index = cnat.getNameIndex();
		/* Out of range. */
		{
		    cnat.setNameIndex(cp.getLength());
		    tryClass(jc);
		    cnat.setNameIndex(cp.getLength() + 1);
		    tryClass(jc);
		    cnat.setNameIndex(cp.getLength() + 100);
		    tryClass(jc);
		    
		    damage += 3;
		}
		
		/* Wrong type. */
		{
		    cnat.setNameIndex(findOppositeType(constants,
						       constants[old_index].
						       getTag()));
		    tryClass(jc);
		    
		    damage += 1;
		}
		cnat.setNameIndex(old_index);
		
		old_index = cnat.getSignatureIndex();
		/* Out of range. */
		{
		    cnat.setSignatureIndex(cp.getLength());
		    tryClass(jc);
		    cnat.setSignatureIndex(cp.getLength() + 1);
		    tryClass(jc);
		    cnat.setSignatureIndex(cp.getLength() + 100);
		    tryClass(jc);
		    
		    damage += 3;
		}
		
		/* Wrong type. */
		{
		    cnat.setSignatureIndex(
			findOppositeType(constants,
					 constants[old_index].
					 getTag()));
		    tryClass(jc);
		    
		    damage += 1;
		}
		cnat.setSignatureIndex(old_index);
	    }
	    else if( constants[lpc] instanceof ConstantCP )
	    {
		ConstantCP ccp;

		/* FieldRef, MethodRef, InterfaceMethodRef */
		
		ccp = (ConstantCP)constants[lpc];

		old_index = ccp.getClassIndex();
		/* Out of range. */
		{
		    ccp.setClassIndex(cp.getLength());
		    tryClass(jc);
		    ccp.setClassIndex(cp.getLength() + 1);
		    tryClass(jc);
		    ccp.setClassIndex(cp.getLength() + 100);
		    tryClass(jc);
		    
		    damage += 3;
		}
		
		/* Wrong type. */
		{
		    ccp.setClassIndex(findOppositeType(constants,
						       constants[old_index].
						       getTag()));
		    tryClass(jc);
		    
		    damage += 1;
		}
		ccp.setClassIndex(old_index);

		old_index = ccp.getNameAndTypeIndex();
		/* Out of range. */
		{
		    ccp.setNameAndTypeIndex(cp.getLength());
		    tryClass(jc);
		    ccp.setNameAndTypeIndex(cp.getLength() + 1);
		    tryClass(jc);
		    ccp.setNameAndTypeIndex(cp.getLength() + 100);
		    tryClass(jc);

		    damage += 3;
		}
		
		/* Wrong type. */
		{
		    ccp.setNameAndTypeIndex(
			findOppositeType(constants,
					 constants[old_index].
					 getTag()));
		    tryClass(jc);
		    
		    damage += 1;
		}
		ccp.setNameAndTypeIndex(old_index);
	    }
	}
	if( damage == errors )
	{
	    for( lpc = 0; lpc < ACCEPT_COUNTS.length; lpc++ )
	    {
		if( damage == ACCEPT_COUNTS[lpc] )
		{
		    success = true;
		}
	    }
	}
	if( success )
	{
	    System.out.println("Success");
	}
	else
	{
	    System.out.println("Damage = " + damage);
	    System.out.println("Errors = " + errors);
	}
    }
}

/* Expected Output:
Success
*/
