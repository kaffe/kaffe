// Copyright (c) 1997, 2004  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/**
 * A Label represents a location in a Code attribute.
 */

public class Label {

  /** Offset in the fixup_offsets and fixup_labels arrays.
   * The offset corresponds to the fixup itself. */
  int first_fixup;

  /** The PC of where the label is, or -1 if not yet defined.
   * This PC may be tentative if we later run processFixups.
   * The offset in the code array is cattr.fixupOffset(first_fixup). */
  int position;

  public final boolean defined () { return position >= 0; }

  public Label ()
  {
    position = -1;
  }

  public Label (CodeAttr code)
  {
    position = -1;
  }

  public Label (int position)
  {
    this.position = position;
  }

  /**
   * Define the value of a label as having the current location.
   * @param code the "Code" attribute of the current method
   */
  public void define (CodeAttr code)
  {
    code.setReachable(true);
    if (position >= 0)
      throw new Error ("label definition more than once");

    position = code.PC;
    first_fixup = code.fixup_count;
    code.fixupAdd(CodeAttr.FIXUP_DEFINE, this);
  }

  
}
