/* gnu.classpath.tools.gjdoc.FieldDocImpl
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

package gnu.classpath.tools.gjdoc;

import java.util.*;
import com.sun.javadoc.*;
import java.lang.reflect.Modifier;

public class FieldDocImpl extends MemberDocImpl implements FieldDoc, Cloneable {

   private FieldDocImpl(ClassDoc containingClass,
                        PackageDoc containingPackage,
                        SourcePosition position) {

      super(containingClass,
	    containingPackage,
            position);
   }

   public static Collection createFromSource(ClassDoc containingClass,
					     PackageDoc containingPackage,
					     char[] source, int startIndex, int endIndex) {
      
      List rcList=new ArrayList();

      String s=new String(source, startIndex, endIndex-startIndex-1);

      Debug.log(9,"Parsing FieldDoc '"+s+"'");

      FieldDocImpl fd=new FieldDocImpl(containingClass,
				       containingPackage,
                                       DocImpl.getPosition(containingClass, source, startIndex));

      int ndx=fd.parseModifiers(source, startIndex, endIndex);

      String definition=new String(source, ndx, endIndex-ndx-1);

      final int STATE_INITIAL = 1;
      final int STATE_BRACKET = 2;
      final int STATE_QUOTE   = 3;
      final int STATE_QUOTEBS = 4;

      int lastFieldDefStart = ndx;
      int state = STATE_INITIAL;

      List fieldDefComponents = new ArrayList();

      for (int i=ndx; i<endIndex; ++i) {

	 char c = source[ndx];
	 switch (state) {
	 case STATE_INITIAL:
	    if ('\"' == c) {
	       state = STATE_QUOTE;
	    }
	    else if ('(' == c) {
	       state = STATE_BRACKET;
	    }
	    else if (',' == c) {
	       String fieldDefComponent = new String(source, lastFieldDefStart, i-lastFieldDefStart-1);
	       fieldDefComponents.add(fieldDefComponent);
	       lastFieldDefStart = i+1;
	    }
	    break;
	    
	 case STATE_QUOTE:
	    if ('\\' == c) {
	       state = STATE_QUOTEBS;
	    }
	    else if ('\"' == c) {
	       state = STATE_INITIAL;
	    }
	    break;

	    
	 case STATE_QUOTEBS:
	    state = STATE_QUOTE;
	    break;

	    
	 case STATE_BRACKET:
	    if ('\"' == c) {
	       state = STATE_QUOTE;
	    }
	    else if (')' == c) {
	       state = STATE_INITIAL;
	    } // end of else
	    break;
	 }
      }

      String fieldDefComponent = new String(source, lastFieldDefStart, endIndex-lastFieldDefStart-1);
      fieldDefComponents.add(fieldDefComponent);

      for (Iterator it = fieldDefComponents.iterator(); it.hasNext(); ) {
	 String fieldDef = (String) it.next();

	 int endx=fieldDef.indexOf('=');
	 if (endx>=0) fieldDef=fieldDef.substring(0,endx);
	 Debug.log(9,"  Field Definition: '"+fieldDef+"'");
	 
	 try {
	    FieldDocImpl fieldDoc=(FieldDocImpl)fd.clone();
	    String dimSuffix="";
	    while (fieldDef.trim().endsWith("[")
		   || fieldDef.trim().endsWith("]")) {
	       fieldDef=fieldDef.trim();
	       dimSuffix=fieldDef.charAt(fieldDef.length()-1)+dimSuffix;
	       fieldDef=fieldDef.substring(0,fieldDef.length()-1);
	    }

	    fieldDoc.setTypeName(fieldDoc.getTypeName()+dimSuffix);

	    fieldDoc.setName(fieldDef.trim());
	    rcList.add(fieldDoc);
	 }
	 catch (CloneNotSupportedException e) {
	    e.printStackTrace();
	 }
      }

      return rcList;
   }

   public boolean isField() {
      return true;
   } 

   public boolean isTransient() { return isTransient; }

   public boolean isVolatile() { return isVolatile; }

   public SerialFieldTag[] serialFieldTags() { return new SerialFieldTag[0]; }

   public int modifierSpecifier() {
      return super.modifierSpecifier()
	 | (isVolatile()?Modifier.VOLATILE:0)
	 | (isTransient()?Modifier.TRANSIENT:0)
	 ;
   }

   protected boolean processModifier(String word) {
      if (super.processModifier(word)) {
	 return true;
      }
      else if (word.equals("transient")) {
	 isTransient=true;
	 return true;	 
      }
      else if (word.equals("volatile")) {
	 isVolatile=true;
	 return true;	 
      }
      else {
	 return false;
      }
   }

   private boolean isTransient;
   private boolean isVolatile;

   void resolve() {
      resolveTags();
   }

   public boolean hasSerialTag() {
      return true; //tagMap.get("serial")!=null;
   }

   public String toString() { return name(); }

   public Object constantValue() {
      return new Integer(0); // FIXME
   }
}
