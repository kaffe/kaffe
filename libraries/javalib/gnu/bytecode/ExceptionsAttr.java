package gnu.bytecode;
import java.io.*;

/**
  * Represents the contents of a standard "Exceptions" attribute.
  * @author      Geoff Berry
  */

public class ExceptionsAttr extends Attribute
{
  // The exception types.
  ClassType[] exceptions;
  // The exception table.
  short[] exception_table;

  /** Add a new ExceptionsAttr to a Method. */
  public ExceptionsAttr(Method meth)
  {
    super("Exceptions");
    addToFrontOf(meth);
  }

  /** Set the Exceptions attribute to refer to classes whose indices
      in the constant pool of `cl' are given by `indices'. */
  public void setExceptions (short[] indices, ClassType cl)
  {
    exception_table = indices;
    exceptions = new ClassType[indices.length];
    ConstantPool cp = cl.getConstants ();
    for (int i = indices.length - 1; i >= 0; -- i)
      exceptions[i] =
	(ClassType)((CpoolClass)cp.getPoolEntry(indices[i])).getClassType ();
  }

  /** Set the Exceptions attribute to refer to the given exception types.
    * @param excep_types the types of the exceptions. */
  public void setExceptions (ClassType[] excep_types)
  {
    exceptions = excep_types;
  }

  public void assignConstants (ClassType cl)
  {
    super.assignConstants(cl);
    ConstantPool cp = cl.getConstants();
    int count = exceptions.length;
    exception_table = new short[ count ];
    for (int i = count - 1; i >= 0; --i)
      {
	exception_table[i] = (short)cp.addClass(exceptions[i]).index;
      }
  }

  /** The size of this Attribute (in bytes) is 2 (for
      number_of_exception) plus 2 * number_of_exceptions. */
  public final int getLength()
  {
    return 2 + 2 * (exceptions == null ? 0 : exceptions.length);
  }

  /** The types of the exceptions in this attr. */
  public final ClassType[] getExceptions()
  {
    return exceptions;
  }

  public void write (DataOutputStream dstr) throws java.io.IOException
  {
    int count = exceptions.length;
    dstr.writeShort(count);
    for (int i = 0;  i < count;  i++)
      {
	dstr.writeShort(exception_table[i]);
      }
  }

  public void print (ClassTypeWriter dst)
  {
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", count: ");
    int count = exceptions.length;
    dst.println(count);
    for (int i = 0;  i < count;  i++)
      {
	int catch_type_index = exception_table[i] & 0xffff;
	dst.print("  ");
	dst.printOptionalIndex(catch_type_index);
	dst.printConstantTersely(catch_type_index, ConstantPool.CLASS);
	dst.println();
      }
  }
}
