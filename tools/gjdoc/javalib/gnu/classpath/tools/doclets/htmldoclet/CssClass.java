/* gnu.classpath.tools.doclets.htmldoclet.CssClass
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

/**
 *  Represents a CSS (Cascading Stylesheet) class. Supports
 *  substituting <code>div</code> and <code>span</code> tags by more
 *  specialized HTML tags.
 */
public class CssClass
{
   public static final CssClass OVERVIEW_TITLE = new CssClass("overview title", "h1");
   public static final CssClass OVERVIEW_SUMMARY = new CssClass("overview summary");
   public static final CssClass OVERVIEW_SUMMARY_LEFT = new CssClass("overview summary left");
   public static final CssClass OVERVIEW_SUMMARY_RIGHT = new CssClass("overview summary right");

   public static final CssClass DEPRECATION_TITLE = new CssClass("deprecation title", "h1");
   public static final CssClass DEPRECATION_SUMMARY = new CssClass("deprecation summary");
   public static final CssClass DEPRECATION_SUMMARY_LEFT = new CssClass("deprecation summary left");
   public static final CssClass DEPRECATION_SUMMARY_RIGHT = new CssClass("deprecation summary right");
   public static final CssClass DEPRECATION_SUMMARY_DESCRIPTION = new CssClass("deprecation summary description");
   public static final CssClass DEPRECATION_TOC = new CssClass("deprecation toc outer");
   public static final CssClass DEPRECATION_TOC_HEADER = new CssClass("deprecation toc header", "h3");
   public static final CssClass DEPRECATION_TOC_LIST = new CssClass("deprecation toc list", "ul");
   public static final CssClass DEPRECATION_TOC_ENTRY = new CssClass("deprecation toc entry", "li");

   public static final CssClass PACKAGE_TITLE = new CssClass("package title", "h1");
   public static final CssClass PACKAGE_SUMMARY = new CssClass("package summary");
   public static final CssClass PACKAGE_SUMMARY_LEFT = new CssClass("package summary left");
   public static final CssClass PACKAGE_SUMMARY_RIGHT = new CssClass("package summary right");
   public static final CssClass PACKAGE_DESCRIPTION_TOP = new CssClass("package description top");
   public static final CssClass PACKAGE_DESCRIPTION_FULL = new CssClass("package description full");
   public static final CssClass PACKAGE_TREE_TITLE = new CssClass("package tree title", "h1");
   public static final CssClass PACKAGE_TREE_SECTION_TITLE = new CssClass("package tree section title", "h2");

   public static final CssClass PACKAGE_MENU_LIST = new CssClass("package menu list", "dd");
   public static final CssClass PACKAGE_MENU_ENTRY = new CssClass("package menu entry", "dt");
   public static final CssClass PACKAGE_MENU_TITLE = new CssClass("package menu title", "h4");

   public static final CssClass CLASS_MENU_LIST = new CssClass("package menu list", "dd");
   public static final CssClass CLASS_MENU_ENTRY = new CssClass("package menu entry", "dt");
   public static final CssClass CLASS_MENU_TITLE = new CssClass("package menu title", "h4");

   public static final CssClass INDEX_TITLE = new CssClass("index title", "h1");
   public static final CssClass INDEX_CATEGORY = new CssClass("index category");
   public static final CssClass INDEX_CATEGORY_HEADER = new CssClass("index category header", "h2");
   public static final CssClass INDEX_ENTRY = new CssClass("index entry");
   public static final CssClass INDEX_ENTRY_DESCRIPTION = new CssClass("index entry description");
   public static final CssClass INDEX_LETTERS = new CssClass("index letters");
   public static final CssClass INDEX_LETTER = new CssClass("index letter");
   public static final CssClass INDEX_LETTER_SPACER = new CssClass("index letter spacer");
   public static final CssClass CLASS_TITLE = new CssClass("class title outer");
   public static final CssClass CLASS_TITLE_PACKAGE = new CssClass("class title-package", "h3");
   public static final CssClass CLASS_TITLE_CLASS = new CssClass("class title-class", "h1");
   public static final CssClass CLASS_SUBCLASSES = new CssClass("class subclasses");
   public static final CssClass CLASS_SUBCLASSES_HEADER = new CssClass("class subclasses header", "h4");
   public static final CssClass CLASS_SYNOPSIS = new CssClass("class synopsis outer");
   public static final CssClass CLASS_SYNOPSIS_DECLARATION = new CssClass("class synopsis declaration");
   public static final CssClass CLASS_SYNOPSIS_SUPERCLASS = new CssClass("class synopsis superclass");
   public static final CssClass CLASS_SYNOPSIS_IMPLEMENTS = new CssClass("class synopsis implements");
   public static final CssClass CLASS_DESCRIPTION = new CssClass("class description");
   public static final CssClass CLASS_SUMMARY = new CssClass("class summary");
   public static final CssClass CLASS_SUMMARY_LEFT = new CssClass("class summary left");
   public static final CssClass CLASS_SUMMARY_RIGHT = new CssClass("class summary right");
   public static final CssClass USAGE_TITLE = new CssClass("usage title", "h1");
   public static final CssClass USAGE_PACKAGE_TITLE = new CssClass("usage package title", "h2");
   public static final CssClass USAGE_USAGE_TITLE = new CssClass("usage usage title", "h3");
   public static final CssClass USAGE_SUMMARY = new CssClass("usage summary");
   public static final CssClass USAGE_SUMMARY_LEFT = new CssClass("usage summary left");
   public static final CssClass USAGE_SUMMARY_RIGHT = new CssClass("usage summary right");
   public static final CssClass MEMBER_DETAIL = new CssClass("member detail outer");
   public static final CssClass MEMBER_DETAIL_NAME = new CssClass("member detail name", "h3");
   public static final CssClass MEMBER_DETAIL_SYNOPSIS = new CssClass("member detail synopsis", "p");
   public static final CssClass MEMBER_DETAIL_DESCRIPTION = new CssClass("member detail description");
   public static final CssClass MEMBER_DETAIL_SPECIFIED_BY_LIST = new CssClass("member detail specified by list");
   public static final CssClass MEMBER_DETAIL_SPECIFIED_BY_HEADER = new CssClass("member detail specified by header", "h4");
   public static final CssClass MEMBER_DETAIL_SPECIFIED_BY_ITEM = new CssClass("member detail specified by item");
   public static final CssClass MEMBER_DETAIL_OVERRIDDEN_LIST = new CssClass("member detail overridden list");
   public static final CssClass MEMBER_DETAIL_OVERRIDDEN_HEADER = new CssClass("member detail overridden header", "h4");
   public static final CssClass MEMBER_DETAIL_OVERRIDDEN_ITEM = new CssClass("member detail overridden item");
   public static final CssClass MEMBER_DETAIL_PARAMETER_LIST = new CssClass("member detail parameter list");
   public static final CssClass MEMBER_DETAIL_PARAMETER_HEADER = new CssClass("member detail parameter header", "h4");
   public static final CssClass MEMBER_DETAIL_PARAMETER_ITEM = new CssClass("member detail parameter item");
   public static final CssClass MEMBER_DETAIL_RETURN_LIST = new CssClass("member detail return list");
   public static final CssClass MEMBER_DETAIL_RETURN_HEADER = new CssClass("member detail return header", "h4");
   public static final CssClass MEMBER_DETAIL_RETURN_ITEM = new CssClass("member detail return item");
   public static final CssClass MEMBER_DETAIL_THROWN_LIST = new CssClass("member detail thrown list");
   public static final CssClass MEMBER_DETAIL_THROWN_HEADER = new CssClass("member detail thrown header", "h4");
   public static final CssClass MEMBER_DETAIL_THROWN_ITEM = new CssClass("member detail thrown item");

   public static final CssClass TABLE_HEADER = new CssClass("table header", "h4");

   public static final CssClass NAVBAR_TOP = new CssClass("navbar div top");
   public static final CssClass NAVBAR_BOTTOM = new CssClass("navbar div bottom");
   public static final CssClass NAVBAR_BOTTOM_SPACER = new CssClass("navbar div bottom spacer", "p");
   public static final CssClass NAVBAR_ITEM_ENABLED = new CssClass("navbar item enabled");
   public static final CssClass NAVBAR_ITEM_DISABLED = new CssClass("navbar item disabled");
   public static final CssClass NAVBAR_ITEM_ACTIVE = new CssClass("navbar item active");

   private String name;
   private String elementName;

   private CssClass(String name)
   {
      this(name, null);
   }

   private CssClass(String name, String elementName)
   {
      this.name = name;
      this.elementName = elementName;
   }

   public String getSpanElementName()
   {
      if (null != this.elementName) {
         return this.elementName;
      }
      else {
         return "span";
      }
   }

   public String getDivElementName()
   {
      if (null != this.elementName) {
         return this.elementName;
      }
      else {
         return "div";
      }
   }

   public String getName()
   {
      return name;
   }
}
