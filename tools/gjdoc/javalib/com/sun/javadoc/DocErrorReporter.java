/* DocErrorReporter.java -- Log errors/warnings during doc generation.
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
  * This interface provides a mechanism for a doclet to log messages
  * during its run.
  */
public interface DocErrorReporter
{

/**
  * This method prints the specified error message.
  *
  * @param err The error message to print.
  */
public abstract void
printError(String err);

/*************************************************************************/

/**
  * This method prints the specified warning message.
  *
  * @param warn The warning message to print.
  */
public abstract void
printWarning(String warn);

/*************************************************************************/

/**
  * This method prints the specifed message.
  *
  * @param msg The message to print.
  */
public abstract void
printNotice(String notice);

} // interface DocErrorReporter

