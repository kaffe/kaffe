// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** Use this Enuemration class to iterate over the Variables in a Scope.
 * Descends into child scopes.
 * @author	Per Bothner <bothner@cygnus.com>
 */

public class VarEnumerator implements java.util.Enumeration
{
  Scope topScope;
  Scope currentScope;
  Variable next;

  public VarEnumerator (Scope scope)
  {
    topScope = scope;
    reset ();
  }

  public final void reset ()
  {
    currentScope = topScope;
    if (topScope != null)
      {
	next = currentScope.firstVar ();
	if (next == null)
	  fixup ();
      }
  }

  private void fixup ()
  {
    while (next == null)
      {
	if (currentScope.firstChild != null)
	  currentScope = currentScope.firstChild;
	else
	  {
	    while (currentScope.nextSibling == null)
	      {
		if (currentScope == topScope)
		  return;
		currentScope = currentScope.parent;
	      }
	    currentScope = currentScope.nextSibling;
	  }
	next = currentScope.firstVar ();
      }
  }

  /** Return the next Variable in the Scope tree, or null if done. */ 
  public final Variable nextVar ()
  {
    Variable result = next;
    if (result != null)
      {
	next = result.nextVar ();
	if (next == null)
	  fixup ();
      }
    return result;
  }

  public final boolean hasMoreElements ()
  {
    return next != null;
  }

  public Object nextElement ()
  {
    Variable result = nextVar ();
    if (result == null)
      throw new java.util.NoSuchElementException("VarEnumerator");
    return result;
  }


  
}
