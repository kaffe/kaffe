// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/** A CONSTANT_Float entry in the constant pool. */

public class CpoolFloat extends CpoolEntry
{
  float value;
  CpoolFloat (ConstantPool cpool, int h, float val)
  {
    super(cpool, h);
    value = val;
    ++cpool.count;
  }

  public int getTag() { return 4; }  // CONSTANT_Float

  final static int hashCode (float val) { return (int) val; }

  void write (DataOutputStream dstr) throws java.io.IOException {
	dstr.writeByte (4);  // CONSTANT_Float
        dstr.writeFloat (value);
    }

  public void print (ClassTypeWriter dst, int verbosity)
  {
    if (verbosity > 0)
      dst.print("Float ");
    dst.print(value);
    if (verbosity > 1)
      {
	dst.print("=0x");
	dst.print(Integer.toHexString(Float.floatToIntBits(value)));
      }
  }
}
