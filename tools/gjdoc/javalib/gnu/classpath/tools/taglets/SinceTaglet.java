/* gnu.classpath.tools.taglets.SinceTaglet
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

package gnu.classpath.tools.taglets;

import java.util.Map;

import java.util.regex.Pattern;
import java.util.regex.Matcher;

import com.sun.tools.doclets.Taglet;

import com.sun.javadoc.Tag;

/**
 *  The default Taglet which handles since information.
 *
 *  @author Julian Scheid (julian@sektor37.de)
 */
public class SinceTaglet implements Taglet {
   
   private static final String NAME = "since";
   private static final String HEADER = "Since:";

   private static boolean enabled = true;
   
   public String getName() {
      return NAME;
   }
    
   public boolean inField() {
      return true;
   }

   public boolean inConstructor() {
      return true;
   }
    
   public boolean inMethod() {
      return true;
   }
   
   public boolean inOverview() {
      return true;
   }

   public boolean inPackage() {
      return true;
   }

   public boolean inType() {
      return true;
   }
    
   public boolean isInlineTag() {
      return false;
   }    

   public static void register(Map tagletMap) {
      SinceTaglet sinceTaglet = new SinceTaglet();
      tagletMap.put(sinceTaglet.getName(), sinceTaglet);
   }

   public String toString(Tag tag) {
      if (enabled) {
         return toString(new Tag[] { tag });
      }
      else {
         return null;
      }
   }

   public String toString(Tag[] tags) {
      if (!enabled || tags.length == 0) {
         return null;
      }
      else {
         
         StringBuffer result = new StringBuffer();
         result.append("<dl class=\"tag list\">");
         result.append("<dt class=\"tag section header\"><b>");
         result.append(HEADER);
         result.append("</b></dt>");
         for (int i = 0; i < tags.length; i++) {
            result.append("<dd>");
            result.append(tags[i].text());
            result.append("</dt>");
         }
         result.append("</dl>");
         return result.toString();
      }
   }

   /**
    *  Enables/disables this taglet.
    */
   public static void setTagletEnabled(boolean enabled)
   {
      SinceTaglet.enabled = enabled;
   }
}
