/* RuleBasedCollator.java -- Concrete Collator Class
   Copyright (C) 1998, 1999, 2000, 2001, 2003  Free Software Foundation, Inc.

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
02111-1307 USA.

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


package java.text;

import java.util.Vector;

/* Written using "Java Class Libraries", 2nd edition, plus online
 * API docs for JDK 1.2 from http://www.javasoft.com.
 * Status: Believed complete and correct
 */

/**
 * This class is a concrete subclass of <code>Collator</code> suitable
 * for string collation in a wide variety of languages.  An instance of
 * this class is normally returned by the <code>getInstance</code> method
 * of <code>Collator</code> with rules predefined for the requested
 * locale.  However, an instance of this class can be created manually
 * with any desired rules.
 * <p>
 * Rules take the form of a <code>String</code> with the following syntax
 * <ul>
 * <li> Modifier: '@' 
 * <li> Relation: '&lt;' | ';' | ',' | '=' : <text>
 * <li> Reset: '&amp;' : <text>
 * </ul>
 * The modifier character indicates that accents sort backward as is the
 * case with French.  The relational operators specify how the text 
 * argument relates to the previous term.  The relation characters have
 * the following meanings:
 * <ul>
 * <li>'&lt;' - The text argument is greater than the prior term at the primary
 * difference level.
 * <li>';' - The text argument is greater than the prior term at the secondary
 * difference level.
 * <li>',' - The text argument is greater than the prior term at the tertiary
 * difference level.
 * <li>'=' - The text argument is equal to the prior term
 * </ul>
 * <p>
 * As for the text argument itself, this is any sequence of Unicode
 * characters not in the following ranges: 0x0009-0x000D, 0x0020-0x002F,
 * 0x003A-0x0040, 0x005B-0x0060, and 0x007B-0x007E. If these characters are 
 * desired, they must be enclosed in single quotes.  If any whitespace is 
 * encountered, it is ignored.  (For example, "a b" is equal to "ab").  
 * <p>
 * The reset operation inserts the following rule at the point where the
 * text argument to it exists in the previously declared rule string.  This
 * makes it easy to add new rules to an existing string by simply including
 * them in a reset sequence at the end.  Note that the text argument, or
 * at least the first character of it, must be present somewhere in the
 * previously declared rules in order to be inserted properly.  If this
 * is not satisfied, a <code>ParseException</code> will be thrown. 
 * <p>
 * This system of configuring <code>RuleBasedCollator</code> is needlessly
 * complex and the people at Taligent who developed it (along with the folks
 * at Sun who accepted it into the Java standard library) deserve a slow
 * and agonizing death.
 * <p>
 * Here are a couple of example of rule strings:
 * <p>
 * "&lt; a &lt; b &lt; c" - This string says that a is greater than b which is 
 * greater than c, with all differences being primary differences.
 * <p>
 * "&lt; a,A &lt; b,B &lt; c,C" - This string says that 'A' is greater than 'a' with
 * a tertiary strength comparison.  Both 'b' and 'B' are greater than 'a' and
 * 'A' during a primary strength comparison.  But 'B' is greater than 'b'
 * under a tertiary strength comparison.
 * <p>
 * "&lt; a &lt; c &amp; a &lt; b " - This sequence is identical in function to the 
 * "&lt; a &lt; b &lt; c" rule string above.  The '&amp;' reset symbol indicates that
 * the rule "&lt; b" is to be inserted after the text argument "a" in the
 * previous rule string segment.
 * <p>
 * "&lt; a &lt; b &amp; y &lt; z" - This is an error.  The character 'y' does not appear
 * anywhere in the previous rule string segment so the rule following the
 * reset rule cannot be inserted.
 * <p>
 * For a description of the various comparison strength types, see the
 * documentation for the <code>Collator</code> class.
 * <p>
 * As an additional complication to this already overly complex rule scheme,
 * if any characters precede the first rule, these characters are considered
 * ignorable.  They will be treated as if they did not exist during 
 * comparisons.  For example, "- &lt; a &lt; b ..." would make '-' an ignorable
 * character such that the strings "high-tech" and "hightech" would
 * be considered identical.
 * <p>
 * A <code>ParseException</code> will be thrown for any of the following
 * conditions:
 * <ul>
 * <li>Unquoted punctuation characters in a text argument.
 * <li>A relational or reset operator not followed by a text argument
 * <li>A reset operator where the text argument is not present in
 * the previous rule string section.
 * </ul>
 *
 * @author Aaron M. Renn <arenn@urbanophile.com>
 * @author Tom Tromey <tromey@cygnus.com>
 * @date March 25, 1999
 */
public class RuleBasedCollator extends Collator
{
  class CollationElement
  {
    String char_seq;
    int primary;
    short secondary;
    short tertiary;

    CollationElement(String char_seq, int primary, short secondary, short tertiary)
    {
      this.char_seq = char_seq;
      this.primary = primary;
      this.secondary = secondary;
      this.tertiary = tertiary;
    }
 
  } // inner class CollationElement

  /**
   * This the the original rule string.
   */
  private String rules;

  /**
   * This is the table of collation element values
   */
  private Object[] ce_table;

  /**
   * This method initializes a new instance of <code>RuleBasedCollator</code>
   * with the specified collation rules.  Note that an application normally
   * obtains an instance of <code>RuleBasedCollator</code> by calling the
   * <code>getInstance</code> method of <code>Collator</code>.  That method
   * automatically loads the proper set of rules for the desired locale.
   *
   * @param rules The collation rule string.
   *
   * @exception ParseException If the rule string contains syntax errors.
   */
  public RuleBasedCollator(String rules) throws ParseException
  {
    this.rules = rules;

    if (rules.equals(""))
      throw new IllegalArgumentException("Empty rule set");

    Vector v = new Vector();
    boolean ignore_chars = true;
    int primary_seq = 0;
    short secondary_seq = 0;
    short tertiary_seq = 0;
    StringBuffer sb = new StringBuffer("");
    for (int i = 0; i < rules.length(); i++)
      {
        char c = rules.charAt(i);

        // Check if it is a whitespace character
        if (((c >= 0x09) && (c <= 0x0D)) || (c == 0x20))
          continue;

        // Primary difference
        if (c == '<')
          {
            ignore_chars = false;
            CollationElement e = new CollationElement(sb.toString(), primary_seq,
                                                      secondary_seq,
                                                      tertiary_seq);
            secondary_seq = 0;
            tertiary_seq = 0;
            ++primary_seq;

            v.add(e);
            sb.setLength(0);
            continue;
          }

        // Secondary difference
        if (c == ';')
          {
            if (primary_seq == 0)
              throw new ParseException(rules, i);

            CollationElement e = new CollationElement(sb.toString(), primary_seq,
                                                      secondary_seq,
                                                      tertiary_seq);
            ++secondary_seq;
            tertiary_seq = 0;

            v.add(e);
            sb.setLength(0);
            continue;
          }

        // Tertiary difference
        if (c == ',')
          {
            if (primary_seq == 0)
              throw new ParseException(rules, i);

            CollationElement e = new CollationElement(sb.toString(), primary_seq,
                                                      secondary_seq,
                                                      tertiary_seq);
            ++tertiary_seq;

            v.add(e);
            sb.setLength(0);
            continue;
          }

        // Is equal to
        if (c == '=')
          {
            if (primary_seq == 0)
              throw new ParseException(rules, i);

            CollationElement e = new CollationElement(sb.toString(), primary_seq,
                                                      secondary_seq,
                                                      tertiary_seq);
            v.add(e);
            sb.setLength(0);
            continue;
          }

        // Sort accents backwards
        if (c == '@')
          {
            throw new ParseException("French style accents not implemented yet", 0);
          }

        // Reset command
        if (c == '&')
          {
            throw new ParseException("Reset not implemented yet", 0);
          }

        // See if we are still reading characters to skip
        if (ignore_chars == true)
          {
            CollationElement e = new CollationElement(c + "", 0, (short)0, 
                                                      (short)0);
            v.add(e);
            continue;
          }

        sb.append(c);
      }

    if (sb.length() > 0)
      {
	CollationElement e = new CollationElement (sb.toString(), primary_seq,
						   secondary_seq, tertiary_seq);
	v.add (e);
      }

    ce_table = v.toArray();
  }

  /**
   * This method returns a <code>String</code> containing the collation rules
   * for this object.
   *
   * @return The collation rules for this object.
   */
  public String getRules()
  {
    return(rules);
  }

  /**
   * This method calculates the collation element value for the specified
   * character(s).
   */
  int getCollationElementValue(String str)
  {
    CollationElement e = null;

    // The table is sorted.  Change to a binary search later.
    for (int i = 0; i < ce_table.length; i++) 
      if (((CollationElement)ce_table[i]).char_seq.equals(str))
        {
          e = (CollationElement)ce_table[i];
          break;
        }

    if (e == null)
      e = new CollationElement(str, 0xFFFF, (short)0xFF, (short)0xFF);

    int retval = (e.primary << 16) + (e.secondary << 8) + e.tertiary;

    return(retval);
  }

  /**
   * This method returns an instance for <code>CollationElementIterator</code>
   * for the specified <code>String</code> under the collation rules for this
   * object.
   *
   * @param str The <code>String</code> to return the <code>CollationElementIterator</code> instance for.
   *
   * @return A <code>CollationElementIterator</code> for the specified <code>String</code>.
   */
  public CollationElementIterator getCollationElementIterator(String str)
  {
    return(new CollationElementIterator(this, str));
  }  

  /**
   * This method returns an instance of <code>CollationElementIterator</code>
   * for the <code>String</code> represented by the specified
   * <code>CharacterIterator</code>.
   *
   * @param ci The <code>CharacterIterator</code> with the desired <code>String</code>.
   *
   * @return A <code>CollationElementIterator</code> for the specified <code>String</code>.
   */
  public CollationElementIterator getCollationElementIterator(CharacterIterator ci)
  {
    StringBuffer sb = new StringBuffer("");

    // Right now we assume that we will read from the beginning of the string.
    char c = ci.first();
    while (c != CharacterIterator.DONE) 
      {
        sb.append(c);
        c = ci.next();
      }

    return(getCollationElementIterator(sb.toString()));
  }

  /**
   * This method returns an integer which indicates whether the first
   * specified <code>String</code> is less than, greater than, or equal to
   * the second.  The value depends not only on the collation rules in
   * effect, but also the strength and decomposition settings of this object.
   *
   * @param s1 The first <code>String</code> to compare.
   * @param s2 A second <code>String</code> to compare to the first.
   *
   * @return A negative integer if s1 &lt; s2, a positive integer
   * if s1 &gt; s2, or 0 if s1 == s2.
   */
  public int compare(String s1, String s2)
  {
    CollationElementIterator cei1 = getCollationElementIterator(s1);
    CollationElementIterator cei2 = getCollationElementIterator(s2);

    for(;;)
      {
        int ord1 = cei1.next(); 
        int ord2 = cei2.next(); 
  
        // Check for end of string
        if (ord1 == CollationElementIterator.NULLORDER)
          if (ord2 == CollationElementIterator.NULLORDER)
            return(0);
          else
            return(-1);
        else if (ord2 == CollationElementIterator.NULLORDER)
          return(1);

        // We know chars are totally equal, so skip
        if (ord1 == ord2)
          continue;

        // Check for primary strength differences
        int prim1 = cei1.primaryOrder(ord1); 
        int prim2 = cei2.primaryOrder(ord2); 

        if (prim1 < prim2)
          return(-1);
        else if (prim1 > prim2)
          return(1);
        else if (getStrength() == PRIMARY)
          continue;

        // Check for secondary strength differences
        int sec1 = cei1.secondaryOrder(ord1);
        int sec2 = cei2.secondaryOrder(ord2);

        if (sec1 < sec2)
          return(-1);
        else if (sec1 > sec2)
          return(1);
        else if (getStrength() == SECONDARY)
          continue;

        // Check for tertiary differences
        int tert1 = cei1.tertiaryOrder(ord1);
        int tert2 = cei2.tertiaryOrder(ord1);

        if (tert1 < tert2)
          return(-1);
        else if (tert1 > tert2)
          return(1);
      }
  }

  /**
   * This method returns an instance of <code>CollationKey</code> for the
   * specified <code>String</code>.  The object returned will have a
   * more efficient mechanism for its comparison function that could
   * provide speed benefits if multiple comparisons are performed, such
   * as during a sort.
   *
   * @param str The <code>String</code> to create a <code>CollationKey</code> for.
   *
   * @return A <code>CollationKey</code> for the specified <code>String</code>.
   */
  public CollationKey getCollationKey(String str)
  {
    CollationElementIterator cei = getCollationElementIterator(str);
    Vector vect = new Vector(25);

    int ord = cei.next();
    cei.reset(); //set to start of string

    while (ord != CollationElementIterator.NULLORDER)
      {
        switch (getStrength())
          {
            case PRIMARY:
               ord = cei.primaryOrder(ord);
               break;

            case SECONDARY:
               ord = cei.secondaryOrder(ord);

            default:
               break;
          }

        vect.add(new Integer(ord)); 
	ord = cei.next(); //increment to next key
      }

    Object[] objarr = vect.toArray();
    byte[] key = new byte[objarr.length * 4];

    for (int i = 0; i < objarr.length; i++)
      {
        int j = ((Integer)objarr[i]).intValue();
        key [i * 4] = (byte)((j & 0xFF000000) >> 24);
        key [i * 4 + 1] = (byte)((j & 0x00FF0000) >> 16);
        key [i * 4 + 2] = (byte)((j & 0x0000FF00) >> 8);
        key [i * 4 + 3] = (byte)(j & 0x000000FF);
      }

    return(new CollationKey(this, str, key));
  }

  /**
   * This method tests this object for equality against the specified 
   * object.  This will be true if and only if the specified object is
   * another reference to this object.
   *
   * @param obj The <code>Object</code> to compare against this object.
   *
   * @return <code>true</code> if the specified object is equal to this object, <code>false</code> otherwise.
   */
  public boolean equals(Object obj)
  {
    if (obj == this)
      return(true);
    else
      return(false);
  }

  /**
   * This method returns a hash value for this object.
   *
   * @return A hash value for this object.
   */
  public int hashCode()
  {
    return(System.identityHashCode(this));
  }

  /**
   * This method creates a copy of this object.
   *
   * @return A copy of this object.
   */
  public Object clone()
  {
    return super.clone();
  }
}
