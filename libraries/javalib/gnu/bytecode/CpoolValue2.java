// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/** A CONSTANT_Long or CONSTANT_Double entry in the constant pool. */

public class CpoolValue2 extends CpoolEntry
{
  int tag;
  long value;

  CpoolValue2 (int tag) { this.tag = tag; }

  CpoolValue2 (ConstantPool cpool, int tag, int hash, long value)
  {
    super (cpool, hash);
    this.tag = tag;
    this.value = value;
    cpool.count++;  /* Long and Double take an extra cpool slot. */
  }

  public int getTag() { return tag; }

  public final long getValue()
  {
    return value;
  }

  static int hashCode (long val) { return (int) val; }

  public int hashCode ()
  {
    if (hash == 0)
      hash = hashCode(value);
    return hash;
  }

  void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeByte (tag);
    dstr.writeLong (value);
  }

  public void print (ClassTypeWriter dst, int verbosity)
  {
    if (tag == ConstantPool.LONG)
      {
	if (verbosity > 0)
	  dst.print("Long ");
	dst.print(value);
	if (verbosity > 1 && value != 0)
	  {
	    dst.print("=0x");
	    dst.print(Long.toHexString(value));
	  }
      }
    else  // tag == ConstantPool.DOUBLE
      {
	if (verbosity > 0)
	  dst.print("Double ");
	dst.print(Double.longBitsToDouble(value));
	if (verbosity > 1)
	  {
	    dst.print("=0x");
	    dst.print(Long.toHexString(value));
	  }
      }
  }
}
