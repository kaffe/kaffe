/* SerialFieldTag.java -- Information about the "@serialField" tag.
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
  * This interface models the "@serialField" tag.
  */
public interface SerialFieldTag extends Tag, Comparable
{

/**
  * This method returns the name of the field.
  *
  * @return The name of the field.
  */
public abstract String
fieldName();

/*************************************************************************/

/**
  * This method returns the type name of the field.
  *
  * @return The type name of the field.
  */
public abstract String
fieldType();

/*************************************************************************/

/**
  * This method returns a <code>ClassDoc</code> instance for the type of
  * the field.  What about primitive types???
  *
  * @return A <code>ClassDoc</code> for the field type.
  */
public abstract ClassDoc
fieldTypeDoc();

/*************************************************************************/

/**
  * This method returns the description of the field.
  *
  * @return The description of the field.
  */
public abstract String
description();

/*************************************************************************/

/**
  * This method compares this object with the specified object in order to
  * determine proper ordering.
  *
  * @param obj The object to compare against.
  *
  * @return A negative number if this object is less than the specified
  * object, zero if the objects are equal, or a positive number if this object
  * is greater than the specified object.
  */
public abstract int
compareTo(Object obj);

} // interface SerialFieldTag

