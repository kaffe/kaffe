// Copyright (c) 1997, 2004  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.IOException;

/** Class to read a ClassType from a DataInputStream (.class file).
 * @author Per Bothner
 */

public class ClassFileInput extends DataInputStream
{
  ClassType ctype;
  InputStream str;

  public ClassFileInput (InputStream str)
       throws IOException
  {
    super(str);
  }

  public ClassFileInput (ClassType ctype, InputStream str)
       throws IOException, ClassFormatError
  {
    super(str);
    this.ctype = ctype;
    if (!readHeader())
      throw new ClassFormatError("invalid magic number");
    ctype.constants = readConstants();
    readClassInfo();
    readFields();
    readMethods();
    readAttributes(ctype);
  }
 
  /** Read a class (in .class format) from an InputStream.
    * @return A new ClassType object representing the class that was read.
    */
  public static ClassType readClassType (InputStream str)
       throws IOException, ClassFormatError
  {
    ClassType ctype = new ClassType();
    ClassFileInput reader = new ClassFileInput(ctype, str);
    return ctype;
  }

  public boolean readHeader () throws IOException
  {
    int magic = readInt();
    if (magic != 0xcafebabe)
      return false;
    short minor_version = readShort();
    short major_version = readShort();
    return true;
  }

  public ConstantPool readConstants () throws IOException
  {
    return new ConstantPool(this);
  }

  public void readClassInfo () throws IOException
  {
    ctype.access_flags = readUnsignedShort();
    CpoolClass clas;
    String name;

    ctype.thisClassIndex = readUnsignedShort();
    clas = (CpoolClass) ctype.constants.getForced(ctype.thisClassIndex,
						  ConstantPool.CLASS);
    name = clas.name.string;
    ctype.this_name = name.replace('/', '.');
    ctype.setSignature("L"+name+";");

    ctype.superClassIndex = readUnsignedShort();
    if (ctype.superClassIndex == 0)
      ctype.setSuper((ClassType) null);
    else
      {
	clas = (CpoolClass) ctype.constants.getForced(ctype.superClassIndex,
						      ConstantPool.CLASS);
	name = clas.name.string;
	ctype.setSuper(name.replace('/', '.'));
      }

    int nInterfaces = readUnsignedShort();
    if (nInterfaces > 0)
      {
	ctype.interfaces = new ClassType[nInterfaces];
	ctype.interfaceIndexes = new int[nInterfaces];
	for (int i = 0;  i < nInterfaces;  i++)
	  {
	    int index = readUnsignedShort();
	    ctype.interfaceIndexes[i] = index;
	    clas = (CpoolClass) ctype.constants.getForced(index,
							  ConstantPool.CLASS);
	    name = clas.name.string.replace('/', '.');
	    ctype.interfaces[i] = ClassType.make(name);
	  }
      }
  }

  public int readAttributes (AttrContainer container) throws IOException
  {
    int count = readUnsignedShort();
    Attribute last = container.getAttributes();
    for (int i = 0;  i < count;  i++)
      {
	if (last != null)
	  {
	    for (;;)
	      {
		Attribute next = last.getNext();
		if (next == null)
		  break;
		last = next;
	      }
	  }
	
	int index = readUnsignedShort();
	CpoolUtf8 nameConstant = (CpoolUtf8)
	  ctype.constants.getForced(index, ConstantPool.UTF8);
	int length = readInt();
	nameConstant.intern();
	Attribute attr = readAttribute(nameConstant.string, length, container);
	if (attr != null)
	  {
	    if (attr.getNameIndex() == 0)
	      attr.setNameIndex(index);
	    if (last == null)
	      container.setAttributes(attr);
	    else
	      {
		if (container.getAttributes()==attr)
		  { /* Move to end. */
		    container.setAttributes(attr.getNext());
		    attr.setNext(null);
		  }
		last.setNext(attr);
	      }
	    last = attr;
	  }
      }
    return count;
  }

  public final void skipAttribute (int length)
    throws IOException
  {
    int read = 0;
    while (read < length)
      {
	int skipped = (int) skip(length - read);
	if (skipped == 0)
	  {
	    if (read() < 0)
	      throw new java.io.EOFException
		("EOF while reading class files attributes"); 
	    skipped = 1;
	  }
	read += skipped;
      }
  }

  public Attribute readAttribute (String name, int length, AttrContainer container)
    throws IOException
  {
    if (name == "SourceFile" && container instanceof ClassType)
      {
	return new SourceFileAttr(readUnsignedShort(), (ClassType) container);
      }
    else if (name == "Code" && container instanceof Method)
      {
        CodeAttr code = new CodeAttr((Method) container);
	code.setMaxStack(readUnsignedShort());
	code.setMaxLocals(readUnsignedShort());
	int code_len = readInt();
	byte[] insns = new byte[code_len];
	readFully(insns);
	code.setCode(insns);
	int exception_table_length = readUnsignedShort();
	for (int i = 0;  i < exception_table_length;  i++)
	  {
	    int start_pc = readUnsignedShort();
	    int end_pc = readUnsignedShort();
	    int handler_pc = readUnsignedShort();
	    int catch_type = readUnsignedShort();
	    code.addHandler(start_pc, end_pc, handler_pc, catch_type);
	  }
	readAttributes(code);
	return code;
      }
    else if (name == "LineNumberTable" && container instanceof CodeAttr)
      {
        int count = 2 * readUnsignedShort();
	short[] numbers = new short[count];
	for (int i = 0;  i < count;  i++)
	  {
	    numbers[i] = readShort();
	  }
	return new LineNumbersAttr(numbers, (CodeAttr) container);
      }
    else if (name == "LocalVariableTable" && container instanceof CodeAttr)
      {
	CodeAttr code = (CodeAttr) container;
	LocalVarsAttr attr = new LocalVarsAttr(code);
	Method method = attr.getMethod();
	if (attr.parameter_scope == null)
	  attr.parameter_scope = method.pushScope();
	if (attr.parameter_scope.end == null)
	  attr.parameter_scope.end = new Label(code.PC);
	Scope scope = attr.parameter_scope;
	ConstantPool constants = method.getConstants();
        int count = readUnsignedShort();
	int prev_start = scope.start.position;
	int prev_end = scope.end.position;
	for (int i = 0;  i < count;  i++)
	  {
	    Variable var = new Variable();
	    int start_pc = readUnsignedShort();
	    int end_pc = start_pc + readUnsignedShort();

	    if (start_pc != prev_start || end_pc != prev_end)
	      {
		while (scope.parent != null
		       && (start_pc < scope.start.position
			   || end_pc > scope.end.position))
		  scope = scope.parent;
		Scope parent = scope;
		scope = new Scope(new Label(start_pc), new Label(end_pc));
		scope.linkChild(parent);
		prev_start = start_pc;
		prev_end = end_pc;
	      }
	    scope.addVariable(var);
	    var.setName(readUnsignedShort(), constants);
	    var.setSignature(readUnsignedShort(), constants);
	    var.offset = readUnsignedShort();
	  }
	return attr;
      }
    else if (name == "ConstantValue" && container instanceof Field)
      {
	return new ConstantValueAttr(readUnsignedShort());
      }
    else if (name == "InnerClasses" && container instanceof ClassType)
      {
        int count = 4 * readUnsignedShort();
	short[] data = new short[count]; 
	for (int i = 0;  i < count;  i++)
	  {
	    data[i] = readShort();
	  }
	return new InnerClassesAttr(data, (ClassType) container);
     }
    else if (name == "Exceptions" && container instanceof Method)
      {
	Method meth = (Method)container;
	int count = readUnsignedShort();
        short[] exn_indices = new short[count];
	for (int i = 0; i < count; ++i)
	  exn_indices[i] = readShort();
        meth.setExceptions(exn_indices);
	return meth.getExceptionAttr();
      }
    else if (name == "SourceDebugExtension" && container instanceof ClassType)
      {
	SourceDebugExtAttr attr
	  = new SourceDebugExtAttr((ClassType) container);
	byte[] data = new byte[length];
	readFully(data, 0, length);
	attr.data = data;
	attr.dlength = length;
	return attr;
      }
    else
      {
	byte[] data = new byte[length];
	readFully(data, 0, length);
	return new MiscAttr(name, data);
      }
  }

  public void readFields () throws IOException
  {
    int nFields = readUnsignedShort();
    ConstantPool constants = ctype.constants;
    for (int i = 0;  i < nFields;  i++)
      {
	int flags = readUnsignedShort();
	int nameIndex = readUnsignedShort();
	int descriptorIndex = readUnsignedShort();
	Field fld = ctype.addField();
	fld.setName(nameIndex, constants);
	fld.setSignature(descriptorIndex, constants);
	fld.flags = flags;
	readAttributes(fld);
      }
  }

  public void readMethods () throws IOException
  {
    int nMethods = readUnsignedShort();
    ConstantPool constants = ctype.constants;
    for (int i = 0;  i < nMethods;  i++)
      {
	int flags = readUnsignedShort();
	int nameIndex = readUnsignedShort();
	int descriptorIndex = readUnsignedShort();
	Method meth = ctype.addMethod(null, flags);
	meth.setName(nameIndex);
	meth.setSignature(descriptorIndex);
	readAttributes(meth);
      }
  }
}
