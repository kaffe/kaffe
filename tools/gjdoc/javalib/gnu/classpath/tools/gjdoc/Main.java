/* gnu.classpath.tools.gjdoc.Main
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
import java.io.*;
import java.util.*;
import java.lang.reflect.*;

public final class Main {

   /**
    *  Do we load classes that are referenced as base
    *  class?
    */
   static final boolean DESCEND_SUPERCLASS = true;

   /**
    *  Do we load classes that are referenced as
    *  interface?
    */
   static final boolean DESCEND_INTERFACES = false;

   /**
    *  Do we load classes that are imported in a 
    *  source file?
    */
   static final boolean DESCEND_IMPORTED   = true;

   /**
    *  Document only public members.
    */
   static final int COVERAGE_PUBLIC    = 0;

   /**
    *  Document only public and protected members.
    */
   static final int COVERAGE_PROTECTED = 1;

   /**
    *  Document public, protected and package private members.
    */
   static final int COVERAGE_PACKAGE   = 2;

   /**
    *  Document all members.
    */
   static final int COVERAGE_PRIVATE   = 3;

   /**
    *  Grid for looking up whether a particular access level
    *  is included in the documentation.
    */
   static final boolean[][] coverageTemplates = new boolean[][] {
      new boolean[] { true, false, false, false },  // public
      new boolean[] { true, true,  false, false },  // protected
      new boolean[] { true, true,  true,  false },  // package
      new boolean[] { true, true,  true,  true  },  // private
   };

   /**
    *  Holds the Singleton instance of this class.
    */
   private static Main instance = new Main();

   /**
    *  Avoid re-instantiation of this class.
    */
   private Main() {}

   private static RootDocImpl rootDoc;

   private ErrorReporter reporter;

   /**
    *  <code>false</code> during Phase I: preparation
    *  of the documentation data. <code>true</code>
    *  during Phase II: documentation output by doclet.
    */
   boolean docletRunning=false;

   //---- Command line options

   /**
    *  Option "-doclet": name of the Doclet class to use.
    */
   private String option_doclet = "gnu.classpath.tools.doclets.xmldoclet.Driver";

   /**
    *  Option "-overview": path to the special overview file.
    */
   private String option_overview;

   /**
    *  Option "-coverage": which members to include in generated documentation.
    */
   private int option_coverage = COVERAGE_PROTECTED;

   /**
    *  Option "-help": display command line usage.
    */
   private boolean option_help;

   /**
    *  Option "-docletpath": path to doclet classes.
    */
   private String option_docletpath;
   
   /**
    *  Option "-classpath": path to additional classes.
    */
   private String option_classpath;

   /**
    *  Option "-sourcepath": path to the Java source files to be documented.
    *  FIXME: this should be a list of paths
    */
   private List   option_sourcepath = new ArrayList();

   /**
    *  Option "-bootclasspath": path to Java bootstrap classes.
    */
   private String option_bootclasspath;

   /**
    *  Option "-extdirs": path to Java extension files.
    */
   private String option_extdirs;

   /**
    *  Option "-verbose": Be verbose when generating documentation.
    */
   private boolean option_verbose;

   /**
    *  Option "-nowarn": Do not print warnings.
    */
   private boolean option_nowarn;

   /**
    *  Option "-locale:" Specify the locale charset of Java source files.
    */
   private String option_locale;

   /**
    *  Option "-encoding": Specify character encoding of Java source files.
    */
   private String option_encoding;

   /**
    *  Option "-J": Specify flags to be passed to Java runtime.
    */
   private List option_java_flags = new LinkedList(); //ArrayList();
   
   /**
    *  Parse all source files/packages and subsequentially
    *  start the Doclet given on the command line.
    *
    *  @param customOptions  List of unrecognized command line tokens
    */
   private void startDoclet(List customOptions) {

      try {

	 //--- Fetch the Class object for the Doclet.

	 Debug.log(1, "loading doclet class...");

	 Class docletClass = Class.forName(option_doclet);
	 //Object docletInstance = docletClass.newInstance();

	 Debug.log(1, "doclet class loaded...");

	 Method startTempMethod=null;
	 Method startMethod=null;
	 Method optionLenMethod=null;
	 Method validOptionsMethod=null;

	 //--- Try to find the optionLength method in the Doclet class.

	 try {
	    optionLenMethod = docletClass.getMethod("optionLength", new Class[]{String.class});
	 }
	 catch (NoSuchMethodException e) {
	    // Ignore if not found; it's OK it the Doclet class doesn't define this method.
	 }

	 //--- Try to find the validOptions method in the Doclet class.

	 try {
	    validOptionsMethod = docletClass.getMethod("validOptions", new Class[]{String[][].class, DocErrorReporter.class});
	 }
	 catch (NoSuchMethodException e) {
	    // Ignore if not found; it's OK it the Doclet class doesn't define this method.
	 }

	 //--- Find the start method in the Doclet class; complain if not found

	 try {
	    startTempMethod = docletClass.getMethod("start", new Class[]{TemporaryStore.class});
	 }
	 catch (Exception e) {
	    // ignore
	 }
	 startMethod = docletClass.getMethod("start", new Class[]{RootDoc.class});

	 //--- Feed the custom command line tokens to the Doclet

	 // stores all recognized options
	 List options = new LinkedList();

	 // stores packages and classes defined on the command line
	 List packageAndClasses = new LinkedList(); 

	 for (Iterator it = customOptions.iterator(); it.hasNext(); ) {
	    String option = (String)it.next();

	    Debug.log(9, "parsing option '"+option+"'");

	    if (option.startsWith("-")) {

	       //--- Parse option
	       
	       int optlen=0;

	       //--- Try to get option length from Doclet class

	       if (optionLenMethod!=null) {
		  
		  optionLenMethod.invoke(null, new Object[]{option});

		  Debug.log(3, "invoking optionLen method");

		  optlen=((Integer)optionLenMethod.invoke(null, new Object[]{option})).intValue();

		  Debug.log(3, "done");
	       }

	       if (optlen<=0) {

		  //--- Complain if not found

		  reporter.printError("Unknown option "+option);
		  shutdown();
	       }
	       else {

		  //--- Read option values

		  String[] optionAndValues=new String[optlen];
		  optionAndValues[0]=option;
		  for (int i=1; i<optlen; ++i) {
		     if (!it.hasNext()) {
			reporter.printError("Missing value for option "+option);
			shutdown();
		     }
		     else {
			optionAndValues[i]=(String)it.next();
		     }
		  }
		  
		  //--- Store option for processing later

		  options.add(optionAndValues);
	       }
	    }
	    else {

	       //--- Add to list of packages/classes if not option or option value

	       packageAndClasses.add(option);
	    }
	 }

	 Debug.log(9, "options parsed...");
	 

	 //--- Complain if no packages or classes specified

	 if (packageAndClasses.isEmpty()) {
	    reporter.printError("No packages or classes specified.");
	    usage();
	    shutdown();
	 }

	 //--- For each class or package specified on the command line,
	 //         check that it exists and find out whether it is a class
         //         or a package

	 for (Iterator it=packageAndClasses.iterator(); it.hasNext(); ) {

	    String classOrPackage=(String)it.next();

	    //--- Check for illegal name

	    if (classOrPackage.startsWith(".") 
		|| classOrPackage.endsWith(".") 
		|| classOrPackage.indexOf("..")>0 
		|| !checkCharSet(classOrPackage,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.")) {
	       throw new ParseException("Illegal class or package name '"+classOrPackage+"'");
	    }

	    //--- Assemble absolute path to package

 	    String classOrPackageRelPath = classOrPackage.replace('.', File.separatorChar);

	    //--- Create one file object each for a possible package directory
	    //         and a possible class file, and find out if they exist.

	    File packageDir = rootDoc.findSourceFile(classOrPackageRelPath);
	    File sourceFile = rootDoc.findSourceFile(classOrPackageRelPath+".java");

	    boolean packageDirExists = packageDir!=null && packageDir.exists() 
	       && packageDir.getCanonicalFile().getAbsolutePath().endsWith(classOrPackageRelPath);

	    boolean sourceFileExists = sourceFile!=null && sourceFile.exists() 
	       && sourceFile.getCanonicalFile().getAbsolutePath().endsWith(classOrPackageRelPath+".java");

	    //--- Complain if neither exists: not found

	    if (!packageDirExists && !sourceFileExists) {
	       reporter.printError("Class or package "+classOrPackage+" not found.");
	       shutdown();
	    }

	    //--- Complain if both exist: ambigious

	    else if (packageDirExists && sourceFileExists) {
	       reporter.printError("Ambigious class/package name "+classOrPackage+".");
	       shutdown();
	    }

	    //--- Otherwise, if the package directory exists, it is a package

	    else if (packageDirExists) {
	       if (!packageDir.isDirectory()) {
		  reporter.printError("File \""+packageDir+"\" doesn't have .java extension.");
		  shutdown();
	       }
	       else {
		  rootDoc.addSpecifiedPackageName(classOrPackage);
	       }
	    }

	    //--- Otherwise, it must be a Java source file file

	    else /* if (sourceFileExists) */ {
	       if (sourceFile.isDirectory()) {
		  reporter.printError("File \""+sourceFile+"\" is a directory!");
		  shutdown();
	       }
	       else {
		  rootDoc.addSpecifiedClassName(classOrPackage);
	       }
	    }
	 }

	 //--- Validate custom options passed on command line
	 //         by asking the Doclet if they are OK.

	 String[][] customOptionArr=(String[][])options.toArray(new String[0][0]);
	 if (validOptionsMethod!=null 
	     && !((Boolean)validOptionsMethod.invoke(null, 
						     new Object[]{customOptionArr,
								  this})).booleanValue()) {
	    // Not ok: shutdown system.
	    shutdown();
	 }

	 rootDoc.setOptions(customOptionArr);

	 rootDoc.build();

	 //--- Our work is done, tidy up memory

	 System.gc();
	 System.gc();

	 //--- Set flag indicating Phase II of documentation generation

	 docletRunning=true;

	 //--- Invoke the start method on the Doclet: produce output

	 reporter.printNotice("Running doclet...");

	 TemporaryStore tstore = new TemporaryStore(this.rootDoc);

	 if (null != startTempMethod) {
	    startTempMethod.invoke(null, new Object[]{ tstore });
	 }
	 else {
	    startMethod.invoke(null, new Object[]{ tstore.getAndClear() });
	 }

	 //--- Let the user know how many warnings/errors occured
      
	 if (reporter.getWarningCount()>0) {
	    System.err.println(reporter.getWarningCount()+" warnings");
	 }
      
	 if (reporter.getErrorCount()>0) {
	    System.err.println(reporter.getErrorCount()+" errors");
	 }

	 System.gc();

	 //--- Done.
      }
      catch (Exception e) {
	 e.printStackTrace();
      }
   }

   /**
    *
    */
   private static boolean validOptions(String options[][], 
				       DocErrorReporter reporter) {

      boolean foundDocletOption = false;
      for (int i = 0; i < options.length; i++) {
	 String[] opt = options[i];
	 if (opt[0].equals("-doclet")) {
	    if (foundDocletOption) {
	       reporter.printError("Only one -doclet option allowed.");
	       return false;
	    } else { 
	       foundDocletOption = true;
	    }
	 } 
      }

      return true;
   }

   /**
    *  Main entry point.
    *
    *  This is the method called when gjdoc is invoked
    *  from the command line.
    */
   public static void main(String[] args) {

      try {

	 //--- For testing purposes only

	 //System.err.println("getting locale...");
	 //java.util.Locale loc = java.util.Locale.getDefault();
	 //System.err.println("locale="+loc.getLanguage()+" ("+loc.getDefault()+"), "+loc.getLanguage()+", "+loc.getVariant()+", "+loc.getCountry());

	 //--- Remember current time for profiling purposes

	 Timer.setStartTime();

	 //--- Handle control to the Singleton instance of this class
	 
	 instance.start(args);
      }
      catch (Exception e) {

	 //--- Report any error

	 e.printStackTrace();
      }
   }

   /**
    *  Parses command line arguments and subsequentially
    *  handles control to the startDoclet() method
    *
    *  @param args Command line arguments, as passed to the main() method
    */
   public void start(String[] args) throws ParseException, IOException {

      //--- Collect unparsed arguments in array and resolve references
      //         to external argument files.

      List arguments = new ArrayList(args.length);

      for (int i=0; i<args.length; ++i) {
	 if (!args[i].startsWith("@")) {
	    arguments.add(args[i]);
	 }
	 else {
	    FileReader reader = new FileReader(args[i].substring(1));
	    StreamTokenizer st = new StreamTokenizer(reader);
	    st.resetSyntax();
	    st.wordChars('\u0000', '\uffff');
	    st.quoteChar('\"');
	    st.whitespaceChars(' ', ' ');
	    st.whitespaceChars('\t', '\t');
	    st.whitespaceChars('\r', '\r');
	    st.whitespaceChars('\n', '\n');
	    while (st.nextToken() != StreamTokenizer.TT_EOF) {
	       arguments.add(st.sval);
	    }
	 }
      }

      //--- Initialize Map for option parsing

      initOptions();

      //--- This will hold all options recognized by gjdoc itself
      //         and their associated arguments.
      //         Contains objects of type String[], where each entry
      //         specifies an option along with its aguments.

      List options=new LinkedList();

      //--- This will hold all command line tokens not recognized
      //         to be part of a standard option.
      //         These options are intended to be processed by the doclet
      //         Contains objects of type String, where each entry is
      //         one unrecognized token.

      List customOptions=new LinkedList();


      rootDoc = new RootDocImpl();
      reporter = rootDoc.getReporter();

      //--- Iterate over all options given on the command line

      for (Iterator it = arguments.iterator(); it.hasNext(); ) {

	 String arg = (String)it.next();

	 //--- Check if gjdoc recognizes this option as a standard option
	 //         and remember the options' argument count
	 
	 int optlen = optionLength(arg);

	 //--- Argument count == 0 indicates that the option is not recognized.
	 //         Add it to the list of custom option tokens

	 if (optlen == 0) {
	    customOptions.add(arg);
	 }

	 //--- Otherwise the option is recognized as a standard option.
	 //         if all required arguments are supplied. Create a new String
	 //         array for the option and its arguments, and store it
	 //         in the options array.

	 else {
	    String[] option=new String[optlen];
	    option[0] = arg;
	    boolean optargs_ok = true;
	    for (int j=1; j<optlen && optargs_ok; ++j) {
	 	if (it.hasNext()) {
	 		option[j] = (String)it.next();
			if (option[j].startsWith("-")) {
				optargs_ok = false;
			}
		}
		else {
			optargs_ok = false;
		}
	    }
	    if (optargs_ok)
	    	options.add(option);
	    else {
	 	//         If the option requires more arguments than given on the
	 	//         command line, issue a fatal error

	    	reporter.printFatal("Missing value for option "+arg+".");
	    }
	 }
      }

      //--- Create an array of String arrays from the dynamic array built above

      String[][] optionArr=(String[][])options.toArray(new String[options.size()][0]);

      //--- Validate all options and issue warnings/errors
      
      if (validOptions(optionArr, rootDoc)) {

	 //--- We got valid options; parse them and store the parsed values
	 //         in 'option_*' fields.

	 readOptions(optionArr);

	 // If we have an empty source path list, add the current directory ('.')

	 if (option_sourcepath.size()==0) option_sourcepath.add(new File("."));

	 //--- We have all information we need to start the doclet at this time

	 rootDoc.setSourcePath(option_sourcepath);

	 startDoclet(customOptions);
      }
   }

   /**
    *  Helper class for parsing command line arguments.
    *  An instance of this class represents a particular
    *  option accepted by gjdoc (e.g. '-sourcepath')
    *  along with the number of expected arguments and
    *  behavior to parse the arguments.
    */
   private abstract class OptionProcessor {

      /**
       *  Number of arguments expected by this option.
       */
      private int argCount;

      /**
       *  Initializes this instance.
       */
      public OptionProcessor(int argCount) { 
	 this.argCount=argCount;
      }

      /**
       *  Overridden by derived classes with behavior
       *  to parse the arguments specified with this
       *  option.
       */
      abstract void process(String[] args);
   }
   
   /**
    *  Maps option tags (e.g. '-sourcepath') to
    *  OptionProcessor objects. Initialized only once
    *  by method initOptions().
    *  FIXME: Rename to 'optionProcessors'.
    */
   private static Map options = null;

   /**
    *  Initialize all OptionProcessor objects needed
    *  to scan/parse command line options.
    *
    *  This cannot be done in a static initializer block
    *  because OptionProcessors need access to the
    *  Singleton instance of the Main class.
    */
   private void initOptions() {

      options = new HashMap();
	 
      //--- Put one OptionProcessor object into the map
      //         for each option recognized.

      options.put("-overview", new OptionProcessor(2) { 
	    void process(String[] args) { option_overview = args[0]; }
	 });
      options.put("-public", new OptionProcessor(1) { 
	    void process(String[] args) { option_coverage = COVERAGE_PUBLIC; }
	 });
      options.put("-protected", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_coverage = COVERAGE_PROTECTED; 
	    }
	 });
      options.put("-package", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_coverage = COVERAGE_PACKAGE; 
	    }
	 });
      options.put("-private", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_coverage = COVERAGE_PRIVATE; 
	    }
	 });
      options.put("-help", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_help = true; 
	    }
	 });
      options.put("-doclet", new OptionProcessor(2) { 
	    void process(String[] args) { 
	       option_doclet = args[0]; 
	    }
	 });
      options.put("-nowarn", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_nowarn = true; 
	    }
	 });
      options.put("-sourcepath", new OptionProcessor(2) { 
	    void process(String[] args) { 
	       Debug.log(1, "-sourcepath is '"+args[0]+"'");
	       for (StringTokenizer st=new StringTokenizer(args[0], File.pathSeparator); st.hasMoreTokens(); ) {
		  String path = st.nextToken();
		  File file = new File(path);
		  if (!(file.exists())) {
		     throw new RuntimeException("The source path " + path + " does not exist.");
		  }
		  option_sourcepath.add(file);
	       }
	    }
	 });
      options.put("-verbose", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_verbose = true; 
	    }
	 });
      options.put("-locale", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_locale = args[0]; 
	    }
	 });
      options.put("-encoding", new OptionProcessor(1) { 
	    void process(String[] args) { 
	       option_encoding = args[0]; 
	    }
	 });
   }

   /**
    *  Determine how many arguments the given option
    *  requires.
    *
    *  @param option  The name of the option without leading dash.
    */
   private static int optionLength(String option) {

      OptionProcessor op=(OptionProcessor)options.get(option);
      if (op!=null)
	 return op.argCount;
      else
	 return 0;
   }

   /**
    *  Process all given options.
    *
    *  Assumes that the options have been validated before.
    *
    *  @param optionArr  Each element is a series of Strings where [0] is
    *                    the name of the option and [1..n] are the arguments
    *                    to the option.
    */
   private void readOptions(String[][] optionArr) {

      //--- For each option, find the appropriate OptionProcessor
      //        and call its process() method

      for (int i=0; i<optionArr.length; ++i) {
	 String[] opt = optionArr[i];
	 String[] args = new String[opt.length-1];
	 System.arraycopy(opt,1,args,0,opt.length-1);
	 OptionProcessor op=(OptionProcessor)options.get(opt[0]);
	 op.process(args);
      }
   }

   /**
    *  Print command line usage.
    *
    */
   private static void usage() {
      System.err.print("\n"+
		       "USAGE: gjdoc [options] [packagenames] "+/*"[sourcefiles] "+*/"[classnames] [@files]\n\n"+
		       "  -doclet <class>         Doclet class to use for generating output\n"+
		       "  -sourcepath <pathlist>  Where to look for source files\n"+
		       "  -d                      Set target directory\n"+
		       /*
		       "  -overview <file>        Read overview documentation from HTML file\n"+
		       */
		       "  -public                 Include only public classes and members\n"+
		       "  -protected              Include protected and public classes and members.\n"+
		       "                          This is the default.\n" + 
		       "  -package                Include package/protected/public classes and members\n"+
		       "  -private                Include all classes and members\n"+
		       "  -verbose                Output messages about what Gjdoc is doing\n"+
		       /*
		       "  -locale <name>            Locale to be used, e.g. en_US or en_US_WIN\n"+
		       "  -encoding <name>          Source file encoding name\n"+
		       */
		       "  -help                   Show this information\n" +
		       "\n" +
		       "Standard doclet options:\n" +
		       "  -genhtml                Generate HTML code instead of XML code. This is the\n" +
		       "                          default.\n" +
		       "  -geninfo                Generate Info code instead of XML code.\n" +
		       "  -xslsheet <file>        If specified, XML files will be written to a\n" +
		       "                          temporary directory and transformed using the\n" +
		       "                          given XSL sheet. The result of the transformation\n" +
		       "                          is written to the output directory. Not required if\n" +
		       "                          -genhtml or -geninfo has been specified.\n" +
		       "  -xmlonly                Generate XML code only, do not generate HTML code.\n" +
		       "  -title                  Title for this set of API documentation.\n" +
		       "  -bottomnote             HTML code to include at the bottom of each page.\n" +
		       "  -nofixhtml              If not specified, heurestics will be applied to\n" +
		       "                          fix broken HTML code in comments.\n" +
		       "  -nohtmlwarn             Do not emit warnings when encountering broken HTML\n" +
		       "                          code.\n" +
		       "  -noemailwarn            Do not emit warnings when encountering strings like\n" +
		       "                          <abc@foo.com>.\n" +
		       "  -indentstep <n>         How many spaces to indent each tag level in\n" +
		       "                          generated XML code.\n" +
		       "  -xsltdriver <class>     Specifies the XSLT driver to use for transformation.\n" +
		       "                          By default, xsltproc is used.\n" +
		       "  -postprocess <class>    XmlDoclet postprocessor class to apply after XSL\n" +
		       "                          transformation.\n" +
		       "  -compress               Generated info pages will be Zip-compressed.\n" +
		       "  -workpath               Specify a temporary directory to use.\n" +
		       "  -taglet                 Adds a Taglet class to the map of taglets.\n" +
		       "  -tagletpath             Sets the CLASSPATH to load subsequent Taglets from.\n" +
		       "\n"
	 );
   }

   /**
    *  Shutdown the generator.
    */
   public void shutdown() {
      System.exit(5);
   }

   public static RootDocImpl getRootDoc() { return rootDoc; }

   public static Main getInstance() { return instance; }

   public boolean includeAccessLevel(int accessLevel) {
      return coverageTemplates[option_coverage][accessLevel];
   }


   public boolean isDocletRunning() { return docletRunning; }

   public static boolean checkCharSet(String toCheck, String charSet) {
      for (int i=0; i<toCheck.length(); ++i) {
	 if (charSet.indexOf(toCheck.charAt(i))<0)
	    return false;
      }
      return true;
   }

   public static void releaseRootDoc() {
      rootDoc = null;
   }
}
