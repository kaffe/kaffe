/* StyleSheet.java -- 
   Copyright (C) 2005 Free Software Foundation, Inc.

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


package javax.swing.text.html;

import gnu.javax.swing.text.html.css.CSSColor;
import gnu.javax.swing.text.html.css.CSSParser;
import gnu.javax.swing.text.html.css.CSSParserCallback;
import gnu.javax.swing.text.html.css.FontSize;
import gnu.javax.swing.text.html.css.FontStyle;
import gnu.javax.swing.text.html.css.FontWeight;
import gnu.javax.swing.text.html.css.Length;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.io.IOException;
import java.io.Reader;
import java.io.Serializable;
import java.io.StringReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

import javax.swing.event.ChangeListener;
import javax.swing.text.AttributeSet;
import javax.swing.text.Element;
import javax.swing.text.MutableAttributeSet;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.View;


/**
 * This class adds support for defining the visual characteristics of HTML views
 * being rendered. This enables views to be customized by a look-and-feel, mulitple
 * views over the same model can be rendered differently. Each EditorPane has its 
 * own StyleSheet, but by default one sheet will be shared by all of the HTMLEditorKit
 * instances. An HTMLDocument can also have a StyleSheet, which holds specific CSS
 * specs. 
 * 
 *  In order for Views to store less state and therefore be more lightweight, 
 *  the StyleSheet can act as a factory for painters that handle some of the 
 *  rendering tasks. Since the StyleSheet may be used by views over multiple
 *  documents the HTML attributes don't effect the selector being used.
 *  
 *  The rules are stored as named styles, and other information is stored to 
 *  translate the context of an element to a rule.
 * 
 * @author Lillian Angel (langel@redhat.com)
 */
public class StyleSheet extends StyleContext
{

  /**
   * Parses CSS stylesheets using the parser in gnu/javax/swing/html/css.
   *
   * This is package private to avoid accessor methods.
   */
  class CSSStyleSheetParserCallback
    implements CSSParserCallback
  {
    /**
     * The selector for which the rules are currently parsed.
     */
    private String[] selector;

    /**
     * Called at the beginning of a statement.
     *
     * @param sel the selector
     */
    public void startStatement(String sel)
    {
      StringTokenizer tokens = new StringTokenizer(sel);
      selector = new String[tokens.countTokens()];
      for (int index = 0; tokens.hasMoreTokens(); index++)
        selector[index] = tokens.nextToken();
    }

    /**
     * Called at the end of a statement.
     */
    public void endStatement()
    {
      selector = null;
    }

    /**
     * Called when a declaration is parsed.
     *
     * @param property the property
     * @param value the value
     */
    public void declaration(String property, String value)
    {
      for (int i = 0; i < selector.length; i++)
        {
          CSSStyle style = (CSSStyle) css.get(selector[i]);
          if (style == null)
            {
              style = new CSSStyle();
              css.put(selector[i], style);
            }
          CSS.Attribute cssAtt = CSS.getAttribute(property);
          Object val = CSS.getValue(cssAtt, value);
          if (cssAtt != null)
            style.addAttribute(cssAtt, val);
          // else  // For debugging only.
          //   System.err.println("no mapping for: " + property);
        }
    }
    
  }

  /**
   * Represents a style that is defined by a CSS rule.
   */
  private class CSSStyle
    extends SimpleAttributeSet
    implements Style
  {

    public String getName()
    {
      // TODO: Implement this for correctness.
      return null;
    }

    public void addChangeListener(ChangeListener listener)
    {
      // TODO: Implement this for correctness.
    }

    public void removeChangeListener(ChangeListener listener)
    {
      // TODO: Implement this for correctness.
    }
    
  }

  /** The base URL */
  URL base;
  
  /** Base font size (int) */
  int baseFontSize;
  
  /** The style sheets stored. */
  StyleSheet[] styleSheet;

  /**
   * Maps element names (selectors) to AttributSet (the corresponding style
   * information).
   */
  HashMap css = new HashMap();

  /**
   * Maps selectors to their resolved styles.
   */
  private HashMap resolvedStyles;

  /**
   * Constructs a StyleSheet.
   */
  public StyleSheet()
  {
    super();
    baseFontSize = 4; // Default font size from CSS
    resolvedStyles = new HashMap();
  }

  /**
   * Gets the style used to render the given tag. The element represents the tag
   * and can be used to determine the nesting, where the attributes will differ
   * if there is nesting inside of elements.
   * 
   * @param t - the tag to translate to visual attributes
   * @param e - the element representing the tag
   * @return the set of CSS attributes to use to render the tag.
   */
  public Style getRule(HTML.Tag t, Element e)
  {
    // Create list of the element and all of its parents, starting
    // with the bottommost element.
    ArrayList path = new ArrayList();
    Element el;
    AttributeSet atts;
    for (el = e; el != null; el = el.getParentElement())
      path.add(el);

    // Create fully qualified selector.
    StringBuilder selector = new StringBuilder();
    int count = path.size();
    // We append the actual element after this loop.
    for (int i = count - 1; i > 0; i--)
      {
        el = (Element) path.get(i);
        atts = el.getAttributes();
        Object name = atts.getAttribute(StyleConstants.NameAttribute);
        selector.append(name.toString());
        if (atts.isDefined(HTML.Attribute.ID))
          {
            selector.append('#');
            selector.append(atts.getAttribute(HTML.Attribute.ID));
          }
        else if (atts.isDefined(HTML.Attribute.CLASS))
          {
            selector.append('.');
            selector.append(atts.getAttribute(HTML.Attribute.CLASS));
          }
        selector.append(' ');
      }
    selector.append(t.toString());
    el = (Element) path.get(0);
    atts = el.getAttributes();
    // For leaf elements, we have to fetch the tag specific attributes.
    if (el.isLeaf())
      {
        Object o = atts.getAttribute(t);
        if (o instanceof AttributeSet)
          atts = (AttributeSet) o;
        else
          atts = null;
      }
    if (atts != null)
      {
        if (atts.isDefined(HTML.Attribute.ID))
          {
            selector.append('#');
            selector.append(atts.getAttribute(HTML.Attribute.ID));
          }
        else if (atts.isDefined(HTML.Attribute.CLASS))
          {
            selector.append('.');
            selector.append(atts.getAttribute(HTML.Attribute.CLASS));
          }
      }
    return getResolvedStyle(selector.toString(), path, t);
  }

  /**
   * Fetches a resolved style. If there is no resolved style for the
   * specified selector, the resolve the style using
   * {@link #resolveStyle(String, List, HTML.Tag)}.
   * 
   * @param selector the selector for which to resolve the style
   * @param path the Element path, used in the resolving algorithm
   * @param tag the tag for which to resolve
   *
   * @return the resolved style
   */
  private Style getResolvedStyle(String selector, List path, HTML.Tag tag)
  {
    Style style = (Style) resolvedStyles.get(selector);
    if (style == null)
      style = resolveStyle(selector, path, tag);
    return style;
  }

  /**
   * Resolves a style. This creates arrays that hold the tag names,
   * class and id attributes and delegates the work to
   * {@link #resolveStyle(String, String[], String[], String[])}.
   *
   * @param selector the selector
   * @param path the Element path
   * @param tag the tag
   *
   * @return the resolved style
   */
  private Style resolveStyle(String selector, List path, HTML.Tag tag)
  {
    int count = path.size();
    String[] tags = new String[count];
    String[] ids = new String[count];
    String[] classes = new String[count];
    for (int i = 0; i < count; i++)
      {
        Element el = (Element) path.get(i);
        AttributeSet atts = el.getAttributes();
        if (i == 0 && el.isLeaf())
          {
            Object o = atts.getAttribute(tag);
            if (o instanceof AttributeSet)
              atts = (AttributeSet) o;
            else
              atts = null;
          }
        if (atts != null)
          {
            HTML.Tag t =
              (HTML.Tag) atts.getAttribute(StyleConstants.NameAttribute);
            if (t != null)
              tags[i] = t.toString();
            else
              tags[i] = null;
            if (atts.isDefined(HTML.Attribute.CLASS))
              classes[i] = atts.getAttribute(HTML.Attribute.CLASS).toString();
            else
              classes[i] = null;
            if (atts.isDefined(HTML.Attribute.ID))
              ids[i] = atts.getAttribute(HTML.Attribute.ID).toString();
            else
              ids[i] = null;
          }
        else
          {
            tags[i] = null;
            classes[i] = null;
            ids[i] = null;
          }
      }
    tags[0] = tag.toString();
    return resolveStyle(selector, tags, ids, classes);
  }

  /**
   * Performs style resolving.
   *
   * @param selector the selector
   * @param tags the tags
   * @param ids the corresponding ID attributes
   * @param classes the corresponding CLASS attributes
   *
   * @return the resolved style
   */
  private Style resolveStyle(String selector, String[] tags, String[] ids,
                             String[] classes)
  {
    // FIXME: This style resolver is not correct. But it works good enough for
    // the default.css.
    int count = tags.length;
    ArrayList styles = new ArrayList();
    for (int i = 0; i < count; i++)
      {
        Style style = (Style) css.get(tags[i]);
        if (style != null)
          styles.add(style);
        // FIXME: Handle ID and CLASS attributes.
      }
    Style[] styleArray = new Style[styles.size()];
    Style resolved = new MultiStyle(selector,
                                    (Style[]) styles.toArray(styleArray));
    resolvedStyles.put(selector, resolved);
    return resolved;
  }

  /**
   * Gets the rule that best matches the selector. selector is a space
   * separated String of element names. The attributes of the returned 
   * Style will change as rules are added and removed.
   * 
   * @param selector - the element names separated by spaces
   * @return the set of CSS attributes to use to render
   */
  public Style getRule(String selector)
  {
    // FIXME: This is a very rudimentary implementation. Should
    // be extended to conform to the CSS spec.
    return (Style) css.get(selector);
  }
  
  /**
   * Adds a set of rules to the sheet. The rules are expected to be in valid
   * CSS format. This is called as a result of parsing a <style> tag
   * 
   * @param rule - the rule to add to the sheet
   */
  public void addRule(String rule)
  {
    CSSStyleSheetParserCallback cb = new CSSStyleSheetParserCallback();
    // FIXME: Handle ref.
    StringReader in = new StringReader(rule);
    CSSParser parser = new CSSParser(in, cb);
    try
      {
        parser.parse();
      }
    catch (IOException ex)
      {
        // Shouldn't happen. And if, then we
        assert false;
      }
  }
  
  /**
   * Translates a CSS declaration into an AttributeSet. This is called
   * as a result of encountering an HTML style attribute.
   * 
   * @param decl - the declaration to get
   * @return the AttributeSet representing the declaration
   */
  public AttributeSet getDeclaration(String decl)
  {
    if (decl == null)
      return SimpleAttributeSet.EMPTY;
    // FIXME: Not implemented.
    return null;     
  }
  
  /**
   * Loads a set of rules that have been specified in terms of CSS grammar.
   * If there are any conflicts with existing rules, the new rule is added.
   * 
   * @param in - the stream to read the CSS grammar from.
   * @param ref - the reference URL. It is the location of the stream, it may
   * be null. All relative URLs specified in the stream will be based upon this
   * parameter.
   * @throws IOException - For any IO error while reading
   */
  public void loadRules(Reader in, URL ref)
    throws IOException
  {
    CSSStyleSheetParserCallback cb = new CSSStyleSheetParserCallback();
    // FIXME: Handle ref.
    CSSParser parser = new CSSParser(in, cb);
    parser.parse();
  }
  
  /**
   * Gets a set of attributes to use in the view. This is a set of
   * attributes that can be used for View.getAttributes
   * 
   * @param v - the view to get the set for
   * @return the AttributeSet to use in the view.
   */
  public AttributeSet getViewAttributes(View v)
  {
    return new ViewAttributeSet(v, this);
  }
  
  /**
   * Removes a style previously added.
   * 
   * @param nm - the name of the style to remove
   */
  public void removeStyle(String nm)
  {
    // FIXME: Not implemented.
    super.removeStyle(nm);
  }
  
  /**
   * Adds the rules from ss to those of the receiver. ss's rules will
   * override the old rules. An added StyleSheet will never override the rules
   * of the receiving style sheet.
   * 
   * @param ss - the new StyleSheet.
   */
  public void addStyleSheet(StyleSheet ss)
  {
    if (styleSheet == null)
      styleSheet = new StyleSheet[] {ss};
    else
      System.arraycopy(new StyleSheet[] {ss}, 0, styleSheet, 
                       styleSheet.length, 1);
  }
  
  /**
   * Removes ss from those of the receiver
   * 
   * @param ss - the StyleSheet to remove.
   */
  public void removeStyleSheet(StyleSheet ss)
  {
    if (styleSheet.length == 1 && styleSheet[0].equals(ss))
      styleSheet = null;
    else
      {
        for (int i = 0; i < styleSheet.length; i++)
          {
            StyleSheet curr = styleSheet[i];
            if (curr.equals(ss))
              {
                StyleSheet[] tmp = new StyleSheet[styleSheet.length - 1];
                if (i != 0 && i != (styleSheet.length - 1))
                  {
                    System.arraycopy(styleSheet, 0, tmp, 0, i);
                    System.arraycopy(styleSheet, i + 1, tmp, i,
                                     styleSheet.length - i - 1);
                  }
                else if (i == 0)
                  System.arraycopy(styleSheet, 1, tmp, 0, styleSheet.length - 1);
                else
                  System.arraycopy(styleSheet, 0, tmp, 0, styleSheet.length - 1);
                
                styleSheet = tmp;
                break;
              }
          }
      }
  }
  
  /**
   * Returns an array of the linked StyleSheets. May return null.
   * 
   * @return - An array of the linked StyleSheets.
   */
  public StyleSheet[] getStyleSheets()
  {
    return styleSheet;
  }
  
  /**
   * Imports a style sheet from the url. The rules are directly added to the
   * receiver.
   * 
   * @param url - the URL to import the StyleSheet from.
   */
  public void importStyleSheet(URL url)
  {
    // FIXME: Not implemented
  }
  
  /**
   * Sets the base url. All import statements that are relative, will be
   * relative to base.
   * 
   * @param base -
   *          the base URL.
   */
  public void setBase(URL base)
  {
    this.base = base;
  }
  
  /**
   * Gets the base url.
   * 
   * @return - the base
   */
  public URL getBase()
  {
    return base;
  }
  
  /**
   * Adds a CSS attribute to the given set.
   * 
   * @param attr - the attribute set
   * @param key - the attribute to add
   * @param value - the value of the key
   */
  public void addCSSAttribute(MutableAttributeSet attr, CSS.Attribute key,
                              String value)
  {
    Object val = CSS.getValue(key, value);
    attr.addAttribute(key, val);
  }
  
  /**
   * Adds a CSS attribute to the given set.
   * This method parses the value argument from HTML based on key. 
   * Returns true if it finds a valid value for the given key, 
   * and false otherwise.
   * 
   * @param attr - the attribute set
   * @param key - the attribute to add
   * @param value - the value of the key
   * @return true if a valid value was found.
   */
  public boolean addCSSAttributeFromHTML(MutableAttributeSet attr, CSS.Attribute key,
                                         String value)
  {
    // FIXME: Need to parse value from HTML based on key.
    attr.addAttribute(key, value);
    return attr.containsAttribute(key, value);
  }
  
  /**
   * Converts a set of HTML attributes to an equivalent set of CSS attributes.
   * 
   * @param htmlAttrSet - the set containing the HTML attributes.
   * @return the set of CSS attributes
   */
  public AttributeSet translateHTMLToCSS(AttributeSet htmlAttrSet)
  {
    // FIXME: Really convert HTML to CSS here.
    AttributeSet cssAttr = htmlAttrSet.copyAttributes();
    MutableAttributeSet cssStyle = addStyle(null, null);
    cssStyle.addAttributes(cssAttr);
    return cssStyle;
  }

  /**
   * Adds an attribute to the given set and returns a new set. This is implemented
   * to convert StyleConstants attributes to CSS before forwarding them to the superclass.
   * The StyleConstants attribute do not have corresponding CSS entry, the attribute
   * is stored (but will likely not be used).
   * 
   * @param old - the old set
   * @param key - the non-null attribute key
   * @param value - the attribute value
   * @return the updated set 
   */
  public AttributeSet addAttribute(AttributeSet old, Object key,
                                   Object value)
  {
    // FIXME: Not implemented.
    return super.addAttribute(old, key, value);       
  }
  
  /**
   * Adds a set of attributes to the element. If any of these attributes are
   * StyleConstants, they will be converted to CSS before forwarding to the 
   * superclass.
   * 
   * @param old - the old set
   * @param attr - the attributes to add
   * @return the updated attribute set
   */
  public AttributeSet addAttributes(AttributeSet old, AttributeSet attr)
  {
    // FIXME: Not implemented.
    return super.addAttributes(old, attr);           
  }
  
  /**
   * Removes an attribute from the set. If the attribute is a
   * StyleConstants, it will be converted to CSS before forwarding to the 
   * superclass.
   * 
   * @param old - the old set
   * @param key - the non-null attribute key
   * @return the updated set 
   */
  public AttributeSet removeAttribute(AttributeSet old, Object key)
  {
    // FIXME: Not implemented.
    return super.removeAttribute(old, key);    
  }
  
  /**
   * Removes an attribute from the set. If any of the attributes are
   * StyleConstants, they will be converted to CSS before forwarding to the 
   * superclass.
   * 
   * @param old - the old set
   * @param attrs - the attributes to remove
   * @return the updated set 
   */
  public AttributeSet removeAttributes(AttributeSet old, AttributeSet attrs)
  {
    // FIXME: Not implemented.
    return super.removeAttributes(old, attrs);    
  }
  
  /**
   * Removes a set of attributes for the element. If any of the attributes is a
   * StyleConstants, they will be converted to CSS before forwarding to the 
   * superclass.
   * 
   * @param old - the old attribute set
   * @param names - the attribute names
   * @return the update attribute set
   */
  public AttributeSet removeAttributes(AttributeSet old, Enumeration names)
  {
    // FIXME: Not implemented.
    return super.removeAttributes(old, names);        
  }
  
  /**
   * Creates a compact set of attributes that might be shared. This is a hook
   * for subclasses that want to change the behaviour of SmallAttributeSet.
   * 
   * @param a - the set of attributes to be represented in the compact form.
   * @return the set of attributes created
   */
  protected StyleContext.SmallAttributeSet createSmallAttributeSet(AttributeSet a)
  {
    return super.createSmallAttributeSet(a);     
  }
  
  /**
   * Creates a large set of attributes. This set is not shared. This is a hook
   * for subclasses that want to change the behaviour of the larger attribute
   * storage format.
   * 
   * @param a - the set of attributes to be represented in the larger form.
   * @return the large set of attributes.
   */
  protected MutableAttributeSet createLargeAttributeSet(AttributeSet a)
  {
    return super.createLargeAttributeSet(a);     
  }
  
  /**
   * Gets the font to use for the given set.
   * 
   * @param a - the set to get the font for.
   * @return the font for the set
   */
  public Font getFont(AttributeSet a)
  {
    FontSize size = (FontSize) a.getAttribute(CSS.Attribute.FONT_SIZE);
    int realSize = 12;
    if (size != null)
      realSize = size.getValue();

    // Decrement size for subscript and superscript.
    Object valign = a.getAttribute(CSS.Attribute.VERTICAL_ALIGN);
    if (valign != null)
      {
        String v = valign.toString();
        if (v.contains("sup") || v.contains("sub"))
          realSize -= 2;
      }

    // TODO: Convert font family.
    String family = "SansSerif";

    int style = Font.PLAIN;
    FontWeight weight = (FontWeight) a.getAttribute(CSS.Attribute.FONT_WEIGHT);
    if (weight != null)
      style |= weight.getValue();
    FontStyle fStyle = (FontStyle) a.getAttribute(CSS.Attribute.FONT_STYLE);
    if (fStyle != null)
      style |= fStyle.getValue();
    return new Font(family, style, realSize);
  }
  
  /**
   * Takes a set of attributes and turns it into a foreground
   * color specification. This is used to specify things like, brigher, more hue
   * etc.
   * 
   * @param a - the set to get the foreground color for
   * @return the foreground color for the set
   */
  public Color getForeground(AttributeSet a)
  {
    CSSColor c = (CSSColor) a.getAttribute(CSS.Attribute.COLOR);
    Color color = null;
    if (c != null)
      color = c.getValue();
    return color;     
  }
  
  /**
   * Takes a set of attributes and turns it into a background
   * color specification. This is used to specify things like, brigher, more hue
   * etc.
   * 
   * @param a - the set to get the background color for
   * @return the background color for the set
   */
  public Color getBackground(AttributeSet a)
  {
    CSSColor c = (CSSColor) a.getAttribute(CSS.Attribute.BACKGROUND_COLOR);
    Color color = null;
    if (c != null)
      color = c.getValue();
    return color;     
  }
  
  /**
   * Gets the box formatter to use for the given set of CSS attributes.
   * 
   * @param a - the given set
   * @return the box formatter
   */
  public BoxPainter getBoxPainter(AttributeSet a)
  {
    return new BoxPainter(a);     
  }
  
  /**
   * Gets the list formatter to use for the given set of CSS attributes.
   * 
   * @param a - the given set
   * @return the list formatter
   */
  public ListPainter getListPainter(AttributeSet a)
  {
    return new ListPainter(a);         
  }
  
  /**
   * Sets the base font size between 1 and 7.
   * 
   * @param sz - the new font size for the base.
   */
  public void setBaseFontSize(int sz)
  {
    if (sz <= 7 && sz >= 1)
      baseFontSize = sz;
  }
  
  /**
   * Sets the base font size from the String. It can either identify
   * a specific font size (between 1 and 7) or identify a relative
   * font size such as +1 or -2.
   * 
   * @param size - the new font size as a String.
   */
  public void setBaseFontSize(String size)
  {
    size.trim();
    int temp = 0;
    try
      {
        if (size.length() == 2)
          {
            int i = new Integer(size.substring(1)).intValue();
            if (size.startsWith("+"))
              temp = baseFontSize + i;
            else if (size.startsWith("-"))
              temp = baseFontSize - i;
          }
        else if (size.length() == 1)
          temp = new Integer(size.substring(0)).intValue();

        if (temp <= 7 && temp >= 1)
          baseFontSize = temp;
      }
    catch (NumberFormatException nfe)
      {
        // Do nothing here
      }
  }
  
  /**
   * TODO
   * 
   * @param pt - TODO
   * @return TODO
   */
  public static int getIndexOfSize(float pt)
  {
    // FIXME: Not implemented.
    return 0;
  }
  
  /**
   * Gets the point size, given a size index.
   * 
   * @param index - the size index
   * @return the point size.
   */
  public float getPointSize(int index)
  {
    // FIXME: Not implemented.
    return 0;    
  }
  
  /**
   * Given the string of the size, returns the point size value.
   * 
   * @param size - the string representation of the size.
   * @return - the point size value.
   */
  public float getPointSize(String size)
  {
    // FIXME: Not implemented.
    return 0;    
  }
  
  /**
   * Convert the color string represenation into java.awt.Color. The valid
   * values are like "aqua" , "#00FFFF" or "rgb(1,6,44)".
   * 
   * @param colorName the color to convert.
   * @return the matching java.awt.color
   */
  public Color stringToColor(String colorName)
  {
    return CSSColor.convertValue(colorName);
  }
  
  /**
   * This class carries out some of the duties of CSS formatting. This enables views
   * to present the CSS formatting while not knowing how the CSS values are cached.
   * 
   * This object is reponsible for the insets of a View and making sure
   * the background is maintained according to the CSS attributes.
   * 
   * @author Lillian Angel (langel@redhat.com)
   */
  public static class BoxPainter extends Object implements Serializable
  {

    private float leftInset;
    private float rightInset;
    private float topInset;
    private float bottomInset;

    /**
     * Package-private constructor.
     * 
     * @param as - AttributeSet for painter
     */
    BoxPainter(AttributeSet as)
    {
      Length l = (Length) as.getAttribute(CSS.Attribute.MARGIN_LEFT);
      if (l != null)
        leftInset = l.getValue();
      l = (Length) as.getAttribute(CSS.Attribute.MARGIN_RIGHT);
      if (l != null)
        rightInset = l.getValue();
      l = (Length) as.getAttribute(CSS.Attribute.MARGIN_TOP);
      if (l != null)
        topInset = l.getValue();
      l = (Length) as.getAttribute(CSS.Attribute.MARGIN_BOTTOM);
      if (l != null)
        bottomInset = l.getValue();
    }
    
    
    /**
     * Gets the inset needed on a given side to account for the margin, border
     * and padding.
     * 
     * @param size - the size of the box to get the inset for. View.TOP, View.LEFT,
     * View.BOTTOM or View.RIGHT.
     * @param v - the view making the request. This is used to get the AttributeSet,
     * amd may be used to resolve percentage arguments.
     * @return the inset
     * @throws IllegalArgumentException - for an invalid direction.
     */
    public float getInset(int size, View v)
    {
      float inset;
      switch (size)
        {
        case View.TOP:
          inset = topInset;
          break;
        case View.BOTTOM:
          inset = bottomInset;
          break;
        case View.LEFT:
          inset = leftInset;
          break;
        case View.RIGHT:
          inset = rightInset;
          break;
        default:
          inset = 0.0F;
      }
      return inset;
    }
    
    /**
     * Paints the CSS box according to the attributes given. This should
     * paint the border, padding and background.
     * 
     * @param g - the graphics configuration
     * @param x - the x coordinate
     * @param y - the y coordinate
     * @param w - the width of the allocated area
     * @param h - the height of the allocated area
     * @param v - the view making the request
     */
    public void paint(Graphics g, float x, float y, float w, float h, View v)
    {
      // FIXME: Not implemented.
    }
  }
  
  /**
   * This class carries out some of the CSS list formatting duties. Implementations
   * of this class enable views to present the CSS formatting while not knowing anything
   * about how the CSS values are being cached.
   * 
   * @author Lillian Angel (langel@redhat.com)
   */
  public static class ListPainter extends Object implements Serializable
  {
    
    /**
     * Attribute set for painter
     */
    AttributeSet as;
    
    /**
     * Package-private constructor.
     * 
     * @param as - AttributeSet for painter
     */
    ListPainter(AttributeSet as)
    {
      this.as = as;
    }
    
    /**
     * Paints the CSS list decoration according to the attributes given.
     * 
     * @param g - the graphics configuration
     * @param x - the x coordinate
     * @param y - the y coordinate
     * @param w - the width of the allocated area
     * @param h - the height of the allocated area
     * @param v - the view making the request
     * @param item - the list item to be painted >=0.
     */
    public void paint(Graphics g, float x, float y, float w, float h, View v,
                      int item)
    {
      // FIXME: Not implemented.
    }
  }

}
