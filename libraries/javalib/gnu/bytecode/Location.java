// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** An abstracted "variable", inherited by Field and Variable. */

public class Location {
  protected String name;
  protected Type type;
  int name_index; /* Index in constant table, or 0 if un-assigned */
  int signature_index; /* Index in constant table, or 0 if un-assigned */

  public final String getName ()
  {
    return name;
  }

  public final void setName (String name)
  {
    this.name = name;
  }

  public final void setName(int name_index, ConstantPool constants)
  {
    if (name_index <= 0)
      name = null;
    else
      {
	CpoolUtf8 nameConstant = (CpoolUtf8)
	  constants.getForced(name_index, ConstantPool.UTF8);
	name = nameConstant.string;
      }
    this.name_index = name_index;
  }

  public final Type getType()
  {
    return type;
  }

  public final void setType(Type type)
  {
    this.type = type;
  }

  public final String getSignature () { return type.getSignature (); }

  public void setSignature (int signature_index, ConstantPool constants)
  {
    CpoolUtf8 sigConstant = (CpoolUtf8)
      constants.getForced(signature_index, ConstantPool.UTF8);
    this.signature_index = signature_index;
    type = Type.signatureToType(sigConstant.string);
  }
}
