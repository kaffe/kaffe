// Copyright (c) 1998, 2004  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** Maintains the state for generating a switch statement. */

public class SwitchState
{
  /** The smallest case value, so far. */
  int minValue;
  /** The largest case value, so far. */
  int maxValue;
  /** The number of cases (not including the default case). */
  int numCases;
  /** The case values, in numerical order (in values[0..numCases-1]). */
  int[] values;
  /** The case locations, in the same order as values. */
  Label[] labels;
  /** The location to jump to if none of the cases match. */
  Label defaultLabel;
  /* Location of the actual switch instruction. */
  Label switch_label;
  Label cases_label;
  Type[] typeState;

  public int getMaxValue() { return maxValue; }

  public SwitchState(CodeAttr code)
  {
    switch_label = new Label(code);
    cases_label = new Label(code);

    code.popType();  // pop switch value

    // Save stack types (except top int) into typeState
    typeState = code.saveStackTypeState(false);

    code.fixupChain(cases_label, switch_label);

    numCases = 0;
  }

  /** Emit a new case, for the given value, whose label is here. */
  public boolean addCase(int value, CodeAttr code)
  {
    Label label = new Label(code);
    boolean ok = addCase (value, label, code);
    label.define(code);
    code.restoreStackTypeState(typeState);
    return ok;
  }

  public void addDefault(CodeAttr code)
  {
    Label label = new Label(code);
    label.define(code);
    addDefault(label, code);
  }

  public void addDefault(Label label, CodeAttr code)
  {
    defaultLabel = label;
    code.restoreStackTypeState(typeState);
  }

  /** Add a new case.
   * @param value the case value to match against at run-time
   * @param label the location to go to if the value matches
   * @param code the CodeAttr of the Method we are generating code for
   * @return true on success;  false if value duplicates an existing value
   */
  public boolean addCase(int value, Label label, CodeAttr code)
  {
    if (values == null)
      {
	values = new int[10];
	labels = new Label[10];
	numCases = 1;
	minValue = maxValue = value;
	values[0] = value;
	labels[0] = label;
	return true;
      }
    int[] old_values = values;
    Label[] old_labels = labels;
    if (numCases >= values.length)
      {
	values = new int[2 * numCases];
	labels = new Label[2 * numCases];
      }
    int copyBefore;
    if (value < minValue)
      {
	copyBefore = 0;
	minValue = value;
      }
    else if (value > maxValue)
      {
	copyBefore = numCases;
	maxValue = value;
      }
    else
      {
	// Binary search.
	int low = 0;
	int hi = numCases - 1;
	copyBefore = 0;
	while (low <= hi)
	{
	  copyBefore = (low + hi) >> 1;
	  if (old_values[copyBefore] >= value)
	    hi = copyBefore - 1;
	  else
	    low = ++ copyBefore;
	}

	if (value == old_values[copyBefore])
	  return false;
      }
    int copyAfter = numCases - copyBefore;
    System.arraycopy(old_values, copyBefore, values, copyBefore+1, copyAfter);
    System.arraycopy(old_values, 0, values, 0, copyBefore);
    values[copyBefore] = value;
    System.arraycopy(old_labels, copyBefore, labels, copyBefore+1, copyAfter);
    System.arraycopy(old_labels, 0, labels, 0, copyBefore);
    labels[copyBefore] = label;
    numCases++;
    return true;
  }

  /** Handle the end of the switch statement.
   * Assume the case value is on the stack; go to the matching case label. */
  public void finish (CodeAttr code)
  {
    if (defaultLabel == null)
      {
	defaultLabel = new Label(code);
	defaultLabel.define(code);
	ClassType ex = ClassType.make("java.lang.RuntimeException");
	code.emitNew(ex);
	code.emitDup(ex);
	code.emitPushString("bad case value!");
	Type[] args = { Type.string_type };
	Method con = ex.addMethod("<init>", Access.PUBLIC,
				  args, Type.void_type);
	code.emitInvokeSpecial(con);
	code.emitThrow();
      }
    Label after_label = new Label(code);
    code.fixupChain(switch_label, after_label);
    if (numCases <= 1)
      {
	code.pushType(Type.int_type);
	if (numCases == 1)
	  {
	    code.emitPushInt(minValue);
	    code.emitGotoIfEq(labels[0]);
	  }
	else
	  {
	    code.emitPop(1);
	  }
	code.emitGoto(defaultLabel);
      }
    else if (2 * numCases >= maxValue - minValue)
      {
	code.reserve(13 + 4 * (maxValue - minValue + 1));
	code.fixupAdd(CodeAttr.FIXUP_SWITCH, null);
	code.put1(170);  // tableswitch
	code.fixupAdd(CodeAttr.FIXUP_CASE, defaultLabel);
	code.PC += 4;
	code.put4(minValue);
	code.put4(maxValue);
	int index = 0;
	for (int i = minValue;  i <= maxValue;  i++)
	  {
	    Label lab = values[index] == i ? labels[index++] : defaultLabel;
	    code.fixupAdd(CodeAttr.FIXUP_CASE, lab);
	    code.PC += 4;
	  }
      }
    else
      {
	code.reserve(9 + 8 * numCases);
	code.fixupAdd(CodeAttr.FIXUP_SWITCH, null);
	code.put1(171);  // lookupswitch
	code.fixupAdd(CodeAttr.FIXUP_CASE, defaultLabel);
	code.PC += 4;
	code.put4(numCases);
	for (int index = 0;  index < numCases;  index++)
	  {
	    code.put4(values[index]);
	    code.fixupAdd(CodeAttr.FIXUP_CASE, labels[index]);
	    code.PC += 4;
	  }
      }
    code.fixupChain(after_label, cases_label);
  }
}
