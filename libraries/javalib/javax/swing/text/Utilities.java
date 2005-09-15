/* Utilities.java --
   Copyright (C) 2004, 2005  Free Software Foundation, Inc.

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


package javax.swing.text;

import java.awt.FontMetrics;
import java.awt.Graphics;

/**
 * A set of utilities to deal with text. This is used by several other classes
 * inside this package.
 *
 * @author Roman Kennke (roman@ontographics.com)
 */
public class Utilities
{
  /**
   * The length of the char buffer that holds the characters to be drawn.
   */
  private static final int BUF_LENGTH = 64;

  /**
   * Creates a new <code>Utilities</code> object.
   */
  public Utilities()
  {
    // Nothing to be done here.
  }

  /**
   * Draws the given text segment. Contained tabs and newline characters
   * are taken into account. Tabs are expanded using the
   * specified {@link TabExpander}.
   *
   * @param s the text fragment to be drawn.
   * @param x the x position for drawing.
   * @param y the y position for drawing.
   * @param g the {@link Graphics} context for drawing.
   * @param e the {@link TabExpander} which specifies the Tab-expanding
   *     technique.
   * @param startOffset starting offset in the text.
   * @return the x coordinate at the end of the drawn text.
   */
  public static final int drawTabbedText(Segment s, int x, int y, Graphics g,
                                         TabExpander e, int startOffset)
  {
    // This buffers the chars to be drawn.
    char[] buffer = s.array;


    // The current x and y pixel coordinates.
    int pixelX = x;
    int pixelY = y;

    // The font metrics of the current selected font.
    FontMetrics metrics = g.getFontMetrics();
    int ascent = metrics.getAscent();

    int pixelWidth = 0;
    int pos = s.offset;
    int len = 0;

    for (int offset = s.offset; offset < (s.offset + s.count); ++offset)
      {
        char c = buffer[offset];
        if (c == '\t' || c == '\n')
          {
            if (len > 0) {
              g.drawChars(buffer, pos, len, pixelX, pixelY + ascent);            
              pixelX += pixelWidth;
              pixelWidth = 0;
            }
            pos = offset+1;
            len = 0;
          }
        
	switch (c)
	  {
	  case '\t':
	    // In case we have a tab, we just 'jump' over the tab.
	    // When we have no tab expander we just use the width of ' '.
	    if (e != null)
	      pixelX = (int) e.nextTabStop((float) pixelX,
					   startOffset + offset - s.offset);
	    else
	      pixelX += metrics.charWidth(' ');
	    break;
	  case '\n':
	    // In case we have a newline, we must jump to the next line.
	    pixelY += metrics.getHeight();
	    pixelX = x;
	    break;
	  default:
            ++len;
	    pixelWidth += metrics.charWidth(buffer[offset]);
	    break;
	  }
      }

    if (len > 0)
      g.drawChars(buffer, pos, len, pixelX, pixelY + ascent);            
    
    return pixelX;
  }

  /**
   * Determines the width, that the given text <code>s</code> would take
   * if it was printed with the given {@link java.awt.FontMetrics} on the
   * specified screen position.
   * @param s the text fragment
   * @param metrics the font metrics of the font to be used
   * @param x the x coordinate of the point at which drawing should be done
   * @param e the {@link TabExpander} to be used
   * @param startOffset the index in <code>s</code> where to start
   * @returns the width of the given text s. This takes tabs and newlines
   * into account.
   */
  public static final int getTabbedTextWidth(Segment s, FontMetrics metrics,
                                             int x, TabExpander e,
                                             int startOffset)
  {
    // This buffers the chars to be drawn.
    char[] buffer = s.array;

    // The current x coordinate.
    int pixelX = x;

    // The current maximum width.
    int maxWidth = 0;

    for (int offset = s.offset; offset < (s.offset + s.count); ++offset)
      {
	switch (buffer[offset])
	  {
	  case '\t':
	    // In case we have a tab, we just 'jump' over the tab.
	    // When we have no tab expander we just use the width of 'm'.
	    if (e != null)
	      pixelX = (int) e.nextTabStop((float) pixelX,
					   startOffset + offset - s.offset);
	    else
	      pixelX += metrics.charWidth(' ');
	    break;
	  case '\n':
	    // In case we have a newline, we must 'draw'
	    // the buffer and jump on the next line.
	    pixelX += metrics.charWidth(buffer[offset]);
	    maxWidth = Math.max(maxWidth, pixelX - x);
	    pixelX = x;
	    break;
	  default:
	    // Here we draw the char.
	    pixelX += metrics.charWidth(buffer[offset]);
	    break;
	  }
      }

    // Take the last line into account.
    maxWidth = Math.max(maxWidth, pixelX - x);

    return maxWidth;
  }

  /**
   * Provides a facility to map screen coordinates into a model location. For a
   * given text fragment and start location within this fragment, this method
   * determines the model location so that the resulting fragment fits best
   * into the span <code>[x0, x]</code>.
   *
   * The parameter <code>round</code> controls which model location is returned
   * if the view coordinates are on a character: If <code>round</code> is
   * <code>true</code>, then the result is rounded up to the next character, so
   * that the resulting fragment is the smallest fragment that is larger than
   * the specified span. If <code>round</code> is <code>false</code>, then the
   * resulting fragment is the largest fragment that is smaller than the
   * specified span.
   *
   * @param s the text segment
   * @param fm the font metrics to use
   * @param x0 the starting screen location
   * @param x the target screen location at which the requested fragment should
   *        end
   * @param te the tab expander to use; if this is <code>null</code>, TABs are
   *        expanded to one space character
   * @param p0 the starting model location
   * @param round if <code>true</code> round up to the next location, otherwise
   *        round down to the current location
   *
   * @return the model location, so that the resulting fragment fits within the
   *         specified span
   */
  public static final int getTabbedTextOffset(Segment s, FontMetrics fm, int x0,
                                              int x, TabExpander te, int p0,
                                              boolean round)
  {
    // At the end of the for loop, this holds the requested model location
    int pos;
    int currentX = x0;
    for (pos = p0; pos < s.getEndIndex(); pos++)
      {
        char nextChar = s.array[pos];
        if (nextChar != '\n')
          currentX += fm.charWidth(nextChar);
        else
          {
            if (te == null)
              currentX += fm.charWidth(' ');
            else
              currentX = (int) te.nextTabStop(currentX, pos);
          }
        if (currentX >= x)
          {
            if (! round)
              pos--;
            break;
          }
      }
    return pos;
  }

  /**
   * Provides a facility to map screen coordinates into a model location. For a
   * given text fragment and start location within this fragment, this method
   * determines the model location so that the resulting fragment fits best
   * into the span <code>[x0, x]</code>.
   *
   * This method rounds up to the next location, so that the resulting fragment
   * will be the smallest fragment of the text, that is greater than the
   * specified span.
   *
   * @param s the text segment
   * @param fm the font metrics to use
   * @param x0 the starting screen location
   * @param x the target screen location at which the requested fragment should
   *        end
   * @param te the tab expander to use; if this is <code>null</code>, TABs are
   *        expanded to one space character
   * @param p0 the starting model location
   *
   * @return the model location, so that the resulting fragment fits within the
   *         specified span
   */
  public static final int getTabbedTextOffset(Segment s, FontMetrics fm, int x0,
                                              int x, TabExpander te, int p0)
  {
    return getTabbedTextOffset(s, fm, x0, x, te, p0, true);
  }
}
