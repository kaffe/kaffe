// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** The state of a try statement. */

public class TryState {
  /** The surrounding TryState, if any. */
  TryState previous;

  /** The label for the code following the entire try-statement. */
  Label end_label;

  /** If this "try" has a "finally", the Label of the "finally" sub-routine. */
  Label finally_subr;

  /** Used for the return address of the finally subroutine (if any). */
  Variable finally_ret_addr;

  /** Non-null if we need a temporary to save the result. */
  Variable saved_result;

  /** If the SP > 0 when we entered the try, the stack is saved here. */
  Variable[] savedStack;

  Label start_try;
  Label end_try;

  /** If we are inside a try, the type of variable matched. */
  ClassType try_type;

  public TryState (CodeAttr code)
  {
    previous = code.try_stack;
    code.try_stack = this;
    start_try = code.getLabel();
  }

}
