// Copyright (c) 1997, 1998, 1999, 2001, 2003, 2004  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/**
  * Represents the contents of a standard "Code" attribute.
  * <p>
  * Most of the actual methods that generate bytecode operation
  * are in this class (typically with names starting with <code>emit</code>),
  * though there are also some in <code>Method</code>.
  * <p>
  * Note that a <code>CodeAttr</code> is an <code>Attribute</code>
  * of a <code>Method</code>, and can in turn contain other
  * <code>Attribute</code>s, such as a <code>LineNumbersAttr</code>.
  *
  * @author      Per Bothner
  */

public class CodeAttr extends Attribute implements AttrContainer
{
  Attribute attributes;
  public final Attribute getAttributes () { return attributes; }
  public final void setAttributes (Attribute attributes)
  { this.attributes = attributes; }
  LineNumbersAttr lines;
  public LocalVarsAttr locals;

  SourceDebugExtAttr sourceDbgExt;

  // In hindsight, maintaining stack_types is more hassle than it is worth.
  // Instead, better to just keep track of SP, which should catch most
  // stack errors, while being more general and less hassle.  FIXME.
  Type[] stack_types;

  int SP;  // Current stack size (in "words")
  private int max_stack;
  private int max_locals;
  /** Current active length of code array.
   * Note that processFixups may expand/contract the code array.  */
  int PC;
  byte[] code;

  /* The exception handler table, as a vector of quadruples
     (start_pc, end_pc, handler_pc, catch_type).
     Only the first exception_table_length quadruples are defined. */
  short[] exception_table;

  /* The number of (defined) exception handlers (i.e. quadruples)
     in exception_table. */
  int exception_table_length;

  /** Not a fixup - a no-op. */
  static final int FIXUP_NONE = 0;
  /** The definition of a label. */
  static final int FIXUP_DEFINE = 1;
  /** The offset points to a tableswitch/lookupswitch - handle padding. */
  static final int FIXUP_SWITCH = 2;
  /** The offset contains a label relative to the previous FIXUP_SWITCH. */
  static final int FIXUP_CASE = 3;
  /** The offset points to a goto instruction.
   * This case up to FIXUP_TRANSFER2 must be contiguous
   * - see the jump-to-jump optimization in processFixups. */
  static final int FIXUP_GOTO = 4;
  /** The offset points to a jsr instruction. */
  static final int FIXUP_JSR = 5;
  /** The offset points to a conditional transfer (if_xxx) instruction. */
  static final int FIXUP_TRANSFER = 6;
  /** A FIXUP_GOTO_, FIXUP_JSR, or FIXUP_TRANSFER that uses a 2-byte offset. */
  static final int FIXUP_TRANSFER2 = 7;
  /** The offsets points to 3 bytes that should be deleted. */
  static final int FIXUP_DELETE3 = 8;
  /** The following instructions are moved to later in the code stream.
   * Instead the instructions starting at the fixup label are patched here.
   * (If the fixup label is null, we're done.)
   * This allows re-arranging code to avoid unneeded gotos.
   * The following instruction is the target of a later FIXUP_MOVE,
   * and we'll insert then when we get to it. */
  static final int FIXUP_MOVE = 9;
  /** The following instructions are moved to the end of the code stream.
   * Same as FIXUP_MOVE, but there is no explicit later FIXUP_MOVE that
   * refers to the following instructions.  Created by beginFragment.
   * The fixup_offset points to the end of the fragment.
   * (The first processFixups patches these to FIXUP_MOVE.) */
  static final int FIXUP_MOVE_TO_END = 10;
  /** FIXUP_TRY with the following FIXUP_CATCH marks an exception handler.
   * The label is the start of the try clause;
   * the current offset marks the exception handler. */
  static final int FIXUP_TRY = 11;
  /** Second half of a FIXUP_TRY/FIXUP_CATCH pair.
   * The label is the ed of the try clause;
   * the current offset is the exception type as a constant pool index. */
  static final int FIXUP_CATCH = 12;
  /** With following FIXUP_LINE_NUMBER associates an offset with a line number.
   * The fixup_offset is the code location; the fixup_label is null. */
  static final int FIXUP_LINE_PC = 13;
  /** With preceding FIXUP_LINE_PC associates an offset with a line number.
   * The fixup_offset is the line number; the fixup_label is null. */
  static final int FIXUP_LINE_NUMBER = 14;
  int[] fixup_offsets;
  Label[] fixup_labels;
  int fixup_count;

  /** This causes a later processFixup to rearrange the code.
   * The codet a target comes here, nstead of the following instructions. */
  public final void fixupChain (Label here, Label target)
  {
    fixupAdd(CodeAttr.FIXUP_MOVE, 0, target);
    here.define(this);
  }

  /** Add a fixup at this location.
   * @param kind one of the FIXUP_xxx codes.
   * @param label varies - typically the target of jump. */
  public final void fixupAdd (int kind, Label label)
  {
    fixupAdd(kind, PC, label);
  }

  final void fixupAdd (int kind, int offset, Label label)
  {
    int count = fixup_count;
    if (count == 0)
      {
	fixup_offsets = new int[30];
	fixup_labels = new Label[30];
      }
    else if (fixup_count == fixup_offsets.length)
      {
	int new_length = 2 * count;
	Label[] new_labels = new Label[new_length];
	System.arraycopy (fixup_labels, 0, new_labels, 0, count);
	fixup_labels = new_labels;
	int[] new_offsets = new int[new_length];
	System.arraycopy (fixup_offsets, 0, new_offsets, 0, count);
	fixup_offsets = new_offsets;
      }
    fixup_offsets[count] = (offset << 4) | kind;
    fixup_labels[count] = label;
    fixup_count = count + 1;
  }

  private final int fixupOffset(int index)
  {
    return fixup_offsets[index] >> 4;
  }

  private final int fixupKind(int index)
  {
    return fixup_offsets[index] & 15;
  }

  /** The stack of currently active conditionals. */
  IfState if_stack;

  /** The stack of currently active try statements. */
  TryState try_stack;

  public final Method getMethod() { return (Method) getContainer(); }

  public final int getPC() { return PC; }

  public final int getSP() { return SP; }

  public final ConstantPool getConstants ()
  {
    return getMethod().classfile.constants;
  }

  /* True if we cannot fall through to bytes[PC] -
     the previous instruction was an uncondition control transfer.  */
  private boolean unreachable_here;
  /** True if control could reach here. */
  public final boolean reachableHere () { return !unreachable_here; }
  public final void setReachable(boolean val) { unreachable_here = !val; }
  public final void setUnreachable() { unreachable_here = true; }

  /** Get the maximum number of words on the operand stack in this method. */
  public int getMaxStack() { return max_stack; }
  /** Get the maximum number of local variable words in this method. */
  public int getMaxLocals() { return max_locals; }

  /** Set the maximum number of words on the operand stack in this method. */
  public void setMaxStack(int n) { max_stack = n; }
  /** Set the maximum number of local variable words in this method. */
  public void setMaxLocals(int n) { max_locals = n; }

  /** Get the code (instruction bytes) of this method.
    * Does not make a copy. */
  public byte[] getCode() { return code; }
  /** Set the code (instruction bytes) of this method.
    * @param code the code bytes (which are not copied).
    * Implicitly calls setCodeLength(code.length). */
  public void setCode(byte[] code) {
    this.code = code; this.PC = code.length; }
  /** Set the length the the code (instruction bytes) of this method.
    * That is the number of current used bytes in getCode().
    * (Any remaing bytes provide for future growth.) */
  public void setCodeLength(int len) { PC = len;}
  /** Set the current lengthof the code (instruction bytes) of this method. */
  public int getCodeLength() { return PC; }

  public CodeAttr (Method meth)
  {
    super ("Code");
    addToFrontOf(meth);
    meth.code = this;
  }

  public final void reserve (int bytes)
  {
    if (code == null)
      code = new byte[100+bytes];
    else if (PC + bytes > code.length)
      {
	byte[] new_code = new byte[2 * code.length + bytes];
	System.arraycopy (code, 0, new_code, 0, PC);
	code = new_code;
      }
  }

  /** Get opcode that implements NOT (x OPCODE y). */
  byte invert_opcode (byte opcode)
  {
    if ((opcode >= 153 && opcode <= 166)
	|| (opcode >= 198 && opcode <= 199))
      return (byte) (opcode ^ 1);
    throw new Error("unknown opcode to invert_opcode");
  }

  /**
   * Write an 8-bit byte to the current code-stream.
   * @param i the byte to write
   */
  public final void put1(int i)
  {
    code[PC++] = (byte) i;
    unreachable_here = false;
  }

  /**
   * Write a 16-bit short to the current code-stream
   * @param i the value to write
   */
  public final void put2(int i)
  {
    code[PC++] = (byte) (i >> 8);
    code[PC++] = (byte) (i);
    unreachable_here = false;
  }

  /**
   * Write a 32-bit int to the current code-stream
   * @param i the value to write
   */
  public final void put4(int i)
  {
    code[PC++] = (byte) (i >> 24);
    code[PC++] = (byte) (i >> 16);
    code[PC++] = (byte) (i >> 8);

    code[PC++] = (byte) (i);
    unreachable_here = false;
  }

  public final void putIndex2 (CpoolEntry cnst)
  {
    put2(cnst.index);
  }

  public final void putLineNumber (String filename, int linenumber)
  {
    getMethod().classfile.setSourceFile(filename);
    putLineNumber(linenumber);
  }

  public final void putLineNumber (int linenumber)
  {
    if (sourceDbgExt != null)
      linenumber = sourceDbgExt.fixLine(linenumber);
    fixupAdd(FIXUP_LINE_PC, null);
    fixupAdd(FIXUP_LINE_NUMBER, linenumber, null);
  }

  public final void pushType(Type type)
  {
    if (type.size == 0)
      throw new Error ("pushing void type onto stack");
    if (stack_types == null)
      stack_types = new Type[20];
    else if (SP + 1 >= stack_types.length) {
      Type[] new_array = new Type[2 * stack_types.length];
      System.arraycopy (stack_types, 0, new_array, 0, SP);
      stack_types = new_array;
    }
    if (type.size == 8)
      stack_types[SP++] = Type.void_type;
    stack_types[SP++] = type;
    if (SP > max_stack)
      max_stack = SP;
  }

  public final Type popType ()
  {
    if (SP <= 0)
      throw new Error("popType called with empty stack "+getMethod());
    Type type = stack_types[--SP];
    if (type.size == 8)
      if (! popType().isVoid())
	throw new Error("missing void type on stack");
    return type;
  }

  public final Type topType ()
  {
    return stack_types[SP - 1];
  }

  /** Compile code to pop values off the stack (and ignore them).
   * @param nvalues the number of values (not words) to pop
   */
  public void emitPop (int nvalues)
  {
    for ( ; nvalues > 0;  --nvalues)
      {
        reserve(1);
	Type type = popType();
	if (type.size > 4)
	  put1(88);  // pop2
	else if (nvalues > 1)
	  { // optimization:  can we pop 2 4-byte words using a pop2
	    Type type2 = popType();
	    if (type2.size > 4)
	      {
		put1(87);  // pop
		reserve(1);
	      }
	    put1(88);  // pop2
	    --nvalues;
	  }
	else
	  put1(87); // pop
      }
  }

  /** Get a new Label for the current location.
   * Unlike Label.define, Does not change reachableHere().
   */
  public Label getLabel ()
  {
    boolean  unreachable = unreachable_here;
    Label label = new Label();
    label.define(this);
    unreachable_here = unreachable;
    return label;
  }

  public void emitSwap ()
  {
    reserve(1);
    Type type1 = popType();
    Type type2 = popType();

    if (type1.size > 4 || type2.size > 4)
      {
	// There is no swap instruction in the JVM for this case.
	// Fall back to a more convoluted way.
	pushType(type2);
	pushType(type1);
	emitDupX();
	emitPop(1);
      }
    else
      {
	pushType(type1);
	put1(95);  // swap
	pushType(type2);
      }
  }

  /** Emit code to duplicate the top element of the stack. */
  public void emitDup ()
  {
    reserve(1);

    Type type = topType();
    put1 (type.size <= 4 ? 89 : 92); // dup or dup2
    pushType (type);
  }

  /** Emit code to duplicate the top element of the stack
      and place the copy before the previous element. */
  public void emitDupX ()
  {
    reserve(1);

    Type type = popType();
    Type skipedType = popType();

    if (skipedType.size <= 4)
      put1 (type.size <= 4 ? 90 : 93); // dup_x1 or dup2_x1
    else
      put1 (type.size <= 4 ? 91 : 94); // dup_x2 or dup2_x2

    pushType (type);
    pushType (skipedType);
    pushType (type);
  }

  /** Compile code to duplicate with offset.
   * @param size the size of the stack item to duplicate (1 or 2)
   * @param offset where to insert the result (must be 0, 1, or 2)
   * The new words get inserted at stack[SP-size-offset]
   */
  public void emitDup (int size, int offset)
  {
    if (size == 0)
      return;
    reserve(1);
    // copied1 and (optionally copied2) are the types of the duplicated words
    Type copied1 = popType ();
    Type copied2 = null;
    if (size == 1)
      {
	if (copied1.size > 4)
	  throw new Error ("using dup for 2-word type");
      }
    else if (size != 2)
      throw new Error ("invalid size to emitDup");
    else if (copied1.size <= 4)
      {
	copied2 = popType();
	if (copied2.size > 4)
	  throw new Error ("dup will cause invalid types on stack");
      }

    int kind;
    // These are the types of the words (in any) that are "skipped":
    Type skipped1 = null;
    Type skipped2 = null;
    if (offset == 0)
      {
	kind = size == 1 ? 89 : 92;  // dup or dup2
      }
    else if (offset == 1)
      {
	kind = size == 1 ? 90 : 93; // dup_x1 or dup2_x1
	skipped1 = popType ();
	if (skipped1.size > 4)
	  throw new Error ("dup will cause invalid types on stack");
      }
    else if (offset == 2)
      {
	kind = size == 1 ? 91 : 94; // dup_x2 or dup2_x2
	skipped1 = popType();
	if (skipped1.size <= 4)
	  {
	    skipped2 = popType();
	    if (skipped2.size > 4)
	      throw new Error ("dup will cause invalid types on stack");
	  }
      }
    else
      throw new Error ("emitDup:  invalid offset");

    put1(kind);
    if (copied2 != null)
      pushType(copied2);
    pushType(copied1);
    if (skipped2 != null)
      pushType(skipped2);
    if (skipped1 != null)
      pushType(skipped1);
    if (copied2 != null)
      pushType(copied2);
    pushType(copied1);
  }

  /**
   * Compile code to duplicate the top 1 or 2 words.
   * @param size number of words to duplicate
   */
  public void emitDup (int size)
  {
    emitDup(size, 0);
  }

  public void emitDup (Type type)
  {
    emitDup(type.size > 4 ? 2 : 1, 0);
  }

  public void enterScope (Scope scope)
  {
    scope.setStartPC(this);
    locals.enterScope(scope);
  }

  public Scope pushScope () {
    Scope scope = new Scope ();
    if (locals == null)
      locals = new LocalVarsAttr(getMethod());
    locals.enterScope(scope);
    if (locals.parameter_scope == null) 
      locals.parameter_scope = scope;
    return scope;
  }

  public Scope getCurrentScope()
  {
    return locals.current_scope;
  }

  public Scope popScope () {
    Scope scope = locals.current_scope;
    locals.current_scope = scope.parent;
    scope.freeLocals(this);
    scope.end = getLabel();
    return scope;
  }

  /** Get the index'th parameter. */
  public Variable getArg (int index)
  {
    return locals.parameter_scope.getVariable(index);
  }

  /**
   * Search by name for a Variable
   * @param name name to search for
   * @return the Variable, or null if not found (in any scope of this Method).
   */
  public Variable lookup (String name)
  {
    Scope scope = locals.current_scope;
    for (; scope != null;  scope = scope.parent)
      {
	Variable var = scope.lookup (name);
	if (var != null)
	  return var;
      }
    return null;
  }

  /** Add a new local variable (in the current scope).
   * @param type type of the new Variable.
   * @return the new Variable. */
  public Variable addLocal (Type type)
  {
    return locals.current_scope.addVariable(this, type, null);
  }

  /** Add a new local variable (in the current scope).
   * @param type type of the new Variable.
   * @param name name of the new Variable.
   * @return the new Variable. */
  public Variable addLocal (Type type, String name)
  {
    return locals.current_scope.addVariable (this, type, name);
  }

  /** Call addLocal for parameters (as implied by method type). */
  public void addParamLocals()
  {
    Method method = getMethod();
    if ((method.access_flags & Access.STATIC) == 0)
      addLocal(method.classfile).setParameter(true);
    int arg_count = method.arg_types.length;
    for (int i = 0;  i < arg_count;  i++)
      addLocal(method.arg_types[i]).setParameter(true);
  }

  public final void emitPushConstant(int val, Type type)
  {
    switch (type.getSignature().charAt(0))
      {
      case 'B':  case 'C':  case 'I':  case 'Z':  case 'S':
	emitPushInt(val);  break;
      case 'J':
	emitPushLong((long)val);  break;
      case 'F':
	emitPushFloat((float)val);  break;
      case 'D':
	emitPushDouble((double)val);  break;
      default:
	throw new Error("bad type to emitPushConstant");
      }
  }

  /* Low-level method to pust a ConstantPool entry.
   * Does not do the appropriatre <code>pushType</code>. */
  public final void emitPushConstant (CpoolEntry cnst)
  {
    reserve(3);
    int index = cnst.index;
    if (cnst instanceof CpoolValue2)
      {
      	put1 (20); // ldc2_w
	put2 (index);
      }
    else if (index < 256)
      {
	put1(18); // ldc
	put1(index);
      }
    else
      {
	put1(19); // ldc_w
	put2(index);
      }
  }

  public final void emitPushInt(int i)
  {
    reserve(3);
    if (i >= -1 && i <= 5)
      put1(i + 3);  // iconst_m1 .. iconst_5
    else if (i >= -128 && i < 128)
      {
	put1(16); // bipush
	put1(i);
      }
    else if (i >= -32768 && i < 32768)
      {
	put1(17); // sipush
	put2(i);
      }
    else
      {
	emitPushConstant(getConstants().addInt(i));
      }
    pushType(Type.int_type);
  }

  public void emitPushLong (long i)
  {
    if (i == 0 || i == 1)
      {
	reserve(1);
	put1 (9 + (int) i);  // lconst_0 .. lconst_1
      }
    else if ((long) (int) i == i)
      {
	emitPushInt ((int) i);
	reserve(1);
	popType();
	put1 (133); // i2l
      }
    else
      {
	emitPushConstant(getConstants().addLong(i));
      }
    pushType(Type.long_type);
  }

  public void emitPushFloat (float x)
  {
    int xi = (int) x;
    if ((float) xi == x && xi >= -128 && xi < 128)
      {
	if (xi >= 0 && xi <= 2)
	  {
	    reserve(1);
	    put1(11 + xi);  // fconst_0 .. fconst_2
	    if (xi == 0 && Float.floatToIntBits(x) != 0) // x == -0.0
	      {
		reserve(1);
		put1(118);  // fneg
	      }
	  }
	else
	  {
	    // Saves space in the constant pool
	    // Probably faster, at least on modern CPUs.
	    emitPushInt (xi);
	    reserve(1);
	    popType();
	    put1 (134); // i2f
	  }
      }
    else
      {
	emitPushConstant(getConstants().addFloat(x));
      }
    pushType(Type.float_type);
  }

  public void emitPushDouble (double x)
  {
    int xi = (int) x;
    if ((double) xi == x && xi >= -128 && xi < 128)
      {
	if (xi == 0 || xi == 1)
	  {
	    reserve(1);
	    put1(14+xi);  // dconst_0 or dconst_1
	    if (xi == 0 && Double.doubleToLongBits(x) != 0L) // x == -0.0
	      {
		reserve(1);
		put1(119);  // dneg
	      }
	  }
	else
	  {
	    // Saves space in the constant pool
	    // Probably faster, at least on modern CPUs.
	    emitPushInt (xi);
	    reserve(1);
	    popType();
	    put1 (135); // i2d
	  }
      }
    else
      {
	emitPushConstant(getConstants().addDouble(x));
      }
    pushType(Type.double_type);
  }

  /** Calculate how many CONSTANT_String constants we need for a string.
   * Each CONSTANT_String can be at most 0xFFFF bytes (as a UTF8 string).
   * Returns a String, where each char, coerced to an int, is the length
   * of a substring of the input that is at most 0xFFFF bytes.
   */
  public final String calculateSplit (String str)
  {
    int strLength = str.length();
    StringBuffer sbuf = new StringBuffer(20);
    // Where the current segments starts, as an index in 'str':
    int segmentStart = 0;
    int byteLength = 0; // Length in bytes of current segment so far.
    for (int i = 0;  i < strLength; i++)
      {
	char ch = str.charAt(i);
	int bytes = ch >= 0x0800 ? 3 : ch >= 0x0080 || ch == 0 ? 2 : 1;
	if (byteLength + bytes > 0xFFFF)
	  {
	    sbuf.append((char) (i - segmentStart));
	    segmentStart = i;
	    byteLength = 0;
	  }
	byteLength += bytes;
      }
    sbuf.append((char) (strLength - segmentStart));
    return sbuf.toString();
  }

  /** Emit code to push the value of a constant String.
   * Uses CONSTANT_String and CONSTANT_Utf8 constant pool entries as needed.
   * Can handle Strings whose UTF8 length is greates than 0xFFFF bytes
   * (the limit of a CONSTANT_Utf8) by generating String concatenation.
   */
  public final void emitPushString (String str)
  {
    if (str == null)
      emitPushNull();
    else
      {
	int length = str.length();
	String segments = calculateSplit(str);
	int numSegments = segments.length();
	if (numSegments <= 1)
	  emitPushConstant(getConstants().addString(str));
	else
	  {
	    if (numSegments == 2)
	      {
		int firstSegment = (int) segments.charAt(0);
		emitPushString(str.substring(0, firstSegment));
		emitPushString(str.substring(firstSegment));
		Method concatMethod
		  = Type.string_type.getDeclaredMethod("concat", 1);
		emitInvokeVirtual(concatMethod);
	      }
	    else
	      {
		ClassType sbufType = ClassType.make("java.lang.StringBuffer");
		emitNew(sbufType);
		emitDup(sbufType);
		emitPushInt(length);
		Type[] args1 = { Type.int_type };
		emitInvokeSpecial(sbufType.getDeclaredMethod("<init>", args1));
		Type[] args2 = { Type.string_type };
		Method appendMethod
		  = sbufType.getDeclaredMethod("append", args2);
		int segStart = 0;
		for (int seg = 0;  seg < numSegments;  seg++)
		  {
		    emitDup(sbufType);
		    int segEnd = segStart + (int) segments.charAt(seg);
		    emitPushString(str.substring(segStart, segEnd));
		    emitInvokeVirtual(appendMethod);
		    segStart = segEnd;
		  }
		emitInvokeVirtual(Type.toString_method);
	      }
	    if (str == str.intern())
	      emitInvokeVirtual(Type.string_type.getDeclaredMethod("intern", 0));
	    return;
	  }
	pushType(Type.string_type);
      }
  }

  /** Push a class constant pool entry.
   * This is only supported by JDK 1.5 and later. */
  public final void emitPushClass (String name)
  {
    emitPushConstant(getConstants().addClass(name));
    pushType(Type.java_lang_Class_type);
  }

  public void emitPushNull ()
  {
    reserve(1);
    put1(1);  // aconst_null
    pushType(Type.pointer_type);
  }

  public final void emitPushThis()
  {
    reserve(1);
    put1(42);  // aload_0
    pushType(getMethod().getDeclaringClass());
  }

  /** Emit code to push a constant primitive array.
   * @param value The array value that we want the emitted code to re-create.
   * @param arrayType The ArrayType that matches value.
   */
  public final void emitPushPrimArray(Object value, ArrayType arrayType)
  {
    Type elementType = arrayType.getComponentType();
    int len = java.lang.reflect.Array.getLength(value);
    emitPushInt(len);
    emitNewArray(elementType);
    char sig = elementType.getSignature().charAt(0);
    for (int i = 0;  i < len;  i++)
      {
	long ival = 0;  float fval = 0;  double dval = 0;
	switch (sig)
	  {
	  case 'J':
	    ival = ((long[]) value)[i];
	    if (ival == 0)
	      continue;
	    break;
	  case 'I':
	    ival = ((int[]) value)[i];
	    if (ival == 0)
	      continue;
	    break;
	  case 'S':
	    ival = ((short[]) value)[i];
	    if (ival == 0)
	      continue;
	    break;
	  case 'C':
	    ival = ((char[]) value)[i];
	    if (ival == 0)
	      continue;
	    break;
	  case 'B':
	    ival = ((byte[]) value)[i];
	    if (ival == 0)
	      continue;
	    break;
	  case 'Z':
	    ival = ((boolean[]) value)[i] ? 1 : 0;
	    if (ival == 0)
	      continue;
	    break;
	  case 'F':
	    fval = ((float[]) value)[i];
	    if (fval == 0.0)
	      continue;
	    break;
	  case 'D':
	    dval = ((double[]) value)[i];
	    if (dval == 0.0)
	      continue;
	    break;
	  }
	emitDup(arrayType);
	emitPushInt(i);
	switch (sig)
	  {
	  case 'Z':
	  case 'C':
	  case 'B':
	  case 'S':
	  case 'I':
	    emitPushInt((int) ival);
	    break;
	  case 'J':
	    emitPushLong(ival);
	    break;
	  case 'F':
	    emitPushFloat(fval);
	    break;
	  case 'D':
	    emitPushDouble(dval);
	    break;
	  }
	emitArrayStore(elementType);
      }
  }



  void emitNewArray (int type_code)
  {
    reserve(2);
    put1(188);  // newarray
    put1(type_code);
  }

  public final void emitArrayLength ()
  {
    if (! (popType() instanceof ArrayType))
      throw new Error( "non-array type in emitArrayLength" );
    
    reserve(1);
    put1(190);  // arraylength
    pushType(Type.int_type);
  }

  /* Returns an integer in the range 0 (for 'int') through 4 (for object
     reference) to 7 (for 'short') which matches the pattern of how JVM
     opcodes typically depend on the operand type. */

  private int adjustTypedOp  (char sig)
  {
    switch (sig)
      {
      case 'I':  return 0;  // int
      case 'J':  return 1;  // long
      case 'F':  return 2;  // float
      case 'D':  return 3;  // double
      default:   return 4;  // object
      case 'B':
      case 'Z':  return 5;  // byte or boolean
      case 'C':  return 6;  // char
      case 'S':  return 7;  // short
      }
  }

  private int adjustTypedOp  (Type type)
  {
    return adjustTypedOp(type.getSignature().charAt(0));
  }

  private void emitTypedOp (int op, Type type)
  {
    reserve(1);
    put1(op + adjustTypedOp(type));
  }

  private void emitTypedOp (int op, char sig)
  {
    reserve(1);
    put1(op + adjustTypedOp(sig));
  }

  /** Store into an element of an array.
   * Must already have pushed the array reference, the index,
   * and the new value (in that order).
   * Stack:  ..., array, index, value => ...
   */
  public void emitArrayStore (Type element_type)
  {
    popType();  // Pop new value
    popType();  // Pop index
    popType();  // Pop array reference
    emitTypedOp(79, element_type);
  }

  /** Load an element from an array.
   * Must already have pushed the array and the index (in that order):
   * Stack:  ..., array, index => ..., value */
  public void emitArrayLoad (Type element_type)
  {
    popType();  // Pop index
    popType();  // Pop array reference
    emitTypedOp(46, element_type);
    pushType(element_type);
  }

  /**
   * Invoke new on a class type.
   * Does not call the constructor!
   * @param type the desired new object type
   */
  public void emitNew (ClassType type)
  {
    reserve(3);
    put1(187); // new
    putIndex2(getConstants().addClass(type));
    pushType(type);
  }

  /** Compile code to allocate a new array.
   * The size should have been already pushed on the stack.
   * @param element_type type of the array elements
   */
  public void emitNewArray (Type element_type, int dims)
  {
    if (popType ().promote () != Type.int_type)
      throw new Error ("non-int dim. spec. in emitNewArray");

    if (element_type instanceof PrimType)
      {
	int code;
	switch (element_type.getSignature().charAt(0))
	  {
	  case 'B':  code =  8;  break;
	  case 'S':  code =  9;  break;
	  case 'I':  code = 10;  break;
	  case 'J':  code = 11;  break;
	  case 'F':  code =  6;  break;
	  case 'D':  code =  7;  break;
	  case 'Z':  code =  4;  break;
	  case 'C':  code =  5;  break;
	  default:   throw new Error("bad PrimType in emitNewArray");
	  }
	emitNewArray(code);
      }
    else if (element_type instanceof ObjectType)
      {
	reserve(3);
	put1(189); // anewarray
	putIndex2(getConstants().addClass((ObjectType) element_type));
      }
    else if (element_type instanceof ArrayType)
    {
      reserve(4);
      put1(197); // multianewarray
      putIndex2 (getConstants ().addClass (new ArrayType (element_type)));
      if (dims < 1 || dims > 255)
	throw new Error ("dims out of range in emitNewArray");
      put1(dims);
      while (-- dims > 0) // first dim already popped
	if (popType ().promote () != Type.int_type)
	  throw new Error ("non-int dim. spec. in emitNewArray");
    }
    else
      throw new Error ("unimplemented type in emitNewArray");

    pushType (new ArrayType (element_type));
  }

  public void emitNewArray (Type element_type)
  {
    emitNewArray (element_type, 1);
  }

  // We may want to deprecate this, because it depends on popType.
  private void emitBinop (int base_code)
  {
    Type type2 = popType().promote();
    Type type1_raw = popType();
    Type type1 = type1_raw.promote();
    if (type1 != type2 || ! (type1 instanceof PrimType))
      throw new Error ("non-matching or bad types in binary operation");
    emitTypedOp(base_code, type1);
    pushType(type1_raw);
  }

  private void emitBinop (int base_code, char sig)
  {
    popType();
    popType();
    emitTypedOp(base_code, sig);
    pushType(Type.signatureToPrimitive(sig));
  }

  private void emitBinop (int base_code, Type type)
  {
    popType();
    popType();
    emitTypedOp(base_code, type);
    pushType(type);
  }

  // public final void emitIntAdd () { put1(96); popType();}
  // public final void emitLongAdd () { put1(97); popType();}
  // public final void emitFloatAdd () { put1(98); popType();}
  // public final void emitDoubleAdd () { put1(99); popType();}

  public final void emitAdd(char sig) { emitBinop (96, sig); }
  public final void emitAdd(PrimType type) { emitBinop (96, type); }
  /** @deprecated */
  public final void emitAdd () { emitBinop (96); }

  public final void emitSub(char sig) { emitBinop (100, sig); }
  public final void emitSub(PrimType type) { emitBinop (100, type); }
  /** @deprecated */
  public final void emitSub () { emitBinop (100); }

  public final void emitMul () { emitBinop (104); }
  public final void emitDiv () { emitBinop (108); }
  public final void emitRem () { emitBinop (112); }
  public final void emitAnd () { emitBinop (126); }
  public final void emitIOr () { emitBinop (128); }
  public final void emitXOr () { emitBinop (130); }

  public final void emitShl () { emitShift (120); }
  public final void emitShr () { emitShift (122); }
  public final void emitUshr() { emitShift (124); }

  private void emitShift (int base_code)
  {
    Type type2 = popType().promote();
    Type type1_raw = popType();
    Type type1 = type1_raw.promote();

    if (type1 != Type.int_type && type1 != Type.long_type)
      throw new Error ("the value shifted must be an int or a long");

    if (type2 != Type.int_type)
      throw new Error ("the amount of shift must be an int");

    emitTypedOp(base_code, type1);
    pushType(type1_raw);
  }

  public final void emitNot(Type type)
  {
    emitPushConstant(1, type);
    emitAdd();
    emitPushConstant(1, type);
    emitAnd();
  }

  public void emitPrimop (int opcode, int arg_count, Type retType)
  {
    reserve(1);
    while (-- arg_count >= 0)
      popType();
    put1(opcode);
    pushType(retType);
  }

  void emitMaybeWide (int opcode, int index)
  {
    if (index >= 256)
      {
	put1(196); // wide
	put1(opcode);
	put2(index);
      }
    else
      {
	put1(opcode);
	put1(index);
      }
  }

  /**
   * Compile code to push the contents of a local variable onto the statck.
   * @param var The variable whose contents we want to push.
   */
  public final void emitLoad (Variable var)
  {
    if (var.dead())
      throw new Error("attempting to push dead variable");
    int offset = var.offset;
    if (offset < 0 || !var.isSimple())
      throw new Error ("attempting to load from unassigned variable "+var
		       +" simple:"+var.isSimple()+", offset: "+offset);
    Type type = var.getType().promote();
    reserve(4);
    int kind = adjustTypedOp(type);
    if (offset <= 3)
      put1(26 + 4 * kind + offset);  // [ilfda]load_[0123]
    else
      emitMaybeWide(21 + kind, offset); // [ilfda]load
    pushType(var.getType());
  }

  public void emitStore (Variable var)
  {
   if (var.dead ())
      throw new Error ("attempting to push dead variable");
    int offset = var.offset;
    if (offset < 0 || !var.isSimple ())
      throw new Error ("attempting to store in unassigned "+var
		       +" simple:"+var.isSimple()+", offset: "+offset);
    Type type = var.getType().promote ();
    reserve(4);
    popType();
    int kind = adjustTypedOp(type);
    if (offset <= 3)
      put1(59 + 4 * kind + offset);  // [ilfda]store_[0123]
    else
      emitMaybeWide(54 + kind, offset); // [ilfda]store
  }


  public void emitInc (Variable var, short inc)
  {
    if (var.dead ())
      throw new Error ("attempting to increment dead variable");
    int offset = var.offset;
    if (offset < 0 || !var.isSimple ())
      throw new Error ("attempting to increment unassigned variable"+var.getName()
		       +" simple:"+var.isSimple()+", offset: "+offset);
    Type type = var.getType().promote ();
    reserve(6);
    if (type != Type.int_type)
      throw new Error("attempting to increment non-int variable");

    boolean wide = offset > 255 || inc > 255 || inc < -256;

    if (wide)
    {
      put1(196); // wide
      put1(132); // iinc
      put2(offset);
      put2(inc);
    }
    else
    {
      put1(132); // iinc
      put1(offset);
      put1(inc);
    }
  }
  

  private final void emitFieldop (Field field, int opcode)
  {
    reserve(3);
    put1(opcode);
    putIndex2(getConstants().addFieldRef(field));
  }

  /** Compile code to get a static field value.
   * Stack:  ... => ..., value */

  public final void emitGetStatic(Field field)
  {
    pushType(field.type);
    emitFieldop (field, 178);  // getstatic
  }

  /** Compile code to get a non-static field value.
   * Stack:  ..., objectref => ..., value */

  public final void emitGetField(Field field)
  {
    popType();
    pushType(field.type);
    emitFieldop(field, 180);  // getfield
  }

  /** Compile code to put a static field value.
   * Stack:  ..., value => ... */

  public final void emitPutStatic (Field field)
  {
    popType();
    emitFieldop(field, 179);  // putstatic
  }

  /** Compile code to put a non-static field value.
   * Stack:  ..., objectref, value => ... */

  public final void emitPutField (Field field)
  {
    popType();
    popType();
    emitFieldop(field, 181);  // putfield
  }

  /** Comptes the number of stack words taken by a list of types. */
  private int words(Type[] types)
  {
    int res = 0;
    for (int i=types.length; --i >= 0; )
      if (types[i].size > 4)
       res+=2;
      else
       res++;
    return res;
  }

  public void emitInvokeMethod (Method method, int opcode)
  {
    reserve(opcode == 185 ? 5 : 3);
    int arg_count = method.arg_types.length;
    boolean is_invokestatic = opcode == 184;
    if (is_invokestatic != ((method.access_flags & Access.STATIC) != 0))
      throw new Error
	("emitInvokeXxx static flag mis-match method.flags="+method.access_flags);
    if (!is_invokestatic)
      arg_count++;
    put1(opcode);  // invokevirtual, invokespecial, or invokestatic
    putIndex2(getConstants().addMethodRef(method));
    if (opcode == 185)  // invokeinterface
      {
	put1(words(method.arg_types)+1); // 1 word for 'this'
	put1(0);
      }
    while (--arg_count >= 0)
      popType();
    if (method.return_type.size != 0)
      pushType(method.return_type);
  }

  public void emitInvoke (Method method)
  {
    int opcode;
    if ((method.access_flags & Access.STATIC) != 0)
      opcode = 184;   // invokestatic
    else if (method.classfile.isInterface())
      opcode = 185;   // invokeinterface
    else if ("<init>".equals(method.getName()))
      opcode = 183;   // invokespecial
    else
      opcode = 182;   // invokevirtual
    emitInvokeMethod(method, opcode);
  }

  /** Compile a virtual method call.
   * The stack contains the 'this' object, followed by the arguments in order.
   * @param method the method to invoke virtually
   */
  public void emitInvokeVirtual (Method method)
  {
    emitInvokeMethod(method, 182);  // invokevirtual
  }

  public void emitInvokeSpecial (Method method)
  {
    emitInvokeMethod(method, 183);  // invokespecial
  }

  /** Compile a static method call.
   * The stack contains the the arguments in order.
   * @param method the static method to invoke
   */
  public void emitInvokeStatic (Method method)
  {
    emitInvokeMethod(method, 184);  // invokestatic
  }

  public void emitInvokeInterface (Method method)
  {
    emitInvokeMethod(method, 185);  // invokeinterface
  }
  
  final void emitTransfer (Label label, int opcode)
  {
    fixupAdd(FIXUP_TRANSFER, label);
    put1(opcode);
    PC += 2;
  }

  /** Compile an unconditional branch (goto).
   * @param label target of the branch (must be in this method).
   */
  public final void emitGoto (Label label)
  {
    fixupAdd(FIXUP_GOTO, label);
    reserve(3);
    put1(167);
    PC += 2;
    setUnreachable();
  }

  public final void emitJsr (Label label)
  {
    fixupAdd(FIXUP_JSR, label);
    reserve(3);
    put1(168);
    PC += 2;
  }

  public final void emitGotoIfCompare1 (Label label, int opcode)
  {
    popType();
    reserve(3);
    emitTransfer (label, opcode);
  }

  public final void emitGotoIfIntEqZero(Label label)
  { emitGotoIfCompare1(label, 153); }
  public final void emitGotoIfIntNeZero(Label label)
  { emitGotoIfCompare1(label, 154); }
  public final void emitGotoIfIntLtZero(Label label)
  { emitGotoIfCompare1(label, 155); }
  public final void emitGotoIfIntGeZero(Label label)
  { emitGotoIfCompare1(label, 156); }
  public final void emitGotoIfIntGtZero(Label label)
  { emitGotoIfCompare1(label, 157); }
  public final void emitGotoIfIntLeZero(Label label)
  { emitGotoIfCompare1(label, 158); }

  public final void emitGotoIfCompare2 (Label label, int logop)
  { 
    if( logop < 153 || logop > 158 )
      throw new Error ("emitGotoIfCompare2: logop must be one of ifeq...ifle");
    
    Type type2 = popType().promote();
    Type type1 = popType().promote();
    reserve(4);
    char sig1 = type1.getSignature().charAt(0);
    char sig2 = type2.getSignature().charAt(0);

    boolean cmpg = (logop == 155 || logop == 158); // iflt,ifle

    if (sig1 == 'I' && sig2 == 'I')
      logop += 6;  // iflt -> if_icmplt etc.
    else if (sig1 == 'J' && sig2 == 'J')
      put1(148);   // lcmp
    else if (sig1 == 'F' && sig2 == 'F')
      put1(cmpg ? 149 : 150);   // fcmpl/fcmpg
    else if (sig1 == 'D' && sig2 == 'D')
      put1(cmpg ? 151 : 152);   // dcmpl/dcmpg
    else if ((sig1 == 'L' || sig1 == '[')
	     && (sig2 == 'L' || sig2 == '[')
	     && logop <= 154)
      logop += 12; // ifeq->if_acmpeq, ifne->if_acmpne
    else
      throw new Error ("invalid types to emitGotoIfCompare2");

    emitTransfer (label, logop);
  }

  // binary comparisons
  /** @deprecated */
  public final void emitGotoIfEq (Label label, boolean invert)
  {
    emitGotoIfCompare2(label, invert ? 154 : 153);
  }

  /** Compile a conditional transfer if 2 top stack elements are equal. */
  public final void emitGotoIfEq (Label label)
  {
    emitGotoIfCompare2(label, 153);
  }

  /** Compile conditional transfer if 2 top stack elements are not equal. */
  public final void emitGotoIfNE (Label label)
  {
    emitGotoIfCompare2(label, 154);
  }

  public final void emitGotoIfLt(Label label)
  { emitGotoIfCompare2(label, 155); }
  public final void emitGotoIfGe(Label label)
  { emitGotoIfCompare2(label, 156); }
  public final void emitGotoIfGt(Label label)
  { emitGotoIfCompare2(label, 157); }
  public final void emitGotoIfLe(Label label)
  { emitGotoIfCompare2(label, 158); }


  /** Compile start of a conditional:
   *   <tt>if (!(<var>x</var> opcode 0)) ...</tt>.
   * The value of <var>x</var> must already have been pushed. */
  public final void emitIfCompare1 (int opcode)
  {
    IfState new_if = new IfState(this);
    if (popType().promote() != Type.int_type)
      throw new Error ("non-int type to emitIfCompare1");
    reserve(3);
    emitTransfer (new_if.end_label, opcode);
    new_if.start_stack_size = SP;
  }

  /** Compile start of conditional:  <tt>if (x != 0) ...</tt>.
   * Also use this if you have pushed a boolean value:  if (b) ... */
  public final void emitIfIntNotZero()
  {
    emitIfCompare1(153); // ifeq
  }

  /** Compile start of conditional:  <tt>if (x == 0) ...</tt>.
   * Also use this if you have pushed a boolean value:  if (!b) ... */
  public final void emitIfIntEqZero()
  {
    emitIfCompare1(154); // ifne
  }

  /** Compile start of conditional:  <tt>if (x <= 0)</tt>. */
  public final void emitIfIntLEqZero()
  {
    emitIfCompare1(157); // ifgt
  }

  /** Compile start of a conditional:  <tt>if (!(x opcode null)) ...</tt>.
   * The value of <tt>x</tt> must already have been pushed and must be of
   * reference type. */
  public final void emitIfRefCompare1 (int opcode)
  {
    IfState new_if = new IfState(this);
    if (! (popType() instanceof ObjectType))
      throw new Error ("non-ref type to emitIfRefCompare1");
    reserve(3);
    emitTransfer (new_if.end_label, opcode);
    new_if.start_stack_size = SP;
  }  
  
  /** Compile start of conditional:  if (x != null) */
  public final void emitIfNotNull()
  {
    emitIfRefCompare1(198); // ifnull
  }

  /** Compile start of conditional:  if (x == null) */
  public final void emitIfNull()
  {
    emitIfRefCompare1(199); // ifnonnull
  }  
  
  /** Compile start of a conditional:  if (!(x OPCODE y)) ...
   * The value of x and y must already have been pushed. */
  public final void emitIfIntCompare(int opcode)
  {
    IfState new_if = new IfState(this);
    popType();
    popType();
    reserve(3);
    emitTransfer(new_if.end_label, opcode);
    new_if.start_stack_size = SP;
  }

  /* Compile start of a conditional:  if (x < y) ... */
  public final void emitIfIntLt()
  {
    emitIfIntCompare(162);  // if_icmpge
  }

  /** Compile start of a conditional:  if (x != y) ...
   * The values of x and y must already have been pushed. */
  public final void emitIfNEq ()
  {
    IfState new_if = new IfState (this);
    emitGotoIfEq(new_if.end_label);
    new_if.start_stack_size = SP;
  }

  /** Compile start of a conditional:  if (x == y) ...
   * The values of x and y must already have been pushed. */
  public final void emitIfEq ()
  {
    IfState new_if = new IfState (this);
    emitGotoIfNE(new_if.end_label);
    new_if.start_stack_size = SP;
  }

  /** Compile start of a conditional:  if (x < y) ...
   * The values of x and y must already have been pushed. */
  public final void emitIfLt ()
  {
    IfState new_if = new IfState (this);
    emitGotoIfGe(new_if.end_label);
    new_if.start_stack_size = SP;
  }

  /** Compile start of a conditional:  if (x >= y) ...
   * The values of x and y must already have been pushed. */
  public final void emitIfGe ()
  {
    IfState new_if = new IfState (this);
    emitGotoIfLt(new_if.end_label);
    new_if.start_stack_size = SP;
  }

  /** Compile start of a conditional:  if (x > y) ...
   * The values of x and y must already have been pushed. */
  public final void emitIfGt ()
  {
    IfState new_if = new IfState (this);
    emitGotoIfLe(new_if.end_label);
    new_if.start_stack_size = SP;
  }

  /** Compile start of a conditional:  if (x <= y) ...
   * The values of x and y must already have been pushed. */
  public final void emitIfLe ()
  {
    IfState new_if = new IfState (this);
    emitGotoIfGt(new_if.end_label);
    new_if.start_stack_size = SP;
  }

  /** Emit a 'ret' instruction.
    * @param var the variable containing the return address */
  public void emitRet (Variable var)
  {
    int offset = var.offset;
    if (offset < 256)
      {
	reserve(2);
	put1(169);  // ret
	put1(offset);
      }
    else
      {
	reserve(4);
	put1(196);  // wide
	put1(169);  // ret
	put2(offset);
      }
  }

  public final void emitThen()
  {
    if_stack.start_stack_size = SP;
  }

  public final void emitIfThen ()
  {
    new IfState(this, null);
  }

  /** Compile start of else clause. */
  public final void emitElse ()
  {
    Label else_label = if_stack.end_label;
    Label end_label = new Label (this);
    if_stack.end_label = end_label;
    if (reachableHere ())
      {
	int growth = SP-if_stack.start_stack_size;
	if_stack.stack_growth = growth;
	if (growth > 0)
	  {
	    if_stack.then_stacked_types = new Type[growth];
	    System.arraycopy (stack_types, if_stack.start_stack_size,
			      if_stack.then_stacked_types, 0, growth);
	  }
	else
	  if_stack.then_stacked_types = new Type[0];  // ???
	emitGoto (end_label);
      }
    while (SP > if_stack.start_stack_size)
      popType();
    SP = if_stack.start_stack_size;
    if (else_label != null)
      else_label.define (this);
    if_stack.doing_else = true;    
  }

  /** Compile end of conditional. */
  public final void emitFi ()
  {
    boolean make_unreachable = false;
    if (! if_stack.doing_else)
      { // There was no 'else' clause.
	if (reachableHere ()
	    && SP != if_stack.start_stack_size)
	  throw new Error("at PC "+PC+" then clause grows stack with no else clause");
      }
    else if (if_stack.then_stacked_types != null)
      {
	int then_clause_stack_size
	  = if_stack.start_stack_size + if_stack.stack_growth;
	if (! reachableHere ())
	  {
	    if (if_stack.stack_growth > 0)
	      System.arraycopy (if_stack.then_stacked_types, 0,
				stack_types, if_stack.start_stack_size,
				if_stack.stack_growth);
	    SP = then_clause_stack_size;
	  }
	else if (SP != then_clause_stack_size)
	  throw new Error("at PC "+PC+": SP at end of 'then' was " +
			  then_clause_stack_size
			  + " while SP at end of 'else' was " + SP);
      }
    else if (unreachable_here)
      make_unreachable = true;

    if (if_stack.end_label != null)
      if_stack.end_label.define (this);
    if (make_unreachable)
      setUnreachable();
    // Pop the if_stack.
    if_stack = if_stack.previous;
  }

  public final void emitConvert (Type from, Type to)
  {
    String to_sig = to.getSignature();
    String from_sig = from.getSignature();
    int op = -1;
    if (to_sig.length() == 1 || from_sig.length() == 1)
      {
	char to_sig0 = to_sig.charAt(0);
	char from_sig0 = from_sig.charAt(0);
	if (from_sig0 == to_sig0)
	  return;
	if (from.size < 4)
	  from_sig0 = 'I';
	if (to.size < 4)
	  {
	    emitConvert(from, Type.int_type);
	    from_sig0 = 'I';
	  }
	if (from_sig0 == to_sig0)
	  return;
	switch (from_sig0)
	  {
	  case 'I':
	    switch (to_sig0)
	      {
	        case 'B':  op = 145;  break;  // i2b
	        case 'C':  op = 146;  break;  // i2c
	        case 'S':  op = 147;  break;  // i2s
		case 'J':  op = 133;  break;  // i2l
		case 'F':  op = 134;  break;  // i2f
		case 'D':  op = 135;  break;  // i2d
	      }
	    break;
	  case 'J':
	    switch (to_sig0)
	      {
		case 'I':  op = 136;  break;  // l2i
		case 'F':  op = 137;  break;  // l2f
		case 'D':  op = 138;  break;  // l2d
	      }
	    break;
	  case 'F':
	    switch (to_sig0)
	      {
		case 'I':  op = 139;  break;  // f2i
		case 'J':  op = 140;  break;  // f2l
		case 'D':  op = 141;  break;  // f2d
	      }
	    break;
	  case 'D':
	    switch (to_sig0)
	      {
		case 'I':  op = 142;  break;  // d2i
		case 'J':  op = 143;  break;  // d2l
		case 'F':  op = 144;  break;  // d2f
	      }
	    break;
	  }
      }
    if (op < 0)
      throw new Error ("unsupported CodeAttr.emitConvert");
    reserve(1);
    popType();
    put1(op);
    pushType(to);
  }

  private void emitCheckcast (Type type, int opcode)
  {
    reserve(3);
    popType();
    put1(opcode);
    if (type instanceof ArrayType)
      {
	ArrayType atype = (ArrayType) type;
	CpoolUtf8 name = getConstants().addUtf8(atype.signature);
	putIndex2(getConstants().addClass(name));
      }
    else if (type instanceof ClassType)
      {
	putIndex2(getConstants().addClass((ClassType) type));
      }
    else
      throw new Error ("unimplemented type " + type
		       + " in emitCheckcast/emitInstanceof");
  } 

  public void emitCheckcast (Type type)
  {
    if (type instanceof ClassType)
      {
	Type top = topType();
	if (top instanceof ClassType
	    && ((ClassType) top).isSubclass((ClassType) type))
	  return;
      }
    emitCheckcast(type, 192);
    pushType(type);
  }

  public void emitInstanceof (Type type)
  {
    emitCheckcast(type, 193);
    pushType(Type.boolean_type);
  }

  public final void emitThrow ()
  {
    popType();
    reserve(1);
    put1 (191);  // athrow
    setUnreachable();
  }

  public final void emitMonitorEnter ()
  {
    popType();
    reserve(1);
    put1 (194);  // monitorenter
  }

  public final void emitMonitorExit ()
  {
    popType();
    reserve(1);
    put1 (195);  // monitorexit
  }

  /** Call pending finalizer functions.
   * @param limit Only call finalizers more recent than this.
   */
  public void doPendingFinalizers (TryState limit)
  {
    TryState stack = try_stack;

    /* If a value is returned, it must be saved to a local variable,
       to prevent a verification error because of inconsistent stack sizes.
    */
    boolean saveResult = ! getMethod().getReturnType().isVoid();
    Variable result = null;

    while (stack != limit)
      {
	if (stack.finally_subr != null         // there is a finally block
	    && stack.finally_ret_addr == null) // 'return' is not inside it
	  {
	    if (saveResult && result == null)
	      {
		result = addLocal(topType());
		emitStore(result);
	      }
	    emitJsr(stack.finally_subr);
	  }

	stack = stack.previous;
      }

    if (result != null)
      emitLoad(result);
    // We'd like to do freeLocal on the result Variable, but then we risk
    // it being re-used in a finalizer, which would trash its value.  We
    // don't have any convenient way to to do that (the pending Scope
    // mechanism is over-kill), we for now we just leak the Variable.
  }

  /**
   * Compile a method return.
   * If inside a 'catch' clause, first call 'finally' clauses.
   * The return value (unless the return type is void) must be on the stack,
   * and have the correct type.
   */
  public final void emitReturn ()
  {
    doPendingFinalizers(null);
    if (getMethod().getReturnType().size == 0)
      {
	reserve(1);
	put1(177); // return
      }
    else
      emitTypedOp (172, popType().promote());
    setUnreachable();
  }

  /** Add an exception handler. */
  public void addHandler (int start_pc, int end_pc,
			  int handler_pc, int catch_type)
  {
    int index = 4 * exception_table_length;
    if (exception_table == null)
      {
	exception_table = new short[20];
      }
    else if (exception_table.length <= index)
      {
	short[] new_table = new short[2 * exception_table.length];
	System.arraycopy(exception_table, 0, new_table, 0, index);
	exception_table = new_table;
      }
    exception_table[index++] = (short) start_pc;
    exception_table[index++] = (short) end_pc;
    exception_table[index++] = (short) handler_pc;
    exception_table[index++] = (short) catch_type;
    exception_table_length++;
  }

  /** Add an exception handler. */
  public void addHandler (Label start_try, Label end_try,
			  ClassType catch_type)
  {
    ConstantPool constants = getConstants();
    int catch_type_index;
    if (catch_type == null)
      catch_type_index = 0;
    else
      catch_type_index = constants.addClass(catch_type).index;
    fixupAdd(FIXUP_TRY, start_try);
    fixupAdd(FIXUP_CATCH, catch_type_index, end_try);
  }

  /** Beginning of code that has a cleanup handler.
   * This is similar to a try-finally, but the cleanup is only
   * done in the case of an exception.  Alternatively, the try clause
   * has to manually do the cleanup with code duplication.
   * Equivalent to: <code>try <var>body</var> catch (Throwable ex) { <var>cleanup</var>; throw ex; }</code>
   * Call <code>emitWithCleanupStart</code> before the <code><var>body</var></code>.
   */
  public void emitWithCleanupStart ()
  {
    new TryState(this);
  }

  /** Called after a <code><var>body</var></code> that has a <code><var>cleanup</var></code> clause.
   * Followed by the <code><var>cleanup</var></code> code.
   */
  public void emitWithCleanupCatch (Variable catchVar)
  {
    emitTryEnd();
    try_stack.saved_result = catchVar;
    int save_SP = SP;
    emitCatchStart(catchVar);
    // Don't trash stack_types, and set things up so the SP has the
    // right value after emitWithCleanupDone (assuming the handler leaves
    // the stack empty after the throw).  The + 1 for the incoming exception.
    SP = save_SP + 1;
  }

  /** Called after generating a <code><var>cleanup</var></code> handler. */

  public void emitWithCleanupDone ()
  {
    Variable catchVar = try_stack.saved_result;
    try_stack.saved_result = null;
    if (catchVar != null)
      emitLoad(catchVar);
    emitThrow();
    emitCatchEnd();
    emitTryCatchEnd();
  }


  public void emitTryStart(boolean has_finally, Type result_type)
  {
    TryState try_state = new TryState(this);
    if (result_type != null && result_type.isVoid())
      result_type = null;
    if (result_type != null || SP > 0)
      {
	pushScope();
	if (result_type != null)
	  try_state.saved_result = addLocal(result_type);
      }
    if (SP > 0)
      {
	Variable[] savedStack = new Variable[SP];
	int i = 0;
	while (SP > 0)
	  {
	    Variable var = addLocal(topType());
	    emitStore(var);
	    savedStack[i++] = var;
	  }
	try_state.savedStack = savedStack;
      }
    if (has_finally)
      try_state.finally_subr = new Label();
  }

  public void emitTryEnd()
  {
    if (try_stack.end_label == null)
      {
	if (try_stack.saved_result != null && reachableHere())
	  emitStore(try_stack.saved_result);
	try_stack.end_label = new Label();
	if (reachableHere())
	  {
	    if (try_stack.finally_subr != null)
	      emitJsr(try_stack.finally_subr);
	    emitGoto(try_stack.end_label);
	  }
	try_stack.end_try = getLabel();
      }
  }

  public void emitCatchStart(Variable var)
  {
    emitTryEnd();
    SP = 0;
    if (try_stack.try_type != null)
      emitCatchEnd();
    ClassType type = var == null ? null : (ClassType) var.getType();
    try_stack.try_type = type;
    addHandler(try_stack.start_try, try_stack.end_try, type);
    if (var != null)
      {
	pushType(type);
	emitStore(var);
      }
    else
      pushType(Type.throwable_type);
  }

  public void emitCatchEnd()
  {
    if (reachableHere())
      {
	if (try_stack.saved_result != null)
	  emitStore(try_stack.saved_result);
	if (try_stack.finally_subr != null)
	  emitJsr(try_stack.finally_subr);
	emitGoto(try_stack.end_label);
      }
    try_stack.try_type = null;
  }

  public void emitFinallyStart()
  {
    emitTryEnd();
    if (try_stack.try_type != null)
      emitCatchEnd();
    SP = 0;
    try_stack.end_try = getLabel();

    pushScope();
    Type except_type = Type.pointer_type;
    Variable except = addLocal(except_type);
    emitCatchStart(null);
    emitStore(except);
    emitJsr(try_stack.finally_subr);
    emitLoad(except);
    emitThrow();
    
    try_stack.finally_subr.define(this);
    Type ret_addr_type = Type.pointer_type;
    try_stack.finally_ret_addr = addLocal(ret_addr_type);
    pushType(ret_addr_type);
    emitStore(try_stack.finally_ret_addr);
  }

  public void emitFinallyEnd()
  {
    emitRet(try_stack.finally_ret_addr);
    setUnreachable();
    popScope();
    try_stack.finally_subr = null;
  }

  public void emitTryCatchEnd()
  {
    if (try_stack.finally_subr != null)
      emitFinallyEnd();
    try_stack.end_label.define(this);
    Variable[] vars = try_stack.savedStack;
    if (vars != null)
      {
	for (int i = vars.length;  --i >= 0; )
	  {
	    Variable v = vars[i];
	    if (v != null) {
	      emitLoad(v);
	    }
	  }
      }
    if (try_stack.saved_result != null)
	emitLoad(try_stack.saved_result);
    if (try_stack.saved_result != null || vars != null)
	popScope();
    try_stack = try_stack.previous;
  }

  public final TryState getCurrentTry ()
  {
    return try_stack;
  }

  public final boolean isInTry()
  {
    // This also return true if we're in  a catch clause, but that is
    // good enough for now.
    return try_stack != null;
  }

  /** Compile a tail-call to position 0 of the current procedure.
   * @param pop_args if true, copy argument registers (except this) from stack.
   * @param scope Scope whose start we jump back to. */
  public void emitTailCall (boolean pop_args, Scope scope)
  {
    if (pop_args)
      {
	Method meth = getMethod();
	int arg_slots = ((meth.access_flags & Access.STATIC) != 0) ? 0 : 1;
	for (int i = meth.arg_types.length;  --i >= 0; )
	  arg_slots += meth.arg_types[i].size > 4 ? 2 : 1;
	for (int i = meth.arg_types.length;  --i >= 0; )
	  {
	    arg_slots -= meth.arg_types[i].size > 4 ? 2 : 1;
	    emitStore(locals.used [arg_slots]);
	  }
      }
    emitGoto(scope.start);
  }

  public void processFixups ()
  {
    if (fixup_count == 0)
      return;

    // For each label, set it to its maximum limit, assuming all
    // fixups causes the code the be expanded.  We need a prepass
    // for this, since FIXUP_MOVEs can cause code to be reordered.
    // Also, convert each FIXUP_MOVE_TO_END to FIXUP_MOVE.

    int delta = 0;
    int instruction_tail = fixup_count;
    fixupAdd(CodeAttr.FIXUP_MOVE, 0, null);

  loop1:
   for (int i = 0;  ;  )
      {
	int offset = fixup_offsets[i];
	int kind = offset & 15;
	offset >>= 4;
	Label label = fixup_labels[i];
	switch (kind)
	  {
	  case FIXUP_TRY:
	  case FIXUP_LINE_PC:
	    i++;
	  case FIXUP_NONE:
	  case FIXUP_CASE:
	  case FIXUP_DELETE3:
	    break;
	  case FIXUP_DEFINE:
	    label.position += delta;
	    break;
	  case FIXUP_SWITCH:
	    delta += 3;  // May need to add up to 3 padding bytes.
	    break;
	  case FIXUP_GOTO:
	    // The first test fails in this case:  GOTO L2; L1: L2:  FIXME
	    if (label.first_fixup == i + 1
		&& fixupOffset(i+1) == offset + 3)
	      {
		// Optimize: GOTO L; L:
		fixup_offsets[i] = (offset << 4) | FIXUP_DELETE3;
		fixup_labels[i] = null;
		delta -= 3;
		break;
	      }
	    // ... else fall through ...
	  case FIXUP_JSR:
	    if (PC >= 0x8000)
	      delta += 2;  // May need to convert goto->goto_w, jsr->jsr_w.
	    break;
	  case FIXUP_TRANSFER:
	    if (PC >= 0x8000)
	      delta += 5;  // May need to add a goto_w.
	    break;
	  case FIXUP_MOVE_TO_END:
	    fixup_labels[instruction_tail] = fixup_labels[i+1];
	    instruction_tail = offset;
	    // ... fall through ...
	  case FIXUP_MOVE:
	    int cur_pc = ((i+1) >= fixup_count ? PC
			  : fixupOffset(fixup_labels[i+1].first_fixup));
	    fixup_offsets[i] = (cur_pc << 4) | FIXUP_MOVE;
	    if (label == null)
	      break loop1;
	    else
	      {
		i = label.first_fixup;
		int next_pc = fixupOffset(i);
		delta = (cur_pc + delta) - next_pc;
		continue;
	      }
	  default:
	    throw new Error("unexpected fixup");
	  }
	i++;
      }
    // Next a loop to fix the position of each label, and calculate
    // the exact number of code bytes.

    // Number of bytes to be inserted or (if negative) removed, so far.
    int new_size = PC;
    // Current delta between final PC and offset in generate code array.
    delta = 0;
  loop2:
    for (int i = 0;  i < fixup_count;  )
      {
	int offset = fixup_offsets[i];
	int kind = offset & 15;
	Label label = fixup_labels[i];
	if (label != null && label.position < 0)
	  throw new Error ("undefined label "+label);
	while (label != null
	       && kind >= FIXUP_GOTO && kind <= FIXUP_TRANSFER2
	       && label.first_fixup + 1 < fixup_count
	       && (fixup_offsets[label.first_fixup + 1]
		   == ((fixup_offsets[label.first_fixup] & 15) | FIXUP_GOTO)))
	  {
	    // Optimize  JUMP L; ... L:  GOTO X
	    // (where the JUMP is any GOTO or other transfer)
	    // by changing the JUMP L to JUMP X.
	    label = fixup_labels[label.first_fixup + 1];
	    fixup_labels[i] = label;
	  }
	offset = offset >> 4;
	switch (kind)
	  {
	  case FIXUP_TRY:
	  case FIXUP_LINE_PC:
	    i++;
	  case FIXUP_NONE:
	  case FIXUP_CASE:
	    break;
	  case FIXUP_DELETE3:
	    delta -= 3;
	    new_size -= 3;
	    break;
	  case FIXUP_DEFINE:
	    label.position = offset + delta;
	    break;
	  case FIXUP_SWITCH:
	    int padding = 3 - (offset+delta) & 3;
	    delta += padding;
	    new_size += padding;
	    break;
	  case FIXUP_GOTO:
	  case FIXUP_JSR:
	  case FIXUP_TRANSFER:
	    int rel = label.position - (offset+delta);
	    if ((short) rel == rel)
	      {
		fixup_offsets[i] = (offset << 4) | FIXUP_TRANSFER2;
	      }
	    else
	      {
		delta += kind == FIXUP_TRANSFER ? 5 : 2;  // need goto_w
		new_size += kind == FIXUP_TRANSFER ? 5 : 2;  // need goto_w
	      }
	    break;
	  case FIXUP_MOVE:
	    if (label == null)
	      break loop2;
	    else
	      {
		i = label.first_fixup;
		int next_pc = fixupOffset(i);
		delta = (offset + delta) - next_pc;
		continue;
	      }
	  default:
	    throw new Error("unexpected fixup");
	  }
	i++;
      }

    byte[] new_code = new byte[new_size];
    int new_pc = 0;
    int next_fixup_index = 0;
    int next_fixup_offset = fixupOffset(0);
  loop3:
    for (int old_pc = 0;  ;  )
      {
	if (old_pc < next_fixup_offset)
	  {
	  new_code[new_pc++] = code[old_pc++];
	  }
	else
	  {
	    int kind = fixup_offsets[next_fixup_index] & 15;
	    Label label = fixup_labels[next_fixup_index];
	    switch (kind)
	      {
	      case FIXUP_NONE:
	      case FIXUP_DEFINE:
		break;
	      case FIXUP_DELETE3:
		old_pc += 3;
		break;
	      case FIXUP_TRANSFER2:
		delta = label.position - new_pc;
		new_code[new_pc++] = code[old_pc];
		new_code[new_pc++] = (byte) (delta >> 8);
		new_code[new_pc++] = (byte) (delta & 0xFF);
		old_pc += 3;
		break;
	      case FIXUP_GOTO:
	      case FIXUP_JSR:
	      case FIXUP_TRANSFER:
		delta = label.position - new_pc;
		byte opcode = code[old_pc];
		if (kind == FIXUP_TRANSFER)
		  {
		    // convert: IF_xxx L to IF_NOT_xxx Lt; GOTO L; Lt:
		    opcode = invert_opcode(opcode);
		    new_code[new_pc++] = opcode;
		    new_code[new_pc++] = 0;
		    new_code[new_pc++] = 8;  // 8 byte offset to Lt.
		    opcode = (byte) 200;  // goto_w
		  }
		else
		  {
		    // Change goto to goto_w; jsr to jsr_w:
		    opcode = (byte) (opcode + (200-167));
		  }
		new_code[new_pc++] = opcode;
		new_code[new_pc++] = (byte) (delta >> 24);
		new_code[new_pc++] = (byte) (delta >> 16);
		new_code[new_pc++] = (byte) (delta >> 8);
		new_code[new_pc++] = (byte) (delta & 0xFF);
		old_pc += 3;
		break;
	      case FIXUP_SWITCH:
		int padding = 3 - new_pc & 3;
		int switch_start = new_pc;
		new_code[new_pc++] = code[old_pc++];
		while (--padding >= 0)
		  new_code[new_pc++] = 0;
		while (next_fixup_index < fixup_count
		       && fixupKind(next_fixup_index + 1) == FIXUP_CASE)
		  {
		    next_fixup_index++;
		    int offset = fixupOffset(next_fixup_index);
		    while (old_pc < offset)
		      new_code[new_pc++] = code[old_pc++];
		    delta = (fixup_labels[next_fixup_index].position
			     - switch_start);
		    new_code[new_pc++] = (byte) (delta >> 24);
		    new_code[new_pc++] = (byte) (delta >> 16);
		    new_code[new_pc++] = (byte) (delta >> 8);
		    new_code[new_pc++] = (byte) (delta & 0xFF);
		    old_pc += 4;
		  }
		break;
	      case FIXUP_TRY:
		addHandler(fixup_labels[next_fixup_index].position,
			   fixup_labels[next_fixup_index+1].position,
			   new_pc,
			   fixupOffset(next_fixup_index+1));
		next_fixup_index++;
		break;
	      case FIXUP_LINE_PC:
		if (lines == null)
		  lines = new LineNumbersAttr(this);
		next_fixup_index++;
		lines.put(fixupOffset(next_fixup_index), new_pc);
		break;
	      case FIXUP_MOVE:
		if (label == null)
		  break loop3;
		else
		  {
		    next_fixup_index = label.first_fixup;
		    old_pc = fixupOffset(next_fixup_index);
		    next_fixup_offset = old_pc;
		    if (label.position != new_pc)
		      throw new Error("bad pc");
		    continue;
		  }
	      default:
		throw new Error("unexpected fixup");
	      }
	    next_fixup_index++;
	    next_fixup_offset = fixupOffset(next_fixup_index);
	  }
      }
    if (new_size != new_pc)
      throw new Error("PC confusion new_pc:"+new_pc+" new_size:"+new_size);
    PC = new_size;
    code = new_code;
    fixup_count = 0;
    fixup_labels = null;
    fixup_offsets = null;
  }

  public void assignConstants (ClassType cl)
  {
    super.assignConstants(cl);
    if (locals != null && locals.container == null && ! locals.isEmpty())
      locals.addToFrontOf(this);
    processFixups();
    Attribute.assignConstants(this, cl);
  }

  public final int getLength()
  {
    return 12 + getCodeLength() + 8 * exception_table_length
      + Attribute.getLengthAll(this);
  }

  public void write (DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeShort (max_stack);
    dstr.writeShort (max_locals);
    dstr.writeInt (PC);
    dstr.write (code, 0, PC);

    dstr.writeShort (exception_table_length);
    int count = exception_table_length;
    for (int i = 0;  --count >= 0;  i += 4)
      {
	dstr.writeShort(exception_table[i]);
	dstr.writeShort(exception_table[i+1]);
	dstr.writeShort(exception_table[i+2]);
	dstr.writeShort(exception_table[i+3]);
      }

    Attribute.writeAll(this, dstr);
  }

  public void print (ClassTypeWriter dst) 
  {
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.print(getLength());
    dst.print(", max_stack:");
    dst.print(max_stack);
    dst.print(", max_locals:");
    dst.print(max_locals);
    dst.print(", code_length:");
    int length = getCodeLength();
    dst.println(length);
    disAssemble(dst, 0, length);
    if (exception_table_length > 0)
      {
	dst.print("Exceptions (count: ");
	dst.print(exception_table_length);
	dst.println("):");
	int count = exception_table_length;
	for (int i = 0;  --count >= 0;  i += 4)
	  {
	    dst.print("  start: ");
	    dst.print(exception_table[i] & 0xffff);
	    dst.print(", end: ");
	    dst.print(exception_table[i+1] & 0xffff);
	    dst.print(", handler: ");
	    dst.print(exception_table[i+2] & 0xffff);
	    dst.print(", type: ");
	    int catch_type_index = exception_table[i+3] & 0xffff;
	    if (catch_type_index == 0)
	      dst.print("0 /* finally */");
	    else
	      {
		dst.printOptionalIndex(catch_type_index);
		dst.printConstantTersely(catch_type_index, ConstantPool.CLASS);
	      }
	    dst.println();
	  }
      }
    dst.printAttributes(this);
  }

  /* DEBUGGING:
  public void disAssembleWithFixups (ClassTypeWriter dst)
  {
    if (fixup_count == 0)
      {
	disAssemble(dst, 0, PC);
	return;
      }
    int prev_pc = 0;
    for (int i = 0;  i < fixup_count; )
      {
	int offset = fixup_offsets[i];
	int kind = offset & 15;
	Label label = fixup_labels[i];
	offset = offset >> 4;
	int pc = offset;
	if (kind == FIXUP_MOVE || kind == FIXUP_MOVE_TO_END)
	  pc = (i+1 >= fixup_count) ? PC : fixup_offsets[i+1] >> 4;
	disAssemble(dst, prev_pc, offset);
	prev_pc = pc;
	dst.print("fixup#");  dst.print(i);
	dst.print(" @");  dst.print(offset);
	switch (kind)
	  {
	  case FIXUP_NONE:
	    dst.println(" NONE");
	    break;
	  case FIXUP_DEFINE:
	    dst.print(" DEFINE ");
	    dst.println(label);
	    break;
	  case FIXUP_SWITCH:
	    dst.println(" SWITCH");
	    break;
	  case FIXUP_CASE:
	    dst.println(" CASE");
	    break;
	  case FIXUP_GOTO:
	    dst.print(" GOTO ");
	    dst.println(label);
	    break;
	  case FIXUP_TRANSFER:
	    dst.print(" TRANSFER ");
	    dst.println(label);
	    break;
	  case FIXUP_TRANSFER2:
	    dst.print(" TRANSFER2 ");
	    dst.println(label);
	    break;
	  case FIXUP_DELETE3:
	    dst.println(" DELETE3");
	    break;
	  case FIXUP_MOVE:
	    dst.print(" MOVE ");
	    dst.println(label);
	    break;
	  case FIXUP_MOVE_TO_END:
	    dst.print(" MOVE_TO_END ");
	    dst.println(label);
	    break;
	  case FIXUP_TRY:
	    dst.print(" TRY start: ");
	    dst.println(label);
	    i++;
	    dst.print(" - end: ");
	    dst.print(fixup_labels[i]);
	    dst.print(" type: ");
	    dst.println(fixup_offsets[i] >> 4);
	    break;
	  case FIXUP_LINE_PC:
	    dst.print(" LINE ");
	    i++;
	    dst.println(fixup_offsets[i] >> 4);
	    break;
	  default:
	    dst.println(" kind:"+fixupKind(i)+" offset:"+fixupOffset(i)+" "+fixup_labels[i]);
	  }
	i++;
      }
  }
  */

  public void disAssemble (ClassTypeWriter dst, int start, int limit)
  {
    boolean wide = false;
    for (int i = start;  i < limit; )
      {
	int oldpc = i++;
	int op = code[oldpc] & 0xff;
	String str = Integer.toString(oldpc);
	int printConstant = 0;
	int j = str.length();
	while (++j <= 3) dst.print(' ');
	dst.print(str);
	dst.print(": ");
	// We do a rough binary partition of the opcodes.
	if (op < 120)
	  {
	    if (op < 87)
	      {
		if (op < 3)  print("nop;aconst_null;iconst_m1;", op, dst);
		else if (op < 9) { dst.print("iconst_");  dst.print(op-3); }
		else if (op < 16) // op >= 9 [lconst_0] && op <= 15 [dconst_1]
		  {
		    char typ;
		    if (op < 11) { typ = 'l';  op -= 9; }
		    else if (op < 14) { typ = 'f';  op -= 11; }
		    else { typ = 'd';  op -= 14; }
		    dst.print(typ);  dst.print("const_");  dst.print(op);
		  }
		else if (op < 21)
		  {
		    if (op < 18)  // op >= 16 [bipush] && op <= 17 [sipush]
		      {
			print("bipush ;sipush ;", op-16, dst);
			int constant;
			if (op == 16)  constant = code[i++];
			else { constant = (short) readUnsignedShort(i);  i+=2;}
			dst.print(constant);
		      }
		    else // op >= 18 [ldc] && op <= 20 [ldc2_w]
		      {
			printConstant = op == 18 ? 1 : 2;
			print("ldc;ldc_w;ldc2_w;", op-18, dst);
		      }
		  }
		else // op >= 21 && op < 87
		  {
		    String load_or_store;
		    if (op < 54) { load_or_store = "load"; }
		    else { load_or_store = "store"; op -=(54-21); }
		    int index;  // -2 if array op;  -1 if index follows
		    if (op < 26) { index = -1; op -= 21; }
		    else if (op < 46) { op -= 26;  index = op % 4;  op >>= 2; }
		    else { index = -2; op -= 46; }
		    dst.print("ilfdabcs".charAt(op));
		    if (index == -2) dst.write('a');
		    dst.print(load_or_store);
		    if (index >= 0) { dst.write('_');  dst.print(index); }
		    else if (index == -1)
		      {
			if (wide) { index = readUnsignedShort(i); i += 2; }
			else { index = code[i] & 0xff; i++; }
			wide = false;
			dst.print(' ');
			dst.print(index);
		      }
		  }
	      }
	    else // op >= 87 && op < 120
	      {
		if (op < 96)
		  print("pop;pop2;dup;dup_x1;dup_x2;dup2;dup2_x1;dup2_x2;swap;"
			, op-87, dst);
		else // op >= 96 [iadd] && op <= 119 [dneg]
		  {
		    dst.print("ilfda".charAt((op-96) % 4));
		    print("add;sub;mul;div;rem;neg;", (op-96)>>2, dst);
		  }
	      }
	  }
	else // op >= 120
	  {
	    if (op < 170)
	      {
		if (op < 132) // op >= 120 [ishl] && op <= 131 [lxor]
		  {
		    dst.print((op & 1) == 0 ? 'i' : 'l');
		    print("shl;shr;ushr;and;or;xor;", (op-120)>>1, dst);
		  }
		else if (op == 132) // iinc
		  {
		    int var_index;
		    int constant;
		    dst.print("iinc");
		    if (! wide)
		      {
			var_index = 0xff & code[i++];
			constant = code[i++];
		      }
		    else
		      {
			var_index = readUnsignedShort(i);
			i += 2;
			constant = (short) readUnsignedShort(i);
			i += 2;
			wide = false;
		      }
		    dst.print(' ');  dst.print(var_index);
		    dst.print(' ');  dst.print(constant);
		  }
		else if (op < 148) // op >= 133 [i2l] && op <= 147 [i2s]
		  {
		    dst.print("ilfdi".charAt((op-133) / 3));
		    dst.print('2');
		    dst.print("lfdifdildilfbcs".charAt(op-133));
		  }
		else if (op < 153) // op >= 148 [lcmp] && op <= 152 [dcmpg]
		  print("lcmp;fcmpl;fcmpg;dcmpl;dcmpg;", op-148, dst);
		else if (op < 169)
		  {
		    if (op < 159)
		      {
			dst.print("if");
			print("eq;ne;lt;ge;gt;le;", op-153, dst);
		      }
		    else if (op < 167)
		      {
			if (op < 165) { dst.print("if_icmp"); }
			else { dst.print("if_acmp"); op -= 165-159; }
			print("eq;ne;lt;ge;gt;le;", op-159, dst);
		      }
		    else
		      print("goto;jsr;", op-167, dst);
		    int delta = (short) readUnsignedShort(i);
		    i += 2;
		    dst.print(' ');  dst.print(oldpc+delta);
		  }
		else
		  {
		    int index;
		    dst.print("ret ");
		    if (wide) { index = readUnsignedShort(i); index += 2; }
		    else { index = code[i] & 0xff; i++; }
		    wide = false;
		    dst.print(index);
		  }
	      }
	    else
	      {
		if (op < 172) //  [tableswitch] or [lookupswitch]
		  {
		    if (fixup_count == 0)
		      i = (i + 3) & ~3; // skip 0-3 byte padding.
		    int code_offset = readInt(i);  i += 4;
		    if (op == 170)
		      {
			dst.print("tableswitch");
			int low = readInt(i);  i += 4;
			int high = readInt(i);  i += 4;
			dst.print(" low: "); dst.print(low);
			dst.print(" high: "); dst.print(high);
			dst.print(" default: "); dst.print(oldpc+code_offset);
			for (;  low <= high;  low++)
			  {
			    code_offset = readInt(i);  i += 4;
			    dst.println();
			    dst.print("  ");  dst.print(low);
			    dst.print(": ");  dst.print(oldpc + code_offset); 
			  }
		      }
		    else
		      {
			dst.print("lookupswitch");
			int npairs = readInt(i);  i += 4;
			dst.print(" npairs: "); dst.print(npairs);
			dst.print(" default: "); dst.print(oldpc+code_offset);
			while (--npairs >= 0)
			  {
			    int match = readInt(i);  i += 4;
			    code_offset = readInt(i);  i += 4;
			    dst.println();
			    dst.print("  ");  dst.print(match);
			    dst.print(": ");  dst.print(oldpc + code_offset); 
			  }
		      }
		  }
		else if (op < 178) // op >= 172 [ireturn] && op <= 177 [return]
		  {
		    if (op < 177) dst.print("ilfda".charAt(op-172));
		    dst.print("return");
		  }
		else if (op < 182) // op >= 178 [getstatic] && op <= 181 [putfield]
		  {
		    print("getstatic;putstatic;getfield;putfield;", op-178, dst);
		    printConstant = 2;
		  }
		else if (op < 185) // op >= 182 && op <= 185 [invoke*]
		  {
		    dst.print("invoke");
		    print("virtual;special;static;", op-182, dst);
		    printConstant = 2;
		  }
		else if (op == 185) // invokeinterface
		  {
		    dst.print("invokeinterface (");
		    int index = readUnsignedShort(i);
		    i += 2;
		    int args = 0xff & code[i];
		    i += 2;
		    dst.print(args + " args)");
		    dst.printConstantOperand(index);
		  }
		else if (op < 196)
		  {
		    print("186;new;newarray;anewarray;arraylength;athrow;checkcast;instanceof;monitorenter;monitorexit;", op-186, dst);
		    if (op == 187 || op == 189 || op == 192 || op == 193)
		      printConstant = 2;
		    else if (op == 188)  // newarray
		      {
			int type = code[i++];
			dst.print(' ');
			if (type >= 4 && type <= 11)
			  print("boolean;char;float;double;byte;short;int;long;",
				type-4, dst);
			else
			  dst.print(type);
		      }
			
		  }
		else if (op == 196) // wide
		  {
		    dst.print("wide");
		    wide = true;
		  }
		else if (op == 197)
		  {
		    dst.print("multianewarray");
		    int index = readUnsignedShort(i);
		    i += 2;
		    dst.printConstantOperand(index);
		    int dims = 0xff & code[i++];
		    dst.print(' ');
		    dst.print(dims);
		  }
		else if (op < 200)
		  {
		    print("ifnull;ifnonnull;", op-198, dst);
		    int delta = (short) readUnsignedShort(i);
		    i += 2;
		    dst.print(' ');  dst.print(oldpc+delta);
		  }
		else if (op < 202)
		  {
		    print("goto_w;jsr_w;", op-200, dst);
		    int delta = readInt(i);
		    i += 4;
		    dst.print(' ');  dst.print(oldpc+delta);
		  }
		else
		  dst.print(op);
	      }
	  }
	if (printConstant > 0)
	  {
	    int index;
	    if (printConstant == 1) index = 0xff & code[i++];
	    else { index = readUnsignedShort(i);  i += 2; }
	    dst.printConstantOperand(index);
	  }
	dst.println();
      }
  }

  private int readUnsignedShort(int offset)
  {
    return ((0xff & code[offset]) << 8) | (0xff & code[offset+1]);
  }

  private int readInt(int offset)
  {
    return (readUnsignedShort(offset) << 16) | readUnsignedShort(offset+2);
  }

  /*
  public saveStack (ClassType into)
  {
    Field[] flds = new Field[SP];
    while (SP > 0)
      {
	Field fld = ?;
	emitStore(fld);
	flds[SP...]
      }
  }
  */

  /* Print the i'th ';'-delimited substring of str on dst. */
  private void print (String str, int i, PrintWriter dst)
  {
    int last = 0;
    int pos = -1;
    for (; i >= 0; i--)
      {
	last = ++pos;
	pos = str.indexOf(';', last);
      }
    dst.write(str, last, pos-last);
  }

  /** Return an object encapsulating the type state of the JVM stack. */
  public Type[] saveStackTypeState(boolean clear)
  {
    if (SP == 0)
      return null;
    Type[] typeState = new Type[SP];
    System.arraycopy(stack_types, 0, typeState, 0, SP);
    if (clear)
      SP = 0;
    return typeState;
  }

  /** Restore a type state as saved by saveStackTypeState. */
  public void restoreStackTypeState (Type[] save)
  {
    if (save == null)
      SP = 0;
    else
      {
	SP = save.length;
	System.arraycopy(save, 0, stack_types, 0, SP);
      }
  }

  public int beginFragment (Label start, Label after)
  {
    int i = fixup_count;
    fixupAdd(FIXUP_MOVE_TO_END, after);
    start.define(this);
    return i;
  }

  /** End a fragment.
   * @param cookie the return value from the previous beginFragment.
   */
  public void endFragment (int cookie)
  {
    fixup_offsets[cookie] = (fixup_count << 4) | FIXUP_MOVE_TO_END;
    Label after = fixup_labels[cookie];
    fixupAdd(FIXUP_MOVE, 0, null);
    after.define(this);
  }
}
