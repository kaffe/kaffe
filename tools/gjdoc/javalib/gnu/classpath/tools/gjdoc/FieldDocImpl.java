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
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA. */

package gnu.classpath.tools.gjdoc;

import java.util.*;
import com.sun.javadoc.*;
import java.lang.reflect.Modifier;

import gnu.classpath.tools.gjdoc.expr.Evaluator;
import gnu.classpath.tools.gjdoc.expr.IllegalExpressionException;

public class FieldDocImpl 
   extends MemberDocImpl 
   implements FieldDoc, Cloneable 
{

   private boolean isTransient;
   private boolean isVolatile;
   private String valueLiteral;
   private Object constantValue;
   private boolean constantValueEvaluated;

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

      FieldDocImpl fd=new FieldDocImpl(containingClass,
				       containingPackage,
                                       DocImpl.getPosition(containingClass, source, startIndex));

      int ndx=fd.parseModifiers(source, startIndex, endIndex);

      if (containingClass.isInterface()) {
         fd.accessLevel = ACCESS_PUBLIC;
      }

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
         String fieldValueLiteral = null;

	 int endx=fieldDef.indexOf('=');
	 if (endx>=0) {
            fieldValueLiteral = fieldDef.substring(endx + 1);
            fieldDef = fieldDef.substring(0,endx);
         }
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
            fieldDoc.setValueLiteral(fieldValueLiteral);
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

   void resolve() {
      resolveTags();
   }

   public boolean hasSerialTag() {
      return true; //tagMap.get("serial")!=null;
   }

   public String toString() { return name(); }

   public Object constantValue() {
      if (!isStatic() 
          || !isFinal() 
          || (!type().isPrimitive() && !"java.lang.String".equals(type().qualifiedTypeName()))
          || type.dimension().length()>0 
          || null == valueLiteral) {

         return null;

      }
      else {
         if (!constantValueEvaluated) {
            String expression = "(" + type().typeName() + ")(" + valueLiteral + ")";
            try {
               this.constantValue = Evaluator.evaluate(expression, 
                                                       (ClassDocImpl)containingClass());
            }
            catch (IllegalExpressionException ignore) {
            }
            constantValueEvaluated = true;
         }
         return this.constantValue;
      }
   }

   private static void appendCharString(StringBuffer result, char c, boolean inSingleCuotes)
   {
      switch (c) {
      case '\b': result.append("\\b"); break;
      case '\t': result.append("\\t"); break;
      case '\n': result.append("\\n"); break;
      case '\f': result.append("\\f"); break;
      case '\r': result.append("\\r"); break;
      case '\"': result.append("\\\""); break;
      case '\'': result.append(inSingleCuotes ? "\\'" : "'"); break;
      default:
         if (c >= 32 && c <= 127) {
            result.append(c);
         }
         else {
            result.append("\\u");
            String hexValue = Integer.toString((int)c, 16);
            int zeroCount = 4 - hexValue.length();
            for (int i=0; i<zeroCount; ++i) {
               result.append('0');
            }
            result.append(hexValue);
         }
      }
   }

   public String constantValueExpression() {
      Object value = constantValue();

      if (null == value) {
         return "null";
      }
      else if (value instanceof String) {
         StringBuffer result = new StringBuffer("\"");
         char[] chars = ((String)value).toCharArray();
         for (int i=0; i<chars.length; ++i) {
            appendCharString(result, chars[i], false);
         }
         result.append("\"");
         return result.toString();
      }
      else if (value instanceof Float) {
         return value.toString() + "f";
      }
      else if (value instanceof Long) {
         return value.toString() + "L";
      }
      else if (value instanceof Character) {
         StringBuffer result = new StringBuffer("'");
         appendCharString(result, ((Character)value).charValue(), false);
         result.append("'");
         return result.toString();
      }
      else /* if (value instanceof Double
               || value instanceof Integer
               || value instanceof Short
               || value instanceof Byte) */ {
         return value.toString();
      }
   }

   void setValueLiteral(String valueLiteral)
   {
      this.valueLiteral = valueLiteral;
   }

   public boolean isStatic()
   {
      return super.isStatic() || containingClass().isInterface();
   }

   public boolean isFinal()
   {
      return super.isFinal() || containingClass().isInterface();
   }
}
