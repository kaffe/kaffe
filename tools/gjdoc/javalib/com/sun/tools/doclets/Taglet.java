/* com.sun.tools.doclets.Taglet
   Copyright (C) 2001 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA. */

package com.sun.tools.doclets;

import com.sun.javadoc.Tag;

public interface Taglet {

   public String getName();

   public boolean inConstructor();

   public boolean inField();

   public boolean inMethod();

   public boolean inOverview();

   public boolean inPackage();

   public boolean inType();

   public boolean isInlineTag();

   public String toString(Tag tag);

   public String toString(Tag[] tagArray);
}
