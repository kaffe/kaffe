// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** The state of a conditional expression or statement. */

public class IfState {

  /** The surrounding IfState, if any. */
  IfState previous;

  /** True if we are curently in the else part of the conditional. */
  boolean doing_else;

  /** The (not-yet-defined) label at the end of the current sub-clause.
   * If doing_else, this is the end of the entire conditional;
   * otherwise, it is the end of the "then" clause. */
  Label end_label;

  /** The stack size before the "then" clause. */
  int start_stack_size;

  /** The change in the stack size caused by the then-clause.
   * Same as then_stacked_types.length, if stack_growth >= 0. */
  int stack_growth;

  /** The types that were pushed by the then-clause. */
  Type[] then_stacked_types;

  public IfState (CodeAttr code)
  {
    this(code, new Label(code));
  }

  public IfState (CodeAttr code, Label endLabel)
  {
    previous = code.if_stack;
    code.if_stack = this;
    end_label = endLabel;
    start_stack_size = code.SP;
  }
}

