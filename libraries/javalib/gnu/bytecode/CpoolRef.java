// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/** A CONSTANT_{Field,Method,InterfaceMethod}Ref entry in the constant pool. */

public class
CpoolRef extends CpoolEntry
{
  CpoolClass clas;
  CpoolNameAndType nameAndType;

  /**
   * The specific kind of Ref constant:
   * CONSTANT_Fieldref (9), CONSTANT_Methodref (10), or
   * CONSTANT_InterfaceMethodref (11).
   */
  int tag;
  public int getTag() { return tag; }

  public final CpoolClass getCpoolClass()
  {
    return clas;
  }

  public final CpoolNameAndType getNameAndType()
  {
    return nameAndType;
  }

  CpoolRef (int tag) { this.tag = tag; }

  CpoolRef (ConstantPool cpool, int hash, int tag,
	    CpoolClass clas, CpoolNameAndType nameAndType)
  {
    super (cpool, hash);
    this.tag = tag;
    this.clas = clas;
    this.nameAndType = nameAndType;
  }

  final static int hashCode (CpoolClass clas, CpoolNameAndType nameAndType)
  {
    return clas.hashCode() ^ nameAndType.hashCode();
  }

  public int hashCode ()
  {
    if (hash == 0)
      hash = hashCode(clas, nameAndType);
    return hash;
  }

  void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeByte (tag);
    dstr.writeShort (clas.index);
    dstr.writeShort (nameAndType.index);
  }

  public void print (ClassTypeWriter dst, int verbosity)
  {
    String str;
    switch (tag)
      {
      case 9:   str = "Field";  break;
      case 10:  str = "Method";  break;
      case 11:  str = "InterfaceMethod";  break;
      default:  str = "<Unknown>Ref";  break;
      }
    if (verbosity > 0)
      {
	dst.print(str);
	if (verbosity == 2)
	  {
	    dst.print(" class: ");
	    dst.printOptionalIndex(clas);
	  }
	else
	  dst.print( ' ');
      }
    clas.print(dst, 0);
    if (verbosity < 2)
      dst.print('.');
    else
      {
	dst.print(" name_and_type: ");
	dst.printOptionalIndex(nameAndType);
	dst.print('<');
      }
    nameAndType.print(dst, 0);
    if (verbosity == 2)
      dst.print('>');
  }
}

