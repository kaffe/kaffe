/* Parameter.java -- Information about parameters to methods.
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
  * This interface models a parameter to a method.
  */
public interface Parameter extends java.io.Serializable
{

/**
  * This method returns the type of the parameter.
  *
  * @return The parameter type.
  */
public abstract Type
type();

/*************************************************************************/

/**
  * This method returns the name of the parameter.
  *
  * @return The parameter name.
  */
public abstract String
name();

/*************************************************************************/

/**
  * This method returns the name of the type of the parameter as a
  * <code>String</code>.
  *
  * @return The name of the type of this parameter.
  */
public abstract String
typeName();

/*************************************************************************/

/**
  * This method returns this parameter as a <code>String</code> that
  * contains both the type name and parameter name.
  *
  * @return This parameter as a <code>String</code>.
  */
public abstract String
toString();

} // interaface Parameter

