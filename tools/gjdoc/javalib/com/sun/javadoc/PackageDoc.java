/* PackageDoc.java -- Document a package
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

public interface PackageDoc extends Doc
{

/**
  * This method returns a list of all the classes and interfaces in
  * this package.  This list will included exceptions and errors.
  *
  * @return The list of classes and interfaces for this package.
  */
public abstract ClassDoc[]
allClasses();

/*************************************************************************/

/**
  * This method returns the list of ordinary classes in this package.  This
  * list will not include any interface, exceptions or errors.
  *
  * @return The list of ordinary classes in this package.
  */
public abstract ClassDoc[]
ordinaryClasses();

/*************************************************************************/

/**
  * This method returns the list of exceptions in this package.
  *
  * @return The list of exceptions in this package.
  */
public abstract ClassDoc[]
exceptions();

/*************************************************************************/

/**
  * This method returns the list of errors in this package.
  *
  * @return The list of errors in this package.
  */
public abstract ClassDoc[]
errors();

/*************************************************************************/

/**
  * This method returns the list of interfaces in this package.
  *
  * @return The list of interfaces in this package.
  */
public abstract ClassDoc[]
interfaces();

/*************************************************************************/

/**
  * This method returns a <code>ClassDoc</code> instance for the specified
  * class.
  *
  * @param name The name of the class to return.
  *
  * @return The requested <code>ClassDoc</code> or <code>null</code> if
  * this class not part of this javadoc run.
  */
public abstract ClassDoc
findClass(String cls);

} // interface PackageDoc

