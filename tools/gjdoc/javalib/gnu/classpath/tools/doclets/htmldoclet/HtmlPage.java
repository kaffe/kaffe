/* gnu.classpath.tools.doclets.htmldoclet.HtmlPage
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

import java.io.File;
import java.io.IOException;
import java.io.BufferedWriter;
import java.io.PrintWriter;
import java.io.FileWriter;

import com.sun.javadoc.Tag;

/**
 *  Allows outputting an HTML document without having to build the
 *  document tree in-memory.
 */
public class HtmlPage 
{
   private PrintWriter out;
   private String pathToRoot;
   private String docType;

   public static final String DOCTYPE_FRAMESET = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">";
   public static final String DOCTYPE_STRICT = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";

   public HtmlPage(File file, String pathToRoot)
      throws IOException
   {
      this(file, pathToRoot, DOCTYPE_STRICT);
   }

   public HtmlPage(File file, String pathToRoot, String docType)
      throws IOException
   {
      this.out = new PrintWriter(new BufferedWriter(new FileWriter(file)));
      this.pathToRoot = pathToRoot;
      this.docType = docType;
   }

   public void beginElement(String elementName)
   {
      out.print('<');
      out.print(elementName);
      out.print('>');
   }

   public void beginElement(String elementName, String attributeName, String attributeValue)
   {
      out.print('<');
      out.print(elementName);
      out.print(' ');
      out.print(attributeName);
      out.print('=');
      out.print('\"');
      out.print(attributeValue);
      out.print('\"');
      out.print('>');
   }

   public void beginElement(String elementName, String[] attributeNames, String[] attributeValues)
   {
      out.print('<');
      out.print(elementName);
      for (int i=0; i<attributeNames.length; ++i) {
         if (null != attributeValues[i]) {
            out.print(' ');
            out.print(attributeNames[i]);
            out.print('=');
            out.print('\"');
            out.print(attributeValues[i]);
            out.print('\"');
         }
      }
      out.print('>');
   }

   public void atomicElement(String elementName)
   {
      out.print('<');
      out.print(elementName);
      out.print("/>");
   }

   public void atomicElement(String elementName, String attributeName, String attributeValue)
   {
      out.print('<');
      out.print(elementName);
      out.print(' ');
      out.print(attributeName);
      out.print('=');
      out.print('\"');
      out.print(attributeValue);
      out.print('\"');
      out.print("/>");
   }

   public void atomicElement(String elementName, String[] attributeNames, String[] attributeValues)
   {
      out.print('<');
      out.print(elementName);
      for (int i=0; i<attributeNames.length; ++i) {
         if (null != attributeValues[i]) {
            out.print(' ');
            out.print(attributeNames[i]);
            out.print('=');
            out.print('\"');
            out.print(attributeValues[i]);
            out.print('\"');
         }
      }
      out.print("/>");
   }


   public void endElement(String elementName)
   {
      out.print("</");
      out.print(elementName);
      out.print('>');
   }

   
   public void beginDiv(CssClass cssClass)
   {
      beginElement(cssClass.getDivElementName(), "class", cssClass.getName());
   }

   public void endDiv(CssClass cssClass)
   {
      endElement(cssClass.getDivElementName());
   }

   public void beginSpan(CssClass cssClass)
   {
      beginElement(cssClass.getSpanElementName(), "class", cssClass.getName());
   }

   public void endSpan(CssClass cssClass)
   {
      endElement(cssClass.getSpanElementName());
   }

   public void hr()
   {
      atomicElement("hr");
   }
   
   public void print(String text)
   {
      out.print(text);
   }

   public void div(CssClass cssClass, String contents)
   {
      beginDiv(cssClass);
      print(contents);
      endDiv(cssClass);
   }

   public void beginPage(String title)
   {
      print("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
      print(docType);
      print("<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">");
      beginElement("head");
      beginElement("title");
      print(title);
      endElement("title");
      endElement("head");
   }

   public void endPage()
   {
      endElement("html");
   }

   public void close()
   {
      out.close();
   }

   public void beginTable(CssClass cssClass)
   {
      beginElement("table", "class", cssClass.getName());
   }

   public void beginRow()
   {
      beginElement("tr");
   }

   public void beginRow(String attribute, String value)
   {
      beginElement("tr", attribute, value);
   }

   public void beginCell()
   {
      beginElement("td");
   }

   public void beginCell(String attribute, String value)
   {
      beginElement("td", attribute, value);
   }

   public void beginCell(CssClass cssClass)
   {
      beginElement("td", "class", cssClass.getName());
   }

   public void endCell()
   {
      endElement("td");
   }

   public void cell(CssClass cssClass, String contents)
   {
      beginCell(cssClass);
      print(contents);
      endCell();
   }

   public void endRow()
   {
      endElement("tr");
   }

   public void rowDiv(CssClass cssClass, String contents)
   {
      beginRow();
      beginCell("colspan", "2");
      beginDiv(cssClass);
      print(contents);
      endDiv(cssClass);
      endCell();
      endRow();
   }

   public void endTable()
   {
      endElement("table");
   }

   public void beginAnchor(String href)
   {
      beginElement("a", "href", href);
   }

   public void beginAnchor(String href, String title, String target)
   {
      beginElement("a", 
                   new String[] { "href", "title", "target" },
                   new String[] { href, title, target });
   }

   public void endAnchor()
   {
      endElement("a");
   }

   public void anchor(String href, String label)
   {
      beginAnchor(href);
      print(label);
      endAnchor();
   }

   public void anchorName(String name)
   {
      atomicElement("a", "name", name);
   }

   public String getPathToRoot()
   {
      return pathToRoot;
   }

   public void beginBody()
   {
      beginElement("body");
   }

   public void endBody()
   {
      endElement("body");
   }
}
