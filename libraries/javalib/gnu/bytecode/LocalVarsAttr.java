// Copyright (c) 1997, 2004  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/* Represents the contents of a standard "LocalVariableTable" attribute.
 * @author      Per Bothner
 */

public class LocalVarsAttr extends Attribute
{
  private Method method;

  /* Map local slot index to the local variable that is there. */
  Variable[] used;
  public Scope current_scope;
  Scope parameter_scope;

  /** Add a new LocalVarsAttr to a CodeAttr. */
  public LocalVarsAttr(CodeAttr code)
  {
    super("LocalVariableTable");
    addToFrontOf(code);
    method = (Method) code.getContainer();
    code.locals = this;
  }

  /** Create, but don't link into method.code's attributes list (yet). */
  public LocalVarsAttr(Method method)
  {
    super("LocalVariableTable");
    CodeAttr code = method.code;
    this.method = method;
    code.locals = this;
  }

  public final Method getMethod() { return method; }

  public VarEnumerator allVars ()
  {
    return new VarEnumerator (parameter_scope);
  }

  public void enterScope (Scope scope) {
    scope.linkChild (current_scope);
    current_scope = scope;
    CodeAttr code = method.getCode();
    for (Variable var = scope.firstVar ();  var != null;  var = var.nextVar ())
      {
	if (var.isSimple ())
	  {
	    if (! var.isAssigned ())
	      var.allocateLocal(code);
	    else if (used[var.offset] == null)
	      used[var.offset] = var;
	    else if (used[var.offset] != var)
	      throw new Error ("inconsistent local variable assignments for "
+var+" != "+used[var.offset]);
	  }
      }
  }

  /** Mark scopes upto specified scope as 'preserved'.
   * This means that the local variable slots are not available for reuse
   * when the scope exits.  The effected scopes are all ancestor scopes from
   * the current scope (inclusive) upto the specified scope (exclusive). */
  public void preserveVariablesUpto(Scope scope)
  {
    for (Scope cur = current_scope;  cur != scope;  cur = cur.parent)
      cur.preserved = true;
  }

  public final boolean isEmpty ()
  {
    VarEnumerator vars = allVars();
    Variable var;
    while ((var = vars.nextVar ()) != null)
      {
	if (var.isSimple () && var.name != null)
          return false;
      }
    return true;
  }

  public final int getCount ()
  {
    int local_variable_count = 0;
    VarEnumerator vars = allVars();
    Variable var;
    while ((var = vars.nextVar ()) != null)
      {
	if (var.shouldEmit())
	  local_variable_count++;
      }
    return local_variable_count;
  }

  public final int getLength()
  {
    return 2 + 10 * getCount();
  }

  public void assignConstants (ClassType cl)
  {
    super.assignConstants(cl);

    VarEnumerator vars = allVars ();
    Variable var;
    while ((var = vars.nextVar ()) != null)
      {
	if (var.isSimple () && var.name != null)
	  {
	    if (var.name_index == 0)
	      var.name_index = cl.getConstants().addUtf8(var.getName()).index;
	    if (var.signature_index == 0)
	      var.signature_index
		= cl.getConstants().addUtf8(var.getType().signature).index;
	  }
      }
  }

  public void write (DataOutputStream dstr) throws java.io.IOException
  {
    VarEnumerator vars = allVars ();
    Variable var;
    dstr.writeShort (getCount());
	    
    for (vars.reset (); (var = vars.nextVar ()) != null; )
      {
	if (var.shouldEmit())
	  {
	    Scope scope = var.scope;
	    int start_pc = scope.start.position;
	    int end_pc = scope.end.position;
	    dstr.writeShort(start_pc);
	    dstr.writeShort(end_pc - start_pc);
	    dstr.writeShort(var.name_index);
	    dstr.writeShort(var.signature_index);
	    dstr.writeShort(var.offset);
	  }
      }
  }

  public void print (ClassTypeWriter dst) 
  {
    VarEnumerator vars = allVars ();
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", count: ");
    dst.println(getCount());
	    
    Variable var;
    for (vars.reset (); (var = vars.nextVar ()) != null; )
      {
	if (var.isSimple () && var.name != null)
	  {
	    dst.print("  slot#");
	    dst.print(var.offset);
	    dst.print(": name: ");
	    dst.printOptionalIndex(var.name_index);
	    dst.print(var.getName());
	    dst.print(", type: ");
	    dst.printOptionalIndex(var.signature_index);
	    dst.printSignature(var.getType());
	    dst.print(" (pc: ");
	    Scope scope = var.scope;
	    int start_pc, end_pc;
	    if (scope == null || scope.start == null || scope.end == null
		|| (start_pc = scope.start.position) < 0
		|| (end_pc = scope.end.position) < 0)
	      dst.print("unknown");
	    else
	      {
		dst.print(start_pc);
		dst.print(" length: ");
		dst.print(end_pc - start_pc);
	      }
	    dst.println(')');
	  }
      }
  }
}
