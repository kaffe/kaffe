/* MethodDoc.java -- Document a method
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
  * This interface is used for documenting ordinary (ie, non-constructor)
  * methods.
  */
public interface MethodDoc extends ExecutableMemberDoc
{

/**
  * This method tests whether or not the method to be documented is abstract.
  *
  * @return <code>true</code> if the method is abstract, <code>false</code>
  * otherwise.
  */
public abstract boolean
isAbstract();

/*************************************************************************/

/**
  * This method returns the return type of the method to be documented.
  *
  * @return The return type of the method to be documented.
  */
public abstract Type
returnType();

/*************************************************************************/

/**
  * This method returns the class containing the method that this method is
  * overriding.
  *
  * @return The class containing the method that this method is overriding,
  * or <code>null</code> if this class is not overriding a method.
  */
public abstract ClassDoc
overriddenClass();

} // interface MethodDoc

