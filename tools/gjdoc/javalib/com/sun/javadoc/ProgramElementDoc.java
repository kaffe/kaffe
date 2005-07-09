/* ProgramElementDoc.java -- Common ops for all program elements.
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
  * This is the comment super-interface of all items that are "program
  * elements".  This includes classes, interfaces, fields, constructors,
  * and methods.
  */
public interface ProgramElementDoc extends Doc
{

/**
  * This method returns the class which contains this element.  If this
  * is a class that is not an inner class, <code>null</code> will be
  * returned.
  *
  * @returned The class element that contains this item, or <code>null</code>
  * if this item is a class that is not an inner class.
  */
public abstract ClassDoc
containingClass();

/*************************************************************************/

/**
  * This method returns the package which contains this element.  If this
  * element is in the default generic package, then the name of the
  * package element returned will be "".
  *
  * @return The package element that contains this item.
  */
public abstract PackageDoc
containingPackage();

/*************************************************************************/

/**
  * This method returns the fully qualified name of this element.
  *
  * @return The fully qualified name of this element.
  */
public abstract String
qualifiedName();

/*************************************************************************/

/**
  * This method returns the modifier specificier number, which is what?
  *
  * @return The modifier for this element.
  */
public abstract int
modifierSpecifier();

/*************************************************************************/

/**
  * This method returns a string with the element modifiers.  For example,
  * the modifiers of a method declaration might be "protected abstract".
  *
  * @return The modifier string.
  */
public abstract String
modifiers();

/*************************************************************************/

/**
  * This method tests whether or not this element is public.
  *
  * @return <code>true</code> if this element is public, <code>false</code>
  * otherwise.
  */
public abstract boolean
isPublic();

/*************************************************************************/

/**
  * This method tests whether or not this element is protected.
  *
  * @return <code>true</code> if this element is protected, <code>false</code>
  * otherwise.
  */
public abstract boolean
isProtected();

/*************************************************************************/

/**
  * This method tests whether or not this element is private.
  *
  * @return <code>true</code> if this element is private, <code>false</code>
  * otherwise.
  */
public abstract boolean
isPrivate();

/*************************************************************************/

/**
  * This method tests whether or not this element is package private.
  *
  * @return <code>true</code> if this element is package private,
  * <code>false</code> otherwise.
  */
public abstract boolean
isPackagePrivate();

/*************************************************************************/

/**
  * This method tests whether or not this element is static.
  *
  * @return <code>true</code> if this element is static, <code>false</code>
  * otherwise.
  */
public abstract boolean
isStatic();

/*************************************************************************/

/**
  * This method tests whether or not this element is final.
  *
  * @return <code>true</code> if this element is final, <code>false</code>
  * otherwise.
  */
public abstract boolean
isFinal();

} // interface ProgramElementDoc

