/* gnu.classpath.tools.gjdoc.RootDocImpl
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
import java.lang.reflect.*;

public class RootDocImpl extends DocImpl implements GjdocRootDoc {

   private ErrorReporter reporter = new ErrorReporter();

   private RandomAccessFile rawCommentCache;

   /**
    *  All options and their corresponding values which are not recognized
    *  by Gjdoc. These are passed to the Doclet as "custom options".
    *  Each element in this array is again a String array, with the 
    *  option name as first element (including prefix dash) and possible
    *  option values as following elements.
    */
   private String[][] customOptionArr;

   /**
    *  The names of all classes explicitly specified on the 
    *  command line.
    *
    *  @contains String
    */
   private List specifiedClassNames = new LinkedList();

   /**
    *  All source files explicitly specified on the command line.
    *
    *  @contains File
    */
   private List specifiedSourceFiles = new LinkedList();

   /**
    *  The names of all packages explicitly specified on the 
    *  command line.
    *
    *  @contains String
    */
   private List specifiedPackageNames = new LinkedList();

   /**
    *  Stores all classes specified by the user: those given by
    *  individual class names on the command line, and those
    *  contained in the packages given on the command line.
    *
    *  @contains ClassDocImpl
    */
   private List classesList = new LinkedList(); //new LinkedList();

   /**
    *  Stores all classes loaded in the course of preparing
    *  the documentation data. Maps the fully qualified name 
    *  of a class to its ClassDocImpl representation.
    *
    *  @contains String->ClassDocImpl
    */
   private Map classDocMap = new HashMap();

   /**
    *  Stores all packages loaded in the course of preparing
    *  the documentation data. Maps the package name 
    *  to its PackageDocImpl representation.
    *
    *  @contains String->PackageDocImpl
    */
   private Map packageDocMap = new HashMap();

   /**
    *  All classes specified by the user, both those explicitly
    *  individually specified on the command line and those contained
    *  in packages specified on the command line (as Array for quick
    *  retrieval by Doclet).  This is created from classesList after
    *  all classes have been loaded.  
    */
   private ClassDocImpl[] classes;

   /**
    *  All classes which were individually specified on the command 
    *  line (as Array for quick retrieval by Doclet). This is created 
    *  from specifiedClassNames after all classes have been loaded.
    */
   private ClassDocImpl[] specifiedClasses;

   /**
    *  All packages which were specified on the command line (as Array
    *  for quick retrieval by Doclet). This is created from
    *  specifiedPackageNames after all classes have been loaded.  
    */
   private PackageDocImpl[] specifiedPackages;


   /**
    *  Temporarily stores a list of classes which are referenced
    *  by classes already loaded and which still have to be
    *  resolved.
    */
   private List scheduledClasses=new LinkedList();

   private List sourcePath;

   private String sourceEncoding;

   //--------------------------------------------------------------------------
   //
   // Implementation of RootDoc interface
   //
   //--------------------------------------------------------------------------

   /**
    *  Return classes and interfaces to be documented. 
    */
   public ClassDoc[] classes() { return classes; } 

   /**
    *  Return a ClassDoc object for the specified class/interface 
    *  name.
    *
    *  @return a ClassDoc object describing the given class, or 
    *  <code>null</code> if no corresponding ClassDoc object
    *  has been constructed.
    */
   public ClassDoc classNamed(String qualifiedName) { 
      return (ClassDoc)classDocMap.get(qualifiedName); 
   } 

   /**
    *  Return an xxx
    */
   public String[][] options() { return customOptionArr; } 

   // Return a PackageDoc for the specified package name 
   public PackageDoc packageNamed(String name) { 
      return (PackageDoc)packageDocMap.get(name); 
   }


   // classes and interfaces specified on the command line. 
   public ClassDoc[] specifiedClasses() { return specifiedClasses; } 

   // packages specified on the command line. 
   public PackageDoc[] specifiedPackages() { return specifiedPackages; }

   // Print error message, increment error count. 
   public void printError(java.lang.String msg) {
      reporter.printError(msg);
   }

   // Print error message, increment error count. 
   public void printFatal(java.lang.String msg) {
      reporter.printFatal(msg);
   }

   // Print a message. 
   public void printNotice(java.lang.String msg) {
      reporter.printNotice(msg);
   }
   
   // Print warning message, increment warning count. 
   public void printWarning(java.lang.String msg) {
      reporter.printWarning(msg);
   }

   public String name() {
      return "RootDoc";
   }

   public ErrorReporter getReporter() {
      return reporter;
   }

   public void build() throws ParseException, IOException {

      Parser parser = new Parser();

      //--- Create a temporary random access file for caching comment text.

      //File rawCommentCacheFile=File.createTempFile("gjdoc_rawcomment",".cache");
      File rawCommentCacheFile = new File("gjdoc_rawcomment.cache");
      rawCommentCacheFile.deleteOnExit();
      rawCommentCache = new RandomAccessFile(rawCommentCacheFile, "rw");

      //--- Parse all files in "java.lang".

      File javaLangSources = findSourceFile("java/lang");
      if (null!=javaLangSources) {
	 parser.processSourceDir(javaLangSources, sourceEncoding);
      }
      else {

	 Debug.log(1, "Sourcepath is "+sourcePath);

	 // Core docs not included in source-path: 
	 // we need to gather the information about java.lang
	 // classes via reflection...

      }

      //--- Parse all files in explicitly specified package directories.
	 
      for (Iterator it=specifiedPackageNames.iterator(); it.hasNext(); ) {

	 String specifiedPackageName = (String)it.next();
	 printNotice("Loading classes for package "+specifiedPackageName+"...");
	 File sourceDir = findSourceFile(specifiedPackageName.replace('.',File.separatorChar));
	 if (null!=sourceDir) {
	    parser.processSourceDir(sourceDir, sourceEncoding);
	 }
	 else {
	    printError("Package '"+specifiedPackageName+"' not found.");
	 }
      }

      //--- Parse all explicitly specified class files.

      for (Iterator it=specifiedClassNames.iterator(); it.hasNext(); ) {

	 String specifiedClassName = (String)it.next();
	 printNotice("Loading class "+specifiedClassName+" ...");
	 File sourceFile = findSourceFile(specifiedClassName.replace('.',File.separatorChar)+".java");
	 if (null!=sourceFile) {
	    parser.processSourceFile(sourceFile, true, sourceEncoding);
	 }
	 else {
	    printError("Class '"+specifiedClassName+"' not found.");
	 }
      }

      List specifiedClassesList = new LinkedList();

      //--- Parse all explicitly specified source files.

      for (Iterator it=specifiedSourceFiles.iterator(); it.hasNext(); ) {

	 File specifiedSourceFile = (File)it.next();
	 printNotice("Loading source file "+specifiedSourceFile+" ...");
         ClassDocImpl classDoc = parser.processSourceFile(specifiedSourceFile, true, sourceEncoding);
         if (null != classDoc) {
            specifiedClassesList.add(classDoc);
            classesList.add(classDoc);
            classDoc.setIsIncluded(true);
            if (0 == classDoc.containingPackage().name().length()) {
               addPackageDoc(classDoc.containingPackage());
            }
         }
      }

      //--- Let the user know that all specified classes are loaded.

      printNotice("Constructing Javadoc information...");

      //--- Load all classes implicitly referenced by explicitly specified classes.

      loadScheduledClasses(parser);

      resolveComments();

      //--- Resolve pending references in all ClassDocImpls

      printNotice("Resolving references in classes...");

      for (Iterator it = classDocMap.values().iterator(); it.hasNext(); ) {
	 ClassDocImpl cd=(ClassDocImpl)it.next();
	 cd.resolve();
      }

      //--- Resolve pending references in all PackageDocImpls

      printNotice("Resolving references in packages...");

      for (Iterator it = packageDocMap.values().iterator(); it.hasNext(); ) {
	 PackageDocImpl pd=(PackageDocImpl)it.next();
	 pd.resolve();
      }

      //--- Assemble the array with all specified classes

      for (Iterator it = specifiedClassNames.iterator(); it.hasNext(); ) {
	 String specifiedClassName = (String)it.next();
	 ClassDocImpl specifiedClassDoc = (ClassDocImpl)classDocMap.get(specifiedClassName);
         if (null == specifiedClassDoc) {
            printWarning("No documentation found for class " +specifiedClassName + " - wrong filename?");
         }
         else {
            specifiedClassDoc.setIsIncluded(true);
            specifiedClassesList.add(specifiedClassDoc);
            classesList.add(specifiedClassDoc);
         }
      }
      this.specifiedClasses=(ClassDocImpl[])specifiedClassesList.toArray(new ClassDocImpl[0]);

      //--- Assemble the array with all specified packages

      Set specifiedPackageSet = new LinkedHashSet();
      for (Iterator it = specifiedPackageNames.iterator(); it.hasNext(); ) {
	 String specifiedPackageName = (String)it.next();
	 PackageDoc specifiedPackageDoc = (PackageDoc)packageDocMap.get(specifiedPackageName);
	 if (null!=specifiedPackageDoc) {
            //System.err.println("include package " + specifiedPackageName);

	    ((PackageDocImpl)specifiedPackageDoc).setIsIncluded(true);
	    specifiedPackageSet.add(specifiedPackageDoc);

	    ClassDoc[] packageClassDocs=specifiedPackageDoc.allClasses();
	    for (int i=0; i<packageClassDocs.length; ++i) {
	       ClassDocImpl specifiedPackageClassDoc=(ClassDocImpl)packageClassDocs[i];
            
	       specifiedPackageClassDoc.setIsIncluded(true);

               /*
               if (specifiedPackageClassDoc.isIncluded()) {
                  System.err.println("include class " + specifiedPackageClassDoc.name() + " (" + specifiedPackageClassDoc + "@" + specifiedPackageClassDoc.hashCode() + ")");
               }
               */

	       classesList.add(specifiedPackageClassDoc);
	    }
	 }
      }
      this.specifiedPackages=(PackageDocImpl[])specifiedPackageSet.toArray(new PackageDocImpl[0]);

      //--- Resolve pending references in comment data of all classes

      printNotice("Resolving references in class comments...");

      for (Iterator it=classDocMap.values().iterator(); it.hasNext(); ) {
	 ClassDocImpl cd=(ClassDocImpl)it.next();
	 cd.resolveComments();
      }

      //--- Resolve pending references in comment data of all packages

      printNotice("Resolving references in package comments...");

      for (Iterator it=packageDocMap.values().iterator(); it.hasNext(); ) {
	 PackageDocImpl pd=(PackageDocImpl)it.next();
	 pd.resolveComments();
      }

      //--- Create array with all loaded classes

      this.classes=(ClassDocImpl[])classesList.toArray(new ClassDocImpl[0]);
      Arrays.sort(this.classes);

      //--- Close comment cache

      parser = null;
      System.gc();
      System.gc();
   }

   public long writeRawComment(String rawComment) {
      try {
	 long pos=rawCommentCache.getFilePointer();
	 //rawCommentCache.writeUTF(rawComment);
         byte[] bytes = rawComment.getBytes("utf-8");
         rawCommentCache.writeInt(bytes.length);
         rawCommentCache.write(bytes);
	 return pos;
      }
      catch (IOException e) {
	 printFatal("Cannot write to comment cache: "+e.getMessage());
	 return -1;
      }
   }

   public String readRawComment(long pos) {
      try {
	 rawCommentCache.seek(pos);
         int sz = rawCommentCache.readInt();
         byte[] bytes = new byte[sz];
         rawCommentCache.read(bytes);
         return new String(bytes, "utf-8");
	 //return rawCommentCache.readUTF();
      }
      catch (IOException e) {
         e.printStackTrace();
	 printFatal("Cannot read from comment cache: "+e.getMessage());
	 return null;
      }
   }

   File findSourceFile(String relPath) {

      for (Iterator it = sourcePath.iterator(); it.hasNext(); ) {
	 File path = (File)it.next();
	 File file = new File(path, relPath);
	 if (file.exists()) return file;
      }

      return null;
   }

   PackageDocImpl findOrCreatePackageDoc(String packageName) {
      PackageDocImpl rc=(PackageDocImpl)getPackageDoc(packageName);
      if (null==rc) {
	 rc=new PackageDocImpl(packageName);
	 if (specifiedPackageNames.contains(packageName)) {
            String packageDirectoryName = packageName.replace('.', File.separatorChar);
            File packageDirectory = findSourceFile(packageDirectoryName);
	    File packageDocFile = new File(packageDirectory, "package.html");
            rc.setPackageDirectory(packageDirectory);
	    if (null!=packageDocFile && packageDocFile.exists()) {
	       try {
                  rc.setRawCommentText(readHtmlBody(packageDocFile));
	       }
	       catch (IOException e) {
		  printWarning("Error while reading documentation for package "+packageName+": "+e.getMessage());
	       }
	    }
	    else {
	       printNotice("No description found for package "+packageName);
	    }
	 }
	 addPackageDoc(rc);
      }
      return rc;
   }

   public void addClassDoc(ClassDoc cd) {
      classDocMap.put(cd.qualifiedName(), cd);
   }

   public void addPackageDoc(PackageDoc pd) {
      packageDocMap.put(pd.name(), pd);
   }

   public PackageDocImpl getPackageDoc(String name) {
      return (PackageDocImpl)packageDocMap.get(name);
   }

   public ClassDocImpl getClassDoc(String qualifiedName) {
      return (ClassDocImpl)classDocMap.get(qualifiedName);
   }

   class ScheduledClass {

      ClassDoc contextClass;
      String qualifiedName;
      ScheduledClass(ClassDoc contextClass, String qualifiedName) {
	 this.contextClass=contextClass;
	 this.qualifiedName=qualifiedName;
      }
      
      public String toString() { return "ScheduledClass{"+qualifiedName+"}"; }
   }

   public void scheduleClass(ClassDoc context, String qualifiedName) throws ParseException, IOException {

      if (classDocMap.get(qualifiedName)==null) {

	 //Debug.log(9,"Scheduling "+qualifiedName+", context "+context+".");

	 scheduledClasses.add(new ScheduledClass(context, qualifiedName));
      }
   }

   /**
    *  Load all classes that were implictly referenced by the classes
    *  (already loaded) that the user explicitly specified on the
    *  command line.
    *
    *  For example, if the user generates Documentation for his simple
    *  'class Test {}', which of course 'extends java.lang.Object',
    *  then 'java.lang.Object' is implicitly referenced because it is
    *  the base class of Test.
    *
    *  Gjdoc needs a ClassDocImpl representation of all classes
    *  implicitly referenced through derivation (base class),
    *  or implementation (interface), or field type, method argument
    *  type, or method return type.
    *
    *  The task of this method is to ensure that Gjdoc has all this
    *  information at hand when it exits.
    *
    * 
    */
   public void loadScheduledClasses(Parser parser) throws ParseException, IOException {

      // Because the referenced classes could in turn reference other
      // classes, this method runs as long as there are still unloaded
      // classes.

      while (!scheduledClasses.isEmpty()) {

	 // Make a copy of scheduledClasses and empty it. This 
	 // prevents any Concurrent Modification issues.
	 // As the copy won't need to grow (as it won't change)
	 // we make it an Array for performance reasons.

	 ScheduledClass[] scheduledClassesArr = (ScheduledClass[])scheduledClasses.toArray(new ScheduledClass[0]);
	 scheduledClasses.clear();

	 // Load each class specified in our array copy
	 
	 for (int i=0; i<scheduledClassesArr.length; ++i) {

	    // The name of the class we are looking for. This name
	    // needs not be fully qualified.
	    
	    String scheduledClassName=scheduledClassesArr[i].qualifiedName;

	    // The ClassDoc in whose context the scheduled class was looked for.
	    // This is necessary in order to resolve non-fully qualified 
	    // class names.
	    ClassDoc scheduledClassContext=scheduledClassesArr[i].contextClass;

	    // If there already is a class doc with this name, skip. There's
	    // nothing to do for us.
	    if (classDocMap.get(scheduledClassName)!=null) {
	       continue;
	    }

	    try {
	       // Try to load the class
	       loadScheduledClass(parser, scheduledClassName, scheduledClassContext);
	    }
	    catch (ParseException e) {

	       /**********************************************************

               // Check whether the following is necessary at all.


	       if (scheduledClassName.indexOf('.')>0) {

	       // Maybe the dotted notation doesn't mean a package
	       // name but instead an inner class, as in 'Outer.Inner'.
	       // so let's assume this and try to load the outer class.

		  String outerClass="";
		  for (StringTokenizer st=new StringTokenizer(scheduledClassName,"."); st.hasMoreTokens(); ) {
		     if (outerClass.length()>0) outerClass+=".";
		     outerClass+=st.nextToken();
		     if (!st.hasMoreTokens()) break;
		     try {
			loadClass(outerClass);
			//FIXME: shouldn't this be loadScheduledClass(outerClass, scheduledClassContext); ???
			continue;
		     }
		     catch (Exception ee) {
		     // Ignore: try next level
		     }
		  }
	       }

	       **********************************************************/

	       // If we arrive here, the class could not be found

	       printWarning("Couldn't load class "+scheduledClassName+" referenced by "+scheduledClassContext);

	       //FIXME: shouldn't this be throw new Error("cannot load: "+scheduledClassName);
	    }
	 }
      }
   }

   private void loadScheduledClass(Parser parser, String scheduledClassName, ClassDoc scheduledClassContext) throws ParseException, IOException {

      ClassDoc loadedClass=(ClassDoc)scheduledClassContext.findClass(scheduledClassName);

      if (loadedClass==null || loadedClass instanceof ClassDocProxy) {

	 File file=findScheduledClassFile(scheduledClassName, scheduledClassContext);
	 if (file!=null) {
	    parser.processSourceFile(file, false, sourceEncoding);
	 }
	 else {
	    // It might be an inner class of one of the outer/super classes.
	    // But we can only check that when they are all fully loaded.
	    boolean retryLater = false;

	    int numberOfProcessedFilesBefore = parser.getNumberOfProcessedFiles();

	    ClassDoc cc = scheduledClassContext.containingClass();
	    while (cc != null && !retryLater) {
	       ClassDoc sc = cc.superclass();
	       while (sc != null && !retryLater) {
		  if (sc instanceof ClassDocProxy) {
		     ((ClassDocImpl)cc).resolve();
		     retryLater = true;
		  }
		  sc = sc.superclass();
	       }
	       cc = cc.containingClass();
	    }

	    // Now that outer/super references have been resolved, try again
	    // to find the class.

	    loadedClass = (ClassDoc)scheduledClassContext.findClass(scheduledClassName);

	    int numberOfProcessedFilesAfter = parser.getNumberOfProcessedFiles();

	    boolean filesWereProcessed = numberOfProcessedFilesAfter > numberOfProcessedFilesBefore;

	    // Only re-schedule class if additional files have been processed
	    // If there haven't, there's no point in re-scheduling.
	    // Will avoid infinite loops of re-scheduling
	    if (null == loadedClass && retryLater && filesWereProcessed)
	       scheduleClass(scheduledClassContext, scheduledClassName);

	    /* A warning needn't be emitted - this is normal, can happen
	       if the scheduled class is in a package which is not
	       included on the command line.

	       else if (null == loadedClass)
	       printWarning("Can't find scheduled class '"
	       + scheduledClassName
	       + "' in context '"
	       + scheduledClassContext.qualifiedName()
	       + "'");
	    */
	 }
      }
   }


   public File findScheduledClassFile(String scheduledClassName, ClassDoc scheduledClassContext) 

      throws ParseException, IOException {

      File rc;

      if (scheduledClassName.indexOf('.')<0) {

	 ClassDoc[] importedClasses=scheduledClassContext.importedClasses();
	 int j;
	 for (j=0; j<importedClasses.length; ++j) {
	    if (importedClasses[j].qualifiedName().endsWith("."+scheduledClassName)) {
	       rc = findClass(importedClasses[j].qualifiedName());
	       if (rc!=null) return rc;
	    }
	 }
	    
	 PackageDoc[] importedPackages=scheduledClassContext.importedPackages();

	 for (j=0; j<importedPackages.length; ++j) {
	    rc = findClass(importedPackages[j].name()+"."+scheduledClassName);
	    if (rc!=null) return rc;
	 }
      }

      rc = findClass(scheduledClassName);
      if (rc!=null) return rc;

      if (scheduledClassContext.containingPackage()!=null) {
	 rc = findClass(scheduledClassContext.containingPackage().name()+"."+scheduledClassName);
	 if (rc!=null) return rc;
      }

      return findClass("java.lang."+scheduledClassName);
   }

   public File findClass(String qualifiedName) throws IOException {

      if (Main.getInstance().isDocletRunning()) return null;

      if (Main.getRootDoc().getClassDoc(qualifiedName)==null) {

	 String relPath=qualifiedName.replace('.',File.separatorChar)+".java";
	 //String filename=new File(Main.getRootDoc().getsourcePath()).getCanonicalFile().getAbsolutePath()+File.separatorChar+relPath;
	 for (Iterator it=sourcePath.iterator(); it.hasNext(); ) {

	    File sourcePath = (File)it.next();

	    String filename=sourcePath.getAbsolutePath()+File.separatorChar+relPath;
	    File file = null;

	    Debug.log(9, "loadClass: trying file "+filename);

	    // FIXME: the following can probably be done simpler and more elegant using File.getParent()

	    while ((!(file=new File(filename)).exists()) 
		   || (!file.getAbsolutePath().toLowerCase().endsWith(relPath.toLowerCase()))) {
	       int ndx=filename.lastIndexOf(File.separatorChar);
	       if (ndx>=0) {
		  filename=filename.substring(0,ndx)+".java";
		  //Debug.log(6,"loadClass: trying file "+filename);
	       }
	       else {
		  file = null;
		  break;
	       }
	    }

	    if (null!=file) return file;
	 }

	 return null;
      }
      else {
	 //Debug.log(9, "class \""+qualifiedName+"\" already loaded.");
	 return null;
      }
   }

   public static boolean recursiveClasses = false;

   public void addSpecifiedPackageName(String packageName) {
      specifiedPackageNames.add(packageName);
   }

   public void addSpecifiedClassName(String className) {
      specifiedClassNames.add(className);
   }

   public void addSpecifiedSourceFile(File sourceFile) {
      specifiedSourceFiles.add(sourceFile);
   }

   public boolean hasSpecifiedPackagesOrClasses() {
      return !specifiedClassNames.isEmpty() 
         || !specifiedPackageNames.isEmpty()
         || !specifiedSourceFiles.isEmpty();
   }

   public void setOptions(String[][] customOptionArr) {
      this.customOptionArr = customOptionArr;
   }

   public void setSourcePath(List sourcePath) {
      this.sourcePath = sourcePath;
   }

   public void finalize() throws Throwable {
      super.finalize();
   }

   public void flush()
   {
      try {
         rawCommentCache.close();
      }
      catch (IOException e) {
         printError("Cannot close raw comment cache");
      }

      rawCommentCache = null;
      customOptionArr = null;
      specifiedClassNames = null;
      specifiedPackageNames = null;
      classesList = null;
      classDocMap = null;
      packageDocMap = null;
      classes = null;
      specifiedClasses = null;
      specifiedPackages = null;
      scheduledClasses = null;
      sourcePath = null;
   }

   public void setSourceEncoding(String sourceEncoding)
   {
      this.sourceEncoding = sourceEncoding;
   }

   public RootDocImpl()
   {
      super(null);
   }

   public static String readHtmlBody(File file) 
      throws IOException
   {
      FileReader fr=new FileReader(file);
      long size = file.length();
      char[] packageDocBuf=new char[(int)(size)];
      int index = 0;
      int i = fr.read(packageDocBuf, index, (int)size);
      while (i > 0) {
         index += i;
         size -= i;
         i = fr.read(packageDocBuf, index, (int)size);
      }
      fr.close();

      // We only need the part between the begin and end body tag.
      String html = new String(packageDocBuf);
      int start = html.indexOf("<body");
      if (start == -1)
         start = html.indexOf("<BODY");
      int end = html.indexOf("</body>");
      if (end == -1)
         end = html.indexOf("</BODY>");
      if (start != -1 && end != -1) {
         // Start is end of body tag.
         start = html.indexOf('>', start) + 1;
         if (start != -1 && start < end)
            html = html.substring(start, end);
      }
      return html.trim();
   }
}
