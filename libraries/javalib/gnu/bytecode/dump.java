// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.InputStream;
import java.io.IOException;
import java.io.FileInputStream;

/** Class to read a ClassType from a DataInputStream (.class file).
 * @author Per Bothner
 */

public class dump extends ClassFileInput
{
  ClassTypeWriter writer;

  public dump (InputStream str)
       throws IOException, ClassFormatError
  {
    super(str);
    this.ctype = new ClassType();
    writer = new ClassTypeWriter (ctype, System.out, 0);
    if (!readHeader())
      throw new ClassFormatError("invalid magic number");
    readConstants();
    readClassInfo();
    readFields();
    readMethods();
    readAttributes(ctype);

    writer.printClassInfo();
    writer.printFields();
    writer.printMethods();
    printAttributes ();
    writer.flush();
  }

  public ConstantPool readConstants () throws IOException
  {
    ctype.constants = super.readConstants();
    if (writer.printConstants)
      writer.printConstantPool();
    return ctype.constants;
  }

  public Attribute readAttribute (String name, int length, AttrContainer container)
    throws IOException
  {
    return super.readAttribute (name, length, container);
  }

  public void printAttributes ()
  {
    AttrContainer attrs = ctype;
    writer.println();
    writer.print("Attributes (count: ");
    writer.print(Attribute.count(attrs));
    writer.println("):");
    writer.printAttributes (attrs);
  }

  /** Reads a .class file, and prints out the contents to System.out.
   * Very rudimentary - prints out the constant pool, and field and method
   * names and types, but only minimal attributes (i.e. no dis-assembly yet).
   * @param args One argument - the name of a .class file.
   */
  public static void main (String[] args)
  {
    if (args.length == 0)
      usage();
    String filename = args[0];
    try
      {
	java.io.InputStream inp = new FileInputStream(filename);
	new dump(inp);
      }
    catch (java.io.FileNotFoundException e)
      {
	System.err.println("File "+filename+" not found");
	System.exit(-1);
      }
    catch (java.io.IOException e)
      {
	System.err.println(e);
	System.exit(-1);
      }
  }

  public static void usage()
  {
    System.err.println("Usage: foo.class");
    System.exit(-1);
  }
}

