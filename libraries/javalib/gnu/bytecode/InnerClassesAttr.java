// Copyright (c) 1998  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
public class InnerClassesAttr  extends Attribute
{
  short count;
  short[] data;

  /** Add a new InnerClassesAttr to a ClassType. */
  public InnerClassesAttr (ClassType cl)
  {
    super("InnerClasses");
    addToFrontOf(cl);
  }

  public InnerClassesAttr (short[] data, ClassType cl)
  {
    this(cl);
    count = (short) (data.length >> 2);
    this.data = data;
  }

  /** Return the length of the attribute in bytes.
    * Does not include the 6-byte header (for the name_index and the length).*/
    public int getLength() { return 2 + 8 * count; }

  /** Write out the contents of the Attribute.
    * Does not write the 6-byte attribute header. */
  public void write (java.io.DataOutputStream dstr)
    throws java.io.IOException
  {
    dstr.writeShort(count);
    for (int i = 0;  i < count;  i++)
      {
	dstr.writeShort(data[4 * i]);     // inner_class_info_index
	dstr.writeShort(data[4 * i + 1]); // outer_class_info_index
	dstr.writeShort(data[4 * i + 2]); // inner_name_index
	dstr.writeShort(data[4 * i + 3]); // inner_class_access_flags
      }
  }

  public void print (ClassTypeWriter dst) 
  {
    ClassType ctype = (ClassType) container;
    ConstantPool constants = ctype.getConstants();
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", count: ");
    dst.println(count);
    for (int i = 0;  i < count;  i++)
      {
	dst.print("  ");
	int index;
	index = data[4*i] & 0xFFFF; // inner_class_info_index
	CpoolEntry centry = constants.getForced(index, ConstantPool.CLASS);
	dst.print(((CpoolClass) centry).getStringName());
	dst.print("; ");

	index = data[4*i+1] & 0xFFFF; // outer_class_info_index
	if (index != 0)
	  {
	    dst.print("Member of ");
	    centry = constants.getForced(index, ConstantPool.CLASS);
	    dst.print(((CpoolClass) centry).getStringName());
	  }
	dst.print("; ");

	index = data[4*i+2] & 0xFFFF; // inner_name_index
	if (index != 0)
	  {
	    centry = constants.getForced(index, ConstantPool.UTF8);
	    dst.print(((CpoolUtf8) centry).string);
	  }
	else
	  dst.print("(Anonymous)");
	dst.print(";");

	dst.print(Access.toString(data[4*i+3] & 0xFFFF, 'C'));
	dst.println();
      }
  }
}
