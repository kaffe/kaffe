// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

public class CpoolUtf8 extends CpoolEntry
{
  String string;

  CpoolUtf8 () { }

  CpoolUtf8 (ConstantPool cpool, int h, String s)
  {
    super (cpool, h);
    string = s;
  }

  public int hashCode ()
  {
    if (hash == 0)
      hash = string.hashCode();
    return hash;
  }

  public final void intern () { string = string.intern(); }

  public int getTag() { return 1; } // CONSTANT_CUtf8

  public final String getString()
  {
    return string;
  }

  void write (DataOutputStream dstr) throws java.io.IOException
  {
	dstr.writeByte (1);  // CONSTANT_Utf8
	dstr.writeUTF (string);
  }

  public void print (ClassTypeWriter dst, int verbosity)
  {
    if (verbosity > 0)
      dst.print("Utf8: ");
    dst.printQuotedString(string);
  }
};
