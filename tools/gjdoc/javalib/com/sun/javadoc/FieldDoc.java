/* FieldDoc.java -- Document a field
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
  * This package is used for documenting fields.
  */
public interface FieldDoc extends MemberDoc
{

/**
  * This method returns the type of this field.
  *
  * @return The type of this field.
  */
public abstract Type
type();

/*************************************************************************/

/**
  * This method tests whether or not the field is transient.
  *
  * @return <code>true</code> if the field is transient, <code>false</code>
  * otherwise.
  */
public abstract boolean
isTransient();

/*************************************************************************/

/**
  * This method tests whether or not the field is volatile.
  *
  * @return <code>true</code> if the field is volatile, <code>false</code>
  * otherwise.
  */
public abstract boolean
isVolatile();

/*************************************************************************/

/**
  * This method returns a list of all "@serialField" tags defined in this
  * field.
  *
  * @return The list of "@serialField" tags for this field.
  */
public abstract SerialFieldTag[]
serialFieldTags();

/*************************************************************************/

/**
  * This method returns the value of this static field.
  *
  * @return The value of this static field.
  */
public abstract Object
constantValue();


/*************************************************************************/

/**
  * This method returns the value of this static field converted to a
  * human-readable string.
  *
  * @return The value of this static field as a human-readable string.
  */
public abstract String
constantValueExpression();



} // interface FieldDoc

