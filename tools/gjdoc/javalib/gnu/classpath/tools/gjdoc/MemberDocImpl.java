/* gnu.classpath.tools.gjdoc.MemberDocImpl
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

public abstract class MemberDocImpl extends ProgramElementDocImpl implements MemberDoc {

   protected String typeName;
   protected Type   type;

   public MemberDocImpl(ClassDoc containingClass,
			PackageDoc containingPackage,
                        SourcePosition position) {

      super(containingClass,
	    containingPackage,
            position);
   }

   public String qualifiedName() {
      return containingClass().qualifiedName()+"."+name();
   }

   public boolean isSynthetic() {
      return false;
   }

   int parseModifiers(char[] source, int startIndex, int endIndex) {

      Debug.log(9,"parseModifiers '"+new String(source,startIndex,endIndex-startIndex)+"'");

      final int STATE_NORMAL = 1;
      final int STATE_STARC  = 2;
      final int STATE_SLASHC = 3;

      int state = STATE_NORMAL;

      String word = "";
      int lastWordStart = startIndex;
      for (; startIndex<endIndex; ++startIndex) {
	 if (state==STATE_STARC) {
	    if (startIndex<endIndex-1 && source[startIndex]=='*' && source[startIndex+1]=='/') {
	       ++startIndex;
	       state=STATE_NORMAL;
	    }
	 }
	 else if (state==STATE_SLASHC) {
	    if (source[startIndex]=='\n') {
	       state=STATE_NORMAL;
	    }
	 }
	 else if (startIndex<endIndex-1 && source[startIndex]=='/' && source[startIndex+1]=='*') {
	    ++startIndex;
	    state=STATE_STARC;
	 }
	 else if (source[startIndex]=='=' || source[startIndex]=='(' || source[startIndex]==';') {
            return lastWordStart;
	 }
	 else if (Parser.WHITESPACE.indexOf(source[startIndex])>=0) {
	    if (word.length()>0 && !word.endsWith(".")) {
	       if (processModifier(word)) {
	       }
	       else if (typeName==null && !isConstructor()) {
		  typeName=word;
	       }
	       else if ((word.startsWith("[") || word.startsWith("]")) && !isConstructor()) {
		  typeName+=word;
	       }
	       else {
		  return lastWordStart;
		  //throw new Error("In FieldComponent: cannot understand word '"+word+"' (typeName="+typeName+", name="+name()+")");
	       }
	       word="";
	       lastWordStart=startIndex;
	    }
	 }
	 else {
	    if (lastWordStart<0) lastWordStart=startIndex;
	    word+=source[startIndex];
	 }
      }

      return startIndex;

   }

    public Type type() {
	//public Type type() throws ParseException { 
	Debug.log(9,"type() called on "+containingClass()+"."+this);
	if (type==null) {
	    try {
		type=((ClassDocImpl)containingClass()).typeForString(typeName);
	    } catch (ParseException e) {
	       System.err.println("FIXME: add try-catch to force compilation");
	       e.printStackTrace();
	    }
	}
	return type;
    }


   protected void setName(String name) {
      this.name=name;
   }
   private String name;


   public String name() {
      return name;
   }

   public void setTypeName(String typeName) { 
      this.typeName=typeName;
      this.type=null;
   }

   public String getTypeName() {
      return typeName;
   }

   // return true if this Doc is include in the active set. 
   public boolean isIncluded() {
      return Main.getInstance().includeAccessLevel(accessLevel);
   } 

   public int compareTo(Object o) {
      if (o instanceof MemberDocImpl) {
	 int rc=name().compareTo(((MemberDocImpl)o).name());
	 if (rc==0) 
	    rc=containingClass().qualifiedName().compareTo(((MemberDocImpl)o).containingClass().qualifiedName());
	 return rc;
      }
      else {
	 return super.compareTo(o);
      }
   }

   void resolve() {

      if (type==null && typeName!=null) {
	 Debug.log(1, "MemberDocImpl.resolve(), looking up type named "+typeName);
	 try {
	    type=((ClassDocImpl)containingClass()).typeForString(typeName);
	 } catch (ParseException e) {
	    //System.err.println("FIXME: add try-catch to force compilation");
	    //e.printStackTrace();
	    Debug.log(1, "INTERNAL WARNING: Couldn't find type for name '"+typeName+"'");
	 }
      }

      if (type instanceof ClassDocProxy) {
	 String className=type.qualifiedTypeName();
	 ClassDoc realClassDoc=containingClass().findClass(className);
	 if (realClassDoc!=null) {
	    type=realClassDoc;
	 }
	 else {
	    //throw new Error("Class not found: "+className);
	    /*** This is not an error, the class was not included
	     * on the command line. Perhaps emit a notice here.
	     *

	    Main.getRootDoc().printError("Class not found '"
			                 + className
					 + "' in class '"
					 + containingClass().qualifiedName()
					 + "' member '"
					 + name()
					 + "'");
	    */
	 }
      }
   }

   public void resolveComments()
   {
      super.resolveComments();

      if (tagMap.isEmpty()) {
         TagContainer inheritedTagMap = ClassDocImpl.findInheritedDoc(containingClass(),
                                                                      this,
                                                                      null);
         if (null != inheritedTagMap) {
            this.tagMap = inheritedTagMap.getTagMap();
         }
      }
   }
}
