/* Doclet.java -- Doclet API
   Copyright (C) 1999 Free Software Foundation, Inc.

This file is part of the com.sun.javadoc implementation of GNU Classpath.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published 
by the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation
Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307 USA. */


package com.sun.javadoc;

/**
  * This class documents the method that must be implemented by a doclet.
  * It may be used as the superclass of a doclet, but this is not required.
  * As long as the doclet implements the <code>start</code> method, all is ok.
  */
public abstract class Doclet
{

/**
  * This is the entry point to a doclet.  All doclets must implement this
  * method.
  *
  * @param rd The <code>RootDoc</code> instance for this javadoc run.
  *
  * @return <code>true</code> on success, <code>false</code> on failure.
  */
public static boolean
start(RootDoc root)
{
  return(false);
}

/*************************************************************************/

/**
  * This method returns the number of arguments to the option, including
  * the option itself.  This is not required of doclets.
  *
  * @param opt The option to check.
  *
  * @return The number of arguments to the option, or zero if the option is
  * unknown, or a negative number if an error occurred.
  */
public static int
optionLength(String opt)
{
  return(0);
}

/*************************************************************************/

/**
  * This method is called to verify that the options supplied by the caller
  * are valid.  This is not required of doclets.
  *
  * @param opts The list of options supplied by the user.
  * @param logger A mechanism for this method to report errors to the user.
  *
  * @return <code>true</code> if the options are valid, <code>false</code>
  * otherwise.
  */
public static boolean
validOptions(String[][] opts, DocErrorReporter logger)
{
  return(true);
}

} // class Doclet

