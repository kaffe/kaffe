// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;


/* Represents the contents of a standard "SourceFile" attribute.
 * @author      Per Bothner
 */

public class SourceFileAttr extends Attribute
{
  String filename;
  int filename_index;

  public String getSourceFile() { return filename; }
  public void setSourceFile(String filename)
  {
    this.filename = filename;
    filename_index = 0;
  }

  public static String fixSourceFile (String fname)
  {
    String fsep = System.getProperty("file.separator", "/");
    if (fsep != null && fsep.length() == 1)
      {
	char fsep0 = fsep.charAt(0);
	if (fsep0 != '/')
	  fname = fname.replace(fsep0, '/');
      }
    return fname;
  }

  public static void setSourceFile (ClassType cl, String filename)
  {
    Attribute attr = Attribute.get (cl, "SourceFile");
    if (attr != null && attr instanceof SourceFileAttr)
      {
	((SourceFileAttr)attr).setSourceFile(filename);
      }
    else
      {
	SourceFileAttr sattr = new SourceFileAttr(filename);
        sattr.addToFrontOf(cl);
      }
  }

  public SourceFileAttr (String filename)
  {
    super("SourceFile");
    this.filename = filename;
  }

  public SourceFileAttr (int index, ClassType ctype)
  { 
    super("SourceFile");
    CpoolUtf8 filenameConstant = (CpoolUtf8)
      ctype.constants.getForced(index, ConstantPool.UTF8);
    this.filename = filenameConstant.string;
    this.filename_index = index;
  } 


  public void assignConstants (ClassType cl)
  {
    super.assignConstants(cl);
    if (filename_index == 0)
      filename_index = cl.getConstants().addUtf8(filename).getIndex();
  }

  public final int getLength() { return 2; }

  public void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeShort(filename_index);
  }

  public void print (ClassTypeWriter dst)
  {
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", \"");
    dst.print(getSourceFile());
    dst.println('\"');
  }
}
