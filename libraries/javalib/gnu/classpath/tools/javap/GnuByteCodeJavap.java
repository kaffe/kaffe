/* gnu.classpath.tools.javah.GnuByteCodeJavap
 Copyright (C) 2005 Free Software Foundation, Inc.

 This file is part of GNU Classpath.

 GNU Classpath is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 GNU Classpath is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GNU Classpath; see the file COPYING.  If not, write to the
 Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 02110-1301 USA. */
package gnu.classpath.tools.javap;

import gnu.bytecode.Access;
import gnu.bytecode.Attribute;
import gnu.bytecode.ClassFileInput;
import gnu.bytecode.ClassType;
import gnu.bytecode.ClassTypeWriter;
import gnu.bytecode.CodeAttr;
import gnu.bytecode.Field;
import gnu.bytecode.InnerClassesAttr;
import gnu.bytecode.LineNumbersAttr;
import gnu.bytecode.LocalVarsAttr;
import gnu.bytecode.Method;
import gnu.bytecode.SourceFileAttr;
import gnu.bytecode.Type;
import gnu.bytecode.VarEnumerator;
import gnu.bytecode.Variable;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.StringTokenizer;

/**
 * 
 * @author C. Brian Jones (cbj@gnu.org)
 */
public class GnuByteCodeJavap extends Javap
{
  private static String CLASS_TABBING = "";
  private static final String COMMENT_END = "   */";
  private static final String COMMENT_START = "/*   ";
  private static final short EXCEPTIONS = 0x02;
  private static final short MODIFIERS = 0x01;
  private static final String TAB = "    ";
  private static String TABBING = TAB;
  private static final short WITHSEMICOLON = 0x04;
  private StringWriter swriter = null;

  private ClassTypeWriter getClassTypeWriter(StringBuffer buf, ClassType ctype)
  {
    swriter = new StringWriter();
    PrintWriter pwriter = new PrintWriter(swriter, true);
    ClassTypeWriter cwriter = new ClassTypeWriter(ctype, pwriter, 0);
    return cwriter;
  }

  private String getMethod(ClassType classType, Method method, int modifiers)
  {
    StringBuffer buf = new StringBuffer();
    int flags = method.getModifiers();
    if ((modifiers & MODIFIERS) != 0)
      {
        if ((flags & Access.PUBLIC) != 0)
          {
            buf.append("public ");
            if (!isShowPublic())
              {
                return null;
              }
          }
        if ((flags & Access.PRIVATE) != 0)
          {
            buf.append("private ");
            if (!isShowPrivate())
              {
                return null;
              }
          }
        if ((flags & Access.PROTECTED) != 0)
          {
            buf.append("protected ");
            if (!isShowProtected())
              {
                return null;
              }
          }
        if ((flags & Access.STATIC) != 0)
          buf.append("static ");
        if ((flags & Access.FINAL) != 0)
          buf.append("final ");
        if ((flags & Access.SYNCHRONIZED) != 0)
          buf.append("synchronized ");
        if ((flags & Access.VOLATILE) != 0)
          buf.append("volatile ");
        if ((flags & Access.TRANSIENT) != 0)
          buf.append("transient ");
        if ((flags & Access.NATIVE) != 0)
          buf.append("native ");
        if ((flags & Access.ABSTRACT) != 0)
          buf.append("abstract ");
        if ((flags & Access.INTERFACE) != 0)
          buf.append("interface ");
      }
    String methodName = method.getName();
    if (methodName.indexOf('$') != methodName.length() - 1)
      methodName = escapeInnerClass(methodName);
    // if a constructor
    if (methodName.equals("<init>"))
      {
        String classtypename = classType.getName();
        classtypename = escapeInnerClass(classtypename);
        buf.append(classtypename);
      }
    else if (methodName.equals("<clinit>"))
      {
        if ((modifiers & MODIFIERS) != 0)
          buf.append("{}");
        else
          buf.append("static {}");
      }
    else
      {
        Type returnType = method.getReturnType();
        String returntypename = returnType.getName();
        returntypename = escapeInnerClass(returntypename);
        buf.append(returntypename);
        buf.append(" ");
        buf.append(methodName);
      }
    if (!methodName.equals("<clinit>"))
      {
        buf.append("(");
        Type[] paramTypes = method.getParameterTypes();
        for (int i = 0; i < paramTypes.length; i++)
          {
            String paramtypename = paramTypes[i].getName();
            paramtypename = escapeInnerClass(paramtypename);
            buf.append(paramtypename);
            if ((i + 1) < paramTypes.length)
              buf.append(", ");
          }
        buf.append(")");
      }
    if ((modifiers & EXCEPTIONS) != 0)
      {
        ClassType[] exceptions = method.getExceptions();
        if (exceptions != null && exceptions.length > 0)
          {
            buf.append(" throws ");
            for (int i = 0; i < exceptions.length; i++)
              {
                String exceptionname = exceptions[i].getName();
                exceptionname = escapeInnerClass(exceptionname);
                buf.append(exceptionname);
                if ((i + 1) < exceptions.length)
                  buf.append(", ");
              }
          }
      }
    if ((modifiers & WITHSEMICOLON) != 0)
      buf.append(";");
    return buf.toString();
  }

  /**
   * @see gnu.classpath.tools.javap.Javap#printClassFile(java.lang.String,
   *      java.io.PrintStream)
   */
  public void printClassFile(String className, PrintStream out)
  {
    StringBuffer buf = new StringBuffer();
    ClassType classType = null;
    try
      {
        InputStream is = findClass(className);
        classType = ClassFileInput.readClassType(is);
      }
    catch (Throwable t)
      {
        out.println("Error: Class " + className + " could not be found.");
        return;
      }
    try
      {
        SourceFileAttr sourceFile = (SourceFileAttr) Attribute
          .get(classType, "SourceFile");
        if (sourceFile != null)
          {
            if (isPrintCompiledFrom())
              {
                buf.append("Compiled from \"");
                buf.append(sourceFile.getSourceFile());
                buf.append("\"");
                out.println(buf.toString());
              }
          }
      }
    catch (ClassCastException cce)
      {
      }
    buf = new StringBuffer();
    buf.append(CLASS_TABBING);
    int flags = classType.getModifiers();
    if ((flags & Access.PUBLIC) != 0)
      buf.append("public ");
    if ((flags & Access.PRIVATE) != 0)
      buf.append("private ");
    if ((flags & Access.PROTECTED) != 0)
      buf.append("protected ");
    if ((flags & Access.STATIC) != 0)
      buf.append("static ");
    if ((flags & Access.FINAL) != 0)
      buf.append("final ");
    /* synchronized flag here is for super bit... */
    //      if ((flags & Access.SYNCHRONIZED) != 0)buf.append(" synchronized");
    if ((flags & Access.VOLATILE) != 0)
      buf.append("volatile ");
    if ((flags & Access.TRANSIENT) != 0)
      buf.append("transient ");
    if ((flags & Access.NATIVE) != 0)
      buf.append("native ");
    if (!classType.isInterface())
      if ((flags & Access.ABSTRACT) != 0)
        buf.append("abstract ");
    if ((flags & Access.INTERFACE) != 0)
      buf.append("interface ");
    if (!classType.isInterface())
      buf.append("class ");
    String classtypename = classType.getName();
    classtypename = escapeInnerClass(classtypename);
    buf.append(classtypename);
    boolean show_super = false;
    ClassType superType = classType.getSuperclass();
    if (superType != null)
      {
        if (classType.isInterface())
          if (superType.getName().equals("java.lang.Object"))
            show_super = true;
        if (!show_super)
          {
            buf.append(" extends ");
            String supertypename = superType.getName();
            supertypename = escapeInnerClass(supertypename);
            buf.append(supertypename);
          }
      }
    ClassType[] interfaces = classType.getInterfaces();
    if (interfaces != null)
      {
        if (interfaces.length > 0)
          {
            if (classType.isInterface())
              buf.append(" extends ");
            else
              buf.append(" implements ");
          }
        for (int i = 0; i < interfaces.length; i++)
          {
            String interfacename = interfaces[i].getName();
            interfacename = escapeInnerClass(interfacename);
            buf.append(interfacename);
            if ((i + 1) < interfaces.length)
              buf.append(", ");
          }
      }
    if (!classType.isInterface())
      buf.append(" {");
    else
      buf.append(" "); // done only to match jdk javap on diff
    out.println(buf.toString());
    if ((flags & Access.SYNCHRONIZED) == 0)
      out.println("    /* ACC_SUPER bit NOT set */");
    if (classType.isInterface())
      out.println(CLASS_TABBING + "{");
    // output fields
    buf = null;
    Field field = classType.getFields();
    while (field != null)
      {
        buf = new StringBuffer();
        buf.append(TABBING);
        flags = field.getModifiers();
        if ((flags & Access.PUBLIC) != 0)
          {
            buf.append("public ");
            if (!isShowPublic())
              {
                field = field.getNext();
                continue;
              }
          }
        if ((flags & Access.PRIVATE) != 0)
          {
            buf.append("private ");
            if (!isShowPrivate())
              {
                field = field.getNext();
                continue;
              }
          }
        if ((flags & Access.PROTECTED) != 0)
          {
            buf.append("protected ");
            if (!isShowProtected())
              {
                field = field.getNext();
                continue;
              }
          }
        if ((flags & Access.STATIC) != 0)
          buf.append("static ");
        if ((flags & Access.FINAL) != 0)
          buf.append("final ");
        if ((flags & Access.SYNCHRONIZED) != 0)
          buf.append("synchronized ");
        if ((flags & Access.VOLATILE) != 0)
          buf.append("volatile ");
        if ((flags & Access.TRANSIENT) != 0)
          buf.append("transient ");
        if ((flags & Access.NATIVE) != 0)
          buf.append("native ");
        if ((flags & Access.ABSTRACT) != 0)
          buf.append("abstract ");
        if ((flags & Access.INTERFACE) != 0)
          buf.append("interface ");
        String fieldtypename = field.getType().getName();
        fieldtypename = escapeInnerClass(fieldtypename);
        buf.append(fieldtypename);
        buf.append(" ");
        buf.append(field.getSourceName());
        buf.append(";");
        out.println(buf.toString());
        if (isPrintSignatures())
          {
            buf = new StringBuffer();
            buf.append(TABBING);
            buf.append(TAB);
            buf.append(COMMENT_START);
            buf.append(field.getSignature());
            buf.append(COMMENT_END);
            out.println(buf.toString());
          }
        field = field.getNext();
      }
    // output methods
    buf = null;
    Method method = classType.getMethods();
    while (method != null)
      {
        buf = new StringBuffer();
        buf.append(TABBING);
        flags = 1;
        flags |= MODIFIERS | EXCEPTIONS | WITHSEMICOLON;
        String methodStr = getMethod(classType, method, flags);
        if (methodStr == null)
          {
            method = method.getNext();
            continue;
          }
        buf.append(getMethod(classType, method, flags));
        out.println(buf.toString());
        buf = null;
        if (isPrintSignatures())
          {
            buf = new StringBuffer();
            buf.append(TABBING);
            buf.append(TAB);
            buf.append(COMMENT_START);
            buf.append(method.getSignature());
            buf.append(COMMENT_END);
            out.println(buf.toString());
            buf = null;
          }
        if (isPrintStackSize() || isPrintNumberLocals() || isPrintMethodArgs())
          {
            CodeAttr code = method.getCode();
            if (code != null)
              {
                buf = new StringBuffer();
                buf.append(TABBING);
                buf.append(TAB);
                buf.append(COMMENT_START);
                if (isPrintStackSize())
                  {
                    buf.append("Stack=");
                    buf.append(code.getMaxStack());
                  }
                if (isPrintNumberLocals())
                  {
                    if (isPrintStackSize())
                      buf.append(", ");
                    buf.append("Locals=");
                    buf.append(code.getMaxLocals());
                  }
                if (isPrintMethodArgs())
                  {
                    if (isPrintStackSize() || isPrintNumberLocals())
                      buf.append(", ");
                    buf.append("Args_size=");
                    Type[] paramTypes = method.getParameterTypes();
                    String methodName = method.getName();
                    int add = 1;
                    if (methodName.equals("<clinit>"))
                      add = 0;
                    int args = paramTypes.length + add;
                    buf.append(args);
                  }
                buf.append(COMMENT_END);
                out.println(buf.toString());
                buf = null;
              }
          }
        method = method.getNext();
      }
    InnerClassesAttr innerClassAttr = (InnerClassesAttr) Attribute
      .get(classType, "InnerClasses");
    if (innerClassAttr != null)
      {
        //        String[][] innerClassNames = innerClassAttr.getClassNames();
        String[][] innerClassNames = new String[0][0];
        //    System.out.println (TABBING + "DBG: " + classType.getName () + "
        // THIS");
        //    for (int i = 0; i < innerClassNames.length; i++)
        //      {
        //        System.out.println (TABBING + "DBG: " + innerClassNames[i][0]);
        //      }
        TABBING = TABBING + TAB;
        CLASS_TABBING = CLASS_TABBING + TAB;
        for (int i = 0; i < innerClassNames.length; i++)
          {
            String tclassName = innerClassNames[i][0];
            tclassName = tclassName.replace('/', '.');
            if (tclassName.indexOf(classType.getName()) == -1)
              continue;
            if (tclassName.length() <= classType.getName().length())
              continue;
            String test = escapeInnerClass(tclassName);
            if (test.indexOf('$') != -1)
              continue;
            InputStream is = null;
            try
              {
                is = findClass(innerClassNames[i][0]);
              }
            catch (ClassNotFoundException e)
              {
                System.err.println("Class '" + innerClassNames[i][0]
                                   + "' not found");
                continue;
              }
            try
              {
                ClassType innerclassType = ClassFileInput.readClassType(is);
                flags = innerclassType.getModifiers();
                StringTokenizer st = new StringTokenizer(innerClassNames[i][1]);
                while (st.hasMoreTokens())
                  {
                    String token = st.nextToken();
                    if (token.equals("public"))
                      flags |= Access.PUBLIC;
                    else if (token.equals("private"))
                      flags |= Access.PRIVATE;
                    else if (token.equals("protected"))
                      flags |= Access.PROTECTED;
                    else if (token.equals("static"))
                      flags |= Access.STATIC;
                    else if (token.equals("final"))
                      flags |= Access.FINAL;
                    else if (token.equals("abstract"))
                      flags |= Access.ABSTRACT;
                    else if (token.equals("interface"))
                      flags |= Access.INTERFACE;
                  }
                innerclassType.setModifiers(flags);
                //            generateOutput(innerclassType, out);
              }
            catch (ClassFormatError cfe)
              {
                System.err.println("Error: " + innerClassNames[i][0] + ": "
                                   + cfe.getMessage());
              }
            catch (IOException ioe)
              {
                System.err.println("Error: " + innerClassNames[i][0] + ": "
                                   + ioe.getMessage());
              }
          }
        TABBING = TABBING.substring(0, TABBING.length() - TAB.length());
        CLASS_TABBING = CLASS_TABBING.substring(0, CLASS_TABBING.length()
                                                   - TAB.length());
      }
    // end of class
    out.println(CLASS_TABBING + "}");
    method = classType.getMethods();
    while (method != null)
      {
        if (isDisassemble() || isPrintLineNumbers() || isPrintLocalVariables())
          {
            try
              {
                CodeAttr code = (CodeAttr) Attribute.get(method, "Code");
                if (code != null)
                  {
                    if (isDisassemble())
                      {
                        buf = new StringBuffer();
                        String methodStr = getMethod(classType, method, 0);
                        if (methodStr != null)
                          {
                            buf.append("Method ");
                            buf.append(methodStr);
                            out.println("");
                            out.println(buf.toString());
                          }
                        int offset = 0;
                        int length = code.getCodeLength();
                        buf = new StringBuffer();
                        ClassTypeWriter writer = getClassTypeWriter(buf,
                                                                    classType);
                        code.disAssemble(writer, offset, length);
                        writer.flush();
                        writer.close();
                        // swriter is set in getClasTypeWriter
                        if (swriter != null)
                          out.print(swriter.toString());
                      }
                    if (isPrintLineNumbers())
                      {
                        LineNumbersAttr lineNumbers = (LineNumbersAttr) Attribute
                          .get(code, "LineNumberTable");
                        if (lineNumbers != null)
                          {
                            buf = new StringBuffer();
                            String methodStr = getMethod(classType, method, 0);
                            if (methodStr != null)
                              {
                                buf.append("Line numbers for method ");
                                buf.append(methodStr);
                                out.println("");
                                out.println(buf.toString());
                                int linenumber_count = lineNumbers
                                  .getLineCount();
                                short[] linenumber_table = lineNumbers
                                  .getLineNumberTable();
                                for (int i = 0; i < linenumber_count; i++)
                                  {
                                    out.print("   line ");
                                    out
                                      .print(linenumber_table[2 * i + 1] & 0xFFFF);
                                    out.print(": ");
                                    out
                                      .println(linenumber_table[2 * i] & 0xFFFF);
                                  }
                              }
                          }
                      }
                    if (isPrintLocalVariables())
                      {
                        try
                          {
                            LocalVarsAttr vars = (LocalVarsAttr) Attribute
                              .get(code, "LocalVariableTable");
                            if (vars != null)
                              {
                                buf = new StringBuffer();
                                String methodStr = getMethod(classType, method,
                                                             0);
                                if (methodStr != null)
                                  {
                                    buf.append("Local variables for method ");
                                    buf.append(methodStr);
                                    out.println("");
                                    out.println(buf.toString());
                                    VarEnumerator varEnum = vars.allVars();
                                    Variable var = varEnum.nextVar();
                                    while (var != null)
                                      {
                                        buf = new StringBuffer();
                                        buf.append("   ");
                                        buf.append(var.getType().getName());
                                        buf.append(" ");
                                        buf.append(var.getName());
                                        buf.append("  pc=");
                                        //                        buf.append(var.getStartPC());
                                        buf.append(", length=");
                                        //                        buf.append(var.getEndPC() -
                                        // var.getStartPC());
                                        buf.append(", slot=");
                                        //                        buf.append(var.getOffset());
                                        out.println(buf.toString());
                                        var = varEnum.nextVar();
                                      }
                                  }
                              }
                          }
                        catch (ClassCastException cce)
                          {
                          }
                      }
                  } // if (code != null)
              }
            catch (ClassCastException cce)
              {
              }
          }
        method = method.getNext();
      }
  }
}