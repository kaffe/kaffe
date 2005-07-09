/* ThrowsTag.java -- Information about "@throws" and "@exception" tags.
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
  * This interface models an "@exception" or "@throws" tag.
  */
public interface ThrowsTag extends Tag
{

/**
  * This method returns the name of the exception.
  *
  * @return The name of the exception.
  */
public abstract String
exceptionName();

/*************************************************************************/

/**
  * This method returns the comment text of the exception.
  *
  * @return The comment text of the exception.
  */
public abstract String
exceptionComment();

/*************************************************************************/

/**
  * This method returns the exception class as a <code>ClassDoc</code>.
  *
  * @return The exception class as a <code>ClassDoc</code>.
  */
public abstract ClassDoc
exception();

} // interface ThrowsTag

