// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** An interface for objects that (may) contain Attribute objects. */

public interface AttrContainer
{
  /** Get the (first) Attribute of this container. */
  public Attribute getAttributes ();

  /** Set the (list of) Attributes of this container. */
  public void setAttributes (Attribute attribute);
}
