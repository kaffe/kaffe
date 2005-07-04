/* gnu.classpath.tools.javap.Javap
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

import gnu.classpath.tools.Util;
import java.io.InputStream;
import java.io.PrintStream;

/**
 * 
 * @author C. Brian Jones (cbj@gnu.org)
 */
public abstract class Javap
{
  private static final String JAVAP_IMPL = "gnu.classpath.tools.javap";

  public final static Javap getInstance() throws ClassNotFoundException
  {
    String impl = "gnu.classpath.tools.javap.GnuByteCodeJavap";
    String userImpl = System.getProperty(JAVAP_IMPL);
    if (userImpl != null)
      impl = userImpl;
    Class implClass = Class.forName(impl);
    try
      {
        Object obj = implClass.newInstance();
        if (obj instanceof Javap)
          return (Javap) obj;
      }
    catch (Throwable t)
      {
      }
    throw new ClassNotFoundException("Unable to create instance of " + impl);
  }
  private boolean disassemble = false;
  private boolean output_verbose = false;
  private boolean print_compiled_from = true;
  private boolean print_line_numbers = false;
  private boolean print_local_variables = false;
  private boolean print_method_args = false;
  private boolean print_number_locals = false;
  private boolean print_signatures = false;
  private boolean print_stacksize = false;
  private boolean show_private = false;
  private boolean show_protected = true;
  private boolean show_public = true;
  private Util util = new Util();

  /**
   * Output for class names of the form foo$1 should not be made foo.1.
   */
  public String escapeInnerClass(String s)
  {
    int idx = s.lastIndexOf('$');
    String anon = s.substring(idx + 1, s.length());
    boolean is_anon = false;
    try
      {
        Integer.parseInt(anon);
        is_anon = true;
        s = s.substring(0, idx);
      }
    catch (NumberFormatException nfe)
      {
      }
    s = s.replace('$', '.');
    if (is_anon)
      s = s + "$" + anon;
    return s;
  }

  protected InputStream findClass(String className) throws ClassNotFoundException
  {
    return util.findClass(className);
  }

  public final String getSearchPath()
  {
    return util.getSearchPath();
  }

  /**
   * @return Returns true if bytecode should be shown.
   */
  public final boolean isDisassemble()
  {
    return disassemble;
  }

  public final boolean isOutputVerbose()
  {
    return output_verbose;
  }

  public boolean isPrintCompiledFrom()
  {
    return print_compiled_from;
  }

  public final boolean isPrintLineNumbers()
  {
    return print_line_numbers;
  }

  public final boolean isPrintLocalVariables()
  {
    return print_local_variables;
  }

  public final boolean isPrintMethodArgs()
  {
    return print_method_args;
  }

  public final boolean isPrintNumberLocals()
  {
    return print_number_locals;
  }

  public final boolean isPrintSignatures()
  {
    return print_signatures;
  }

  public final boolean isPrintStackSize()
  {
    return print_stacksize;
  }

  public final boolean isShowPrivate()
  {
    return show_private;
  }

  public final boolean isShowProtected()
  {
    return show_protected;
  }

  public final boolean isShowPublic()
  {
    return show_public;
  }

  /**
   * Providers will implement this method to appropriately print the given class
   * to the provided output stream in javap format. This format depends upon the
   * command line options specified.
   * 
   * @param className The name of the class that would be examined
   * @param out The output stream to write to
   */
  public abstract void printClassFile(String className, PrintStream out);

  public final void setClasspath(String path)
  {
    util.setClasspath(path);
  }

  /**
   * @param disassemble The value is true if bytecode should be shown.
   */
  public final void setDisassemble(boolean disassemble)
  {
    this.disassemble = disassemble;
  }

  public final void setOutputVerbose(boolean output_verbose)
  {
    this.output_verbose = output_verbose;
  }

  public void setPrintCompiledFrom(boolean print_compiled_from)
  {
    this.print_compiled_from = print_compiled_from;
  }

  public final void setPrintLineNumbers(boolean print_line_numbers)
  {
    this.print_line_numbers = print_line_numbers;
  }

  public final void setPrintLocalVariables(boolean print_local_variables)
  {
    this.print_local_variables = print_local_variables;
  }

  public final void setPrintMethodArgs(boolean print_method_args)
  {
    this.print_method_args = print_method_args;
  }

  public final void setPrintNumberLocals(boolean print_number_locals)
  {
    this.print_number_locals = print_number_locals;
  }

  public final void setPrintSignatures(boolean print_signatures)
  {
    this.print_signatures = print_signatures;
  }

  public final void setPrintStackSize(boolean print_stacksize)
  {
    this.print_stacksize = print_stacksize;
  }

  public final void setShowPrivate(boolean show_private)
  {
    this.show_private = show_private;
  }

  public final void setShowProtected(boolean show_protected)
  {
    this.show_protected = show_protected;
  }

  public final void setShowPublic(boolean show_public)
  {
    this.show_public = show_public;
  }
}