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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;

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

   public HtmlPage(File file, String pathToRoot, String encoding)
      throws IOException
   {
      this(file, pathToRoot, encoding, DOCTYPE_STRICT);
   }

   public HtmlPage(File file, String pathToRoot, String encoding, String docType)
      throws IOException
   {
      OutputStream fileOut = new FileOutputStream(file);
      Writer writer;
      if (null != encoding) {
         writer = new OutputStreamWriter(fileOut,
                                         encoding);
      }
      else {
         writer = new OutputStreamWriter(fileOut);
      }
      this.out = new PrintWriter(new BufferedWriter(writer));
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
      String[] divAttributeNames = cssClass.getAttributeNames();
      String[] divAttributeValues = cssClass.getAttributeValues();
      if (null == divAttributeNames) {
         divAttributeNames = new String[0];
      }
      if (null == divAttributeValues) {
         divAttributeValues = new String[0];
      }

      String[] attributeNames = new String[1 + divAttributeNames.length];
      String[] attributeValues = new String[1 + divAttributeValues.length];
      
      System.arraycopy(divAttributeNames, 0, attributeNames, 1, divAttributeNames.length);
      System.arraycopy(divAttributeValues, 0, attributeValues, 1, divAttributeNames.length);

      beginElement(cssClass.getDivElementName(), attributeNames, attributeValues);
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

   public void beginPage(String title, String charset)
   {
      print("<?xml version=\"1.0\" encoding=\"" + charset + "\"?>\n");
      print(docType);
      print("<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">");
      beginElement("head");
      beginElement("title");
      print(title);
      endElement("title");
      atomicElement("meta", 
                    new String[] { "http-equiv", "content" },
                    new String[] { "Content-Type", "text/html; charset=" + charset });
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
