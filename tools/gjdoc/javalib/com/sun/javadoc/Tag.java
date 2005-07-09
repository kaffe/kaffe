/* Tag.java -- Common operations on Javadoc tags.
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
  * This is the super-interface for all Javadoc tags.
  */
public interface Tag extends java.io.Serializable
{
/**
  * This method returns the name of the tag.
  *
  * @return The name of the tag.
  */
public abstract String
name();

/*************************************************************************/

/**
  * This method returns the kind of tag.  ????
  *
  * @return The kind of the tag.
  */
public abstract String
kind();

/*************************************************************************/

/**
  * This method returns the text for this tag.
  *
  * @return The text for this tag.
  */
public abstract String
text();

/*************************************************************************/

/**
  * This method returns the tag as a <code>String</code>.  What kind of
  * string?
  *
  * @return This tag as a <code>String</code>.
  */
public abstract String
toString();

/*************************************************************************/

/**
  * This method returns the inline tags for this comment.
  *
  * @return The inline tags for this comment.
  */
public abstract Tag[]
inlineTags();

/*************************************************************************/

/**
  * This method returns the first sentence of the doc comment as an array
  * of <code>Tag</code>'s.
  *
  * @return The first sentence of the comment as tags.
  */
public abstract Tag[]
firstSentenceTags();

} // interface Tag

