/* TypeVariable.java -- Document a Java type variable.
   Copyright (C) 2005 Free Software Foundation, Inc.

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
 * This class represents a type variable, which is used to parameterize
 * the types used in a method or class.  For example,
 * <code>List&lt;E&gt;</code> has the type variable, <code>E</code>.  Type
 * variables may have explicit bounds, such as <code>&lt;T extends
 * Book&gt;</code>, which specifies that the type is a sub-class of
 * <code>Book</code>.
 *
 * @since 1.5
 * @author Andrew John Hughes (gnu_andrew@member.fsf.org)
 */ 
public interface TypeVariable 
  extends Type
{

  /**
   * Returns the bounds of this type variable.  These are the types
   * represented in the <code>extends</code> clause.
   *
   * @return an array of types which specify the bounds of this variable.
   *         The array is empty if there are no explicit bounds.
   */
  Type[] bounds();

  /**
   * Returns the class, interface, method or constructor in which this
   * type variable was declared.
   *
   * @return the owning program element for this type variable.
   */
  ProgramElementDoc owner();

}
