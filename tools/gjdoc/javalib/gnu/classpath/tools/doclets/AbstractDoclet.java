/* gnu.classpath.tools.doclets.AbstractDoclet
   Copyright (C) 2004 Free Software Foundation, Inc.

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

package gnu.classpath.tools.doclets;

import com.sun.javadoc.ClassDoc;
import com.sun.javadoc.ConstructorDoc;
import com.sun.javadoc.Doc;
import com.sun.javadoc.Doclet;
import com.sun.javadoc.FieldDoc;
import com.sun.javadoc.MethodDoc;
import com.sun.javadoc.PackageDoc;
import com.sun.javadoc.Parameter;
import com.sun.javadoc.RootDoc;
import com.sun.javadoc.Tag;

import com.sun.tools.doclets.Taglet;

import gnu.classpath.tools.taglets.AuthorTaglet;
import gnu.classpath.tools.taglets.DeprecatedTaglet;
import gnu.classpath.tools.taglets.GenericTaglet;
import gnu.classpath.tools.taglets.SinceTaglet;
import gnu.classpath.tools.taglets.VersionTaglet;
import gnu.classpath.tools.taglets.TagletContext;

import gnu.classpath.tools.IOToolkit;
import gnu.classpath.tools.FileSystemClassLoader;

import java.io.File;
import java.io.IOException;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.InvocationTargetException;

import java.text.MessageFormat;

import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Set;
import java.util.SortedSet;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.TreeSet;

/**
 *  An abstract Doclet implementation with helpers for common tasks
 *  performed by Doclets.
 */
public abstract class AbstractDoclet
{
   /**
    *  Mapping from tag type to Taglet for user Taglets specified on
    *  the command line.
    */
   private Map tagletMap = new LinkedHashMap();

   /**
    *  Stores the package groups specified in the user
    *  options. Contains objects of type PackageGroup.
    */
   private List packageGroups = new LinkedList();

   /**
    *  The current classpath for loading taglet classes.
    */
   private String tagletPath;

   /**
    *  Keeps track of the tags mentioned by the user during option
    *  processiong so that an error can be emitted if a tag is
    *  mentioned more than once.
    */
   private List mentionedTags = new LinkedList();

   public static int optionLength(String option) {
      return instance.getOptionLength(option);
   }

   public static boolean validOptions(String[][] options) {
      return true;
   }

   private static AbstractDoclet instance;

   protected static void setInstance(AbstractDoclet instance)
   {
      AbstractDoclet.instance = instance;
   }

   protected abstract void run()
      throws IOException;

   public static boolean start(RootDoc rootDoc) 
   {
      try {

         instance.startInstance(rootDoc);
         return true;
      }
      catch (DocletConfigurationException e) {
         System.err.println(e.getMessage());
         return false;
      }
      catch (Exception e) {
         e.printStackTrace();
         return false;
      }
   }

   protected RootDoc getRootDoc()
   {
      return this.rootDoc;
   }

   private RootDoc rootDoc;

   private void startInstance(RootDoc rootDoc)
      throws DocletConfigurationException, IOException
   {
      this.rootDoc = rootDoc;

      // Set the default Taglet order

      registerTaglet(new VersionTaglet());
      registerTaglet(new AuthorTaglet());
      registerTaglet(new SinceTaglet());
      registerTaglet(new StandardTaglet("deprecated"));
      registerTaglet(new StandardTaglet("see"));
      registerTaglet(new StandardTaglet("param"));

      // Process command line options

      for (int i=0, ilim=rootDoc.options().length; i<ilim; ++i) {
            
         String[] optionArr = rootDoc.options()[i];
         String optionTag = optionArr[0];

         DocletOption option = (DocletOption)nameToOptionMap.get(optionTag);

         if (null != option) {
            option.set(optionArr);
         }
      }

      // Enable/disable standard taglets based on user input

      AuthorTaglet.setTagletEnabled(optionAuthor.getValue());
      VersionTaglet.setTagletEnabled(optionVersion.getValue());
      SinceTaglet.setTagletEnabled(!optionNoSince.getValue());
      DeprecatedTaglet.setTagletEnabled(!optionNoDeprecated.getValue());

      if (!getTargetDirectory().exists()) {
         if (!getTargetDirectory().mkdirs()) {
            throw new DocletConfigurationException("Cannot create target directory " 
                                                   + getTargetDirectory());
         }
      }

      run();
   }

   public File getTargetDirectory()
   {
      return optionTargetDirectory.getValue();
   }

   private DocletOptionFile optionTargetDirectory = 
     new DocletOptionFile("-d", 
                          new File(System.getProperty("user.dir")));

   private DocletOptionFlag optionNoEmailWarn = 
     new DocletOptionFlag("-noemailwarn");

   private DocletOptionFlag optionAuthor = 
     new DocletOptionFlag("-author");

   private DocletOptionFlag optionVersion = 
     new DocletOptionFlag("-version");

   private DocletOptionFlag optionNoSince = 
     new DocletOptionFlag("-nosince");

   private DocletOptionFlag optionNoDeprecated = 
     new DocletOptionFlag("-nodeprecated");

   private DocletOptionGroup optionGroup = 
     new DocletOptionGroup("-group");

   private DocletOptionFlag optionDocFilesSubDirs = 
     new DocletOptionFlag("-docfilessubdirs");

   private DocletOptionColonSeparated optionExcludeDocFilesSubDir = 
     new DocletOptionColonSeparated("-excludedocfilessubdir");

   private DocletOptionTagletPath optionTagletPath = 
     new DocletOptionTagletPath("-tagletpath");

   private DocletOptionTag optionTaglet = 
     new DocletOptionTag("-taglet");

   private DocletOptionTag optionTag = 
     new DocletOptionTag("-tag");

   private class DocletOptionTaglet
      extends DocletOption
   {
      DocletOptionTaglet(String optionName)
      {
         super(optionName);
      }
      
      public int getLength()
      {
         return 2;
      }

      public boolean set(String[] optionArr)
      {

         boolean tagletLoaded = false;

         String useTagletPath = AbstractDoclet.this.tagletPath;
         if (null == useTagletPath) {
            useTagletPath = System.getProperty("java.class.path");
         }

         try {
            Class tagletClass;
            try {
               tagletClass
                  = new FileSystemClassLoader(useTagletPath).loadClass(optionArr[1]);
            }
            catch (ClassNotFoundException e) {
               // If not found on specified tagletpath, try default classloader
               tagletClass
                  = Class.forName(optionArr[1]);
            }
            Method registerTagletMethod
               = tagletClass.getDeclaredMethod("register", new Class[] { java.util.Map.class });

            if (!registerTagletMethod.getReturnType().equals(Void.TYPE)) {
               printError("Taglet class '" + optionArr[1] + "' found, but register method doesn't return void.");
            }
            else if (registerTagletMethod.getExceptionTypes().length > 0) {
               printError("Taglet class '" + optionArr[1] + "' found, but register method contains throws clause.");
            }
            else if ((registerTagletMethod.getModifiers() & (Modifier.STATIC | Modifier.PUBLIC | Modifier.ABSTRACT)) != (Modifier.STATIC | Modifier.PUBLIC)) {
               printError("Taglet class '" + optionArr[1] + "' found, but register method isn't public static, or is abstract..");
            }
            else {
               Map tempMap = new HashMap();
               registerTagletMethod.invoke(null, new Object[] { tempMap });
               tagletLoaded = true;
               String name = (String)tempMap.keySet().iterator().next();
               Taglet taglet = (Taglet)tempMap.get(name);
               tagletMap.put(name, taglet);
               mentionedTags.add(taglet);
            }
         }
         catch (NoSuchMethodException e) {
            printError("Taglet class '" + optionArr[1] + "' found, but doesn't contain the register method.");
         }
         catch (SecurityException e) {
            printError("Taglet class '" + optionArr[1] + "' cannot be loaded: " + e.getMessage());
         }
         catch (InvocationTargetException e) {
            printError("Taglet class '" + optionArr[1] + "' found, but register method throws exception: " + e.toString());
         }
         catch (IllegalAccessException e) {
            printError("Taglet class '" + optionArr[1] + "' found, but there was a problem when accessing the register method: " + e.toString());
         }
         catch (IllegalArgumentException e) {
            printError("Taglet class '" + optionArr[1] + "' found, but there was a problem when accessing the register method: " + e.toString());
         }
         catch (ClassNotFoundException e) {
            printError("Taglet class '" + optionArr[1] + "' cannot be found.");
         }
         return tagletLoaded;
      }
   }

   private class DocletOptionGroup 
      extends DocletOption
   {
      DocletOptionGroup(String optionName)
      {
         super(optionName);
      }
      
      public int getLength()
      {
         return 3;
      }

      public boolean set(String[] optionArr)
      {
         try {
            PackageMatcher packageMatcher = new PackageMatcher();

            StringTokenizer tokenizer = new StringTokenizer(optionArr[2], ":");
            while (tokenizer.hasMoreTokens()) {
               String packageWildcard = tokenizer.nextToken();
               packageMatcher.addWildcard(packageWildcard);
            }
            
            SortedSet groupPackages = packageMatcher.match(rootDoc.specifiedPackages());

            packageGroups.add(new PackageGroup(optionArr[1], groupPackages));

            return true;
         }
         catch (InvalidPackageWildcardException e) {
            return false;
         }
      }
   }

   private class DocletOptionTagletPath
      extends DocletOption
   {
      DocletOptionTagletPath(String optionName)
      {
         super(optionName);
      }
      
      public int getLength()
      {
         return 2;
      }

      public boolean set(String[] optionArr)
      {
         AbstractDoclet.this.tagletPath = optionArr[1];
         return true;
      }
   }

   private class DocletOptionTag
      extends DocletOption
   {
      DocletOptionTag(String optionName)
      {
         super(optionName);
      }
      
      public int getLength()
      {
         return 2;
      }

      public boolean set(String[] optionArr)
      {
         String tagSpec = optionArr[1];
         boolean validTagSpec = false;
         int ndx1 = tagSpec.indexOf(':');
         if (ndx1 < 0) {
            Taglet taglet = (Taglet)tagletMap.get(tagSpec);
            if (null == taglet) {
               printError("There is no standard tag '" + tagSpec + "'.");
            }
            else {
               if (mentionedTags.contains(taglet)) {
                  printError("Tag '" + tagSpec + "' has been added or moved before.");
               }
               else {
                  mentionedTags.add(taglet);
                           
                  // re-append taglet
                  tagletMap.remove(tagSpec);
                  tagletMap.put(tagSpec, taglet);
               }
            }
         }
         else {
            int ndx2 = tagSpec.indexOf(':', ndx1 + 1);
            if (ndx2 > ndx1 && ndx2 < tagSpec.length() - 1) {
               String tagName = tagSpec.substring(0, ndx1);
               String tagHead = null;
               if (tagSpec.charAt(ndx2 + 1) == '\"') {
                  if (tagSpec.charAt(tagSpec.length() - 1) == '\"') {
                     tagHead = tagSpec.substring(ndx2 + 2, tagSpec.length() - 1);
                     validTagSpec = true;
                  }
               }
               else {
                  tagHead = tagSpec.substring(ndx2 + 1);
                  validTagSpec = true;
               }

               boolean tagScopeOverview = false;
               boolean tagScopePackages = false;
               boolean tagScopeTypes = false;
               boolean tagScopeConstructors = false;
               boolean tagScopeMethods = false;
               boolean tagScopeFields = false;
               boolean tagDisabled = false;
                        
            tag_option_loop:
               for (int n=ndx1+1; n<ndx2; ++n) {
                  switch (tagSpec.charAt(n)) {
                  case 'X': 
                     tagDisabled = true;
                     break;
                  case 'a':
                     tagScopeOverview = true;
                     tagScopePackages = true;
                     tagScopeTypes = true;
                     tagScopeConstructors = true;
                     tagScopeMethods = true;
                     tagScopeFields = true;
                     break;
                  case 'o':
                     tagScopeOverview = true;
                     break;
                  case 'p':
                     tagScopePackages = true;
                     break;
                  case 't':
                     tagScopeTypes = true;
                     break;
                  case 'c':
                     tagScopeConstructors = true;
                     break;
                  case 'm':
                     tagScopeMethods = true;
                     break;
                  case 'f':
                     tagScopeFields = true;
                     break;
                  default:
                     validTagSpec = false;
                     break tag_option_loop;
                  }
               }
                        
               if (validTagSpec) {
                  GenericTaglet taglet
                     = new GenericTaglet(tagName,
                                         tagHead,
                                         tagScopeOverview,
                                         tagScopePackages,
                                         tagScopeTypes,
                                         tagScopeConstructors,
                                         tagScopeMethods,
                                         tagScopeFields);
                  taglet.setTagletEnabled(!tagDisabled);
                  taglet.register(tagletMap);
                  mentionedTags.add(taglet);
               }
            }
         }
         if (!validTagSpec) {
            printError("Value for option -tag must be in format \"<tagname>:Xaoptcmf:<taghead>\".");
         }
         return validTagSpec;
      }
   }

   private DocletOption[] commonOptions = 
      {
         optionTargetDirectory,
         optionAuthor,
         optionVersion,
         optionNoSince,
         optionNoDeprecated,
         optionGroup,
         optionDocFilesSubDirs,
         optionExcludeDocFilesSubDir,
         optionTagletPath,
         optionTaglet,
         optionTag,
      };

   private void registerOptions()
   {
      if (!optionsRegistered) {
         for (int i=0; i<commonOptions.length; ++i) {
            DocletOption option = commonOptions[i];
            registerOption(option);
         }
         DocletOption[] docletOptions = getOptions();
         for (int i=0; i<docletOptions.length; ++i) {
            DocletOption option = docletOptions[i];
            registerOption(option);
         }
         optionsRegistered = true;
      }
   }

   protected abstract DocletOption[] getOptions();

   private boolean optionsRegistered = false;

   private void registerOption(DocletOption option) 
   {
      nameToOptionMap.put(option.getName(), option);
   }

   private Map nameToOptionMap = new HashMap();

   private int getOptionLength(String optionName)
   {
      registerOptions();
      DocletOption option = (DocletOption)nameToOptionMap.get(optionName);
      if (null != option) {
         return option.getLength();
      }
      else {
         return -1;
      }
   }

   protected List getKnownDirectSubclasses(ClassDoc classDoc)
   {
      List result = new LinkedList();
      if (!"java.lang.Object".equals(classDoc.qualifiedName())) {
         ClassDoc[] classes = rootDoc.classes();
         for (int i=0; i<classes.length; ++i) {
            if (classDoc == classes[i].superclass()) {
               result.add(classes[i]);
            }
         }
      }
      return result;
   }

   protected static class IndexKey
      implements Comparable
   {
      private String name;

      public IndexKey(String name)
      {
         this.name = name;
      }

      public boolean equals(Object other)
      {
         return false;
      }

      public int compareTo(Object other)
      {
         return name.toLowerCase().compareTo(((IndexKey)other).name.toLowerCase());
      }

      public String getName()
      {
         return name;
      }
   }
   
   private Map categorizedIndex;

   protected Map getCategorizedIndex()
   {
      if (null == categorizedIndex) {
         categorizedIndex = new LinkedHashMap();
         
         Map indexMap = getIndexByName();
         Iterator it = indexMap.keySet().iterator();
         char previousCategoryLetter = '\0';
         Character keyLetter = null;
         while (it.hasNext()) {
            IndexKey key = (IndexKey)it.next();
            char firstChar = Character.toUpperCase(key.getName().charAt(0));
            if (firstChar != previousCategoryLetter) {
               keyLetter = new Character(firstChar);
               previousCategoryLetter = firstChar;
               categorizedIndex.put(keyLetter, new LinkedList());
            }
            List letterList = (List)categorizedIndex.get(keyLetter);
            letterList.add(indexMap.get(key));
         }
      }

      return categorizedIndex;
   }


   private Map indexByName;

   protected Map getIndexByName()
   {
      if (null == indexByName) {
         // Create index

         // Collect index
            
         indexByName = new TreeMap();

         // Add packages to index

         PackageDoc[] packages = rootDoc.specifiedPackages();
         for (int i=0, ilim=packages.length; i<ilim; ++i) {
            PackageDoc c = packages[i];
            indexByName.put(new IndexKey(c.name()), c);
         }

         // Add classes, fields and methods to index

         ClassDoc[] sumclasses = rootDoc.classes();
         for (int i=0, ilim=sumclasses.length; i<ilim; ++i) {
            ClassDoc c = sumclasses[i];
            if (null == c.containingClass()) {
               indexByName.put(new IndexKey(c.name() + " " + c.containingPackage().name()), c);
            }
            else {
               indexByName.put(new IndexKey(c.name().substring(c.containingClass().name().length() + 1)
                                            + " " + c.containingClass().name() + " " + c.containingPackage().name()), c);
            }
            FieldDoc[] fields = c.fields();
            for (int j=0, jlim=fields.length; j<jlim; ++j) {
               indexByName.put(new IndexKey(fields[j].name() + " " + fields[j].containingClass().name() + " " + fields[j].containingPackage().name()), fields[j]);
            }
            MethodDoc[] methods = c.methods();
            for (int j=0, jlim=methods.length; j<jlim; ++j) {
               MethodDoc method = methods[j];
               indexByName.put(new IndexKey(method.name() + method.signature() + " " + method.containingClass().name() + " " + method.containingPackage().name()), method);
            }
            ConstructorDoc[] constructors = c.constructors();
            for (int j=0, jlim=constructors.length; j<jlim; ++j) {
               ConstructorDoc constructor = constructors[j];
               indexByName.put(new IndexKey(constructor.name() + constructor.signature() + " " + constructor.containingClass().name() + " " + constructor.containingPackage().name()), constructor);
            }
         }
      }
      return indexByName;
   }

   private void registerTaglet(Taglet taglet)
   {
      tagletMap.put(taglet.getName(), taglet);
   }

   protected void printTaglets(Tag[] tags, TagletContext context, TagletPrinter output) 
   {
      for (Iterator it = tagletMap.keySet().iterator(); it.hasNext(); ) {
         String tagName = (String)it.next();
         Object o = tagletMap.get(tagName);
         Taglet taglet = (Taglet)o;

         if (!taglet.isInlineTag()
             && ((context != TagletContext.CONSTRUCTOR || taglet.inConstructor())
                 || (context != TagletContext.FIELD || taglet.inField())
                 || (context != TagletContext.METHOD || taglet.inMethod())
                 || (context != TagletContext.OVERVIEW || taglet.inOverview())
                 || (context != TagletContext.PACKAGE || taglet.inPackage())
                 || (context != TagletContext.TYPE || taglet.inType()))) {
            
            List tagsOfThisType = new LinkedList();
            for (int i=0; i<tags.length; ++i) {
               if (tags[i].name().substring(1).equals(tagName)) {
                  tagsOfThisType.add(tags[i]);
               }
            }

            if (!tagsOfThisType.isEmpty()) {
               Tag[] tagletTags = (Tag[])tagsOfThisType.toArray(new Tag[tagsOfThisType.size()]);

               String tagletString = taglet.toString(tagletTags);
               if (null != tagletString) {
                  output.printTagletString(tagletString);
               }
            }
         }
      }
   }

   /**
    *  @param usedClassToPackagesMap  ClassDoc to (PackageDoc to (UsageType to (Set of Doc)))
    */
   private void addUsedBy(Map usedClassToPackagesMap,
                          ClassDoc usedClass, UsageType usageType, Doc user, PackageDoc userPackage)
   {
      Map packageToUsageTypeMap = (Map)usedClassToPackagesMap.get(usedClass);
      if (null == packageToUsageTypeMap) {
         packageToUsageTypeMap = new HashMap();
         usedClassToPackagesMap.put(usedClass, packageToUsageTypeMap);
      }

      Map usageTypeToUsersMap = (Map)packageToUsageTypeMap.get(userPackage);
      if (null == usageTypeToUsersMap) {
         usageTypeToUsersMap = new HashMap();
         packageToUsageTypeMap.put(userPackage, usageTypeToUsersMap);
      }

      Set userSet = (Set)usageTypeToUsersMap.get(usageType);
      if (null == userSet) {
         userSet = new TreeSet(); // FIXME: we need the collator from Main here
         usageTypeToUsersMap.put(usageType, userSet);
      }
      userSet.add(user);
   }

   /**
    *  Create the cross reference database.
    */
   private Map collectUsage() {

      Map usedClassToPackagesMap = new HashMap();

      ClassDoc[] classes = rootDoc.classes();
      for (int i = 0, ilim = classes.length; i < ilim; ++ i) {
         ClassDoc clazz = classes[i];

         // classes derived from
         for (ClassDoc superclass = clazz.superclass(); superclass != null; 
              superclass = superclass.superclass()) {
            addUsedBy(usedClassToPackagesMap,
                      superclass, UsageType.CLASS_DERIVED_FROM, clazz, clazz.containingPackage());
         }

         FieldDoc[] fields = clazz.fields();
         for (int j = 0, jlim = fields.length; j < jlim; ++ j) {
            FieldDoc field = fields[j];

            // fields of type                  
            ClassDoc fieldType = field.type().asClassDoc();
            if (null != fieldType) {
               addUsedBy(usedClassToPackagesMap,
                         fieldType, UsageType.FIELD_OF_TYPE, 
                         field, clazz.containingPackage());
            }
         }

         MethodDoc[] methods = clazz.methods();
         for (int j = 0, jlim = methods.length; j < jlim; ++ j) {
            MethodDoc method = methods[j];

            // methods with return type

            ClassDoc returnType = method.returnType().asClassDoc();
            if (null != returnType) {
               addUsedBy(usedClassToPackagesMap,
                         returnType, UsageType.METHOD_WITH_RETURN_TYPE, 
                         method, clazz.containingPackage());
            }
            Parameter[] parameters = method.parameters();
            for (int k=0; k<parameters.length; ++k) {

               // methods with parameter type

               Parameter parameter = parameters[k];
               ClassDoc parameterType = parameter.type().asClassDoc();
               if (null != parameterType) {
                  addUsedBy(usedClassToPackagesMap,
                            parameterType, UsageType.METHOD_WITH_PARAMETER_TYPE, 
                            method, clazz.containingPackage());
               }
            }

            // methods which throw

            ClassDoc[] thrownExceptions = method.thrownExceptions();
            for (int k = 0, klim = thrownExceptions.length; k < klim; ++ k) {
               ClassDoc thrownException = thrownExceptions[k];
               addUsedBy(usedClassToPackagesMap,
                         thrownException, UsageType.METHOD_WITH_THROWN_TYPE, 
                         method, clazz.containingPackage());
            }
         }
                  
         ConstructorDoc[] constructors = clazz.constructors();
         for (int j = 0, jlim = constructors.length; j < jlim; ++ j) {

            ConstructorDoc constructor = constructors[j];

            Parameter[] parameters = constructor.parameters();
            for (int k = 0, klim = parameters.length; k < klim; ++ k) {

               // constructors with parameter type
                     
               Parameter parameter = parameters[k];
               ClassDoc parameterType = parameter.type().asClassDoc();
               if (null != parameterType) {
                  addUsedBy(usedClassToPackagesMap,
                            parameterType, UsageType.CONSTRUCTOR_WITH_PARAMETER_TYPE, 
                            constructor, clazz.containingPackage());
               }
            }

            // constructors which throw

            ClassDoc[] thrownExceptions = constructor.thrownExceptions();
            for (int k = 0, klim = thrownExceptions.length; k < klim; ++ k) {
               ClassDoc thrownException = thrownExceptions[k];
               addUsedBy(usedClassToPackagesMap,
                         thrownException, UsageType.CONSTRUCTOR_WITH_THROWN_TYPE, 
                         constructor, clazz.containingPackage());
            }
         }
      }
      return usedClassToPackagesMap;
   }

   Map usedClassToPackagesMap = null;

   protected Map getUsageOfClass(ClassDoc classDoc)
   {
      if (null == this.usedClassToPackagesMap) {
         this.usedClassToPackagesMap = collectUsage();
      }
      return (Map)usedClassToPackagesMap.get(classDoc);
   }

   protected static class UsageType
   {
      public static final UsageType CLASS_DERIVED_FROM = new UsageType("class-derived-from");
      public static final UsageType FIELD_OF_TYPE = new UsageType("field-of-type");
      public static final UsageType METHOD_WITH_RETURN_TYPE = new UsageType("method-with-return-type");
      public static final UsageType METHOD_WITH_PARAMETER_TYPE = new UsageType("method-with-parameter-type");
      public static final UsageType METHOD_WITH_THROWN_TYPE = new UsageType("method-with-thrown-type");
      public static final UsageType CONSTRUCTOR_WITH_PARAMETER_TYPE = new UsageType("constructor-with-parameter-type");
      public static final UsageType CONSTRUCTOR_WITH_THROWN_TYPE = new UsageType("constructor-with-thrown-type");
      private String id;

      private UsageType(String id)
      {
         this.id = id;
      }

      public String toString() { 
         return "UsageType{id=" + id + "}"; 
      }

      public String getId() {
         return id;
      }
   }

   private ResourceBundle resources;

   protected String getString(String key)
   {
      if (null == resources) {
         Locale currentLocale = Locale.getDefault();

         resources
            = ResourceBundle.getBundle("htmldoclet.HtmlDoclet", currentLocale);
      }

      return resources.getString(key);
   }

   protected String format(String key, String value1)
   {
      return MessageFormat.format(getString(key), new Object[] { value1 });
   }

   protected List getPackageGroups()
   {
      return packageGroups;
   }

   protected void copyDocFiles(File sourceDir, File targetDir)
      throws IOException
   {
      File sourceDocFiles = new File(sourceDir, "doc-files");
      File targetDocFiles = new File(targetDir, "doc-files");

      if (sourceDocFiles.exists()) {
         IOToolkit.copyDirectory(sourceDocFiles,
                                 targetDocFiles,
                                 optionDocFilesSubDirs.getValue(),
                                 optionExcludeDocFilesSubDir.getSubdirs());
      }
   }

   private Set sourcePaths;

   /**
    *  Try to determine the source directory for the given package by
    *  looking at the path specified by -sourcepath, or the current
    *  directory if -sourcepath hasn't been specified.
    *
    *  @throws IOException if the source directory couldn't be
    *  located.
    */
   protected File getPackageSourceDir(PackageDoc packageDoc)
      throws IOException
   {
      if (null == sourcePaths) {
         for (int i=0; i<rootDoc.options().length; ++i) {
            if ("-sourcepath".equals(rootDoc.options()[i][0])) {
               sourcePaths = new LinkedHashSet();
               String sourcepathString = rootDoc.options()[i][1];
               StringTokenizer st = new StringTokenizer(sourcepathString, File.pathSeparator);
               while (st.hasMoreTokens()) {
                  sourcePaths.add(new File(st.nextToken()));
               }
            }
         }
         if (null == sourcePaths) {
            sourcePaths = new LinkedHashSet();
            sourcePaths.add(new File(System.getProperty("user.dir")));
         }
      }

      String packageSubDir = packageDoc.name().replace('.', File.separatorChar);
      Iterator it = sourcePaths.iterator();
      while (it.hasNext()) {
         File pathComponent = (File)it.next();
         File packageDir = new File(pathComponent, packageSubDir);
         if (packageDir.exists()) {
            return packageDir;
         }
      }
      throw new IOException("Couldn't locate source directory for package " + packageDoc.name());
   }

   protected void printError(String error) 
   {
      if (null != rootDoc) {
	 rootDoc.printError(error);
      }
      else {
	 System.err.println("ERROR: "+error);
      }
   }

   protected void printWarning(String warning) 
   {
      if (null != rootDoc) {
	 rootDoc.printWarning(warning);
      }
      else {
	 System.err.println("WARNING: "+warning);
      }
   }

   protected void printNotice(String notice) 
   {
      if (null != rootDoc) {
	 rootDoc.printNotice(notice);
      }
      else {
	 System.err.println(notice);
      }
   }

   protected static ClassDoc getOuterClassDoc(ClassDoc classDoc)
   {
      while (null != classDoc.containingClass()) {
         classDoc = classDoc.containingClass();
      }
      return classDoc;
   }
}


/** missing:

            + " -title <text>            Title for this set of API documentation (deprecated, -doctitle should be used instead).\n"
            + " -link <extdoc URL>       Link to external javadoc-generated documentation you want to link to\n"
            + " -linkoffline <extdoc URL> <packagelistLoc>  Link to external javadoc-generated documentation for the specified package-list\n"
            + " -noqualifier all|<packagename1:packagename2:...> Do not qualify package name from ahead of class names\n"
            + " -nocomment               Suppress the entire comment body including the main description and all tags, only generate the declarations\n"

*/
