/*
  Copyright (c) 2001, 2003 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details. */

package gnu.classpath.tools.rmi.rmic;

/** Subclass of Compiler that can be used to invoke gcj.  */
public class Compile_gcj extends CompilerProcess
{
  private static final String [] COMPILER_ARGS = 
  {
    "gcj",
    "-C"
  };

  public String[] computeArguments (String filename)
  {
    return computeTypicalArguments(COMPILER_ARGS,
				   getDestination(),
				   filename);
  }
}
