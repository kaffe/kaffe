/* RootDoc.java -- Information about a javadoc run.
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
 * This interface is the root of the javadoc application. All the properties and
 * arguments are attached to the class that will implements this interface. You
 * can retrieve all the options of the tool with this interface.
 */
public interface RootDoc extends Doc, DocErrorReporter
{

  /**
   * This method returns the command line element used to invoke this instance
   * of javadoc.
   * 
   * @return The command line arguments for this run.
   */
  public abstract String[][] options();

  /** ********************************************************************** */

  /**
   * This method returns the list of packages that were specified on the command
   * line.
   * 
   * @return The packages specified on the command line.
   */
  public abstract PackageDoc[] specifiedPackages();

  /** ********************************************************************** */

  /**
   * This method returns the list of classes that were specified on the command
   * line.
   * 
   * @return The classes specified on the command line.
   */
  public abstract ClassDoc[] specifiedClasses();

  /** ********************************************************************** */

  /**
   * This method returns the list of classes and interfaces to be documented.
   * 
   * @return The list of classes and interfaces to be documented.
   */
  public abstract ClassDoc[] classes();

  /** ********************************************************************** */

  /**
   * This method returns a <code>ClassDoc</code> instance for the name class
   * or interface.
   * 
   * @param name
   *          The class or interface to look up.
   * @return The requested <code>ClassDoc</code>, or null if the specified
   *         class is not part of this javadoc run.
   */
  public abstract ClassDoc classNamed(String name);

  /** ********************************************************************** */

  /**
   * This method returns a <code>PackageDoc</code> instance for the named
   * package.
   * 
   * @param name
   *          The package to look up.
   * @return The requested <code>PackageDoc</code>, or null if the specified
   *         package is not part of this javadoc run.
   */
  public abstract PackageDoc packageNamed(String name);

} // interface RootDoc

