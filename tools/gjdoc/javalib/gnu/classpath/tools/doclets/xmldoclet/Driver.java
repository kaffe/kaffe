/* gnu.classpath.tools.doclets.xmldoclet.Driver
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

package gnu.classpath.tools.doclets.xmldoclet;

import com.sun.javadoc.*;
import java.io.*;

import com.sun.tools.doclets.Taglet;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

import java.text.DateFormat;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import gnu.classpath.tools.gjdoc.TemporaryStore;

import gnu.classpath.tools.doclets.xmldoclet.doctranslet.DocTranslet;

/**
 *  A Doclet which retrieves all information presented by the Doclet
 *  API, dumping it to stdout in XML format.
 *
 *  @author Julian Scheid
 */
public class Driver {

   public static final String XMLDOCLET_VERSION = "0.6.1";

   /**
    *  Used for redirecting error messages to <code>/dev/null</code>.
    */
   private static class NullErrorReporter implements DocErrorReporter {
      public void printError(String ignore) {}
      public void printWarning(String ignore) {}
      public void printNotice(String ignore) {}
   }

   /**
    *  Used for writing error messages to <code>System.err</code>.
    */
   private static class BackupErrorReporter implements DocErrorReporter {
      public void printError(String msg) { 
         System.err.println("ERROR: " + msg); 
      }
      public void printWarning(String msg) {
         System.err.println("WARNING: " + msg); 
      }
      public void printNotice(String msg) {
         System.err.println(msg); 
      }
   }

   /*
    *  Taglet context constants.
    */
   private static final int CONTEXT_CONSTRUCTOR  = 1;
   private static final int CONTEXT_FIELD        = 2;
   private static final int CONTEXT_METHOD       = 3;
   private static final int CONTEXT_OVERVIEW     = 4;
   private static final int CONTEXT_PACKAGE      = 5;
   private static final int CONTEXT_TYPE         = 6;

   /**
    *  Errors and warnings will be printed using this after
    *  <code>rootDoc</code> has been garbage collected.
    */
   private DocErrorReporter reporter = new BackupErrorReporter();

   /**
    *  All XML output will go to this stream.
    */
   private PrintWriter out;

   /**
    *  How many spaces to indent each XML node level,
    *  i.e. Tab size for output.
    */
   private static int indentStep = 1;

   /**
    *  Won't output superfluous spaces if set to true.
    *  If set to false, output will be more legible.
    */
   private boolean compress = false;

   /**
    *  Won't output warning messages while fixing
    *  HTML code if set to true.
    */
   private boolean noHTMLWarn = false;

   /**
    *  Won't output warning messages when encountering tags
    *  that look like an email address if set to true.
    */
   private boolean noEmailWarn = false;

   /**
    *  Will fix HTML if necessary so that each comment
    *  contains valid XML code if set to true. If set
    *  to false, HTML code will not be modified and
    *  instead encapsulated in a CDATA section.
    */
   private boolean fixHTML = true;

   /**
    *  User-specified name of the directory where the final version of
    *  the generated files will be written to.
    *
    *  If no XSLT sheet is given, the XML output will go directly into
    *  this directory. Otherwise, XML output will go to a temporary
    *  directory and XSLT output will go to this directory.
    */
   private File targetDirectory = null;

   /**
    *  Directory where XML output will be written to. If no XSLT
    *  sheet was given, this is the target directory specified
    *  by the user. Otherwise, this is a temporary directory.
    */
   private File xmlTargetDirectory;

   /**
    *  Contains a number of TargetContexts which describe which XSLT
    *  sheet to apply to the output of this doclet, to what directory
    *  the XSLT output is written, and which postprocess driver to use
    *  to process XSLT output.
    */
   private List targets = new ArrayList();

   /**
    *  XML text to include at the end of every generated page. Read
    *  from the file specified on the command line using -bottomnote.
    *  If present, this will be written to the main output file
    *  (index.xml) in node /gjdoc:rootDoc/gjdoc:bottomnote.
    */
   private String bottomNote;

   /**
    *  Brief description of the package set. Can be specified on the
    *  command line using -title.  This will be written to the main
    *  output file (index.xml) in node
    *  /gjdoc:rootDoc/gjdoc:title. The HTML generating XSLT sheet
    *  uses this for example in window titles.
    */
   private String title;

   /**
    *  Path to the directory where temporary files should be stored.
    *  Defaults to system tempdir, but can be overridden by user 
    *  with -workpath.
    */
   private String workingPath = System.getProperty("java.io.tmpdir");

   /**
    *  Temporary directory created by this doclet where all 
    *  temporary files will be stored in. If no temporary
    *  files are needed (i.e. no XSLT postprocessing stage
    *  specified by user), this is <code>null</code>.
    */
    private File workingDirectory;

   /**
    *  Stores the Doclet API RootDoc we are operating on.
    */
   private RootDoc rootDoc;

   /**
    *  XML namespace prefix used for all tags, except for HTML
    *  tags copied from Javadoc comments. Excluding colon.
    */
   public static final String tagPrefix = "gjdoc";

   /**
    *  Classpath for loading Taglet classes.
    */
   private String tagletPath = null;

   /**
    * The current class that is being processed.
    * Set in outputClassDoc().
    */
   private ClassDoc currentClass;

   /**
    * The current member that is being processed.
    * Set in outputMemberDoc().
    */
   private MemberDoc currentMember;

   /**
    * The current constructor/method that is being processed.
    * Set in outputExecutableMemberDoc().
    */
   private ExecutableMemberDoc currentExecMember;

   /**
    * Mapping from tag type to Taglet.
    */
   private Map tagletMap = new LinkedHashMap();

   public static boolean start(TemporaryStore _rootDocWrapper) {
      return new Driver().instanceStart((RootDoc)_rootDocWrapper.getAndClear());
   }

   /**
    *  Official Doclet entry point.
    */
   public static boolean start(RootDoc _rootDoc) {

      // Create a new XmlDoclet instance and delegate control.
      TemporaryStore tstore = new TemporaryStore(_rootDoc);
      _rootDoc = null;
      return new Driver().instanceStart((RootDoc)tstore.getAndClear());
   }

   /**
    *  Output an XML tag describing a com.sun.javadoc.Type object.
    *  Assumes that the tag does not have subtags.
    *
    *  @param level  Level of indentation. Will be multiplied by 
    *                <code>indentStep</code> to yield actual amount
    *                of whitespace inserted at start of line.
    *  @param tag    Identifier for the XML tag being output.
    *  @param type   The Javadoc Type to be output.
    */
   protected void outputType(int level, String tag, Type type) {
      outputType(level, tag, type, true);
   }

   protected void outputType(int level, String tag, Type type, boolean atomic) {
      println(level, "<"+tagPrefix+":"+tag+" typename=\""+type.typeName()+"\""+
	      " qualifiedtypename=\""+type.qualifiedTypeName()+"\""
	      +(type.dimension().length()==0?"":" dimension=\""+type.dimension()+"\"")
	      +(atomic?"/":"")+">");
   }

   protected void outputExecutableMemberDocBody(int level, ExecutableMemberDoc memberDoc) {

      currentExecMember = memberDoc;

      outputMemberDocBody(level, memberDoc);

      Parameter[] parameters = memberDoc.parameters();
      for (int i=0, ilim=parameters.length; i<ilim; ++i) {
	 Parameter parameter = parameters[i];
	 outputType(level, "parameter name=\""+parameter.name()+"\"", parameter.type());
      }

      ClassDoc[] exceptions = memberDoc.thrownExceptions();
      for (int i=0, ilim=exceptions.length; i<ilim; ++i) {
	 ClassDoc exception = exceptions[i];
	 outputType(level, "thrownException", exception);
       }

      printAtomTag(level, "signature full=\""+memberDoc.signature()+"\" flat=\""+memberDoc.flatSignature()+"\"");

      if (memberDoc.isNative()) {
	 printAtomTag(level, "isNative");
      }

      if (memberDoc.isSynchronized()) {
	 printAtomTag(level, "isSynchronized");
      }
   }

   protected void outputMethodDoc(int level, MethodDoc methodDoc) {
      println();
      printOpenTag(level, "methoddoc name=\""+methodDoc.name()+"\"");
      outputExecutableMemberDocBody(level+1, methodDoc);
      outputType(level+1, "returns", methodDoc.returnType());
      printCloseTag(level, "methoddoc");
   }

   protected void outputMemberDocBody(int level, MemberDoc memberDoc) {
      currentMember = memberDoc;
      outputProgramElementDocBody(level, memberDoc);
   }

   protected void outputFieldDocBody(int level, FieldDoc fieldDoc) {
      outputType(level, "type", fieldDoc.type());
      if (fieldDoc.isTransient()) {
	 printAtomTag(level, "isTransient");
      }
      if (fieldDoc.isVolatile()) {
	 printAtomTag(level, "isVolatile");
      }
   }

   private void outputFieldDoc(int level, FieldDoc fieldDoc) {
      println();
      printOpenTag(level, "fielddoc name=\""+fieldDoc.name()+"\"");
      outputMemberDocBody(level+1, fieldDoc);
      outputFieldDocBody(level+1, fieldDoc);
      printCloseTag(level, "fielddoc");
   }

   protected void outputConstructorDoc(int level, ConstructorDoc constructorDoc) {
      println();
      printOpenTag(level, "constructordoc name=\""+constructorDoc.name()+"\"");
      outputExecutableMemberDocBody(level+1, constructorDoc);
      printCloseTag(level, "constructordoc");
   }

   protected void outputSuperInterfacesRec(int level, ClassDoc classDoc) {
      if (null!=classDoc) {
	 ClassDoc[] interfaces = classDoc.interfaces();
	 for (int i=0, ilim=interfaces.length; i<ilim; ++i) {
	    outputType(level, "superimplements", interfaces[i]);
	 }
	 outputSuperInterfacesRec(level, classDoc.superclass());
      }
   }

   protected void outputClassDocSummary(ClassDoc classDoc) {
      println();
      printOpenTag(1, "classdoc name=\""+classDoc.name()+"\" qualifiedtypename=\""+classDoc.qualifiedName()+"\"");
      if (null!=classDoc.superclass()) {
	 outputType(2, "superclass", classDoc.superclass());
      }

      ClassDoc[] interfaces = classDoc.interfaces();
      for (int i=0, ilim=interfaces.length; i<ilim; ++i) {
	 outputType(2, "implements", interfaces[i]);
      }
      outputSuperInterfacesRec(2, classDoc.superclass());

      printAtomTag(2, "containingPackage name=\""+classDoc.containingPackage().name()+"\"");
      if (classDoc.isError()) {
         printAtomTag(2, "isError");
      }
      if (classDoc.isException()) {
         printAtomTag(2, "isException");
      }
      if (classDoc.isInterface()) {
         printAtomTag(2, "isInterface");
      }

      printCloseTag(1, "classdoc");
   }

   protected void outputPackageDoc(PackageDoc packageDoc) {
      println();
      printOpenTag(1, "packagedoc name=\""+packageDoc.name()+"\"");
      if (packageDoc.firstSentenceTags().length > 0) {
	 printOpenTag(2, "firstSentenceTags", false);
	 outputTags(3, packageDoc.firstSentenceTags(), true, CONTEXT_PACKAGE);
	 printCloseTag(0, "firstSentenceTags");
      }

      if (packageDoc.tags().length > 0) {
	 printOpenTag(2, "tags");
	 outputTags(3, packageDoc.tags(), true, CONTEXT_PACKAGE);
	 printCloseTag(2, "tags");
      }

      if (packageDoc.seeTags().length > 0) {
	 printOpenTag(2, "seeTags");
	 outputTags(3, packageDoc.seeTags(), true, CONTEXT_PACKAGE);
	 printCloseTag(2, "seeTags");
      }

      ClassDoc[] allClasses = (ClassDoc[]) packageDoc.allClasses().clone();
      Arrays.sort(allClasses, new Comparator() {
	    public int compare(Object o1, Object o2) {
	       return ((ClassDoc)o1).name().compareTo(((ClassDoc)o2).name());
	    }
	 });

      if (false) {
	 for (int i = 0, ilim = allClasses.length; i < ilim; ++ i) {
	    printAtomTag(2, "containsClass qualifiedtypename=\""+allClasses[i].qualifiedTypeName()+"\"");
	 }
      }

      printCloseTag(1, "packagedoc");
   }

   protected void outputClassDoc(ClassDoc classDoc) {

      currentClass = classDoc;

      println();
      printOpenTag(1, "classdoc xmlns=\"http://www.w3.org/TR/REC-html40\" xmlns:"+tagPrefix+"=\"http://www.gnu.org/software/cp-tools/gjdocxml\" name=\""+classDoc.name()+"\" qualifiedtypename=\""+classDoc.qualifiedName()+"\"");

      if (null!=classDoc.superclass()) {
	 outputType(2, "superclass", classDoc.superclass());
      }

      ClassDoc[] interfaces = classDoc.interfaces();
      for (int i=0, ilim=interfaces.length; i<ilim; ++i) {
	 outputType(2, "implements", interfaces[i]);
      }
      outputSuperInterfacesRec(2, classDoc.superclass());

      outputProgramElementDocBody(2, classDoc);
      if (classDoc.isAbstract())
	 printAtomTag(2, "isAbstract");
      if (classDoc.isSerializable())
	 printAtomTag(2, "isSerializable");
      if (classDoc.isExternalizable())
	 printAtomTag(2, "isExternalizable");
      if (classDoc.definesSerializableFields())
	 printAtomTag(2, "definesSerializableFields");

      ConstructorDoc[] constructors = classDoc.constructors();
      for (int i=0, ilim=constructors.length; i<ilim; ++i) {
	 outputConstructorDoc(2, constructors[i]);
      }

      MethodDoc[] methods = classDoc.methods();
      for (int i=0, ilim=methods.length; i<ilim; ++i) {
	 outputMethodDoc(2, methods[i]);
      }

      FieldDoc[] fields = classDoc.fields();
      for (int i=0, ilim=fields.length; i<ilim; ++i) {
	 outputFieldDoc(2, fields[i]);
      }

      printCloseTag(1, "classdoc");

      currentClass = null;
      currentMember = null;
      currentExecMember = null;
   }

   protected void outputDocBody(int level, Doc doc) {

      int context = CONTEXT_TYPE;

      if (doc.isClass()) {
	 printAtomTag(level, "isClass");
      }
      if (doc.isConstructor()) {
	 printAtomTag(level, "isConstructor");
         context = CONTEXT_CONSTRUCTOR;
      }
      if (doc.isError()) {
	 printAtomTag(level, "isError");
      }
      if (doc.isException()) {
	 printAtomTag(level, "isException");
      }
      if (doc.isField()) {
	 printAtomTag(level, "isField");
         context = CONTEXT_FIELD;
      }
      if (doc.isIncluded()) {
	 printAtomTag(level, "isIncluded");
      }
      if (doc.isInterface()) {
	 printAtomTag(level, "isInterface");
      }
      if (doc.isMethod()) {
	 printAtomTag(level, "isMethod");
         context = CONTEXT_METHOD;
      }
      if (doc.isOrdinaryClass()) {
	 printAtomTag(level, "isOrdinaryClass");
      }

      if (doc.inlineTags().length > 0) {
	 printOpenTag(level, "inlineTags", false);
	 outputTags(level+1, doc.inlineTags(), true, context);
	 printCloseTag(0, "inlineTags");
      }

      if (doc.firstSentenceTags().length > 0) {
	 printOpenTag(level, "firstSentenceTags", false);
	 outputTags(level+1, doc.firstSentenceTags(), true, context);
	 printCloseTag(0, "firstSentenceTags");
      }

      if (doc.tags().length > 0) {
	 printOpenTag(level, "tags");
	 outputTags(level+1, doc.tags(), true, context);
	 printCloseTag(level, "tags");
      }

      if (doc.seeTags().length > 0) {
	 printOpenTag(level, "seeTags");
	 outputTags(level+1, doc.seeTags(), true, context);
	 printCloseTag(level, "seeTags");
      }
   }

   protected void outputProgramElementDocBody(int level, ProgramElementDoc programElementDoc) {
      outputDocBody(level, programElementDoc);
      printAtomTag(level, "containingPackage name=\""+programElementDoc.containingPackage().name()+"\"");
      if (null!=programElementDoc.containingClass()) {
	 outputType(level, "containingClass", programElementDoc.containingClass());
      }
      String access;
      if (programElementDoc.isPublic()) 
	 access="public";
      else if (programElementDoc.isProtected()) 
	 access="protected";
      else if (programElementDoc.isPrivate()) 
	 access="private";
      else if (programElementDoc.isPackagePrivate()) 
	 access="package";
      else
	 throw new RuntimeException("Huh? "+programElementDoc+" is neither public, protected, private nor package protected.");
      printAtomTag(level, "access scope=\""+access+"\"");
      if (programElementDoc.isFinal())
	 printAtomTag(level, "isFinal");
      if (programElementDoc.isStatic())
	 printAtomTag(level, "isStatic");
   }

   protected void outputTags(int level, Tag[] tags, boolean descend, int context) {

      HtmlRepairer htmlRepairer = new HtmlRepairer(rootDoc, noHTMLWarn, noEmailWarn,
						   currentClass, currentMember);

      for (int i=0, ilim=tags.length; i<ilim; ++i) {
	 Tag tag = tags[i];
	 if (!"Text".equals(tag.name())) {
	    printOpenTag(0 /* don't introduce additional whitespace */, 
			 "tag kind=\""+tag.kind()+"\" name=\""+tag.name()+"\"", false);
	 }
	 if (tag instanceof ThrowsTag) {
	    ThrowsTag throwsTag = (ThrowsTag)tag;
	    if (null!=throwsTag.exception()) {
	       outputType(level+1, "exception", throwsTag.exception());
	    }
	    else {
	       StringBuffer sb = new StringBuffer("In ThrowsTag: Exception ");
	       sb.append(throwsTag.exceptionName());
	       sb.append(" not found in ");
	       if (currentExecMember instanceof MethodDoc) {
		   MethodDoc m = (MethodDoc)currentExecMember;
		   sb.append(m.returnType().typeName());
		   sb.append(m.returnType().dimension());
		   sb.append(' ');
	       }
	       sb.append(currentClass.qualifiedName());
	       sb.append('.');
	       sb.append(currentExecMember.name());
	       sb.append('(');
	       Parameter[] params = currentExecMember.parameters();
	       for (int j=0; j < params.length; j++) {
		   sb.append(params[j].type().typeName());
		   sb.append(params[j].type().dimension());
		   sb.append(' ');
		   sb.append(params[j].name());
		   if (j != params.length-1)
			sb.append(", ");
	       }
	       sb.append(')');
	       printWarning(sb.toString());

	       printAtomTag(level+1, "exception typename=\""+throwsTag.exceptionName()+"\"");
	    }
	 }
	 else if (tag instanceof ParamTag) {
	    ParamTag paramTag = (ParamTag)tag;
	    printAtomTag(level+1, "parameter name=\""+paramTag.parameterName()+"\"");
	 }

	 if (null != tag.text()) {
	    //printOpenTag(level+1, "text", false);
	    if (fixHTML) {
	       print(htmlRepairer.getWellformedHTML(tag.text()));
	       print(htmlRepairer.terminateText());
	    }
	    else {
	       print("<![CDATA["+cdata(tag.text())+"]]>");
	    }
	    //printCloseTag(0 /* don't introduce additional whitespace */, "text");
	 }
	 else {
	    printWarning("Tag got null text: "+tag);
	 }

	 if (descend && ("@throws".equals(tag.name()) || "@param".equals(tag.name()))) {
	    if (tag.firstSentenceTags().length>0) {
	       printOpenTag(level+1, "firstSentenceTags", false);
	       outputTags(level+2, tag.firstSentenceTags(), false, context);
	       printCloseTag(0, "firstSentenceTags");
	    }
	    
	    if (tag.inlineTags().length>0) {
	       printOpenTag(level+1, "inlineTags", false);
	       outputTags(level+2, tag.firstSentenceTags(), false, context);
	       printCloseTag(0, "inlineTags");
	    }
	 }

	 if (fixHTML && i == ilim - 1) {
	    String terminateText = htmlRepairer.terminateText();
	    if (null != terminateText && terminateText.length() > 0) {
	       print(terminateText);
	    }
	 }

	 if (!"Text".equals(tag.name())) {

            Taglet inlineTaglet = (Taglet)tagletMap.get(tag.name().substring(1));
            if (null != inlineTaglet && inlineTaglet.isInlineTag()) {
	       printOpenTag(0, "inlineTagletText", false);
               print(inlineTaglet.toString(tag));
	       printCloseTag(0, "inlineTagletText");
            }

	    printCloseTag(0, "tag", false);
	 }
      }

      outputTagletText(tags, context, htmlRepairer);
   }
   
   void outputTagletText(Tag[] tags, int context, HtmlRepairer htmlRepairer) {

      for (Iterator it = tagletMap.keySet().iterator(); it.hasNext(); ) {
         String tagName = (String)it.next();
         Object o = tagletMap.get(tagName);
         Taglet taglet = (Taglet)o;
         List tagsOfThisType = new ArrayList();

         if (!taglet.isInlineTag()
             && ((context != CONTEXT_CONSTRUCTOR || taglet.inConstructor())
                 || (context != CONTEXT_FIELD || taglet.inField())
                 || (context != CONTEXT_METHOD || taglet.inMethod())
                 || (context != CONTEXT_OVERVIEW || taglet.inOverview())
                 || (context != CONTEXT_PACKAGE || taglet.inPackage())
                 || (context != CONTEXT_TYPE || taglet.inType()))) {
            
            for (int i=0, ilim=tags.length; i<ilim; ++i) {
               if (tags[i].name().substring(1).equals(tagName)) {
                  tagsOfThisType.add(tags[i]);
               }
            }
            if (!tagsOfThisType.isEmpty()) {
               printOpenTag(0, "tagletText tagName=\"" + tagName + "\"");
               Tag[] tagletTags = (Tag[])tagsOfThisType.toArray(new Tag[tagsOfThisType.size()]);
               String tagletString = getTagletForName(tagName).toString(tagletTags);
               if (fixHTML) {
                  print(htmlRepairer.getWellformedHTML(tagletString));
                  print(htmlRepairer.terminateText());
               }
               else {
                  print("<![CDATA["+cdata(tagletString)+"]]>");
               }
               printCloseTag(0, "tagletText", false);
            }
         }
      }
   }
   

   protected Taglet getTagletForName(String name) {
      return (Taglet)tagletMap.get(name);
   }

   /**
    *  Inofficial entry point. We got an instance here.
    */
   protected boolean instanceStart(RootDoc _rootDoc) {

      this.rootDoc = _rootDoc;
      _rootDoc = null;
      
      boolean xmlOnly = false;

      try {
	 {

	    // Process command line options passed through to this doclet
	    
	    TargetContext targetContext = null;
	    
	    TargetContext htmlTargetContext
               = new TargetContext(DocTranslet.fromClasspath("/doctranslets/html/gjdoc.xsl"), 
                                   targetDirectory);

	    for (int i=0, ilim=rootDoc.options().length; i<ilim; ++i) {

	       String[] option = rootDoc.options()[i];
	       String optionTag = option[0];

	       if ("-d".equals(optionTag)) {
		  if (null == targetDirectory) {
		     targetDirectory = new File(option[1]);
		  }
		  if (null != targetContext) {
		     targetContext.setTargetDirectory(new File(option[1]));
		  }
	       }

	       else if ("-nofixhtml".equals(optionTag)) {
		  fixHTML = false;
                  printError("-nofixhtml currently not supported.");
                  return false;
	       }
	       else if ("-compress".equals(optionTag)) {
		  compress = true;
	       }
	       else if ("-nohtmlwarn".equals(optionTag)) {
		  noHTMLWarn = true;
	       }
	       else if ("-noemailwarn".equals(optionTag)) {
		  noEmailWarn = true;
	       }
	       else if ("-indentstep".equals(optionTag)) {
		  indentStep = Integer.parseInt(option[1]);
	       }
	       else if ("-doctranslet".equals(optionTag)) {
		  targets.add(targetContext = new TargetContext(DocTranslet.fromJarFile(new File(option[1])), 
                                                                targetDirectory));
	       }
	       else if ("-genhtml".equals(optionTag)) {
		  htmlTargetContext.setTargetDirectory(targetDirectory);
		  targets.add(targetContext = htmlTargetContext);
	       } 
	       else if ("-geninfo".equals(optionTag)) {
                  targetContext
                              = new TargetContext(DocTranslet.fromClasspath("/doctranslets/info/gengj.xsl"), 
                                                  targetDirectory);
		  targets.add(targetContext);
		  if (!fixHTML) {
		     printNotice("NOTE: -geninfo implies -fixhtml.");
		     fixHTML = true;
		  }
	       }
	       else if ("-gendocbook".equals(optionTag)) {
                  targetContext = new TargetContext(DocTranslet.fromClasspath("/doctranslets/docbook/gengj.xsl"), 
                                                    targetDirectory);
		  targets.add(targetContext);
		  if (!fixHTML) {
		     printNotice("NOTE: -gendocbook implies -fixhtml.");
		     fixHTML = true;
		  }
	       }
	       else if ("-genpdf".equals(optionTag)) {
                  targetContext
                     = new TargetContext(DocTranslet.fromClasspath("/doctranslets/docbook/gengj.xsl"), 
                                         targetDirectory);
                                         /** "gnu.classpath.tools.doclets.xmldoclet.DocBookPostprocessor") **/
		  targets.add(targetContext);
		  if (!fixHTML) {
		     printNotice("NOTE: -genpdf implies -fixhtml.");
		     fixHTML = true;
		  }
	       }
	       else if ("-xmlonly".equals(optionTag)) {
		  xmlOnly = true;
	       }
	       else if ("-bottomnote".equals(optionTag)) {

		  FileReader reader = new FileReader(option[1]);
		  StringWriter writer = new StringWriter();
		  char[] buf = new char[256];
		  int nread;
		  while ((nread = reader.read(buf)) >= 0) {
		     writer.write(buf, 0, nread);
		  }
		  writer.flush();
		  bottomNote = writer.toString();
		  writer.close();
		  reader.close();
	       }
	       else if ("-title".equals(optionTag)) {

		  title = option[1];
	       }
	       else if ("-workpath".equals(optionTag)) {

		  workingPath = option[1];
	       }
	       else if ("-tagletpath".equals(optionTag)) {

                  if (null == tagletPath) {
                     tagletPath = option[1];
                  }
                  else {
                     tagletPath = tagletPath + File.pathSeparator + option[1];
                  }
	       }
               else if ("-taglet".equals(optionTag)) {

                  boolean tagletLoaded = false;

                  String useTagletPath = this.tagletPath;
                  if (null == useTagletPath) {
                     useTagletPath = System.getProperty("java.class.path");
                  }

                  try {
                     Class tagletClass;
                     try {
                        tagletClass
                           = new FileSystemClassLoader(useTagletPath).loadClass(option[1]);
                     }
                     catch (ClassNotFoundException e) {
                        // If not found on specified tagletpath, try default classloader
                        tagletClass
                           = Class.forName(option[1]);
                     }
                     Method registerTagletMethod
                        = tagletClass.getDeclaredMethod("register", new Class[] { java.util.Map.class });

                     if (!registerTagletMethod.getReturnType().equals(Void.TYPE)) {
                        printError("Taglet class '" + option[1] + "' found, but register method doesn't return void.");
                     }
                     else if (registerTagletMethod.getExceptionTypes().length > 0) {
                        printError("Taglet class '" + option[1] + "' found, but register method contains throws clause.");
                     }
                     else if ((registerTagletMethod.getModifiers() & (Modifier.STATIC | Modifier.PUBLIC | Modifier.ABSTRACT)) != (Modifier.STATIC | Modifier.PUBLIC)) {
                        printError("Taglet class '" + option[1] + "' found, but register method isn't public static, or is abstract..");
                     }
                     else {
                        registerTagletMethod.invoke(null, new Object[] { tagletMap });
                        tagletLoaded = true;
                     }
                  }
                  catch (NoSuchMethodException e) {
                     printError("Taglet class '" + option[1] + "' found, but doesn't contain the register method.");
                  }
                  catch (SecurityException e) {
                     printError("Taglet class '" + option[1] + "' cannot be loaded: " + e.getMessage());
                  }
                  catch (InvocationTargetException e) {
                     printError("Taglet class '" + option[1] + "' found, but register method throws exception: " + e.toString());
                  }
                  catch (IllegalAccessException e) {
                     printError("Taglet class '" + option[1] + "' found, but there was a problem when accessing the register method: " + e.toString());
                  }
                  catch (IllegalArgumentException e) {
                     printError("Taglet class '" + option[1] + "' found, but there was a problem when accessing the register method: " + e.toString());
                  }
                  catch (ClassNotFoundException e) {
                     printError("Taglet class '" + option[1] + "' cannot be found.");
                  }
                  if (!tagletLoaded) {
                     return false;
                  }
               }
	    }

	    // It is illegal to specify targets AND -xmlonly.

	    if (xmlOnly && targets.size() > 0) {

	       printError("You can only specify one of -xmlonly and a target format.");
	       return false;
	    }

	    // If no target was specified and XML only was not
	    // requested, use HTML as default target.

	    if (!xmlOnly && targets.size() == 0) {
	       targets.add(targetContext = htmlTargetContext);
	    }

	    // Set the same target directory for all output.

	    // FIXME: Allow separate target directories for different
	    // output formats.

	    for (Iterator it = targets.iterator(); it.hasNext(); ) {
	       TargetContext t = (TargetContext)it.next();
	       t.setTargetDirectory(targetDirectory);
	    }

	    // Create temporary directory if necessary

	    if (xmlOnly) {

	       xmlTargetDirectory = targetDirectory;
	    }
	    else {

	       File workingTopDirectory = new File(workingPath);

	       workingDirectory = new File(workingTopDirectory, "gjdoc.tmp."+System.currentTimeMillis());
	    
	       if (!workingDirectory.mkdir()) {
		  printError("Cannot create temporary directory at "+System.getProperty("java.io.tmpdir"));
		  return false;
	       }

	       File xmlTempDirectory = new File(workingDirectory, "xmloutput");

	       if (!xmlTempDirectory.mkdir()) {
		  printError("Cannot create temporary directory for XML output at "+System.getProperty("java.io.tmpdir"));
		  return false;
	       }

	       xmlTargetDirectory = xmlTempDirectory;
	    }

	    printNotice("Writing XML Index file...");

	    // Assign output stream

	    setTargetFile("index.xml");

	    // Output XML document header

	    println(0, "<?xml version=\"1.0\"?>");
	    println("<!DOCTYPE gjdoc SYSTEM \"dtd/gjdoc.dtd\">");
	    println();
	    printOpenTag(0, "rootdoc xmlns=\"http://www.w3.org/TR/REC-html40\" xmlns:gjdoc=\"http://www.gnu.org/software/cp-tools/gjdocxml\"");
	 
	    if (null != bottomNote) {
	       printOpenTag(1, "bottomnote");
	       print(bottomNote);
	       printCloseTag(1, "bottomnote");
	    }

	    if (null != title) {
	       printOpenTag(1, "title");
	       println(2, title);
	       printCloseTag(1, "title");
	    }
	 
	    printOpenTag(1, "created");
	    println(2, DateFormat.getDateInstance(DateFormat.LONG, Locale.US).format(new java.util.Date()));
	    printCloseTag(1, "created");


	    // Output summary of all classes specified on command line

	    println();
	    println(1, "<!-- Classes specified by user on command line -->");
	    ClassDoc[] specifiedClasses = rootDoc.specifiedClasses();
	    for (int i=0, ilim=specifiedClasses.length; i<ilim; ++i) {
	       ClassDoc sc = specifiedClasses[i];
	       printAtomTag(1, "specifiedclass fqname=\""+sc.qualifiedName()+"\" name=\""+sc.name()+"\"");
	    }
	    specifiedClasses = null;

	    // Output summary of all packages specified on command line

	    println();
	    println(1, "<!-- Packages specified by user on command line -->");
	    PackageDoc[] specifiedPackages = rootDoc.specifiedPackages();
	    for (int i=0, ilim=specifiedPackages.length; i<ilim; ++i) {
	       PackageDoc sp = specifiedPackages[i];
	       printAtomTag(1, "specifiedpackage name=\""+sp.name()+"\"");
	    }
	    specifiedPackages = null;
	 
	    // Output information on all packages for which documentation
	    // has been made available via the Doclet API

	    println();
	    println(1, "<!-- Documentation for all packages -->");
	    PackageDoc[] packages = rootDoc.specifiedPackages();
	    for (int i=0, ilim=packages.length; i<ilim; ++i) {
	       PackageDoc c = packages[i];
	       outputPackageDoc(c);
	    }
	    packages = null;

	    // Output brief summary on all classes for which documentation
	    // has been made available via the Doclet API.
	    //
	    // While this is redundant, it can speed up XSLT
	    // processing by orders of magnitude

	    println();
	    println(1, "<!-- Brief summary for all classes -->");
	    ClassDoc[] sumclasses = rootDoc.classes();
	    for (int i=0, ilim=sumclasses.length; i<ilim; ++i) {
	       ClassDoc c = sumclasses[i];
	       outputClassDocSummary(c);
	    }
	    sumclasses = null;
	    
	    // Output closing tag, finish output stream

	    println();
	    printCloseTag(0, "rootdoc");

	    closeTargetFile();


	    // Output information on all classes for which documentation
	    // has been made available via the Doclet API
	    
	    println();
	    println(1, "<!-- Documentation for all classes -->");
	    ClassDoc[] classes = rootDoc.classes();
	    for (int i = 0, ilim = classes.length; i < ilim; ++ i) {
	       ClassDoc c = classes[i];

	       printNotice("Writing XML information for "+c.qualifiedName()+"...");
	       
	       setTargetFile(c.qualifiedName().replace('/','.')+".xml");
	       
	       println("<?xml version=\"1.0\"?>");
               println("<!DOCTYPE gjdoc SYSTEM \"dtd/gjdoc.dtd\">");
	       
	       outputClassDoc(c);
	       
	       closeTargetFile();
	    }
	    classes = null;
	 }
	 
         // Copy DTD files to temporary directory
         
         String[] resources = new String[] {
            "gjdoc.dtd",
            "dbcentx.mod",
            "ent/iso-amsa.ent",
            "ent/iso-amsb.ent",
            "ent/iso-amsc.ent",
            "ent/iso-amsn.ent",
            "ent/iso-amso.ent",
            "ent/iso-amsr.ent",
            "ent/iso-box.ent",
            "ent/iso-cyr1.ent",
            "ent/iso-cyr2.ent",
            "ent/iso-dia.ent",
            "ent/iso-grk1.ent",
            "ent/iso-grk2.ent",
            "ent/iso-grk3.ent",
            "ent/iso-grk4.ent",
            "ent/iso-lat1.ent",
            "ent/iso-lat2.ent",
            "ent/iso-num.ent",
            "ent/iso-pub.ent",
            "ent/iso-tech.ent",
         };

         File tempDtdDirectory = new File(xmlTargetDirectory, "dtd");
         File tempDtdEntDirectory = new File(tempDtdDirectory, "ent");

         if (tempDtdDirectory.mkdir() && tempDtdEntDirectory.mkdir()) {
            for (int i = 0; i < resources.length; ++ i) {
               copyResourceToFile("/dtd/" + resources[i], 
                                  new File(tempDtdDirectory, resources[i]));
            }
         }
         else {
            printError("Cannot create temporary directories for DTD data at " + tempDtdDirectory);
            return false;
         }

	 // All information has been output. Apply stylesheet if given.

	 gnu.classpath.tools.gjdoc.Main.releaseRootDoc();
	 this.rootDoc = null;
	 this.currentClass = null;
	 this.currentMember = null;
	 this.currentExecMember = null;
	 
	 System.gc();
	 
	 // From this point we are only operating on files, so we don't
	 // need this anymore and can free up some memory

         for (Iterator it = targets.iterator(); it.hasNext(); ) {

            TargetContext target = (TargetContext)it.next();

	    // We have XSLT postprocessing, run DocTranslet.

            DocTranslet docTranslet = DocTranslet.fromClasspath("/doctranslets/html/gjdoc.xsl");

            target.getDocTranslet().apply(xmlTargetDirectory, 
                                          target.getTargetDirectory(), 
                                          reporter);
	 }

	 // Done

	 targets = null;

	 System.gc();
         Runtime.getRuntime().runFinalization();

	 return true;
      }
      catch (Exception e) {

	 // Something went wrong. Report to stderr and pass error to
	 // Javadoc Reporter

	 e.printStackTrace();
	 printError(e.toString());
	 return false;
      }
      finally {

	 // In any case, delete the working directory if we created one

	 if (null != workingDirectory) {

	    if (!deleteRecursive(workingDirectory)) {
	       printWarning("Could not delete temporary directory at "+workingDirectory);
	    }
	 }

	 printNotice("Done.");
      }
   }

   /**
    * Recursively delete the specified directory and its contents,
    * like <code>rm -Rf directory</code>
    *
    * @return <code>true</code> on success
    */
   private static boolean deleteRecursive(File directory) {

      boolean success = true;

      File[] files = directory.listFiles();

      for (int i=0, ilim=files.length; i<ilim; ++i) {

	 File file = files[i];

	 if (file.isDirectory()) {

	    success = deleteRecursive(file) && success;
	 }
	 else {

	    success = file.delete() && success;
	 }
      }

      return directory.delete() && success;
   }

   /**
    *  Prints a string to stdout and appends a newline.  Convenience
    *  method.  
    */
   protected void println(String str) {
      out.println(str);
   }

   /**
    *  Prints a string to stdout without appending a newline.
    *  Convenience method.  
    */
   protected void print(String str) {
      out.print(str);
   }

   /**
    *  In standard mode, prints an empty line to stdout.
    *  In thight mode, nothing happens.
    *  Convenience method.  
    */
   protected void println() {
      if (!compress) {
	 out.println();
      }
   }

   /**
    *  In standard mode, prints the given text indented to stdout and appends newline. 
    *  In tight mode, doesn't print indentation or newlines.
    */
   protected void print(int indentLevel, String msg) {
      if (compress) {
	 out.print(msg);
      }
      else {
	 StringBuffer indentation = new StringBuffer();
	 for (int i=0; i<indentLevel*indentStep; ++i) {
	    indentation.append(' ');
	 }
	 out.print(indentation+msg);
      }
   }
   
   /**
    *  In tight mode, prints a message at a given indentation level.
    *  In standard mode, appends a newline in addition.
    */
   protected void println(int indentLevel, String msg) {
      print(indentLevel, msg);
      if (!compress) out.println();
   }

   /**
    *  Prints an atom tag at the given indentation level.
    */
   protected void printAtomTag(int level, String tag) {
      println(level, "<"+tagPrefix+":"+replaceCharsInTag(tag)+"/>");
   }

   /**
    *  Prints an open tag at the given indentation level.
    */
   protected void printOpenTag(int level, String tag) {
      printOpenTag(level, replaceCharsInTag(tag), true);
   }

   /**
    *  Prints an open tag at the given indentation level and
    *  conditionally appends a newline (if not in tight mode).
    */
   protected void printOpenTag(int level, String tag, boolean appendNewline) {
      if (appendNewline && !compress) {
	 println(level, "<"+tagPrefix+":"+replaceCharsInTag(tag)+">");
      }
      else {
	 print(level, "<"+tagPrefix+":"+replaceCharsInTag(tag)+">");
      }
   }

   /**
    *  Prints a close tag at the given indentation level.
    */
   protected void printCloseTag(int level, String tag) {
      printCloseTag(level, tag, true);
   }

   /**
    *  Prints a close tag at the given indentation level and
    *  conditionally appends a newline (if not in tight mode).
    */
   protected void printCloseTag(int level, String tag, boolean appendNewline) {
      if (appendNewline && !compress) {
	 println(level, "</"+tagPrefix+":"+replaceCharsInTag(tag)+">");
      }
      else {
	 print(level, "</"+tagPrefix+":"+replaceCharsInTag(tag)+">");
      }
   }

   public static int optionLength(String option) {
      if ("-d".equals(option)) return 2;
      else if ("-fixhtml".equals(option)) return 1;
      else if ("-compress".equals(option)) return 1;
      else if ("-nohtmlwarn".equals(option)) return 1;
      else if ("-noemailwarn".equals(option)) return 1;
      else if ("-indentstep".equals(option)) return 2;
      else if ("-xslsheet".equals(option)) return 2;
      else if ("-xsltdriver".equals(option)) return 2;
      else if ("-postprocess".equals(option)) return 2;
      else if ("-genhtml".equals(option)) return 1;
      else if ("-geninfo".equals(option)) return 1;
      else if ("-gendocbook".equals(option)) return 1;
      else if ("-xmlonly".equals(option)) return 1;
      else if ("-bottomnote".equals(option)) return 2;
      else if ("-workpath".equals(option)) return 2;
      else if ("-title".equals(option)) return 2;
      else if ("-tagletpath".equals(option)) return 2;
      else if ("-taglet".equals(option)) return 2;
      else return -1;
   }

   public static boolean validOptions(String[][] options) {
      return true;
   }


   /**
    *  Workaround for non well-formed comments: fix tag contents
    *  by replacing <code>&lt;</code> with <code>&amp;lt;</code>,
    *  <code>&gt;</code> with <code>&amp;gt;</code> and
    *  <code>&amp;</code> with <code>&amp;amp;</code>.
    *
    *  @param tagContent  String to process
    *
    *  @return given String with all special characters replaced by 
    *          HTML entities.
    */
   private static String replaceCharsInTag(String tagContent) {
      return 
	 replaceString(
	    replaceString(
	       replaceString(
		  tagContent, 
		  "<", "&lt;"
		  ), 
	       ">", "&gt;"
	       ),
	    "&", "&amp;"
	    );
   }

   /**
    *  Replaces all occurences of string <code>needle</code> within string
    *  <code>haystack</code> by string <code>replacement</code>.
    *
    *  @param haystack    The string to search and replace in.
    *  @param needle      The string which is searched for.
    *  @param replacement The string by which every occurence of <code>needle</code> is replaced.
    */
   private static String replaceString(String haystack, String needle, String replacement) {
      int ndx = haystack.indexOf(needle);
      if (ndx<0)
	 return haystack;
      else
	 return haystack.substring(0, ndx) + replacement 
	    + replaceString(haystack.substring(ndx+needle.length()), needle, replacement);
   }

   protected void setTargetFile(String filename) throws IOException {

      OutputStream fileOut = new FileOutputStream(new File(xmlTargetDirectory, filename));
      out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(fileOut, "UTF8")));;
   }

   protected void closeTargetFile() {

      out.flush();
      out.close();
   }

   private String cdata(String str) {

      if (null==str) {
	 return str;
      } // end of if ((null==str)

      StringBuffer rc = new StringBuffer();
      for (int i=0; i<str.length(); ++i) {
	 char c = str.charAt(i);
	 if (c==0x09 || c==0x0a || c==0x0d || (c>=0x20 && c<=0xd7ff) || (c>=0xe000 && c<=0xfffd) || (c>=0x10000 && c<=0x10ffff)) {
	    rc.append(c);
	 }
	 else {
	    printWarning("Invalid Unicode character 0x"+Integer.toString(c, 16)+" in javadoc markup has been stripped.");
	 } // end of else
	 
      }
      return rc.toString();
   }

   static void copyResourceToFile(String resourceName, File target) throws IOException {
      
      InputStream in = Driver.class.getResourceAsStream(resourceName);

      if (null != in) {

	 FileOutputStream out = new FileOutputStream(target);
	 int size;
	 byte[] buffer = new byte[512];
	 while ((size = in.read(buffer)) >= 0) {
	    out.write(buffer, 0, size);
	 }
	 out.close();
      }
      else {

	 throw new IOException("Can't find resource named "+resourceName);
      }
   }

   private void printError(String error) {
      if (null != rootDoc) {
	 rootDoc.printError(error);
      }
      /*
      else if (null != reporter) {
	 reporter.printError(error);
      }
      */
      else {
	 System.err.println("ERROR: "+error);
      }
   }

   private void printWarning(String warning) {
      if (null != rootDoc) {
	 rootDoc.printWarning(warning);
      }
      /*
      else if (null != reporter) {
	 reporter.printWarning(warning);
      }
      */
      else {
	 System.err.println("WARNING: "+warning);
      }
   }

   private void printNotice(String notice) {
      if (null != rootDoc) {
	 rootDoc.printNotice(notice);
      }
      /*
      else if (null != reporter) {
	 reporter.printNotice(notice);
      }
      */
      else {
	 System.err.println(notice);
      }
   }
}
