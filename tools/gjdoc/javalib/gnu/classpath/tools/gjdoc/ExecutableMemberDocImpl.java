/* gnu.classpath.tools.gjdoc.ExecutableMemberDocImpl
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
import java.io.*;
import com.sun.javadoc.*;

public class ExecutableMemberDocImpl extends MemberDocImpl implements ExecutableMemberDoc {

   protected ExecutableMemberDocImpl(ClassDoc containingClass,
				     PackageDoc containingPackage,
                                     SourcePosition position) {
      
      super(containingClass,
	    containingPackage,
            position);
   }

   protected boolean processModifier(String word) {
      if (super.processModifier(word)) {
	 return true;
      }
      else if (word.equals("synchronized")) {
	 isSynchronized=true;
	 return true;	 
      }
      else if (word.equals("native")) {
	 isNative=true;
	 return true;	 
      }
      else if (word.equals("abstract")) {
	 isAbstract=true;
	 return true;	 
      }
      else {
	 return false;
      }
   }

   private boolean isAbstract=false;
   private boolean isNative=false;
   private boolean isSynchronized=false;

   public boolean isAbstract() { return isAbstract; }

   public boolean isNative() { return isNative; }

   public boolean isSynchronized() { return isSynchronized; }

   public ClassDoc[] thrownExceptions() { return thrownExceptions; }

   public Parameter[] parameters() { return parameters; }

   public ThrowsTag[] throwsTags() { 
      return (ThrowsTag[])getTagArr("throws", throwsTagEmptyArr);
   }

   public ParamTag[] paramTags() { 
      return (ParamTag[])getTagArr("param", paramTagEmptyArr);
   }

   public String signature() { return signature; }
   public String flatSignature() { return flatSignature; }

   public ClassDoc overriddenClass() { 
      for (ClassDocImpl cdi=(ClassDocImpl)containingClass().superclass(); cdi!=null; cdi=(ClassDocImpl)cdi.superclass()) {
	 if (null!=cdi.findMethod(name(), signature()))
	    return cdi;
      }
      return null;
   }

   public static ExecutableMemberDocImpl createFromSource(ClassDoc containingClass,
							  PackageDoc containingPackage,
							  char[] source, int startIndex, int endIndex) throws IOException, ParseException {

      int lastchar=32;
      String methodName="";
      for (int i=startIndex; i<endIndex && source[i]!='('; ++i) {
	 if (Parser.WHITESPACE.indexOf(lastchar)>=0 && Parser.WHITESPACE.indexOf(source[i])<0)
	    methodName=""+source[i];
	 else if (Parser.WHITESPACE.indexOf(source[i])<0)
	    methodName+=source[i];
	    
	 lastchar=source[i];
      }

      ExecutableMemberDocImpl rc;

      SourcePosition position = DocImpl.getPosition(containingClass, source, startIndex);

      if (methodName.equals(((ClassDocImpl)containingClass).getClassName())) {
	 
	 // Constructor

	 rc=new ConstructorDocImpl(containingClass,
				   containingPackage,
                                   position);
      }
      else {

	 // Normal method

	 rc=new MethodDocImpl(containingClass,
			      containingPackage,
                              position);
      }
      
      //System.err.println("Parsing '"+new String(source, startIndex, endIndex-startIndex)+"'");

      if (containingClass.isInterface())
	 rc.accessLevel=ACCESS_PUBLIC;

      int ndx=rc.parseModifiers(source, startIndex, endIndex);
      String name="";

      final int STATE_NORMAL=1;
      final int STATE_STARC=2;
      final int STATE_SLASHC=3;

      int state=STATE_NORMAL;

      while (source[ndx]!='(' && ndx<endIndex) {
	 if (state==STATE_NORMAL) {
	    if (ndx<endIndex-1 && source[ndx]=='/' && source[ndx+1]=='/') {
	       ++ndx;
	       state=STATE_SLASHC;
	    }
	    else if (ndx<endIndex-1 && source[ndx]=='/' && source[ndx+1]=='*') {
	       ++ndx;
	       state=STATE_STARC;
	    }
	    else {
	       name+=source[ndx];
	    }
	 }
	 else if (state==STATE_SLASHC) {
	    if (source[ndx]=='\n')
	       state=STATE_NORMAL;
	 }
	 else if (state==STATE_STARC) {
	    if (ndx<endIndex-1 && source[ndx]=='*' && source[ndx+1]=='/') {
	       ++ndx;
	       state=STATE_NORMAL;
	    }
	 }
	 ++ndx;
      }
      rc.setName(name.trim());

      state=STATE_NORMAL;
      
      ++ndx;
      int endx;
      String param="";
      List parameterList=new ArrayList();
      for (endx=ndx; endx<endIndex; ++endx) {
	 if (state==STATE_SLASHC) {
	    if (source[endx]=='\n') {
	       state=STATE_NORMAL;
	    }
	 }
	 else if (state==STATE_STARC) {
	    if (source[endx]=='*' && source[endx+1]=='/') {
	       state=STATE_NORMAL;
	       ++endx;
	    }
	 }
	 else if (source[endx]=='/' && source[endx+1]=='*') {
	    state=STATE_STARC;
	    ++endx;
	 }
	 else if (source[endx]=='/' && source[endx+1]=='/') {
	    state=STATE_SLASHC;
	    ++endx;
	 }
	 else if (source[endx]==',' || source[endx]==')') {
	    param=param.trim();
	    if (param.length()>0) {
	       int n;
	       for (n=param.length()-1; n>=0; --n)
		  if (Parser.WHITESPACE.indexOf(param.charAt(n))>=0)
		     break;
	       String paramType=param.trim();
	       String paramName="";
	       if (n>0) {
		  paramType=param.substring(0,n).trim();
		  paramName=param.substring(n).trim();
	       }
	       String dimSuffix="";
	       while (paramName.length()>0 && "[]".indexOf(paramName.charAt(paramName.length()-1))>=0) {
		  dimSuffix=paramName.charAt(paramName.length()-1)+dimSuffix;
		  paramName=paramName.substring(0,paramName.length()-1).trim();
	       }
	       paramType+=dimSuffix;
	       if (paramType.indexOf("][")>=0 && paramType.indexOf("[][]")<0) {
		  throw new Error("dimSuffix='"+dimSuffix+"' paramType='"+paramType+"'");
	       }
	       parameterList.add(new ParameterImpl(paramName, paramType, 
						   ((ClassDocImpl)containingClass).typeForString(paramType)));

	       param="";
	    }
	 }
	 else
	    param+=source[endx];

	 if (source[endx]==')')
	    break;
      }

      rc.setParameters((Parameter[])parameterList.toArray(new Parameter[0]));
      
      ++endx;
      String word="";
      String dimSuffix="";
      boolean haveThrowsKeyword=false;
      List thrownExceptionsList=new ArrayList();

      state=STATE_NORMAL;
      for (; endx<endIndex; ++endx) {
	 if (state==STATE_SLASHC) {
	    if (source[endx]=='\n') state=STATE_NORMAL;
	 }
	 else if (state==STATE_STARC) {
	    if (source[endx]=='*' && source[endx+1]=='/') {
	       state=STATE_NORMAL;
	       ++endx;
	    }
	 }
	 else if (source[endx]=='/' && source[endx+1]=='*') {
	    state=STATE_STARC;
	    ++endx;
	 }
	 else if (source[endx]=='/' && source[endx+1]=='/') {
	    state=STATE_SLASHC;
	    ++endx;
	 }
	 else if (Parser.WHITESPACE.indexOf(source[endx])>=0) {
	    word=word.trim();
	    if (!haveThrowsKeyword && word.length()>0) {
	       if (word.equals("throws")) haveThrowsKeyword=true;
	       else System.err.println("ARGH! "+word);
	       word="";
	    }
	 }
	 else if (source[endx]=='[' || source[endx]==']') {
	    dimSuffix += source[endx];
	 }
	 else if (source[endx]==',' || source[endx]=='{') {
	    word=word.trim();
	    if (word.length()>0) {
	       //System.err.println("have thrown exception '"+word+"'");
	       ClassDoc exceptionType=rc.containingClass().findClass(word);
	       if (exceptionType==null) {
		  exceptionType=new ClassDocProxy(word, rc.containingClass());
		  //System.err.println("not found: "+exceptionType);
	       }
	       thrownExceptionsList.add(exceptionType);
	    }
	    if (source[endx]=='{') {
	       break;
	    }
	    else {
	       word="";
	    }
	 }
	 else {
	    word+=source[endx];
	 }
      }

      if (dimSuffix.length()>0) {
	 rc.setTypeName(rc.getTypeName()+dimSuffix);
      }

      rc.setThrownExceptions((ClassDoc[])thrownExceptionsList.toArray(new ClassDoc[0]));

      //System.err.println("typeName="+rc.typeName);

      return rc;
   }

   private ClassDoc[] thrownExceptions;
   private Parameter[] parameters;
   private String signature;
   private String flatSignature;

   void setParameters(Parameter[] parameters) {
      this.parameters=parameters;
   }

   void setThrownExceptions(ClassDoc[] thrownExceptions) {
      this.thrownExceptions=thrownExceptions;
   }

   void resolve() {

      for (int i=0; i<thrownExceptions.length; ++i) {
	 if (thrownExceptions[i] instanceof ClassDocProxy) {
	    String className=thrownExceptions[i].qualifiedName();
	    ClassDoc realClassDoc=containingClass().findClass(className);
	    if (realClassDoc!=null)
	       thrownExceptions[i]=realClassDoc;
	 }
      }

      StringBuffer signatureBuf=new StringBuffer();
      StringBuffer flatSignatureBuf=new StringBuffer();

      for (int i=0; i<parameters.length; ++i) {
	 ((ParameterImpl)parameters[i]).resolve(containingClass());

	 if (signatureBuf.length()>0) {
	    signatureBuf.append(",");
	    flatSignatureBuf.append(",");
	 }
	 signatureBuf.append(parameters[i].type().qualifiedTypeName());
	 flatSignatureBuf.append(parameters[i].type().typeName());
         signatureBuf.append(parameters[i].type().dimension());
         flatSignatureBuf.append(parameters[i].type().dimension());
      }
      this.signature="("+signatureBuf.toString()+")";
      this.flatSignature="("+flatSignatureBuf.toString()+")";

      super.resolve();

   }

   public int compareTo(Object o) {
      if (o instanceof MemberDocImpl) {
	 int rc=(name()+signature()).compareTo((((MemberDocImpl)o).name())+signature());
	 if (rc==0) 
	    rc=containingClass().qualifiedName().compareTo(((MemberDocImpl)o).containingClass().qualifiedName());
	 return rc;
      }
      else {
	 return 0;
      }
   }
}

