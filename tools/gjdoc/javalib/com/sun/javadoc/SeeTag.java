/* SeeTag.java -- Information about "@see" tags.
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
  * This interface models an "@see" tag.
  */
public interface SeeTag extends Tag
{

/**
  * This method returns the label for this tag.  What is this????
  *
  * @return The label for this tag.
  */
public abstract String
label();

/*************************************************************************/

/**
  * This method returns the package of the referenced item.
  *
  * @return The package of the referenced item, or <code>null</code> if no
  * package is found.
  */
public abstract PackageDoc
referencedPackage();

/*************************************************************************/

/**
  * This method returns the name of the class referenced in the tag.
  *
  * @return The name of the class referenced in the tag.
  */
public abstract String
referencedClassName();

/*************************************************************************/

/**
  * This method returns a <code>ClassDoc</code> instance for the class
  * referenced in the tag.
  *
  * @return A <code>ClassDoc</code> for the class referenced in the tag.
  */
public abstract ClassDoc
referencedClass();

/*************************************************************************/

/**
  * This method returns the name of the member referenced in the tag.
  *
  * @return The name of the member referenced in the tag.
  */
public abstract String
referencedMemberName();

/*************************************************************************/

/**
  * This method returns a <code>MemberDoc</code> instance for the member
  * referenced in the tag.
  *
  * @return A <code>MemberDoc</code> for the member referenced in the tag.
  */
public abstract MemberDoc
referencedMember();

} // interface SeeTag

