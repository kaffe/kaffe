// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/**
  * Represents a method in a <code>ClassType</code>.
  * <p>
  * A <code>Method</code> contain a <code>CodeAttr</code> object;
  * the interface for generating bytecode instructions is primarily
  * in <code>CodeAttr</code>.
  * <p>
  * All the methods whose name start with <code>compile_</code> are
  * deprecated, and should not be used;  use the methods
  * in <code>CodeAttr</code>instead.
  */

public class Method implements AttrContainer {
  private String name;
  Type[] arg_types;
  Type return_type;
  int access_flags;
  int name_index; /* Index in constant table, or 0 if un-assigned */
  int signature_index; /* Index in constant table, or 0 if un-assigned */
  Method next;
  ClassType classfile;

  Attribute attributes;
  public final Attribute getAttributes () { return attributes; }
  public final void setAttributes (Attribute attributes)
  { this.attributes = attributes; }

  ExceptionsAttr exceptions;
  public final ExceptionsAttr getExceptionAttr () { return exceptions; }

  public void setExceptions (short[] exn_indices)
  {
    if (exceptions == null)
      exceptions = new ExceptionsAttr (this);
    exceptions.setExceptions (exn_indices, classfile);
  }

  public void setExceptions (ClassType[] exn_types)
  {
    if (exceptions == null)
      exceptions = new ExceptionsAttr(this);
    exceptions.setExceptions(exn_types);
  }

  CodeAttr code;
  public final CodeAttr getCode () { return code; }

  Method (ClassType clfile, int flags)
  {
    if (clfile.last_method == null)
       clfile.methods = this;
    else
      clfile.last_method.next = this;
    clfile.last_method = this;
    clfile.methods_count++;
    access_flags = flags;
    classfile = clfile;
  }

  public final void setStaticFlag (boolean is_static) {
    if (is_static)
      access_flags |= Access.STATIC;
    else
      access_flags ^= ~Access.STATIC;
  }

  public final boolean getStaticFlag () {
    return (access_flags & Access.STATIC) != 0;
  }

  public final boolean isAbstract()
  {
    return (access_flags & Access.ABSTRACT) != 0;
  }

  public int getModifiers ()
  {
    return access_flags;
  }

  public void setModifiers (int modifiers)
  {
    access_flags = modifiers;
  }

  public final ConstantPool getConstants ()
  {
    return classfile.constants;
  }
    
  public Scope pushScope () {
    prepareCode(0);
    return code.pushScope();
  }

  /** True if control could reach here. */
  public final boolean reachableHere () { return code.reachableHere(); }

  public Scope popScope () { return code.popScope(); }

  /**
   * Allocate slots for a local variable (or parameter).
   * @param local the variable we need to allocate
   * @result the index of the (first) slot.
   * @deprecated
   */
  public void allocate_local (Variable local)
  {
    local.allocateLocal(code);
  }

  /** Allocate a Code attribute, and prepare to generate code.
   * Most code generators should use the startCode convenience method. */
  public void initCode ()
  {
    if (classfile.constants == null)
      classfile.constants = new ConstantPool();
    prepareCode(0);
    code.sourceDbgExt = classfile.sourceDbgExt;
    code.pushScope();
  }

  /**
   * @deprecated Use startCode instead
   */
  public void init_param_slots ()
  {
    initCode ();
    code.addParamLocals();
  }

  /** Recommended method to create a new CodeAttr for this Method. */
  public CodeAttr startCode ()
  {
    initCode();
    code.addParamLocals();
    return code;
  }

  void kill_local (Variable var) { var.freeLocal(code); }

  /** Method that must be called before we generate any instructions.
    * Set so there is room for at least max_size bytes of code.
    */
  void prepareCode(int max_size)
  {
    if (code == null)
      code = new CodeAttr(this);
    code.reserve(max_size);
  }

  // This method should be called before we generate code for
  // an instruction (or sequence).
  // An upper bound of the intruction length is max_size.
  // deprecated!
  void instruction_start_hook (int max_size)
  {
    prepareCode(max_size);
  }

  final Type pop_stack_type () { return code.popType(); }
  final void push_stack_type (Type type) { code.pushType(type); }

  public void compile_checkcast (Type type)
  {
    code.emitCheckcast (type);
  }

  public void maybe_compile_checkcast (Type type)
  {
    Type stack_type = code.topType();
    if (type != stack_type)  // FIXME rather simple-minded, but safe.
      code.emitCheckcast(type);
  }

  /**
   * Comple code to push the contents of a local variable onto the statck.
   * @param var The variable whose contents we want to push.
   * @deprecated
   */
  public void push_var (Variable var) { code.emitLoad (var); }
  /**
   * @deprecated
   */
  public void compile_push_value (Variable var) { code.emitLoad(var); }

  /** 
    * @deprecated
   */
  public void compile_store_value (Variable var)
  {
    code.emitStore(var);
  }

  public void compile_push_this ()
  {
    code.emitPushThis();
  }

  void write (DataOutputStream dstr, ClassType classfile)
       throws java.io.IOException
  {
    dstr.writeShort (access_flags);
    dstr.writeShort (name_index);
    dstr.writeShort (signature_index);

    Attribute.writeAll(this, dstr);
  }

  private String signature;

  public String getSignature ()
  {
    if (signature == null)
      {
	StringBuffer buf = new StringBuffer(100);
	int args_count = arg_types.length; 
	buf.append('(');
	for (int i = 0; i < args_count; i++)
	  buf.append (arg_types[i].getSignature());
	buf.append(')');
	buf.append(return_type.getSignature());
	signature = buf.toString();
      }
    return signature;
  }

  public void setSignature (String signature)
  {
    int len = signature.length();
    if (len < 3 || signature.charAt(0) != '(')
      throw new ClassFormatError("bad method signature");
    int pos = 1;
    java.util.Stack types = new java.util.Stack();
    for (;;)
      {
	int arg_sig_len = Type.signatureLength(signature, pos);
	if (arg_sig_len < 0)
	  {
	    if (pos < len && signature.charAt(pos) == ')')
	      break;
	    throw new ClassFormatError("bad method signature");
	  }
	Type arg_type = Type.signatureToType(signature, pos, arg_sig_len);
	types.push(arg_type);
	pos += arg_sig_len;
      }
    arg_types = new Type[types.size()];
    for (int i = types.size();  --i >= 0; )
      arg_types[i] = (Type) types.pop();
    return_type = Type.signatureToType(signature, pos+1, len-pos-1);
  }

  public void setSignature (int signature_index)
  {
    CpoolUtf8 sigConstant = (CpoolUtf8)
      getConstants().getForced(signature_index, ConstantPool.UTF8);
    this.signature_index = signature_index;
    setSignature(sigConstant.string);
  }

  void assignConstants ()
  {
    ConstantPool constants = getConstants();
    if (name_index == 0 && name != null)
      name_index = constants.addUtf8(name).index;
    if (signature_index == 0)
      signature_index = constants.addUtf8(getSignature()).index;
    Attribute.assignConstants(this, classfile);
  }

  public ClassType getDeclaringClass() { return classfile; }

  public final Type getReturnType() { return return_type; }

  public final Type[] getParameterTypes() { return arg_types; }

  public final ClassType[] getExceptions()
  {
    if (exceptions == null) return null;
    return exceptions.getExceptions();
  }

  public final String getName ()
  {
    return name;
  }

  public final void setName(String name)
  {
    this.name = name;
  }

  public final void setName(int name_index)
  {
    if (name_index <= 0)
      name = null;
    else
      {
	CpoolUtf8 nameConstant = (CpoolUtf8)
	  getConstants().getForced(name_index, ConstantPool.UTF8);
	name = nameConstant.string;
      }
    this.name_index = name_index;
  }

  public final Method getNext()
  {
    return next;
  }

  public String toString()
  {
    StringBuffer sbuf = new StringBuffer(100);
    sbuf.append(getDeclaringClass().getName());
    sbuf.append('.');
    sbuf.append(name);
    if (arg_types != null)
      {
	int args_count = arg_types.length; 
	sbuf.append('(');
	for (int i = 0; i < args_count; i++)
	  {
	    if (i > 0)
	      sbuf.append(',');
	    sbuf.append (arg_types[i].getName());
	  }
	sbuf.append(')');
	sbuf.append(return_type.getName());
      }
    return sbuf.toString();
  }
};
