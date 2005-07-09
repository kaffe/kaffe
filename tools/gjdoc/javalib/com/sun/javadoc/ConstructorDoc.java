/* ConstructorDoc.java -- Document a Java class constructor
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
  * This interface is used for documenting constructors.
  */
public interface ConstructorDoc extends ExecutableMemberDoc
{

/**
  * This method returns the qualified name of the constructor. What is this
  * really?
  *
  * @return The qualified name of the constructor.
  */
public abstract String
qualifiedName();

} // interface ConstructorDoc

