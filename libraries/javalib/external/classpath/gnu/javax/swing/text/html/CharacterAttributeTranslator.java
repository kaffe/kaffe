/* CharacterAttributeTranslator.java -- 
   Copyright (C) 2006  Free Software Foundation, Inc.

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
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package gnu.javax.swing.text.html;

import java.awt.Color;
import java.util.HashMap;
import javax.swing.text.AbstractDocument;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Element;
import javax.swing.text.ElementIterator;
import javax.swing.text.GapContent;
import javax.swing.text.MutableAttributeSet;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.html.HTML.Tag;

/**
 * This is a small utility class to translate HTML character attributes to
 * Swing StyleConstants
 */
public class CharacterAttributeTranslator
{
  private static final HashMap colorMap = new HashMap();
  static 
  {
    colorMap.put("aqua" , "#00FFFF");
    colorMap.put("blue" , "#0000FF");
    colorMap.put("black", "#000000");
    colorMap.put("fuchsia" , "#FF00FF");
    colorMap.put("gray" , "#808080");
    colorMap.put("green" , "#008000");
    colorMap.put("lime" , "#00FF00");
    colorMap.put("maroon" , "#800000");
    colorMap.put("navy" , "#000080");
    colorMap.put("olive" , "#808000");
    colorMap.put("purple" , "#800080");
    colorMap.put("red" , "#FF0000");
    colorMap.put("silver" , "#C0C0C0");
    colorMap.put("teal" , "#008080");
    colorMap.put("white" , "#FFFFFF");
    colorMap.put("yellow" , "#FFFF00");
  };

  private static Color getColor(String s)
  {
    String s2 = (String)colorMap.get(s.toLowerCase());
    if( s2 == null )
      s2 = s;
    try
      {
	return Color.decode(s2);
      }
    catch(NumberFormatException nfe)
      {
	return null;
      }
  }
  
  public static boolean translateTag(MutableAttributeSet charAttr, 
				     Tag t, MutableAttributeSet a)
  {
    if(t == Tag.FONT)
      {
	if(a.getAttribute("color") != null)
	  {
	    Color c = getColor(""+a.getAttribute("color"));
	    if( c == null )
	      return false;
	    charAttr.addAttribute(StyleConstants.Foreground, c);
	    return true;
	  }

	if(a.getAttribute("size") != null)
	  {
	    // FIXME
	    //	    charAttr.addAttribute(StyleConstants.FontSize, 
	    //				  new java.lang.Integer(72));
	    return true;
	  }
      }

    if( t == Tag.B )
      {
	charAttr.addAttribute(StyleConstants.Bold, new Boolean(true));
	return true;
      }

    if( t == Tag.I )
      {
	charAttr.addAttribute(StyleConstants.Italic, new Boolean(true));
	return true;
      }

    if( t == Tag.U )
      {
	charAttr.addAttribute(StyleConstants.Underline, new Boolean(true));
	return true;
      }

    if( t == Tag.STRIKE )
      {
	charAttr.addAttribute(StyleConstants.StrikeThrough, new Boolean(true));
	return true;
      }

    if( t == Tag.SUP )
      {
	charAttr.addAttribute(StyleConstants.Superscript, new Boolean(true));
	return true;
      }

    if( t == Tag.SUB )
      {
	charAttr.addAttribute(StyleConstants.Subscript, new Boolean(true));
	return true;
      }
    return false;
  }
}
