// Copyright (c) 2000  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;


/* Represents the contents of a standard "ConstantValue" attribute.
 * @author      Per Bothner
 */

public class ConstantValueAttr extends Attribute
{
  Object value;
  int value_index;

  public Object getValue(ConstantPool cpool)
  {
    if (value != null)
      return value;
    CpoolEntry entry = cpool.getPoolEntry(value_index);
    switch (entry.getTag())
      {
      case ConstantPool.STRING:
	value = ((CpoolString) entry).getString().getString();
	break;
      case ConstantPool.INTEGER:
	value = new Integer(((CpoolValue1) entry).value);
	break;
      case ConstantPool.LONG:
	value = new Long(((CpoolValue2) entry).value);
	break;
      case ConstantPool.FLOAT:
	float f = Float.intBitsToFloat(((CpoolValue1) entry).value);
	value = new Float(f);
	break;
      case ConstantPool.DOUBLE:
	double d = Double.longBitsToDouble(((CpoolValue2) entry).value);
	value = new Double(d);
	break;
      }
    return value;
  }

  public ConstantValueAttr (Object value)
  {
    super("ConstantValue");
    this.value = value;
  }

  public ConstantValueAttr (int index)
  { 
    super("ConstantValue");
    this.value_index = index;
  } 

  public void assignConstants (ClassType cl)
  {
    super.assignConstants(cl);
    if (value_index == 0)
      {
	ConstantPool cpool = cl.getConstants();
	CpoolEntry entry = null;
	if (value instanceof String)
	  entry = cpool.addString((String) value);
	else if (value instanceof Integer)
	  entry = cpool.addInt(((Integer) value).intValue());
	else if (value instanceof Long)
	  entry = cpool.addLong(((Long) value).longValue());
	else if (value instanceof Float)
	  entry = cpool.addFloat(((Float) value).floatValue());
	else if (value instanceof Long)
	  entry = cpool.addDouble(((Double) value).doubleValue());
	value_index = entry.getIndex();
      }
  }

  public final int getLength() { return 2; }

  public void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeShort(value_index);
  }

  public void print (ClassTypeWriter dst)
  {
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", value: ");
    if (value_index == 0)
      {
	Object value = getValue(dst.ctype.constants);
	if (value instanceof String)
	  dst.printQuotedString((String) value);
	else
	  dst.print(value);
      }
    else
      {
	if (dst.printConstants)
	  {
	    dst.print(value_index);
	    dst.print('=');
	  }
	CpoolEntry entry = dst.ctype.constants.getPoolEntry(value_index);
	entry.print(dst, 1);
      }
    dst.println();
  }
}
