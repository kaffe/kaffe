/* gnu.classpath.tools.gjdoc.ClassDocImpl
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

import com.sun.javadoc.*;
import java.util.*;
import java.io.*;

public class ClassDocImpl extends ProgramElementDocImpl implements ClassDoc, WritableType {

   private ClassDoc baseClassDoc;
   private ClassDoc[] importedClasses;
   private PackageDoc[] importedPackages;
   private boolean definesSerializableFields;
   private FieldDoc[] serialPersistentField;
   private MethodDoc[] serializationMethods;
   private String dimension = "";

   public ClassDocImpl(ClassDoc containingClass,
		       PackageDoc containingPackage,
		       int accessLevel,
		       boolean isFinal,
		       boolean isStatic,
                       SourcePosition position) {
      super(containingClass, containingPackage, accessLevel, isFinal, isStatic,
            position);
      this.baseClassDoc = this;
   }

   public ClassDocImpl(ClassDoc containingClass,
		       PackageDoc containingPackage,
		       ClassDoc[] importedClasses,
		       PackageDoc[] importedPackages,
                       SourcePosition position) {
      super(containingClass, containingPackage,
            position);
      this.importedClasses=importedClasses;
      this.importedPackages=importedPackages;
      this.baseClassDoc = this;
   }   

   // Return constructors in class. 
   public ConstructorDoc[] constructors() {
      return constructors;
   } 

   // Return true if Serializable fields are explicitly defined with the special class member serialPersistentFields. 
   public boolean definesSerializableFields() {
      return definesSerializableFields;
   } 

   // Return fields in class. 
   public FieldDoc[] fields() {
      return fields;
   } 

   /**
    *   Find a class within the context of this class. 
    *
    *   @todo check this against java lang spec. (class id hiding)
    */
   public ClassDoc findClass(String className) {

      //Debug.log(9,"findClass("+className+")");
      ClassDoc rc;
      //Debug.log(9,"trying by name");
      rc=Main.getRootDoc().classNamed(className);
      if (rc!=null) return rc;
      
      /*
      System.err.println("trying '"+containingPackage().name()+"."+className+"'");
      rc=Main.getRootDoc().classNamed(containingPackage().name()+"."+className);
      if (rc!=null) return rc;
      */

      for (int i=0; i<importedClasses.length; ++i) {
	 //Debug.log(9,"trying (by imported class)");
	 if (importedClasses[i].qualifiedName().endsWith("."+className)) {
	    rc=Main.getRootDoc().classNamed(importedClasses[i].qualifiedName());
	    if (rc!=null) return rc;
	 }
	 if (className.startsWith(importedClasses[i].name()+".")) {
	    //Debug.log(9,"trying (by inner class) "+importedClasses[i].qualifiedName()+className.substring(importedClasses[i].name().length()));
	    rc=Main.getRootDoc().classNamed(importedClasses[i].qualifiedName()+className.substring(importedClasses[i].name().length()));
	    if (rc!=null) return rc;
	 }
      }

      for (ClassDoc cdi=this; cdi!=null; cdi=cdi.containingClass()) {
	 for (ClassDoc sdi=cdi; sdi!=null; sdi=sdi.superclass()) {
	    rc=Main.getRootDoc().classNamed(sdi.qualifiedName()+"."+className);
	    if (rc!=null) return rc;
	 }

	 rc=Main.getRootDoc().classNamed(cdi.qualifiedName()+"."+className);
	 if (rc!=null) return rc;
      }

      //Debug.log(9,"importedPackages.length="+importedPackages.length+", this="+this);
      for (int i=0; i<importedPackages.length; ++i) {
	 //Debug.log(9,"trying (by package) "+importedPackages[i].name()+"."+className+"...");
	 rc=Main.getRootDoc().classNamed(importedPackages[i].name()+"."+className);
	 if (rc!=null) return rc;
      }
      rc=Main.getRootDoc().classNamed(containingPackage().name()+"."+className);
      if (rc!=null) {
	 return rc;
      }
      return Main.getRootDoc().classNamed("java.lang."+className);
   } 

   // Get the list of classes declared as imported. 
   public ClassDoc[] importedClasses() {
      return importedClasses;
   } 

   // Get the list of packages declared as imported. 
   public PackageDoc[] importedPackages() {
      return importedPackages;
   } 

   // Return inner classes within this class. 
   public ClassDoc[] innerClasses() {
      return innerClasses;
   } 

   void setInnerClasses(ClassDoc[] innerClasses) {
      this.innerClasses=innerClasses;
   }

   // Return interfaces implemented by this class or interfaces extended by this interface. 
   public ClassDoc[] interfaces() {
      return interfaces;
   } 

   public void setInterfaces(ClassDoc[] interfaces) {
      this.interfaces=interfaces;
   } 

   // Return true if this class is abstract 
   public boolean isAbstract() {
      return isAbstract || isInterface();
   } 

   public boolean isInterface() {
      return isInterface;
   }

   // Return true if this class is abstract 
   public void setIsAbstract(boolean b) {
      this.isAbstract=b;
   } 

   // Return true if this class implements java.io.Externalizable. 
   public boolean isExternalizable() {
      return implementsInterface("java.io.Externalizable");
   } 

   // Return true if this class implements java.io.Serializable. 
   public boolean isSerializable() {
      return implementsInterface("java.io.Serializable");
   } 

   public boolean implementsInterface(String name) {
      for (ClassDoc cdi=this; cdi!=null; cdi=(ClassDoc)cdi.superclass()) {
	 if (cdi instanceof ClassDocImpl) {
	    ClassDoc[] cdiInterfaces=(ClassDoc[])cdi.interfaces();
            if (null != cdiInterfaces) {
               for (int i=0; i<cdiInterfaces.length; ++i) {
                  if (cdiInterfaces[i].qualifiedName().equals(name))
                     return true;
               }
            }
	 }
	 else {
	    //throw new RuntimeException("implementsInterface(\""+name+"\") failed: Not a ClassDocImpl:"+cdi);
	 }
      }
      return false;
   }

   // Return methods in class. 
   public MethodDoc[] methods() {
      return methods;
   } 

   // Return the Serializable fields of class. Return either a list of default fields documented by serial tag or return a single FieldDoc for serialPersistentField member. 
   public FieldDoc[] serializableFields() {
      if (serialPersistentField!=null) {
	 return serialPersistentField;
      }
      else{
	 return serializableFields;
      }
   } 

   // Return the serialization methods for this class. 
   public MethodDoc[] serializationMethods() {
      return serializationMethods;
   } 

   // Test whether this class is a subclass of the specified class. 
   public boolean subclassOf(ClassDoc cd) {
      for (ClassDocImpl cdi=(ClassDocImpl)superclass(); cdi!=null; cdi=(ClassDocImpl)cdi.superclass()) {
	 if (cdi.equals(cd))
	    return true;
      }
      return false;
   } 

   // Return the superclass of this class 
   public ClassDoc superclass() {
      return superclass;
   } 

   // Implementation of Interface Type

   public ClassDoc asClassDoc() {

      return (ClassDoc)this;
   }

   public String typeName() { return name(); }
   
   public String qualifiedTypeName() { 
      return (containingPackage()!=null && containingPackage()!=PackageDocImpl.DEFAULT_PACKAGE)?(containingPackage().name()+"."+name()):(name()); 
   }

   public String qualifiedName() { return qualifiedTypeName(); }

   public String dimension() { return dimension; }

   public String toString() { return "ClassDoc{"+qualifiedTypeName()+"}"; }


   public static ClassDocImpl createInstance(ClassDoc containingClass,
					     PackageDoc containingPackage,
					     ClassDoc[] importedClasses,
					     PackageDoc[] importedPackages,
					     char[] source, int startIndex, int endIndex) throws ParseException, IOException {

      String superclassName = "java.lang.Object";

      ClassDocImpl rc=new ClassDocImpl(containingClass,
				       containingPackage,
				       importedClasses,
				       importedPackages,
                                       null);
      rc.setPosition(ClassDocImpl.getPosition(rc, source, startIndex));
      List implementedInterfaces = new ArrayList();
      
      String word="";
      int item=0;
      
      final int STATE_NORMAL = 1;
      final int STATE_SLASHC = 2;
      final int STATE_STARC  = 3;

      int state=STATE_NORMAL;
      char prev=0;
      for (int ndx=startIndex; ndx<=endIndex; ++ndx) {
	 char c=(ndx==endIndex)?10:source[ndx];
	 if (state==STATE_SLASHC) {
	    if (c=='\n') {
	       state=STATE_NORMAL;
	       c=0;
	    }
	 }
	 else if (state==STATE_STARC) {
	    if (c=='/' && prev=='*') {
	       state=STATE_NORMAL;
	       c=0;
	    }
	 }
	 else {

	    boolean processWord=false;

	    if (c=='/' && prev=='/') {
	       state=STATE_SLASHC;
	       c=0;
	       word=word.substring(0,word.length()-1);
	       processWord=true;
	    }
	    else if (c=='*' && prev=='/') {
	       state=STATE_STARC;
	       c=0;
	       word=word.substring(0,word.length()-1);
	       processWord=true;
	    }
	    else if (c=='{' || c==',' || Parser.WHITESPACE.indexOf(c)>=0) {
	       processWord=true;
	    }
	    else {
	       word+=c;
	    }

	    if (processWord && word.length()>0) {
	       if (item==0) {
		  if (rc.processModifier(word)) {
		  }
		  else if (word.equals("abstract")) {
		     rc.setIsAbstract(true);
		  }
		  else if (word.equals("class")) {
		     rc.setIsInterface(false);
		     item=1;
		  }
		  else if (word.equals("interface")) {
		     rc.setIsInterface(true);
		     item=1;
		  }
		  else if (word.equals("strictfp")) {
		  }
		  else {
		     Main.getRootDoc().printWarning("unknown modifier '"+word+"'");
		  }
	       }
	       else if (word.equals("extends")) {
		  item=2;
	       }
	       else if (word.equals("implements")) {
		  item=3;
	       }
	       else if (item==1) {
		  rc.setClass(word);
	       }
	       else if (item==2) {
		  //Debug.log(9,"setting baseclass of "+rc+" to "+word);
		  superclassName=word;
	       }
	       else if (item==3) {
		  implementedInterfaces.add(word);
	       }      
	       word="";
	    }

	    if (c=='{') break;
	 }
	 prev=c;
      }

      if (rc.name()==null) {
	 throw new ParseException("No classdef found in expression \""+new String(source,startIndex,endIndex-startIndex)+"\"");
      }

      ClassDoc superclassProxy=new ClassDocProxy(superclassName, rc);

      if (!rc.qualifiedName().equals("java.lang.Object")) {
	 rc.setSuperclass(superclassProxy);
      }

      if (!rc.isInterface()) {
	 ClassDoc[] interfaces=new ClassDoc[implementedInterfaces.size()];
	 for (int i=0; i<interfaces.length; ++i) {
	    interfaces[i]=new ClassDocProxy((String)implementedInterfaces.get(i), rc);
	 }
	 rc.setInterfaces(interfaces);
      }
      else {
	 if (!superclassName.equals("java.lang.Object"))
	    rc.setInterfaces(new ClassDoc[]{superclassProxy});
	 else
	    rc.setInterfaces(new ClassDoc[0]);
      }

      //Debug.log(9,rc.name());

      if (rc.isInterface() && rc.containingClass()!=null) {
	 rc.setIsStatic(true);
      }
      return rc;
   }

   public void setFields(FieldDoc[] fields) {
      this.fields=fields;
   }

   public void setSerializableFields(FieldDoc[] sfields) {
      this.serializableFields=sfields;
   }

   public void setMethods(MethodDoc[] methods) {
      this.methods=methods;
   }

   public void setConstructors(ConstructorDoc[] constructors) {
      this.constructors=constructors;
   }

   // Returns the name of this Doc item. 
   public String name() {
      if (containingClass==null) {
	 return className;
      }
      else {
	 return containingClass.name()+"."+className;
      }
   } 

   public String getClassName() {
      return className;
   }

   public void setClass(String className) {
      this.className=className;
   }

   void setSuperclass(ClassDoc superclass) {
      this.superclass=superclass;
   }

   public void resolve() throws ParseException {
      if (!resolved) {
	 resolved=true;

	 if (containingClass!=null)
	    ((ClassDocImpl)containingClass).resolve();

	 //Debug.log(9,"resolving class '"+qualifiedName()+"'");
	 /*
	 for (int i=0; i<importedPackages.length; ++i) {
	       Debug.log(9,"class "+qualifiedName()+" imports "+importedPackages[i].name());
	 }
	 */

	 if (superclass instanceof ClassDocProxy) {

	    String className=superclass.qualifiedName();
	    ClassDoc realClassDoc=findClass(className);

	    if (realClassDoc==null) {
	       /*
	       if (true) { // Main.recursiveClasses) {
		  throw new ParseException("In class '"+qualifiedName()+"': class '"+className+"' not found.");
	       }
	       */
	    }
	    else {
	       superclass=realClassDoc;
	    }
	 }

         if (null != interfaces) {
            for (int i=0; i<interfaces.length; ++i) {
               if (interfaces[i] instanceof ClassDocProxy) {
                  //Debug.log(9,"class "+qualifiedName()+" implements "+interfaces[i].qualifiedName());
                  String className=interfaces[i].qualifiedName();
                  ClassDoc realClassDoc=findClass(className);
                  if (realClassDoc==null) {
                     /*
                       if (Main.recursiveClasses) {
                       throw new ParseException("In class '"+qualifiedName()+"': class '"+className+"' not found.");
                       }
                     */
                  }
                  else {
                     //Debug.log(9,"found class '"+className+"': "+interfaces[i]);
                     interfaces[i]=realClassDoc;
                  }
               }
            }
         }

	 if (fields!=null) {
            for (int i=0; i<fields.length; ++i) {
               ((FieldDocImpl)fields[i]).resolve();
               if (fields[i].name().equals("serialPersistentField")) {
                  serialPersistentField=new FieldDoc[]{fields[i]};
                  definesSerializableFields=true;
               }
            }
         }

         if (methods!=null) {
            for (int i=0; i<methods.length; ++i) {
               ((MethodDocImpl)methods[i]).resolve();
            }
         }

         if (constructors!=null) {
            for (int i=0; i<constructors.length; ++i) {
               ((ConstructorDocImpl)constructors[i]).resolve();
            }
         }

	 List isSerMethodList=new ArrayList();

         if (null != maybeSerMethodList) {
            for (Iterator it=maybeSerMethodList.iterator(); it.hasNext(); ) {
               MethodDocImpl method=(MethodDocImpl)it.next();
               method.resolve();

               if (((method.name().equals("readObject")
                     && method.signature().equals("(java.io.ObjectInputStream)"))
                    || (method.name().equals("writeObject")
                        && method.signature().equals("(java.io.ObjectOutputStream)")))) {

                  isSerMethodList.add(method);
               }
            }
            this.serializationMethods=(MethodDoc[])isSerMethodList.toArray(new MethodDoc[0]);
            maybeSerMethodList=null;
         }
      }
   }

   public FieldDoc findField(String fieldName) {
      for (int i=0; i<fields.length; ++i) {
	 if (fields[i].name().equals(fieldName)) {
	    return fields[i];
	 }
      }
      return null;
   }

   public void resolveComments() {

      super.resolveComments();

      if (null != fields) {
         for (int i=0; i<fields.length; ++i) {
            ((FieldDocImpl)fields[i]).resolveComments();
         }
      }

      if (null != serializableFields) {
         for (int i=0; i<serializableFields.length; ++i) {
            ((FieldDocImpl)serializableFields[i]).resolveComments();
         }
      }
      if (null != methods) {
         for (int i=0; i<methods.length; ++i) {
            ((MethodDocImpl)methods[i]).resolveComments();
         }
      }
      if (null != constructors) {
         for (int i=0; i<constructors.length; ++i) {
            ((ConstructorDocImpl)constructors[i]).resolveComments();
         }
      }

      resolveTags();
   }


   private String className=null;

   private boolean isAbstract;
   private boolean isInterface;
   private ClassDoc[] interfaces;
   private FieldDoc[] fields;
   private FieldDoc[] serializableFields;
   private MethodDoc[] methods;
   private ConstructorDoc[] constructors;

   private boolean resolved=false;

   private ClassDoc superclass;

   // Is this Doc item a class. 
   public boolean isClass() {
      return !isInterface;
   } 

   // return true if this Doc is include in the active set. 
   public boolean isIncluded() {
      if (this == baseClassDoc) {
         return isIncluded; // && Main.getRootDoc().includeAccessLevel(accessLevel);
      }
      else {
         return baseClassDoc.isIncluded();
      }
   } 

   void setIsIncluded(boolean b) {
      this.isIncluded=b;
   }

   private boolean isIncluded=false;

   void setImportedClasses(ClassDoc[] importedClasses) {
      this.importedClasses=importedClasses;
   }

   private static Map typeMap = new HashMap();

   Type typeForString(String typeName) throws ParseException {
      String orgTypename=typeName;
      int ndx=typeName.indexOf('[');
      String dim="";
      if (ndx>=0) {
	 for (int i=ndx; i<typeName.length(); ++i) {
	    if ("[]".indexOf(typeName.charAt(i))>=0) {
	       dim+=typeName.charAt(i);
	    }
	 }
	 typeName=typeName.substring(0,ndx).trim();
      }

      ClassDoc classDoc = findClass(typeName);
      if (null!=classDoc) {
	 try {
            if (classDoc.dimension().equals(dim)) {
               return classDoc;
            }
            else {
               Type rc = (Type) ((WritableType)classDoc).clone();
               ((WritableType)rc).setDimension(dim);
               return rc;
            }
	 }
	 catch (CloneNotSupportedException e) {
	    throw new ParseException(e.toString());
	 }
      }

      Type type = (Type)typeMap.get(typeName+dim);
      if (null!=type) {
	 try {
            if (type.dimension().equals(dim)) {
               return type;
            }
            else {
               Type rc = (Type) ((WritableType)type).clone();
               ((WritableType)rc).setDimension(dim);
               return rc;
            }
	 }
	 catch (CloneNotSupportedException e) {
	    throw new ParseException(e.toString());
	 }
      }
      
      if ("boolean".equals(typeName)
	  || "char".equals(typeName)
	  || "byte".equals(typeName)
	  || "short".equals(typeName)
	  || "int".equals(typeName)
	  || "long".equals(typeName)
	  || "void".equals(typeName)
	  || "float".equals(typeName)
	  || "double".equals(typeName)) {
	 Type rc=new TypeImpl(null, typeName, dim);
	 typeMap.put(typeName+dim, rc);
	 return rc;
      }

      if (Main.getInstance().isDocletRunning()) {
	 //System.err.println(findClass("java.lang.String"));
	 //throw new ParseException("Doclet running, class not found: "+typeName+" ("+orgTypename+")");
      }
      Type rc=new ClassDocProxy(typeName, this);
      ((WritableType)rc).setDimension(dim);
      return rc;
   }

   public boolean isException() {
      for (ClassDoc cdi=this; 
	   cdi!=null; 
	   cdi=cdi.superclass()) {

	 if ("java.lang.Exception".equals(cdi.qualifiedName()))
	    return true;
      }
      return false;
   }

   public boolean isError() {
      for (ClassDoc cdi=this; cdi!=null; cdi=cdi.superclass()) {
	 if ("java.lang.Error".equals(cdi.qualifiedName()))
	    return true;
      }
      return false;
   }

   public boolean isOrdinaryClass() {
      return !isException() && !isError() && !isInterface();
   }

   public void setIsInterface(boolean b) {
      this.isInterface=b;
   }

   public ExecutableMemberDoc findExecutableRec(String nameAndSignature) {
      ExecutableMemberDoc rc;
      for (ClassDocImpl cdi=this; cdi!=null; ) {
	 rc=cdi.findMethod(nameAndSignature);
	 if (rc!=null) return rc;
	 rc=cdi.findConstructor(nameAndSignature);
	 if (rc!=null) return rc;

	 ClassDoc superclass = cdi.superclass();
	 if (null == superclass) {
	    break;
	 }
	 else if (superclass instanceof ClassDocImpl) {
	    cdi=(ClassDocImpl) superclass;
	 }
	 else {
	    Main.getRootDoc().printWarning("In context "+qualifiedName()+": superclass "+superclass.qualifiedName()+" hasn't been loaded.");
	    break;
	 }
      }
      return null;
   } 

   public ConstructorDoc findConstructor(String nameAndSignature) {
      int ndx=nameAndSignature.indexOf('(');
      if (ndx<=0)
	 return null;
      else {
         String fullSignature = resolveSignature(nameAndSignature.substring(ndx));
	 return findConstructor(nameAndSignature.substring(0,ndx),
                                fullSignature);
      }
   }

   public ConstructorDoc findConstructor(String name, String signature) {
      if (null != constructors) {
         for (int i=0; i<constructors.length; ++i) {
            if (constructors[i].name().equals(name) && constructors[i].signature().equals(signature))
               return constructors[i];
         }
      }
      return null;
   }

   public MethodDoc findMethod(String nameAndSignature) {
      int ndx=nameAndSignature.indexOf('(');
      if (ndx<=0) {
	 return null;
      }
      else {
         String name = nameAndSignature.substring(0,ndx);
         String fullSignature = resolveSignature(nameAndSignature.substring(ndx));
	 return findMethod(name, fullSignature);
      }
   }

   private String resolveSignature(String signature) 
   {
      signature = signature.substring(1, signature.length() - 1);
      StringTokenizer st = new StringTokenizer(signature, ",");
      StringBuffer fullSignature = new StringBuffer("(");
      while (st.hasMoreTokens()) {
         String type = st.nextToken().trim();
         int ndx = type.length();
         while (ndx > 0 && type.charAt(ndx - 1) == '[' || type.charAt(ndx - 1) == ']') {
            -- ndx;
         }
         String dim = type.substring(ndx);
         type = type.substring(0, ndx);
         ClassDoc typeClass = findClass(type);
         if (fullSignature.length() > 1) {
            fullSignature.append(",");
         }
         if (null != typeClass) {
            fullSignature.append(typeClass.qualifiedName());
         }
         else {
            fullSignature.append(type);
         }
         fullSignature.append(dim);
      }
      fullSignature.append(')');
      return fullSignature.toString();
   }

   public MethodDoc findMethod(String name, String signature) {
      if (null != methods) {
         for (int i=0; i<methods.length; ++i) {
            if (methods[i].name().equals(name) && methods[i].signature().equals(signature))
               return methods[i];
         }
      }
      return null;
   }

   public boolean equals(Object o) {
      return (o!=null) && (o instanceof ClassDoc) && ((ClassDoc)o).qualifiedName().equals(qualifiedName());
   }

   private ClassDoc[] innerClasses;

   private List maybeSerMethodList;
   
   void setMaybeSerMethodList(List maybeSerMethodList) {
      this.maybeSerMethodList=maybeSerMethodList;
   }

   public void setDimension(String dimension) {
      this.dimension = dimension;
   }

   public Object clone() throws CloneNotSupportedException {
      ClassDocImpl result = (ClassDocImpl)super.clone();
      result.baseClassDoc = baseClassDoc;
      return result;
   }

   public int superHashCode()
   {
      return super.hashCode();
   }

   public int hashCode()
   {
      return qualifiedTypeName().hashCode();
   }

   public ClassDoc getBaseClassDoc()
   {
      return baseClassDoc;
   }
}

