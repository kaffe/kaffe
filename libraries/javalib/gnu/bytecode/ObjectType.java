// Copyright (c) 1997, 2000  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/**
  * Semi-abstract class object reference types.
  * <p>
  * Extended by ClassType and ArrayType. */

public class ObjectType extends Type
{
  protected ObjectType ()
  {
    size = 4;
  }

  public ObjectType (String name)
  {
    this_name = name;
    size = 4;
  }

  // Miscellaneous bits:
  final static int ADD_FIELDS_DONE  = 1;
  final static int ADD_METHODS_DONE = 2;
  // A ClassType that we can expect to have a corresponding reflectClass.
  final static int EXISTING_CLASS = 4;
  /* */ public int flags;

  public final boolean isExisting()
  {
    return (flags & EXISTING_CLASS) != 0;
  }

  public final void setExisting(boolean existing)
  {
    if (existing) flags |= EXISTING_CLASS;
    else flags &= ~ EXISTING_CLASS;
  }

  /** Returns class name if a class type, signature if an array type.
   * In both cases, uses '/' rather than '.' after packages prefixes.
   * Seems rather arbitrary - but that is how classes are represented
   * in the constant pool (CONSTANT_Class constants).
   * Also, Class.forName is the same, except using '.'.
   */
  public String getInternalName()
  {
    return getName().replace('.', '/');
  }

  /** Get the java.lang.Class object for the representation type. */
  public Class getReflectClass()
  {
    try
      {
	if (reflectClass == null)
	  {
	    /* #ifdef JAVA2 */
	    /* Specifies optional 'initialize' argument. */
	    reflectClass = Class.forName(getInternalName().replace('/', '.'),
					 false, getClass().getClassLoader());
	    /* #endif */
	    /* #ifndef JAVA2 */
	    // reflectClass = Class.forName(getInternalName().replace('/', '.'));
	    /* #endif */
	  }
        flags |= EXISTING_CLASS;
      }
    catch (java.lang.ClassNotFoundException ex)
      {
        if ((flags & EXISTING_CLASS) != 0)
          throw new RuntimeException("no such class: "+getName());
      }
    return reflectClass;
  }

  public Type getImplementationType()
  {
    return this == nullType ? pointer_type
      : this == tostring_type ? string_type : this;
  }

  public Type promote ()
  {
    return this == nullType ? pointer_type : this;
  }

  public int compare(Type other)
  {
    // Assume this == nullType.
    return other == nullType ? 0 : -1;
  }

  /** Convert an object to a value of this Type.
   * Throw a ClassCastException when this is not possible. */
  public Object coerceFromObject (Object obj)
  {
    if (obj != null)
      {
	if (this == Type.tostring_type)
	  return obj.toString();
        Class clas = getReflectClass();
        Class objClass = obj.getClass();
        if (! clas.isAssignableFrom(objClass))
          throw new ClassCastException("don't know how to coerce "
                                       + objClass.getName() + " to "
                                       + getName());
      }
    return obj;
  }

  /** Compile (in given method) cast from Object to this Type. */
  public void emitCoerceFromObject (CodeAttr code)
  {
    if (this == Type.tostring_type)
      {
	// This would be nice but it doesn't verify, alas!
	// code.reserve(4);
	// code.emitDup();
	// code.put1(198); // ifnull
	// code.put2(6);  // skip after emitInvokeVirtual.
	// code.emitInvokeVirtual(Type.toString_method);
	code.emitDup();
	code.emitIfNull();
	code.emitPop(1);
	code.emitPushNull();
	code.emitElse();
	code.emitInvokeVirtual(Type.toString_method);
	code.emitFi();
      }
    else if (this != Type.pointer_type)
      code.emitCheckcast(this);
  }
}
