// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/**
  * Represents an Attribute of an AttrContainer.
  * <p>
  * Various sub-classses are used for standard attributes,
  * or you can use MiscAttr for a generic attribute.
  * @author      Per Bothner
  */

public abstract class Attribute
{
  /** Every Attribute belongs to some AttrContainer object. */
  AttrContainer container;
  /** Return the Attribute container that contains this Attribute. */
  public final  AttrContainer getContainer() { return container; }
  public final void setContainer(AttrContainer container)
  { this.container = container; }

  Attribute next;
  /** Get the next Attribute belonging to getContainer(). */
  public final Attribute getNext() { return next; }
  /** Set the next Attribute in the chain belonging to getContainer(). */
  public final void setNext(Attribute next) { this.next = next; }

  /** Add this to (the front of) of the specified attribute container. */
  public void addToFrontOf(AttrContainer container)
  {
    setContainer(container);
    setNext(container.getAttributes()); 
    container.setAttributes(this);
  }

  String name; // This is an interned string.

  // If > 0, the constant-pool index of name.
  // If -1, means attribute should be skipped on output.
  int name_index;

  /** Returns true if this attribute should be skipped on output. */
  public final boolean isSkipped() { return name_index < 0; }

  /** Iff skip, cause this attributed to be skipped on output. */
  public final void setSkipped(boolean skip) { name_index = skip ? -1 : 0; }

  /** Cause this attributed to be skipped on output. */
  public final void setSkipped() { name_index = -1; }

  public final String getName() { return name; }
  public final void setName(String name) { this.name = name.intern(); }

  public final int getNameIndex() { return name_index; }
  public final void setNameIndex(int index) { name_index = index; }

  /** Create a new Attribute.
    * @param name - an interned String that names the Attribute. */
  public Attribute (String name)
  {
    this.name = name;
  }

  /** Find an Attribute by name, in an attribute cointainer.
    * @param container the attribute container to search
    * @param name the (interned) name of the attribute we are seeking
    * @return the matching Attribute, or null if the search failed.
    */
  public static Attribute get (AttrContainer container, String name)
  {
    for (Attribute attr = container.getAttributes();
	 attr != null;  attr = attr.next)
      {
	if (attr.getName() == name)
	  return attr;
      }
    return null;
  }

  /** Add any needed constant pool entries for this Attribute.
    * Overridden by sub-classes.
    * Do any other cleanup needed before writing out a .class file. */
  public void assignConstants (ClassType cl)
  {
    if (name_index == 0)
      name_index = cl.getConstants().addUtf8(name).getIndex();
  }

  /** Add any needed constant pool entries for all attributes in a container.
    * Do any other cleanup needed before writing out a .class file. */
  public static void assignConstants (AttrContainer container, ClassType cl)
  {
    for (Attribute attr = container.getAttributes();
	 attr != null;  attr = attr.next)
      {
	if (! attr.isSkipped())
	  attr.assignConstants(cl);
      }
  }

  /** Return the length of the attribute in bytes.
    * Does not include the 6-byte header (for the name_index and the length).*/
  abstract public int getLength();

  /** Return the length of all the attributes (with headers) in bytes. */
  public static int getLengthAll (AttrContainer container)
  {
    int length = 0;
    for (Attribute attr = container.getAttributes();
	 attr != null;  attr = attr.next)
      {
	if (! attr.isSkipped())
	  length += 6 + attr.getLength();
      }
    return length;
  }

  /** Write out the contents of the Attribute.
    * Does not write the 6-byte attribute header. */
  abstract public void write (DataOutputStream dstr)
    throws java.io.IOException;

  public static int count (AttrContainer container)
  {
    int count = 0;
    for (Attribute attr = container.getAttributes();
	 attr != null;  attr = attr.next)
      {
	if (!attr.isSkipped())
	  count++;
      }
    return count;
  }

  public static void writeAll (AttrContainer container, DataOutputStream dstr)
    throws java.io.IOException
  {
    int count = count(container);
    dstr.writeShort(count);
    for (Attribute attr = container.getAttributes();
	 attr != null;  attr = attr.next)
      {
	if (attr.isSkipped())
	  continue;
	if (attr.name_index == 0)
	  throw new Error("Attribute.writeAll called without assignConstants");
	dstr.writeShort(attr.name_index);
	dstr.writeInt(attr.getLength());
	attr.write(dstr);
      }
  }

  public void print (ClassTypeWriter dst)
  {
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.println(getLength());
  }

};
