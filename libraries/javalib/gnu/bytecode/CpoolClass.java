// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/** A CONSTANT_Class entry in the constant pool. */

public class CpoolClass extends CpoolEntry {
  CpoolUtf8 name;

  CpoolClass () { }

  CpoolClass (ConstantPool cpool, int hash, CpoolUtf8 n)
  {
    super (cpool, hash);
    name = n;
  }

  public int getTag() { return ConstantPool.CLASS; }

  public final CpoolUtf8 getName()
  {
    return name;
  }

  /** Get name of the class as a String. */
  public final String getStringName()
  {
    return name.string;
  }

  /** Get corresponding ObjectType (ClassType or ArrayType). */
  public final ObjectType getClassType ()
  {
    String name = this.name.string;
    if (name.charAt (0) == '[')
      return (ObjectType)Type.signatureToType (name);
    else
      return ClassType.make (name.replace ('/', '.'));
  }
  
  final static int hashCode (CpoolUtf8 name)
  {
    return name.hashCode() ^ 0xF0F;
  }

  public int hashCode ()
  {
    if (hash == 0)
      hash = hashCode(name);
    return hash;
  }

  void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeByte (ConstantPool.CLASS);
    dstr.writeShort (name.index);
  }

  public void print (ClassTypeWriter dst, int verbosity)
  {
    if (verbosity == 1)
      dst.print("Class ");
    else if (verbosity > 1)
      {
	dst.print("Class name: ");
	dst.printOptionalIndex(name);
      }
    dst.print(name.string.replace('/', '.'));
  }
}
