// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/**
  * Represents the contents of a standard "LineNumberTable" attribute.
  * @author      Per Bothner
  */

public class LineNumbersAttr extends Attribute
{
  // The line number table.  Each even entry (starting with index 0) is a PC,
  // and the following odd entry is the linenumber.  Each number is
  // actually unsigned, so should be masked with 0xFFFF.
  short[] linenumber_table;
  // The number of linenumber (pairs) in linenumber_table.
  int linenumber_count;

  /** Add a new LineNumbersAttr to a CodeAttr. */
  public LineNumbersAttr(CodeAttr code)
  {
    super("LineNumberTable");
    addToFrontOf(code);
    code.lines = this;
  }

  public LineNumbersAttr(short[] numbers, CodeAttr code)
  {
    this(code);
    linenumber_table = numbers;
    linenumber_count = numbers.length >> 1;
  }

  /** Add a new line number entry.
    * @param linenumber the number in the source file for this entry
    * @param PC the byte code location for the code for this line number. */
  public void put (int linenumber, int PC)
  {
    if (linenumber_table == null)
      linenumber_table = new short[32];
    else if (2 * linenumber_count >= linenumber_table.length)
      {
	short[] new_linenumbers = new short [2 * linenumber_table.length];
	System.arraycopy (linenumber_table, 0, new_linenumbers, 0,
			  2 * linenumber_count);
	linenumber_table = new_linenumbers;
      }
    linenumber_table[2 * linenumber_count] = (short) PC;
    linenumber_table[2 * linenumber_count + 1] = (short) linenumber;
    linenumber_count++;
  }

  /** Get the number of line number entries. */
  public final int getLength() { return 2 + 4 * linenumber_count; }

  public int getLineCount () { return linenumber_count; }
  public short[] getLineNumberTable () { return linenumber_table; }

  public void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeShort (linenumber_count);
    int count = 2 * linenumber_count;
    for (int i = 0;  i < count;  i++)
      {
	dstr.writeShort(linenumber_table[i]);
      }
  }

  public void print (ClassTypeWriter dst) 
  {
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", count: ");
    dst.println(linenumber_count);
    for (int i = 0;  i < linenumber_count;  i++)
      {
	dst.print("  line: ");
	dst.print(linenumber_table[2 * i + 1] & 0xFFFF); // line number
	dst.print(" at pc: ");
	dst.println(linenumber_table[2 * i] & 0xFFFF);   // start_pc
      }
  }
}
