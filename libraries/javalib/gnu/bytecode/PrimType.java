package gnu.bytecode;

public class PrimType extends Type {

  public PrimType (String nam, String sig, int siz, Class reflectClass) {
    super(nam, sig);
    size = siz;
    this.reflectClass = reflectClass;
    Type.registerTypeForClass(reflectClass, this);
  }

  protected PrimType(PrimType type)
  {
    super(type.this_name, type.signature);
    size = type.size;
    reflectClass = type.reflectClass;
  }

  public Object coerceFromObject (Object obj)
  {
    if (obj.getClass() == reflectClass)
      return obj;
    char sig1 = (signature == null || signature.length() != 1) ? ' '
      : signature.charAt(0);
    switch (sig1)
      {
      case 'B':	return new Byte(((Number) obj).byteValue());
      case 'S':	return new Short(((Number) obj).shortValue());
      case 'I':	return new Integer(((Number) obj).intValue());
      case 'J':	return new Long(((Number) obj).longValue());
      case 'F':	return new Float(((Number) obj).floatValue());
      case 'D':	return new Double(((Number) obj).doubleValue());
      }
    throw new ClassCastException("don't know how to coerce "
				 + obj.getClass().getName() + " to "
				 + getName());
  }

  /** Coerce value to a char.
   * Only defined if getSignature() is "C". */
  public char charValue (Object value)
  {
    return ((Character) value).charValue();
  }

  /** Coerce value to a boolean.
   * Only defined if getSignature() is "Z". */
  public static boolean booleanValue (Object value)
  {
    return ! (value instanceof Boolean) || ((Boolean) value).booleanValue();
  }

  public void emitCoerceToObject (CodeAttr code)
  {
    char sig1 = getSignature().charAt(0);
    ClassType clas;
    Method method;
    String cname;
    Type[] args;
    switch (sig1)
      {
      case 'Z':
	clas = ClassType.make("java.lang.Boolean");
	code.emitIfIntNotZero();
	code.emitGetStatic(clas.getDeclaredField("TRUE"));
	code.emitElse();
	code.emitGetStatic(clas.getDeclaredField("FALSE"));
	code.emitFi();
	return;
      case 'C':  cname = "java.lang.Character"; break;
      case 'B':  cname = "java.lang.Byte";      break;
      case 'S':  cname = "java.lang.Short";     break;
      case 'I':  cname = "java.lang.Integer";   break;
      case 'J':  cname = "java.lang.Long";      break;
      case 'F':  cname = "java.lang.Float";     break;
      case 'D':  cname = "java.lang.Double";    break;
      default:   cname = null; // Should never happen.
      }
    clas = ClassType.make(cname);
    args = new Type[1];
    args[0] = this;
    method = clas.getDeclaredMethod("<init>", args);
    code.emitNew(clas);
    code.emitDupX();
    code.emitSwap();
    code.emitInvokeSpecial(method);
  }

  public void emitIsInstance (CodeAttr code)
  {
    char sig1 = (signature == null || signature.length() != 1) ? ' '
      : signature.charAt(0);
    if (sig1 == 'Z')  // boolean
      boolean_ctype.emitIsInstance(code);
    else if (sig1 == 'V')
      {
	code.emitPop(1);
	code.emitPushInt(1);
      }
    // Have left out Character -> char, since not used by Kawa.
    else
      number_type.emitIsInstance(code);
  }

  public void emitCoerceFromObject (CodeAttr code)
  {
    char sig1 = (signature == null || signature.length() != 1) ? ' '
      : signature.charAt(0);
    if (sig1 == 'Z')  // boolean
      {
	code.emitCheckcast(boolean_ctype);
	code.emitInvokeVirtual(booleanValue_method);
      }
    else if (sig1 == 'V')
      code.emitPop(1);
    else
      {
	code.emitCheckcast(number_type);
	if (sig1 == 'I' || sig1 == 'S' || sig1 == 'B')
	  code.emitInvokeVirtual(intValue_method);
	else if (sig1 == 'J')
	  code.emitInvokeVirtual(longValue_method);
	else if (sig1 == 'D')
	  code.emitInvokeVirtual(doubleValue_method);
	else if (sig1 == 'F')
	  code.emitInvokeVirtual(floatValue_method);
	// Have left out Character -> char, since not used by Kawa.
	else
	  super.emitCoerceFromObject(code);
      }
  }

  public static int compare(PrimType type1, PrimType type2)
  {
    char sig1 = type1.signature.charAt(0);
    char sig2 = type2.signature.charAt(0);

    if (sig1 == sig2)
      return 0;

    // Anything can be converted to void, but not vice versa.
    if (sig1 == 'V')
      return 1;
    if (sig2 == 'V')
      return -1;

    // In Java, no other type can be converted to/from boolean.
    // Other languages, including C and Scheme are different:
    // "everything" can be converted to a boolean.
    if (sig1 == 'Z' || sig2 == 'Z')
      return -3;

    if (sig1 == 'C')
      return type2.size > 2 ? -1 : -3;
    if (sig2 == 'C')
      return type1.size > 2 ? 1 : -3;

    if (sig1 == 'D')
      return 1;
    if (sig2 == 'D')
      return -1;
    if (sig1 == 'F')
      return 1;
    if (sig2 == 'F')
      return -1;
    if (sig1 == 'J')
      return 1;
    if (sig2 == 'J')
      return -1;
    if (sig1 == 'I')
      return 1;
    if (sig2 == 'I')
      return -1;
    if (sig1 == 'S')
      return 1;
    if (sig2 == 'S')
      return -1;
    // Can we get here?
    return -3;
  }

  public int compare(Type other)
  {
    if (other instanceof PrimType)
      return compare(this, (PrimType) other);
    if (! (other instanceof ClassType))
      return -3;
    char sig1 = signature.charAt(0);
    String otherName = other.getName();
    if (otherName == null)
       return -1;
    // This is very incomplete!  FIXME.
    switch (sig1)
      {
      case 'V':
        return 1;
      case 'D':
        if (otherName.equals("java.lang.Double")
            || otherName.equals("gnu.math.DFloat"))
          return 0; // Or maybe 1?
        break;
      case 'I':
        if (otherName.equals("java.lang.Integer"))
          return 0; // Or maybe 1?
        if (otherName.equals("gnu.math.IntNum"))
          return -1;
        break;
      }
    if (otherName.equals("java.lang.Object")
	|| other == tostring_type)
      return -1;
    return -2;
  }
}
