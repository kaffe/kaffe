/* gnu.classpath.tools.doclets.htmldoclet.HtmlDoclet
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

package gnu.classpath.tools.doclets.htmldoclet;

import gnu.classpath.tools.IOToolkit;

import gnu.classpath.tools.doclets.AbstractDoclet;
import gnu.classpath.tools.doclets.DocletConfigurationException;
import gnu.classpath.tools.doclets.DocletOption;
import gnu.classpath.tools.doclets.DocletOptionFile;
import gnu.classpath.tools.doclets.DocletOptionFlag;
import gnu.classpath.tools.doclets.DocletOptionString;
import gnu.classpath.tools.doclets.PackageGroup;
import gnu.classpath.tools.doclets.TagletPrinter;

import gnu.classpath.tools.taglets.TagletContext;

import gnu.classpath.tools.java2xhtml.Java2xhtml;

import gnu.classpath.tools.StringToolkit;

import com.sun.javadoc.*;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.StringWriter;

import java.net.MalformedURLException;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

public class HtmlDoclet
   extends AbstractDoclet
{
   private static String filenameExtension = ".html";

   /**
    *  Contains ExternalDocSet.
    */
   private List externalDocSets = new LinkedList();

   /**
    *  Contains String->ExternalDocSet.
    */
   private Map packageNameToDocSet = new HashMap();

   private void printNavBar(HtmlPage output, String currentPage, ClassDoc currentClass)
   {
         output.beginDiv(CssClass.NAVBAR_TOP);

         boolean overviewLevel = ("overview".equals(currentPage)
                                  || "full-tree".equals(currentPage)
                                  || "alpha-index".equals(currentPage));

         if ("overview".equals(currentPage)) {
            output.beginSpan(CssClass.NAVBAR_ITEM_ACTIVE);
            output.print("Overview");
            output.endSpan(CssClass.NAVBAR_ITEM_ACTIVE);
         }
         else {
            output.beginSpan(CssClass.NAVBAR_ITEM_ENABLED);
            output.beginAnchor(output.getPathToRoot() + "/index-noframes" + filenameExtension);
            output.print("Overview");
            output.endAnchor();
            output.endSpan(CssClass.NAVBAR_ITEM_ENABLED);
         }

         output.print(" ");

         if (!overviewLevel) {
            if ("package".equals(currentPage)) {
               output.beginSpan(CssClass.NAVBAR_ITEM_ACTIVE);
               output.print("Package");
               output.endSpan(CssClass.NAVBAR_ITEM_ACTIVE);
            }
            else {
               output.beginSpan(CssClass.NAVBAR_ITEM_ENABLED);
               output.beginAnchor("package-summary" + filenameExtension);
               output.print("Package");
               output.endAnchor();
               output.endSpan(CssClass.NAVBAR_ITEM_ENABLED);
            }
         }
         else {
            output.beginSpan(CssClass.NAVBAR_ITEM_DISABLED);
            output.print("Package");
            output.endSpan(CssClass.NAVBAR_ITEM_DISABLED);
         }

         if (optionUse.getValue() || optionLinkSource.getValue()) {
            output.print(" ");

            if (null != currentClass) {
               if ("class".equals(currentPage)) {
                  output.beginSpan(CssClass.NAVBAR_ITEM_ACTIVE);
                  output.print("Class");
                  output.endSpan(CssClass.NAVBAR_ITEM_ACTIVE);
               }
               else {
                  output.beginSpan(CssClass.NAVBAR_ITEM_ENABLED);
                  output.beginAnchor(currentClass.name() + filenameExtension);
                  output.print("Class");
                  output.endAnchor();
                  output.endSpan(CssClass.NAVBAR_ITEM_ENABLED);
               }
            }
            else {
               output.beginSpan(CssClass.NAVBAR_ITEM_DISABLED);
               output.print("Class");
               output.endSpan(CssClass.NAVBAR_ITEM_DISABLED);
            }

            if (optionUse.getValue()) {
               output.print(" ");

               if (null != currentClass) {
                  output.beginSpan(CssClass.NAVBAR_ITEM_ENABLED);
                  output.beginAnchor(currentClass.name() + "-uses" + filenameExtension);
                  output.print("Use");
                  output.endAnchor();
                  output.endSpan(CssClass.NAVBAR_ITEM_ENABLED);
               }
               else {
                  output.beginSpan(CssClass.NAVBAR_ITEM_DISABLED);
                  output.print("Use");
                  output.endSpan(CssClass.NAVBAR_ITEM_DISABLED);
               }
            }

            if (optionLinkSource.getValue()) {
               output.print(" ");

               if (null != currentClass) {
                  output.beginSpan(CssClass.NAVBAR_ITEM_ENABLED);
                  String targetClassName = currentClass.name();
                  String targetAnchor = "";
                  if (null != currentClass.containingClass()) {
                     targetClassName = getOuterClassDoc(currentClass).name();
                     targetAnchor = "#line." + currentClass.position().line();
                  }
                  output.beginAnchor(targetClassName + "-source" + filenameExtension + targetAnchor);
                  output.print("Source");
                  output.endAnchor();
                  output.endSpan(CssClass.NAVBAR_ITEM_ENABLED);
               }
               else {
                  output.beginSpan(CssClass.NAVBAR_ITEM_DISABLED);
                  output.print("Source");
                  output.endSpan(CssClass.NAVBAR_ITEM_DISABLED);
               }
            }
         }


         if (!optionNoTree.getValue()) {
            output.print(" ");

            if ("tree".equals(currentPage) || "package-tree".equals(currentPage)) {
               output.beginSpan(CssClass.NAVBAR_ITEM_ACTIVE);
               output.print("Tree");
               output.endSpan(CssClass.NAVBAR_ITEM_ACTIVE);
            }
            else {
               output.beginSpan(CssClass.NAVBAR_ITEM_ENABLED);
               output.beginAnchor("tree" + filenameExtension);
               output.print("Tree");
               output.endAnchor();
               output.endSpan(CssClass.NAVBAR_ITEM_ENABLED);
            }
         }

         output.print(" ");

         String indexName;
         if (optionSplitIndex.getValue()) {
            indexName = "alphaindex-1";
         }
         else {
            indexName = "alphaindex";
         }

         output.beginAnchor(output.getPathToRoot() + "/" + indexName + filenameExtension);
         output.print("Index");
         output.endAnchor();

         if (!optionNoDeprecatedList.getValue()) {
            output.print(" ");
            
            output.beginAnchor(output.getPathToRoot() + "/deprecated" + filenameExtension);
            output.print("Deprecated");
            output.endAnchor();
         }

         if (!optionNoHelp.getValue()) {
            output.print(" ");
            
            output.beginAnchor(output.getPathToRoot() + "/help" + filenameExtension);
            output.print("Help");
            output.endAnchor();
         }

         output.print(" ");

         output.beginAnchor(output.getPathToRoot() + "/about" + filenameExtension);
         output.print("About");
         output.endAnchor();

         output.endDiv(CssClass.NAVBAR_TOP);
   }

   private void printNavBarTop(HtmlPage output, String currentPage)
   {
      printNavBarTop(output, currentPage, null);
   }

   private void printNavBarTop(HtmlPage output, String currentPage, ClassDoc currentClass)
   {
      if (!optionNoNavBar.getValue()) {
         output.beginTable(CssClass.NAVBAR_TOP);
         output.beginRow();
         output.beginCell();
         printNavBar(output, currentPage, currentClass);
         output.endCell();
         if (null != optionHeader.getValue()) {
            output.beginCell();
            output.print(replaceDocRoot(output, optionHeader.getValue()));
            output.endCell();
         }
         output.endRow();
         output.endTable();
      }
   }

   private void printNavBarBottom(HtmlPage output, String currentPage)
   {
      printNavBarBottom(output, currentPage, null);
   }

   private void printNavBarBottom(HtmlPage output, String currentPage, ClassDoc currentClass)
   {
      if (!optionNoNavBar.getValue()) {
         output.beginDiv(CssClass.NAVBAR_BOTTOM_SPACER);
         output.print(" ");
         output.endDiv(CssClass.NAVBAR_BOTTOM_SPACER);
         output.beginTable(CssClass.NAVBAR_BOTTOM);
         output.beginRow();
         output.beginCell();
         printNavBar(output, currentPage, currentClass);
         output.endCell();
         if (null != optionFooter.getValue()) {
            output.beginCell();
            output.print(replaceDocRoot(output, optionFooter.getValue()));
            output.endCell();
         }
         output.endRow();
         output.endTable();
      }

      if (null != optionBottom.getValue()) {
         output.hr();
         output.print(replaceDocRoot(output, optionBottom.getValue()));
      }
   }

   private void printPackagePageClasses(HtmlPage output, ClassDoc[] classDocs, String header)
   {
      if (classDocs.length > 0) {
         output.beginTable(CssClass.PACKAGE_SUMMARY);
         output.rowDiv(CssClass.TABLE_HEADER, header);

         for (int i=0; i<classDocs.length; ++i) {
            ClassDoc classDoc = classDocs[i];
            if (classDoc.isIncluded()) {
               output.beginRow();
            
               output.beginCell(CssClass.PACKAGE_SUMMARY_LEFT);
               printType(output, classDoc);
               output.endCell();

               output.beginCell(CssClass.PACKAGE_SUMMARY_RIGHT);
               printTags(output, classDoc.firstSentenceTags(), true);
               output.endCell();
               output.endRow();
            }
         }
         output.endTable();
      }
   }

   private void printPackagesListFile()
      throws IOException
   {
      PrintWriter out
         = new PrintWriter(new OutputStreamWriter(new FileOutputStream(new File(getTargetDirectory(),
                                                                                "package-list")),
                                                  "UTF-8"));

      PackageDoc[] packages = getRootDoc().specifiedPackages();
      for (int i=0; i<packages.length; ++i) {
         String packageName = packages[i].name();
         if (packageName.length() > 0) {
            out.println(packageName);
         }
      }

      out.close();
   }

   private void printPackagePage(File packageDir, String pathToRoot, PackageDoc packageDoc)
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(packageDir,
                                              "package-summary" + filenameExtension),
                                     pathToRoot);
      output.beginPage(packageDoc.name());
      output.beginBody();
      printNavBarTop(output, "package");

      output.beginDiv(CssClass.PACKAGE_TITLE);
      output.print("Package ");
      if (packageDoc.name().length() > 0) {
         output.print(packageDoc.name());
      }
      else {
         output.print("&lt;Unnamed&gt;");
      }
      output.endDiv(CssClass.PACKAGE_TITLE);

      output.beginDiv(CssClass.PACKAGE_DESCRIPTION_TOP);
      printTags(output, packageDoc.firstSentenceTags(), true);
      output.endDiv(CssClass.PACKAGE_DESCRIPTION_TOP);
      
      printPackagePageClasses(output, packageDoc.interfaces(), 
                              "Interface Summary");
      printPackagePageClasses(output, packageDoc.ordinaryClasses(), 
                              "Class Summary");
      printPackagePageClasses(output, packageDoc.exceptions(), 
                              "Exception Summary");
      printPackagePageClasses(output, packageDoc.errors(), 
                              "Error Summary");

      output.anchorName("description");
      output.beginDiv(CssClass.PACKAGE_DESCRIPTION_FULL);
      printTags(output, packageDoc.inlineTags(), false);
      output.endDiv(CssClass.PACKAGE_DESCRIPTION_FULL);

      printNavBarBottom(output, "package");
      output.endBody();
      output.endPage();
      output.close();
   }

   static class TreeNode
      implements Comparable
   {
      ClassDoc classDoc;
      SortedSet children = new TreeSet();

      TreeNode(ClassDoc classDoc) {
         TreeNode.this.classDoc = classDoc;
      }
      
      public boolean equals(Object other)
      {
         return classDoc.equals(((TreeNode)other).classDoc);
      }

      public int compareTo(Object other)
      {
         return classDoc.compareTo(((TreeNode)other).classDoc);
      }

      public int hashCode()
      {
         return classDoc.hashCode();
      }
   }

   private TreeNode addClassTreeNode(Map treeMap, ClassDoc classDoc)
   {
      TreeNode node = (TreeNode)treeMap.get(classDoc.qualifiedName());
      if (null == node) {
         node = new TreeNode(classDoc);
         treeMap.put(classDoc.qualifiedName(), node);

         ClassDoc superClassDoc = (ClassDoc)classDoc.superclass();
         if (null != superClassDoc) {
            TreeNode parentNode = addClassTreeNode(treeMap, superClassDoc);
            parentNode.children.add(node);
         }
      }
      return node;
   }

   private TreeNode addInterfaceTreeNode(Map treeMap, ClassDoc classDoc)
   {
      TreeNode node = (TreeNode)treeMap.get(classDoc.qualifiedName());
      if (null == node) {
         node = new TreeNode(classDoc);
         treeMap.put(classDoc.qualifiedName(), node);

         ClassDoc[] superInterfaces = classDoc.interfaces();
         if (null != superInterfaces && superInterfaces.length > 0) {
            for (int i=0; i<superInterfaces.length; ++i) {
               TreeNode parentNode = addInterfaceTreeNode(treeMap, superInterfaces[i]);
               parentNode.children.add(node);
            }
         }
         else {
            TreeNode rootNode = (TreeNode)treeMap.get("<root>");
            if (null == rootNode) {
               rootNode = new TreeNode(null);
               treeMap.put("<root>", rootNode);
            }
            rootNode.children.add(node);
         }
      }
      return node;
   }

   private void printPackageTreeRec(HtmlPage output, TreeNode node)
   {
      output.beginElement("li");
      if (node.classDoc.isIncluded()) {
         output.print(node.classDoc.containingPackage().name());
         output.print(".");
         printType(output, node.classDoc);
      }
      else {
         output.print(node.classDoc.qualifiedName());
      }
      output.endElement("li");
      output.beginElement("ul");
      Iterator it = node.children.iterator();
      while (it.hasNext()) {
         TreeNode child = (TreeNode)it.next();
         printPackageTreeRec(output, child);
      }
      output.endElement("ul");
   }

   private void printClassTree(HtmlPage output, ClassDoc[] classDocs)
   {      
      Map classTreeMap = new HashMap();

      for (int i=0; i<classDocs.length; ++i) {
         ClassDoc classDoc = classDocs[i];
         if (!classDoc.isInterface()) {
            addClassTreeNode(classTreeMap, classDoc);
         }
      }

      TreeNode root = (TreeNode)classTreeMap.get("java.lang.Object");
      if (null != root) {
         output.div(CssClass.PACKAGE_TREE_SECTION_TITLE, "Class Hierarchy");
         output.beginElement("ul");
         printPackageTreeRec(output, root);
         output.endElement("ul");
      }
   }

   private void printInterfaceTree(HtmlPage output, ClassDoc[] classDocs)
   {
      Map interfaceTreeMap = new HashMap();

      for (int i=0; i<classDocs.length; ++i) {
         ClassDoc classDoc = classDocs[i];
         if (classDoc.isInterface()) {
            addInterfaceTreeNode(interfaceTreeMap, classDoc);
         }
      }

      TreeNode interfaceRoot = (TreeNode)interfaceTreeMap.get("<root>");
      if (null != interfaceRoot) {
         Iterator it = interfaceRoot.children.iterator();
         if (it.hasNext()) {
            output.div(CssClass.PACKAGE_TREE_SECTION_TITLE, "Interface Hierarchy");
            output.beginElement("ul");
            while (it.hasNext()) {
               TreeNode node = (TreeNode)it.next();
               printPackageTreeRec(output, node);
            }
            output.endElement("ul");
         }
      }

   }

   private void printPackageTreePage(File packageDir, String pathToRoot, PackageDoc packageDoc)
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(packageDir,
                                              "tree" + filenameExtension),
                                     pathToRoot);
      output.beginPage(packageDoc.name() + " Hierarchy");
      output.beginBody();
      printNavBarTop(output, "package-tree");

      output.div(CssClass.PACKAGE_TREE_TITLE, "Hierarchy for Package " + packageDoc.name());

      ClassDoc[] classDocs = packageDoc.allClasses();
      printClassTree(output, classDocs);
      printInterfaceTree(output, classDocs);

      printNavBarBottom(output, "package-tree");
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printFullTreePage()
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              "tree" + filenameExtension),
                                     ".");
      output.beginPage("Hierarchy");
      output.beginBody();
      printNavBarTop(output, "full-tree");

      output.div(CssClass.PACKAGE_TREE_TITLE, "Hierarchy");

      ClassDoc[] classDocs = getRootDoc().classes();
      printClassTree(output, classDocs);
      printInterfaceTree(output, classDocs);

      printNavBarBottom(output, "full-tree");
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printIndexEntry(HtmlPage output, Doc entry)
   {
      output.beginDiv(CssClass.INDEX_ENTRY);
      String anchor = null;
      String description = null;
      if (entry instanceof PackageDoc) {
         output.beginAnchor(getPackageURL((PackageDoc)entry) + "package-summary" + filenameExtension);
         output.print(entry.name());
         output.endAnchor();
         output.print(" - package");
      }
      else if (entry instanceof ClassDoc) {
         ClassDoc classDoc = (ClassDoc)entry;
         output.beginAnchor(getClassURL(classDoc));
         output.print(entry.name());
         output.endAnchor();
         output.print(" - ");
         if (entry.isInterface()) {
            output.print("interface ");
         }
         else if (entry.isException()) {
            output.print("exception ");
         }
         else if (entry.isError()) {
            output.print("error ");
         }
         else {
            output.print("class ");
         }
         String packageName = classDoc.containingPackage().name();
         if (packageName.length() > 0) {
            output.print(packageName);
            output.print(".");
         }
         printType(output, classDoc);
      }
      else {
         ProgramElementDoc memberDoc = (ProgramElementDoc)entry;
         output.beginAnchor(getMemberDocURL(output, memberDoc));
         output.print(entry.name());
         if (memberDoc instanceof ExecutableMemberDoc) {
            output.print(((ExecutableMemberDoc)memberDoc).signature());
         }
         output.endAnchor();
         output.print(" - ");

         if (memberDoc.isStatic()) {
            output.print("static ");
         }

         if (entry.isConstructor()) {
            output.print("constructor for class ");
         }
         else if (entry.isMethod()) {
            output.print("method in class ");
         }
         else if (entry.isField()) {
            output.print("field in class ");
         }
         ClassDoc containingClass = memberDoc.containingClass();
         String packageName = containingClass.containingPackage().name();
         if (packageName.length() > 0) {
            output.print(packageName);
            output.print(".");
         }
         printType(output, containingClass);
      }
      output.beginDiv(CssClass.INDEX_ENTRY_DESCRIPTION);
      printTags(output, entry.firstSentenceTags(), true);
      output.endDiv(CssClass.INDEX_ENTRY_DESCRIPTION);
      output.endDiv(CssClass.INDEX_ENTRY);
   }

   private void printFrameSetPage()
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              "index" + filenameExtension),
                                     ".",
                                     HtmlPage.DOCTYPE_FRAMESET);
      
      String title;
      if (null == optionWindowTitle.getValue()) {
         title = "Generated API Documentation";
      }
      else {
         title = optionWindowTitle.getValue();
      }
      output.beginPage(title);
      output.beginElement("frameset", "cols", "20%,80%");
      output.beginElement("frameset", "rows", "25%,75%");
      output.atomicElement("frame", 
                           new String[] { "src", "name" }, 
                           new String[] { "all-packages" + filenameExtension, "packages" });
      output.atomicElement("frame", 
                           new String[] { "src", "name" }, 
                           new String[] { "all-classes" + filenameExtension, "classes" });
      output.endElement("frameset");
      output.atomicElement("frame", 
                           new String[] { "src", "name" }, 
                           new String[] { "index-noframes" + filenameExtension, "content" });
      output.endElement("frameset");
      output.endPage();
      output.close();
   }

   private void printPackagesMenuPage()
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              "all-packages" + filenameExtension),
                                     ".");
      output.beginPage("Package Menu");
      output.beginBody();

      output.div(CssClass.PACKAGE_MENU_TITLE, "Packages");

      output.beginDiv(CssClass.PACKAGE_MENU_LIST);

      Set packageDocs = getAllPackages();
      Iterator it = packageDocs.iterator();
      while (it.hasNext()) {
         PackageDoc packageDoc = (PackageDoc)it.next();
         output.beginDiv(CssClass.PACKAGE_MENU_ENTRY);
         output.beginAnchor(getPackageURL(packageDoc) + "classes" + filenameExtension,
                            null,
                            "classes");
         if (packageDoc.name().length() > 0) {
            output.print(packageDoc.name());
         }
         else {
            output.print("&lt;unnamed package&gt;");
         }
         output.endAnchor();
         output.endDiv(CssClass.PACKAGE_MENU_ENTRY);
      }

      output.endDiv(CssClass.PACKAGE_MENU_LIST);
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printClassMenuList(HtmlPage output, ClassDoc[] classDocs)
   {
      output.beginDiv(CssClass.CLASS_MENU_LIST);

      for (int i=0; i<classDocs.length; ++i) {
         ClassDoc classDoc = classDocs[i];
         if (classDoc.isIncluded()) {
            output.beginDiv(CssClass.CLASS_MENU_ENTRY);
            output.beginAnchor(getClassDocURL(output, classDoc),
                               classDoc.qualifiedTypeName(),
                               "content");
            output.print(classDoc.name());
            output.endAnchor();
            output.endDiv(CssClass.CLASS_MENU_ENTRY);
         }
      }

      output.endDiv(CssClass.CLASS_MENU_LIST);
   }

   private void printAllClassesMenuPage()
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              "all-classes" + filenameExtension),
                                     ".");
      output.beginPage("Class Menu");
      output.beginBody();

      output.div(CssClass.CLASS_MENU_TITLE, "All Classes");

      printClassMenuList(output, getRootDoc().classes());

      output.endBody();
      output.endPage();
      output.close();
   }

   private void printPackageClassesMenuPage(File packageDir, String pathToRoot, PackageDoc packageDoc)
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(packageDir,
                                              "classes" + filenameExtension),
                                     pathToRoot);

      output.beginPage(packageDoc.name() + " Class Menu");
      output.beginBody();

      output.beginDiv(CssClass.CLASS_MENU_TITLE);
      output.beginAnchor("package-summary" + filenameExtension, "", "content");
      if (packageDoc.name().length() > 0) {
         output.print(packageDoc.name());
      }
      else {
         output.print("&lt;Unnamed&gt;");
      }
      output.endAnchor();
      output.endDiv(CssClass.CLASS_MENU_TITLE);

      printClassMenuList(output, packageDoc.allClasses());

      output.endBody();
      output.endPage();
      output.close();
   }

   private void printSplitIndex()
      throws IOException
   {
      Map categorizedIndex = getCategorizedIndex();
      Iterator it = categorizedIndex.keySet().iterator();
      int n = 1;
      while (it.hasNext()) {
         Character c = (Character)it.next();
         List classList = (List)categorizedIndex.get(c);
         printIndexPage(n++, c, classList);
      }
   }

   private void printIndexPage()
      throws IOException
   {
      printIndexPage(0, null, null);
   }

   private void printIndexPage(int index, Character letter, List classList)
      throws IOException
   {
      String pageName = "alphaindex";
      if (null != letter) {
         pageName += "-" + index;
      }
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              pageName + filenameExtension),
                                     ".");
      output.beginPage("Alphabetical Index");
      output.beginBody();
      printNavBarTop(output, "index");

      {
         output.div(CssClass.INDEX_TITLE, "Alphabetical Index");

         output.beginDiv(CssClass.INDEX_LETTERS);

         Iterator it = getCategorizedIndex().keySet().iterator();
         int n = 1;
         while (it.hasNext()) {
            Character c = (Character)it.next();
            output.beginSpan(CssClass.INDEX_LETTER);
            if (letter != null) {
               output.beginAnchor("alphaindex-" + n + filenameExtension);
            }
            else {
               output.beginAnchor("#" + c);
            }
            output.print(c.toString());
            output.endAnchor();
            output.endSpan(CssClass.INDEX_LETTER);     
            output.beginSpan(CssClass.INDEX_LETTER_SPACER);
            output.print(" ");
            output.endSpan(CssClass.INDEX_LETTER_SPACER);
            ++n;
         }
      }

      output.endDiv(CssClass.INDEX_LETTERS);

      if (null != letter) {
         printIndexCategory(output, letter, classList);
      }
      else {
         Map categorizedIndex = getCategorizedIndex();
         Iterator categoryIt = categorizedIndex.keySet().iterator();

         while (categoryIt.hasNext()) {
            letter = (Character)categoryIt.next();
            classList = (List)categorizedIndex.get(letter);
            output.anchorName(letter.toString());
            printIndexCategory(output, letter, classList);
         }
      }

      printNavBarBottom(output, "index");
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printIndexCategory(HtmlPage output, Character letter, List classList)
   {
      Iterator it = classList.iterator();

      output.div(CssClass.INDEX_CATEGORY_HEADER, letter.toString());
      output.beginDiv(CssClass.INDEX_CATEGORY);
      while (it.hasNext()) {
         Doc entry = (Doc)it.next();
         printIndexEntry(output, entry);
      }
      output.endDiv(CssClass.INDEX_CATEGORY);
   }

   private void printDeprecationSummary(HtmlPage output, List docs, String header)
   {
      if (!docs.isEmpty()) {
         output.beginTable(CssClass.DEPRECATION_SUMMARY);
         output.rowDiv(CssClass.TABLE_HEADER, header);

         Iterator it = docs.iterator();
         while (it.hasNext()) {
            Doc doc = (Doc)it.next();
            output.beginRow();
            
            output.beginCell(CssClass.DEPRECATION_SUMMARY_LEFT);
            if (doc instanceof Type) {
               printType(output, (Type)doc);
            }
            else {
               ProgramElementDoc memberDoc = (ProgramElementDoc)doc;
               output.beginAnchor(getMemberDocURL(output, memberDoc));
               output.print(memberDoc.containingClass().qualifiedName());
               output.print(".");
               output.print(memberDoc.name());
               if (memberDoc instanceof ExecutableMemberDoc) {
                  output.print(((ExecutableMemberDoc)memberDoc).flatSignature());
               }
               output.endAnchor();
            }
            output.beginDiv(CssClass.DEPRECATION_SUMMARY_DESCRIPTION);
            printTags(output, doc.tags("deprecated")[0].firstSentenceTags(), true);
            output.endDiv(CssClass.DEPRECATION_SUMMARY_DESCRIPTION);

            output.endCell();

            output.endRow();
         }
         output.endTable();
      }
   }

   private void printDeprecationPage()
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              "deprecated" + filenameExtension),
                                     ".");
      output.beginPage("Deprecated API");
      output.beginBody();
      printNavBarTop(output, "deprecated");

      output.div(CssClass.DEPRECATION_TITLE, "Deprecated API");

      List deprecatedInterfaces = new LinkedList();
      List deprecatedExceptions = new LinkedList();
      List deprecatedErrors = new LinkedList();
      List deprecatedClasses = new LinkedList();
      List deprecatedFields = new LinkedList();
      List deprecatedMethods = new LinkedList();
      List deprecatedConstructors = new LinkedList();

      ClassDoc[] classDocs = getRootDoc().classes();
      for (int i=0; i<classDocs.length; ++i) {
         ClassDoc classDoc = classDocs[i];
         {
            Tag[] deprecatedTags = classDoc.tags("deprecated");
            if (null != deprecatedTags && deprecatedTags.length > 0) {
               if (classDoc.isInterface()) {
                  deprecatedInterfaces.add(classDoc);
               }
               else if (classDoc.isException()) {
                  deprecatedExceptions.add(classDoc);
               }
               else if (classDoc.isError()) {
                  deprecatedErrors.add(classDoc);
               }
               else {
                  deprecatedClasses.add(classDoc);
               }
            }
         }
         ConstructorDoc[] constructors = classDoc.constructors();
         for (int j=0; j<constructors.length; ++j) {
            Tag[] deprecatedTags = constructors[j].tags("deprecated");
            if (null != deprecatedTags && deprecatedTags.length > 0) {
               deprecatedConstructors.add(constructors[j]);
            }
         }
         MethodDoc[] methods = classDoc.methods();
         for (int j=0; j<methods.length; ++j) {
            Tag[] deprecatedTags = methods[j].tags("deprecated");
            if (null != deprecatedTags && deprecatedTags.length > 0) {
               deprecatedMethods.add(methods[j]);
            }
         }
         FieldDoc[] fields = classDoc.fields();
         for (int j=0; j<fields.length; ++j) {
            Tag[] deprecatedTags = fields[j].tags("deprecated");
            if (null != deprecatedTags && deprecatedTags.length > 0) {
               deprecatedFields.add(fields[j]);
            }
         }
      }

      output.beginDiv(CssClass.DEPRECATION_TOC);
      output.div(CssClass.DEPRECATION_TOC_HEADER, "Contents");
      output.beginDiv(CssClass.DEPRECATION_TOC_LIST);
      if (!deprecatedInterfaces.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#interfaces", "Deprecated Interfaces");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      if (!deprecatedClasses.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#classes", "Deprecated Classes");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      if (!deprecatedExceptions.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#exceptions", "Deprecated Exceptions");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      if (!deprecatedErrors.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#errors", "Deprecated Errors");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      if (!deprecatedFields.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#fields", "Deprecated Fields");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      if (!deprecatedFields.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#methods", "Deprecated Methods");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      if (!deprecatedFields.isEmpty()) {
         output.beginDiv(CssClass.DEPRECATION_TOC_ENTRY);
         output.anchor("#constructors", "Deprecated Constructors");
         output.endDiv(CssClass.DEPRECATION_TOC_ENTRY);
      }
      output.endDiv(CssClass.DEPRECATION_TOC_LIST);
      output.endDiv(CssClass.DEPRECATION_TOC);

      output.anchorName("interfaces");
      printDeprecationSummary(output, deprecatedInterfaces, "Deprecated Interfaces");

      output.anchorName("classes");
      printDeprecationSummary(output, deprecatedClasses, "Deprecated Classes");

      output.anchorName("exceptions");
      printDeprecationSummary(output, deprecatedExceptions, "Deprecated Exceptions");

      output.anchorName("errors");
      printDeprecationSummary(output, deprecatedErrors, "Deprecated Errors");

      output.anchorName("fields");
      printDeprecationSummary(output, deprecatedFields, "Deprecated Fields");

      output.anchorName("methods");
      printDeprecationSummary(output, deprecatedMethods, "Deprecated Methods");

      output.anchorName("constructors");
      printDeprecationSummary(output, deprecatedConstructors, "Deprecated Constructors");

      printNavBarBottom(output, "deprecated");
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printOverviewPage()
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(getTargetDirectory(),
                                              "index-noframes" + filenameExtension),
                                     ".");
      output.beginPage("Overview");
      output.beginBody();

      printNavBarTop(output, "overview");

      String overviewHeader;
      if (null == optionDocTitle.getValue()) {
         overviewHeader = "Overview";
      }
      else {
         overviewHeader = optionDocTitle.getValue();
      }
      output.div(CssClass.OVERVIEW_TITLE, overviewHeader);

      /*
      output.beginDiv(CssClass.PACKAGE_DESCRIPTION_TOP);
      printTags(output, packageDoc.firstSentenceTags(), true);
      output.endDiv(CssClass.PACKAGE_DESCRIPTION_TOP);
      */

      List packageGroups = getPackageGroups();

      if (packageGroups.isEmpty()) {
      
         printOverviewPackages(output, getAllPackages(),
                               "All Packages");
      }
      else {
         Set otherPackages = new LinkedHashSet();
         otherPackages.addAll(getAllPackages());

         Iterator it = packageGroups.iterator();
         while (it.hasNext()) {
            PackageGroup packageGroup = (PackageGroup)it.next();
            printOverviewPackages(output, 
                                  packageGroup.getPackages(),
                                  packageGroup.getName());
            otherPackages.removeAll(packageGroup.getPackages());
         }

         if (!otherPackages.isEmpty()) {
            printOverviewPackages(output, 
                                  otherPackages,
                                  "Other Packages");
         }
      }

      /*
      output.anchorName("description");
      output.beginDiv(CssClass.PACKAGE_DESCRIPTION_FULL);
      printTags(output, packageDoc.inlineTags(), false);
      output.endDiv(CssClass.PACKAGE_DESCRIPTION_FULL);
      */

      printNavBarBottom(output, "overview");
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printOverviewPackages(HtmlPage output, Collection packageDocs, String header)
   {
      output.beginTable(CssClass.OVERVIEW_SUMMARY);
      output.rowDiv(CssClass.TABLE_HEADER, header);

      Iterator it = packageDocs.iterator();
      while (it.hasNext()) {
         PackageDoc packageDoc = (PackageDoc)it.next();
         output.beginRow();
         
         output.beginCell(CssClass.OVERVIEW_SUMMARY_LEFT);
         output.beginAnchor(getPackageURL(packageDoc) + "package-summary" + filenameExtension);
         output.print(packageDoc.name());
         output.endAnchor();
         output.endCell();

         output.beginCell(CssClass.OVERVIEW_SUMMARY_RIGHT);
         printTags(output, packageDoc.firstSentenceTags(), true);
         output.endCell();
         output.endRow();
      }
      output.endTable();
   }

   private void printClassUsagePage(File packageDir, String pathToRoot, ClassDoc classDoc)
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(packageDir,
                                              classDoc.name() + "-uses" + filenameExtension),
                                     pathToRoot);
      output.beginPage(classDoc.name());
      output.beginBody();
      printNavBarTop(output, "uses", classDoc);

      output.div(CssClass.USAGE_TITLE, "Uses of class " + classDoc.qualifiedName());

      Map packageToUsageTypeMap = getUsageOfClass(classDoc);
      if (null != packageToUsageTypeMap) {

         Iterator packagesIterator = packageToUsageTypeMap.keySet().iterator();
         while (packagesIterator.hasNext()) {
            PackageDoc packageDoc = (PackageDoc)packagesIterator.next();

            output.div(CssClass.USAGE_PACKAGE_TITLE, "Uses in package " + packageDoc.name());

            Map usageTypeToUsersMap = (Map)packageToUsageTypeMap.get(packageDoc);
            Iterator usageTypeIterator = usageTypeToUsersMap.keySet().iterator();
            while (usageTypeIterator.hasNext()) {
               UsageType usageType = (UsageType)usageTypeIterator.next();
               
               output.beginTable(CssClass.USAGE_SUMMARY);
               output.rowDiv(CssClass.TABLE_HEADER, format("usagetype." + usageType.getId(), 
                                                           classDoc.qualifiedName()));

               Set users = (Set)usageTypeToUsersMap.get(usageType);
               Iterator userIterator = users.iterator();
               while (userIterator.hasNext()) {
                  Doc user = (Doc)userIterator.next();
                  if (user instanceof ClassDoc) {
                     output.beginCell(CssClass.USAGE_SUMMARY_LEFT);
                     output.print("class");
                     output.endCell();

                     output.beginCell(CssClass.USAGE_SUMMARY_RIGHT);
                     printType(output, ((ClassDoc)user));
                     output.endCell();
                  }
                  else if (user instanceof FieldDoc) {
                     FieldDoc fieldDoc = (FieldDoc)user;

                     output.beginCell(CssClass.USAGE_SUMMARY_LEFT);
                     printType(output, ((FieldDoc)user).type());
                     output.endCell();

                     output.beginCell(CssClass.USAGE_SUMMARY_RIGHT);
                     output.beginAnchor(getMemberDocURL(output, (FieldDoc)user));
                     output.print(((FieldDoc)user).name());
                     output.endAnchor();
                     output.endCell();
                  }
                  else if (user instanceof MethodDoc) {
                     MethodDoc methodDoc = (MethodDoc)user;

                     output.beginCell(CssClass.USAGE_SUMMARY_LEFT);
                     printType(output, ((MethodDoc)user).returnType());
                     output.endCell();

                     output.beginCell(CssClass.USAGE_SUMMARY_RIGHT);
                     output.beginAnchor(getMemberDocURL(output, (MethodDoc)user));
                     output.print(((MethodDoc)user).name());
                     output.endAnchor();
                     printParameters(output, (ExecutableMemberDoc)user);
                     output.endCell();
                  }
                  else if (user instanceof ConstructorDoc) {
                     ConstructorDoc constructorDoc = (ConstructorDoc)user;

                     output.beginCell(CssClass.USAGE_SUMMARY_LEFT);
                     output.endCell();

                     output.beginCell(CssClass.USAGE_SUMMARY_RIGHT);
                     output.beginAnchor(getMemberDocURL(output, (ConstructorDoc)user));
                     output.print(((ConstructorDoc)user).name());
                     output.endAnchor();
                     printParameters(output, (ExecutableMemberDoc)user);
                     output.endCell();
                  }
               }
            }
         }
      }
      printNavBarBottom(output, "uses", classDoc);
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printClassPage(File packageDir, String pathToRoot, ClassDoc classDoc)
      throws IOException
   {
      HtmlPage output = new HtmlPage(new File(packageDir,
                                              classDoc.name() + filenameExtension),
                                     pathToRoot);
      output.beginPage(classDoc.name());
      output.beginBody();
      printNavBarTop(output, "class", classDoc);
      
      output.beginDiv(CssClass.CLASS_TITLE);
      output.div(CssClass.CLASS_TITLE_PACKAGE, 
                 classDoc.containingPackage().name());
      output.div(CssClass.CLASS_TITLE_CLASS, 
                 getClassTypeName(classDoc) 
                 + " " + classDoc.name());


      List knownDirectSubclasses = getKnownDirectSubclasses(classDoc);
      if (!knownDirectSubclasses.isEmpty()) {
         output.beginDiv(CssClass.CLASS_SUBCLASSES);
         output.div(CssClass.CLASS_SUBCLASSES_HEADER, "Known Direct Subclasses:");
         Iterator it = knownDirectSubclasses.iterator();
         while (it.hasNext()) {
            printType(output, (ClassDoc)it.next());
            if (it.hasNext()) {
               output.print(", ");
            }
         }
         output.endDiv(CssClass.CLASS_SUBCLASSES_HEADER);
      }

      output.hr();

      output.beginDiv(CssClass.CLASS_SYNOPSIS);
      output.beginDiv(CssClass.CLASS_SYNOPSIS_DECLARATION);
      output.print(getFullModifiers(classDoc) + ' ' + getClassTypeKeyword(classDoc) 
                   + ' ');
      if (optionLinkSource.getValue() && null != classDoc.position()) {
         output.beginAnchor(getOuterClassDoc(classDoc).name() + "-source" + filenameExtension + "#line." + classDoc.position());
         output.print(classDoc.name());
         output.endAnchor();
      }
      else {
         output.print(classDoc.name());
      }
      output.endDiv(CssClass.CLASS_SYNOPSIS_DECLARATION);

      if (null != classDoc.superclass()) {
         output.beginDiv(CssClass.CLASS_SYNOPSIS_SUPERCLASS);
         output.print("extends ");
         printType(output, classDoc.superclass());
         output.endDiv(CssClass.CLASS_SYNOPSIS_SUPERCLASS);
      }

      ClassDoc[] interfaces = classDoc.interfaces();
      if (interfaces.length > 0) {
         output.beginDiv(CssClass.CLASS_SYNOPSIS_IMPLEMENTS);
         output.print("implements ");
         for (int i=0; i<interfaces.length; ++i) {
            if (i>0) {
               output.print(", ");
            }
            printType(output, interfaces[i]);
         }
         output.endDiv(CssClass.CLASS_SYNOPSIS_IMPLEMENTS);
      }
      output.endDiv(CssClass.CLASS_SYNOPSIS);

      output.hr();

      output.beginDiv(CssClass.CLASS_DESCRIPTION);
      printTags(output, classDoc.inlineTags(), false);
      output.endDiv(CssClass.CLASS_DESCRIPTION);

      printTaglets(output, classDoc.tags(), TagletContext.TYPE);
      
      printProgramElementDocs(output, classDoc.fields(), 
                              "Field Summary");
      printProgramElementDocs(output, classDoc.constructors(), 
                              "Constructor Summary");
      printProgramElementDocs(output, classDoc.methods(), 
                              "Method Summary");

      ClassDoc superClassDoc = classDoc.superclass();
      while (null != superClassDoc) {
         printInheritedMembers(output, superClassDoc.methods(),
                               "Methods inherited from " + superClassDoc.qualifiedName());
         superClassDoc = superClassDoc.superclass();
      }

      printMemberDetails(output, classDoc.fields(), 
                         "Field Details");
      printMemberDetails(output, classDoc.constructors(), 
                         "Constructor Details");
      printMemberDetails(output, classDoc.methods(), 
                         "Method Details");

      printNavBarBottom(output, "class", classDoc);
      output.endBody();
      output.endPage();
      output.close();
   }

   private void printInheritedMembers(HtmlPage output,
                                      ProgramElementDoc[] memberDocs, String header)
   {
      if (memberDocs.length > 0) {
         output.beginTable(CssClass.CLASS_SUMMARY);
         output.rowDiv(CssClass.TABLE_HEADER, header);

         output.beginRow();
         output.beginCell();
         for (int i=0; i<memberDocs.length; ++i) {
            ProgramElementDoc memberDoc = memberDocs[i];
            if (i > 0) {
               output.print(", ");
            }
            output.beginAnchor(getMemberDocURL(output, memberDoc));
            output.print(memberDoc.name());
            output.endAnchor();
         }
         output.endCell();
         output.endRow();
         output.endTable();
      }
   }

   private void collectSpecifiedByRecursive(Set specifyingInterfaces, 
                                            ClassDoc classDoc,
                                            MethodDoc methodDoc)
   {
      ClassDoc[] interfaces = classDoc.interfaces();
      for (int i=0; i<interfaces.length; ++i) {
         MethodDoc[] methods = interfaces[i].methods();
         for (int j=0; j<methods.length; ++j) {
            if (methods[j].name().equals(methodDoc.name())
                && methods[j].signature().equals(methodDoc.signature())) {
               specifyingInterfaces.add(interfaces[i]);
            }
         }
         collectSpecifiedByRecursive(specifyingInterfaces,
                                     interfaces[i],
                                     methodDoc);
      }
   }

   private void printMemberDetails(HtmlPage output,
                                   ProgramElementDoc[] memberDocs, String header)
   {
      if (memberDocs.length > 0) {
         output.div(CssClass.TABLE_HEADER, header);

         for (int i=0; i<memberDocs.length; ++i) {
            if (i>0) {
               output.hr();
            }

            ProgramElementDoc memberDoc = memberDocs[i];

            output.anchorName(getMemberAnchor(memberDoc));

            output.beginDiv(CssClass.MEMBER_DETAIL);
            output.div(CssClass.MEMBER_DETAIL_NAME, memberDoc.name());

            output.beginDiv(CssClass.MEMBER_DETAIL_SYNOPSIS);
            output.print(getFullModifiers(memberDoc));
            if (memberDoc.isMethod()) {
               output.print(" ");
               printType(output, ((MethodDoc)memberDoc).returnType());
            }
            output.print(" ");

            if (optionLinkSource.getValue() && null != memberDoc.position()) {
               ClassDoc containingClass = memberDoc.containingClass();
               while (null != containingClass.containingClass()) {
                  containingClass = containingClass.containingClass();
               }
               String href = containingClass.name() + "-source" + filenameExtension + "#line." + memberDoc.position().line();
               output.beginAnchor(href);
               output.print(memberDoc.name());
               output.endAnchor();
            }
            else {
               output.print(memberDoc.name());
            }

            if (memberDoc.isConstructor() || memberDoc.isMethod()) {
               printParameters(output, (ExecutableMemberDoc)memberDoc);
            }
            output.endDiv(CssClass.MEMBER_DETAIL_SYNOPSIS);

            output.beginDiv(CssClass.MEMBER_DETAIL_DESCRIPTION);
            printTags(output, memberDoc.inlineTags(), false);
            output.endDiv(CssClass.MEMBER_DETAIL_DESCRIPTION);

            if (memberDoc.isConstructor() || memberDoc.isMethod()) {

               if (memberDoc.isMethod()) {
                  Set specifyingInterfaces = new LinkedHashSet();
                  for (ClassDoc cd = memberDoc.containingClass();
                       null != cd; cd = cd.superclass()) {
                     collectSpecifiedByRecursive(specifyingInterfaces,
                                                 cd, 
                                                 (MethodDoc)memberDoc);
                  }

                  if (!specifyingInterfaces.isEmpty()) {
                     output.beginDiv(CssClass.MEMBER_DETAIL_SPECIFIED_BY_LIST);
                     output.div(CssClass.MEMBER_DETAIL_SPECIFIED_BY_HEADER, "Specified by:");
                     Iterator it = specifyingInterfaces.iterator();
                     while (it.hasNext()) {
                        ClassDoc specifyingInterface = (ClassDoc)it.next();
                        output.beginDiv(CssClass.MEMBER_DETAIL_SPECIFIED_BY_ITEM);
                        output.print(memberDoc.name() + " in interface ");
                        printType(output, specifyingInterface);
                        output.endDiv(CssClass.MEMBER_DETAIL_SPECIFIED_BY_ITEM);
                     }
                     output.endDiv(CssClass.MEMBER_DETAIL_SPECIFIED_BY_LIST);
                  }
                  
                  ClassDoc overriddenClassDoc = null;

                  for (ClassDoc superclassDoc = memberDoc.containingClass().superclass();
                       null != superclassDoc && null == overriddenClassDoc;
                       superclassDoc = superclassDoc.superclass()) {
                     
                     MethodDoc[] methods = superclassDoc.methods();
                     for (int j=0; j<methods.length; ++j) {
                        if (methods[j].name().equals(memberDoc.name())
                            && methods[j].signature().equals(((MethodDoc)memberDoc).signature())) {
                           overriddenClassDoc = superclassDoc;
                           break;
                        }
                     }
                  }

                  if (null != overriddenClassDoc) {
                     output.beginDiv(CssClass.MEMBER_DETAIL_OVERRIDDEN_LIST);
                     output.div(CssClass.MEMBER_DETAIL_OVERRIDDEN_HEADER, "Overrides:");
                     output.beginDiv(CssClass.MEMBER_DETAIL_OVERRIDDEN_ITEM);

                     output.print(memberDoc.name() + " in interface ");
                     printType(output, overriddenClassDoc);

                     output.endDiv(CssClass.MEMBER_DETAIL_OVERRIDDEN_ITEM);
                     output.endDiv(CssClass.MEMBER_DETAIL_OVERRIDDEN_LIST);
                  }
               }

               ExecutableMemberDoc execMemberDoc
                  = (ExecutableMemberDoc)memberDoc;

               Parameter[] parameters = execMemberDoc.parameters();
               if (parameters.length > 0) {
                  output.beginDiv(CssClass.MEMBER_DETAIL_PARAMETER_LIST);
                  output.div(CssClass.MEMBER_DETAIL_PARAMETER_HEADER, "Parameters:");
                  for (int j=0; j<parameters.length; ++j) {
                     Parameter parameter = parameters[j];
                     ParamTag[] paramTags = execMemberDoc.paramTags();
                     ParamTag paramTag = null;
                     for (int k=0; k<paramTags.length; ++k) {
                        if (paramTags[k].parameterName().equals(parameter.name())) {
                           paramTag = paramTags[k];
                           break;
                        }
                     }

                     output.beginDiv(CssClass.MEMBER_DETAIL_PARAMETER_ITEM);
                     output.print(parameter.name());
                     if (null != paramTag) {
                        output.print(" - ");
                        printTags(output, paramTag.inlineTags(), false);
                     }
                     output.endDiv(CssClass.MEMBER_DETAIL_PARAMETER_ITEM);
                  }
                  output.endDiv(CssClass.MEMBER_DETAIL_PARAMETER_LIST);
               }

               if (execMemberDoc.isMethod() 
                   && !"void".equals(((MethodDoc)execMemberDoc).returnType().typeName())) {

                  Tag[] returnTags = execMemberDoc.tags("@return");
                  if (returnTags.length > 0) {
                     output.beginDiv(CssClass.MEMBER_DETAIL_RETURN_LIST);
                     output.div(CssClass.MEMBER_DETAIL_RETURN_HEADER, "Returns:");
                     output.beginDiv(CssClass.MEMBER_DETAIL_RETURN_ITEM);

                     printTags(output, returnTags, false);

                     output.endDiv(CssClass.MEMBER_DETAIL_RETURN_ITEM);
                     output.endDiv(CssClass.MEMBER_DETAIL_RETURN_LIST);
                  }
               }

               ClassDoc[] thrownExceptions = execMemberDoc.thrownExceptions();
               if (thrownExceptions.length > 0) {
                  output.beginDiv(CssClass.MEMBER_DETAIL_THROWN_LIST);
                  output.div(CssClass.MEMBER_DETAIL_THROWN_HEADER, "Throws:");
                  for (int j=0; j<thrownExceptions.length; ++j) {
                     ClassDoc exception = thrownExceptions[j];
                     ThrowsTag[] throwsTags = execMemberDoc.throwsTags();
                     ThrowsTag throwsTag = null;
                     for (int k=0; k<throwsTags.length; ++k) {
                        if (null != throwsTags[k].exception()
                            && throwsTags[k].exception().equals(exception)) {
                           throwsTag = throwsTags[k];
                           break;
                        }
                     }

                     output.beginDiv(CssClass.MEMBER_DETAIL_THROWN_ITEM);
                     printType(output, exception);
                     if (null != throwsTag) {
                        output.print(" - ");
                        printTags(output, throwsTag.inlineTags(), false);
                     }
                     output.endDiv(CssClass.MEMBER_DETAIL_THROWN_ITEM);
                  }
                  output.endDiv(CssClass.MEMBER_DETAIL_THROWN_LIST);
               }
            }

            TagletContext context;
            if (memberDoc.isField()) {
               context = TagletContext.FIELD;
            }
            else if (memberDoc.isMethod()) {
               context = TagletContext.METHOD;
            }
            else if (memberDoc.isConstructor()) {
               context = TagletContext.CONSTRUCTOR;
            }
            else {
               // assert(false);  -- we should have a field, method or constructor here
               throw new RuntimeException("Assertion failed: expected field, method or constructor");
            }

            printTaglets(output, memberDoc.tags(), context);

            output.endDiv(CssClass.MEMBER_DETAIL);
         }
      }
   }


   private void printParameters(HtmlPage output, ExecutableMemberDoc memberDoc)
   {
      Parameter[] parameters = memberDoc.parameters();
      output.print("(");
      for (int j=0; j<parameters.length; ++j) {
         if (j > 0) {
            output.print(", ");
         }
         printType(output, parameters[j].type());
         output.print(" ");
         output.print(parameters[j].name());
      }
      output.print(")");
   }

   private void printProgramElementDocs(HtmlPage output,
                                        ProgramElementDoc[] memberDocs, String header)
   {
      if (memberDocs.length > 0) {
         output.beginTable(CssClass.CLASS_SUMMARY);
         output.rowDiv(CssClass.TABLE_HEADER, header);

         for (int i=0; i<memberDocs.length; ++i) {
            ProgramElementDoc memberDoc = memberDocs[i];
            output.beginRow();

            output.beginCell(CssClass.CLASS_SUMMARY_LEFT);
            output.print(getFullModifiers(memberDoc) + " ");
            if (memberDoc.isMethod()) {
               printType(output, ((MethodDoc)memberDoc).returnType());
            }
            else if (memberDoc.isField()) {
               printType(output, ((FieldDoc)memberDoc).type());
            }
            output.endCell();

            output.beginCell(CssClass.CLASS_SUMMARY_RIGHT);
            output.print(" ");
            output.beginAnchor("#" + getMemberAnchor(memberDoc));
            output.print(memberDoc.name());
            output.endAnchor();
            if (memberDoc.isConstructor() || memberDoc.isMethod()) {
               printParameters(output, (ExecutableMemberDoc)memberDoc);
            }
            output.endCell();
            output.endRow();

            output.beginRow();
            output.beginCell();
            output.endCell();
            output.beginCell();
            printTags(output, memberDoc.firstSentenceTags(), true);
            output.endCell();
            output.endRow();
         }
         output.endTable();
      }
   }

   private void printTag(HtmlPage output, Tag tag, boolean firstSentence)
   {
      if ("Text".equals(tag.name())) {
         output.print(tag.text());
      }
      else if ("@link".equals(tag.name())) {
         SeeTag seeTag = (SeeTag)tag;
         String href = null;
         MemberDoc referencedMember = seeTag.referencedMember();
         if (null != seeTag.referencedClass()) {
            href = getClassDocURL(output, seeTag.referencedClass());
            if (null != referencedMember) {
               href  += '#' + referencedMember.name();
               if (referencedMember.isMethod() || referencedMember.isConstructor()) {
                  href += ((ExecutableMemberDoc)referencedMember).signature();
               }
            }
            else if (null != seeTag.referencedMemberName()) {
               href = null;
            }
         }
         else if (null != referencedMember) {
            href = '#' + referencedMember.name();
               if (referencedMember.isMethod() || referencedMember.isConstructor()) {
                  href += ((ExecutableMemberDoc)referencedMember).signature();
               }
         }

         String label = tag.text();
         if (label.startsWith("#")) {
            label = label.substring(1);
         }
         else {
            label = label.replace('#', '.');
         }
         
         if (null != href) {
            output.beginAnchor(href);
            output.print(label);
            output.endAnchor();
         }
         else {
            output.print(label);
         }
      }
      else if ("@docRoot".equals(tag.name())) {         
         output.print(output.getPathToRoot());
      }
      else if (firstSentence) {
         printTags(output, tag.firstSentenceTags(), true);
      }
      else {
         printTags(output, tag.inlineTags(), false);
      }
   }

   private void printTags(HtmlPage output, Tag[] tags, boolean firstSentence)
   {
      for (int i=0; i<tags.length; ++i) {
         printTag(output, tags[i], firstSentence);
      }
   }

   private String getClassDocURL(HtmlPage output, ClassDoc classDoc)
   {
      return output.getPathToRoot() 
         + "/"
         + getPackageURL(classDoc.containingPackage()) 
         + classDoc.name() + filenameExtension;
   }

   private String getMemberDocURL(HtmlPage output, ProgramElementDoc memberDoc)
   {
      ClassDoc classDoc = memberDoc.containingClass();
      String result = output.getPathToRoot() 
         + "/"
         + getPackageURL(classDoc.containingPackage()) 
         + classDoc.name() + filenameExtension + "#" + memberDoc.name();
      if (memberDoc instanceof ExecutableMemberDoc) {
         result += ((ExecutableMemberDoc)memberDoc).signature();
      }
      return result;
   }

   private void printType(HtmlPage output, Type type)
   {
      ClassDoc asClassDoc = type.asClassDoc();
      String url = null;
      if (null != asClassDoc && asClassDoc.isIncluded()) {
         url = getClassDocURL(output, asClassDoc);
      }
      else /* if (!type.isPrimitive()) */ {
         if (type.qualifiedTypeName().length() > type.typeName().length()) {
            String packageName = type.qualifiedTypeName();
            packageName = packageName.substring(0, packageName.length() - type.typeName().length() - 1);
            
            ExternalDocSet externalDocSet
               = (ExternalDocSet)packageNameToDocSet.get(packageName);
            if (null != externalDocSet) {
               try {
                  url = externalDocSet.getClassDocURL(packageName, type.typeName());
               }
               catch (MalformedURLException ignore) {
               }
            }
         }
      }

      
      if (null != url) {
         output.beginAnchor(url);
         output.print(asClassDoc.name());
         output.endAnchor();
         output.print(asClassDoc.dimension());
      }
      else {
         output.print(type.qualifiedTypeName());
      }
   }

   private void printTaglets(final HtmlPage output, Tag[] tags, TagletContext context) 
   {
      super.printTaglets(tags, context, new TagletPrinter() {
            public void printTagletString(String tagletString) {
               output.print(tagletString);
            }
         });
   }

   private String getPackageURL(PackageDoc packageDoc)
   {
      if (packageDoc.name().length() > 0) {
         return packageDoc.name().replace('.', '/') + '/';
      }
      else {
         return "";
      }
   }

   private String getClassURL(ClassDoc classDoc)
   {
      return getPackageURL(classDoc.containingPackage()) + classDoc.name() + filenameExtension;
   }

   protected void run()
      throws DocletConfigurationException, IOException
   {
      printNotice("HtmlDoclet running.");
      {
         Iterator it = externalDocSets.iterator();
         while (it.hasNext()) {
            ExternalDocSet externalDocSet = (ExternalDocSet)it.next();
            printNotice("Fetching package list for external documentation set.");     
            try {
               externalDocSet.load(getTargetDirectory());
            }
            catch (FileNotFoundException e) {
               throw new DocletConfigurationException("Cannot fetch package list from " + externalDocSet.getPackageListDir());
            }
            Iterator pit = externalDocSet.getPackageNames().iterator();
            while (pit.hasNext()) {
               String packageName = (String)pit.next();
               packageNameToDocSet.put(packageName, externalDocSet);
            }
         }
      }

      printNotice("Writing HTML overview file");
      printFrameSetPage();
      printPackagesMenuPage();
      printAllClassesMenuPage();
      printOverviewPage();
      printPackagesListFile();
      if (!optionNoTree.getValue()) {
         printFullTreePage();
      }      
      if (!optionNoIndex.getValue()) {
         if (!optionSplitIndex.getValue()) {
            printIndexPage();
         }
         else {
            printSplitIndex();
         }
      }
      if (!optionNoHelp.getValue()) {
         InputStream helpIn;
         if (null != optionHelpFile.getValue()){ 
            helpIn = new FileInputStream(optionHelpFile.getValue());
         }
         else {
            helpIn = getClass().getResourceAsStream("/htmldoclet/help.xhtml");
         }
         FileOutputStream helpOut = new FileOutputStream(new File(getTargetDirectory(),
                                                                  "help" + filenameExtension));
         IOToolkit.copyStream(helpIn, helpOut);
         helpIn.close();
         helpOut.close();
      }

      File stylesheetTargetFile = new File(getTargetDirectory(),
                                           "gjdochtml.css");

      if (null != optionStylesheetFile.getValue()) { 
         IOToolkit.copyFile(optionStylesheetFile.getValue(),
                            stylesheetTargetFile);
      }
      else {
         InputStream cssIn = getClass().getResourceAsStream("/htmldoclet/gjdochtml-vanilla.css");
         FileOutputStream cssOut = new FileOutputStream(stylesheetTargetFile);
         IOToolkit.copyStream(cssIn, cssOut);
         cssIn.close();
         cssOut.close();
      }

      if (!optionNoDeprecatedList.getValue()) {
         printDeprecationPage();
      }

      Collection packageDocs = getAllPackages();
      Iterator it = packageDocs.iterator();
      while (it.hasNext()) {
         PackageDoc packageDoc = (PackageDoc)it.next();
         File packageDir = new File(getTargetDirectory(),
                                    packageDoc.name().replace('.', File.separatorChar));
         if (!packageDir.exists() && !packageDir.mkdirs()) {
            throw new IOException("Couldn't create directory " + packageDir);
         }
         File sourcePackageDir = null;
         try {
            sourcePackageDir = getPackageSourceDir(packageDoc);
            copyDocFiles(sourcePackageDir, packageDir);
         }
         catch (IOException ignore) {
         }
         String pathToRoot = getPathToRoot(packageDir, getTargetDirectory());
         String packageName = packageDoc.name();
         if (0 == packageName.length()) {
            packageName = "<unnamed>";
         }
         printNotice("Writing HTML files for package " + packageName);
         printPackagePage(packageDir, pathToRoot, packageDoc);
         if (!optionNoTree.getValue()) {
            printPackageTreePage(packageDir, pathToRoot, packageDoc);
         }
         printPackageClassesMenuPage(packageDir, pathToRoot, packageDoc);
         ClassDoc[] classDocs = packageDoc.allClasses();
         for (int j=0; j<classDocs.length; ++j) {
            ClassDoc classDoc = classDocs[j];
            if (classDoc.isIncluded()) {
               printClassPage(packageDir, pathToRoot, classDocs[j]);
               if (optionUse.getValue()) {
                  printClassUsagePage(packageDir, pathToRoot, classDocs[j]);
               }
               if (optionLinkSource.getValue() && null == classDoc.containingClass()) {
                  if (null != sourcePackageDir) {
                     Java2xhtml java2xhtml = new Java2xhtml();
                     Properties properties = new Properties();
                     properties.setProperty("isCodeSnippet", "true");
                     properties.setProperty("hasLineNumbers", "true");
                     java2xhtml.setProperties(properties);
                     
                     StringWriter sourceBuffer = new StringWriter();
                     File sourceFile = new File(sourcePackageDir,
                                                classDoc.name() + ".java");
                     FileReader sourceReader = new FileReader(sourceFile);
                     IOToolkit.copyStream(sourceReader, sourceBuffer);
                     sourceReader.close();
                     String result = java2xhtml.makeHTML(sourceBuffer.getBuffer(), sourceFile.getName());
                     
                     File targetFile = new File(packageDir,
                                                classDoc.name() + "-source" + filenameExtension);
                     FileWriter targetWriter = new FileWriter(targetFile);
                     targetWriter.write(result);
                     targetWriter.close();
                  }
                  else {
                     printWarning("Cannot locate source file for class " + classDoc.qualifiedTypeName());
                  }
               }
            }
         }
      }
   }

   private String getPathToRoot(File subDir, File rootDir)
   {
      StringBuffer result = new StringBuffer();
      while (!subDir.equals(rootDir)) {
         subDir = subDir.getParentFile();
         result.append("../");
      }
      if (0 == result.length()) {
         result.append(".");
      }
      return result.toString();
   }

   private String getClassTypeName(ClassDoc classDoc) 
   {
      if (classDoc.isInterface()) {
         return "Interface";
      }
      else {
         return "Class";
      }
   }

   private String getClassTypeKeyword(ClassDoc classDoc) 
   {
      if (classDoc.isInterface()) {
         return "interface";
      }
      else {
         return "class";
      }
   }

   private String getMemberAnchor(ProgramElementDoc memberDoc)
   {
      String anchor = memberDoc.name();
      if (memberDoc.isConstructor() || memberDoc.isMethod()) {
         anchor += ((ExecutableMemberDoc)memberDoc).signature();
      }
      return anchor;
   }

   private String getFullModifiers(ProgramElementDoc memberDoc)
   {
      if (memberDoc.isClass() && ((ClassDoc)memberDoc).isAbstract()) {
         return memberDoc.modifiers() + " abstract";
      }
      else {
         return memberDoc.modifiers();
      }
   }

   protected DocletOption[] getOptions()
   {
      return options;
   }

   private DocletOptionFlag optionNoNavBar = 
     new DocletOptionFlag("-nonavbar");

   private DocletOptionFlag optionNoTree = 
     new DocletOptionFlag("-notree");

   private DocletOptionFlag optionNoDeprecatedList = 
     new DocletOptionFlag("-nodeprecatedlist");

   private DocletOptionFlag optionNoIndex = 
     new DocletOptionFlag("-noindex");

   private DocletOptionFlag optionUse = 
     new DocletOptionFlag("-use");

   private DocletOptionFlag optionNoHelp = 
     new DocletOptionFlag("-nohelp");

   private DocletOptionFlag optionSplitIndex = 
     new DocletOptionFlag("-splitindex");

   private DocletOptionString optionHeader = 
     new DocletOptionString("-header");

   private DocletOptionString optionFooter = 
     new DocletOptionString("-footer");

   private DocletOptionString optionBottom = 
     new DocletOptionString("-bottom");

   private DocletOptionString optionWindowTitle = 
     new DocletOptionString("-windowtitle");

   private DocletOptionString optionDocTitle = 
     new DocletOptionString("-doctitle");

   private DocletOptionFile optionHelpFile = 
     new DocletOptionFile("-helpfile");

   private DocletOptionFile optionStylesheetFile = 
     new DocletOptionFile("-stylesheetfile");

   private DocletOptionFlag optionLinkSource = 
     new DocletOptionFlag("-linksource");

   private DocletOption optionLink = 
     new DocletOption("-link") {
        
        public int getLength()
        {
           return 2;
        }

        public boolean set(String[] optionArr)
        {
           externalDocSets.add(new ExternalDocSet(optionArr[1], null));
           return true;
        }
     };

   private DocletOption optionLinkOffline = 
     new DocletOption("-linkoffline") {
        
        public int getLength()
        {
           return 3;
        }

        public boolean set(String[] optionArr)
        {
           externalDocSets.add(new ExternalDocSet(optionArr[1], optionArr[2]));
           return true;
        }
     };

   private DocletOption[] options = 
      {
         optionNoNavBar,
         optionNoTree,
         optionNoDeprecatedList,
         optionNoIndex,
         optionNoHelp,
         optionUse,
         optionSplitIndex,
         optionHeader,
         optionFooter,
         optionBottom,
         optionHelpFile,
         optionStylesheetFile,
         optionWindowTitle,
         optionDocTitle,
         optionLinkSource,
         optionLink,
         optionLinkOffline,
      };

   static {
      setInstance(new HtmlDoclet());
   }

   private static String replaceDocRoot(HtmlPage output, String str)
   {
      return StringToolkit.replace(str, "{@docRoot}", output.getPathToRoot());
   }
}
