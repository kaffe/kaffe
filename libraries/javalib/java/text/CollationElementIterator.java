/* CollationElementIterator.java -- Walks through collation elements
   Copyright (C) 1998, 1999, 2001, 2002, 2003, 2004  Free Software Foundation

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
import java.util.NoSuchElementException;
import java.util.Map;
import java.util.SortedMap;

/* Written using "Java Class Libraries", 2nd edition, plus online
 * API docs for JDK 1.2 from http://www.javasoft.com.
 * Status: Believed complete and correct to JDK 1.1.
 */

/**
 * This class walks through the character collation elements of a 
 * <code>String</code> as defined by the collation rules in an instance of 
 * <code>RuleBasedCollator</code>.  There is no public constructor for
 * this class.  An instance is created by calling the
 * <code>getCollationElementIterator</code> method on 
 * <code>RuleBasedCollator</code>.
 *
 * @author Aaron M. Renn <arenn@urbanophile.com>
 * @author Tom Tromey <tromey@cygnus.com>
 * @author Guilhem Lavaux <guilhem.lavaux@free.fr>
 */
public final class CollationElementIterator
{
  /**
   * This is a constant value that is returned to indicate that the end of 
   * the string was encountered.
   */
  public static final int NULLORDER = -1;

  /**
   * This is the RuleBasedCollator this object was created from.
   */
  RuleBasedCollator collator;

  /**
   * This is the String that is being iterated over.
   */
  String text;

  /**
   * This is the index into the collation decomposition where we are currently scanning.
   */
  int index;

  /**
   * This is the index into the String where we are currently scanning.
   */
  int textIndex;

  /**
   * Array containing the collation decomposition of the
   * text given to the constructor.
   */
  private Object[] text_decomposition;

  /**
   * This method initializes a new instance of <code>CollationElementIterator</code>
   * to iterate over the specified <code>String</code> using the rules in the
   * specified <code>RuleBasedCollator</code>.
   *
   * @param collator The <code>RuleBasedCollation</code> used for calculating collation values
   * @param text The <code>String</code> to iterate over.
   */
  CollationElementIterator(RuleBasedCollator collator, String text)
  {
    this.collator = collator;
    
    setText (text);    
  }

  RuleBasedCollator.CollationElement nextBlock()
  {
    if (index >= text_decomposition.length)
      return null;
    
    RuleBasedCollator.CollationElement e =
      (RuleBasedCollator.CollationElement) text_decomposition[index++];
    
    textIndex += e.key.length();

    return e;
  }

  RuleBasedCollator.CollationElement previousBlock()
  {
    if (index == 0)
      return null;
    
    index--;
    RuleBasedCollator.CollationElement e =
      (RuleBasedCollator.CollationElement) text_decomposition[index];

    textIndex -= e.key.length();
    
    return e;
  }

  /**
   * This method returns the collation ordering value of the next character sequence
   * in the string (it may be an extended character following collation rules).
   * This method will return <code>NULLORDER</code> if the
   * end of the string was reached.
   *
   * @return The collation ordering value.
   */
  public int next()
  {
    RuleBasedCollator.CollationElement e = nextBlock();

    if (e == null)
      return NULLORDER;
    
    return e.getValue();
  }

  /**
   * This method returns the collation ordering value of the previous character
   * in the string.  This method will return <code>NULLORDER</code> if the
   * beginning of the string was reached.
   *
   * @return The collation ordering value.
   */
  public int previous()
  {
    RuleBasedCollator.CollationElement e = previousBlock();

    if (e == null)
      return NULLORDER;
    
    return e.getValue();
  }

  /**
   * This method returns the primary order value for the given collation
   * value.
   *
   * @param value The collation value returned from <code>next()</code> or <code>previous()</code>.
   *
   * @return The primary order value of the specified collation value.  This is the high 16 bits.
   */
  public static final int primaryOrder(int order)
  {
    // From the JDK 1.2 spec.
    return order >>> 16;
  }

  /**
   * This method resets the internal position pointer to read from the
   * beginning of the <code>String again.
   */
  public void reset()
  {
    index = 0;
    textIndex = 0;
  }

  /**
   * This method returns the secondary order value for the given collation
   * value.
   *
   * @param value The collation value returned from <code>next()</code> or <code>previous()</code>.
   *
   * @return The secondary order value of the specified collation value.  This is the bits 8-15.
   */
  public static final short secondaryOrder(int order)
  {
    // From the JDK 1.2 spec.
    return (short) ((order >>> 8) & 255);
  }

  /**
   * This method returns the tertiary order value for the given collation
   * value.
   *
   * @param value The collation value returned from <code>next()</code> or <code>previous()</code>.
   *
   * @return The tertiary order value of the specified collation value.  This is the low eight bits.
   */
  public static final short tertiaryOrder(int order)
  {
    // From the JDK 1.2 spec.
    return (short) (order & 255);
  }

  /**
   * This method sets the <code>String</code> that it is iterating over
   * to the specified <code>String</code>.
   *
   * @param text The new <code>String</code> to iterate over.
   *
   * @since 1.2
   */
  public void setText(String text)
  {
    int idx = 0;

    this.text = text;
    this.index = 0;

    String work_text = text.intern();

    Vector v = new Vector();
    // Build element collection ordered as they come in "text".
    while (idx < work_text.length())
      {
	String key, key_old;

	Object object = null;
	int p = 1;
	
	// IMPROVE: use a TreeMap with a prefix-ordering rule.
	key_old = key = null;
	do
	  {
	    if (object != null)
	      key_old = key;
	    key = work_text.substring (idx, idx+p);
	    object = collator.prefix_tree.get (key);
	    p++;
	  }
	while (idx+p <= work_text.length());
	
	if (object == null)
	  key = key_old;
	
	RuleBasedCollator.CollationElement prefix =
	  (RuleBasedCollator.CollationElement) collator.prefix_tree.get (key);
	
	if (prefix == null)
	  {
	    RuleBasedCollator.CollationElement e =
	      collator.getDefaultElement(work_text.charAt (idx));
	    
	    v.add (e);
	    idx++;
	    continue;
	  }

	if (prefix.expansion != null)
	  {
	    work_text = prefix.expansion
	      + work_text.substring (idx+prefix.key.length());
	    idx = 0;
	    v.add (prefix);
	  }
	else
	  {
	    if (!prefix.ignore)
	      v.add (prefix);
	    idx += prefix.key.length();
	  }
      }
    
    text_decomposition = v.toArray();
  }

  /**
   * This method sets the <code>String</code> that it is iterating over
   * to the <code>String</code> represented by the specified
   * <code>CharacterIterator</code>.
   *
   * @param source The <code>CharacterIterator</code> containing the new
   * <code>String</code> to iterate over.
   */
  public void setText(CharacterIterator source)
  {
    StringBuffer expand = new StringBuffer();

    // For now assume we read from the beginning of the string.
    for (char c = source.first();
	 c != CharacterIterator.DONE;
	 c = source.next())
      expand.append(c);

    setText(expand.toString());
  }

  /**
   * This method returns the current offset into the <code>String</code>
   * that is being iterated over.
   *
   * @return The iteration index position.
   *
   * @since 1.2
   */
  public int getOffset()
  {
    return textIndex;
  }

  /**
   * This method sets the iteration index position into the current
   * <code>String</code> to the specified value.  This value must not
   * be negative and must not be greater than the last index position
   * in the <code>String</code>.
   *
   * @param offset The new iteration index position.
   *
   * @exception IllegalArgumentException If the new offset is not valid.
   */
  public void setOffset(int offset)
  {
    if (offset < 0)
      throw new IllegalArgumentException("Negative offset: " + offset);

    if ((text.length() > 0) && (offset > 0))
      throw new IllegalArgumentException("Offset too large: " + offset);
    else if (offset > (text.length() - 1))
      throw new IllegalArgumentException("Offset too large: " + offset);

    textIndex = 0;
    for (int i=0;i<text_decomposition.length;i++)
      {
	RuleBasedCollator.CollationElement e =
	  (RuleBasedCollator.CollationElement) text_decomposition[i];
	int idx = textIndex + e.key.length();
	
	if (idx > offset)
	  break;
	textIndex = idx;
      }
  }

  /**
   * This method returns the maximum length of any expansion sequence that
   * ends with the specified collation order value.  (Whatever that means).
   *
   * @param value The collation order value
   *
   * @param The maximum length of an expansion sequence.
   */
  public int getMaxExpansion(int value)
  {
    return 1;
  }
}
