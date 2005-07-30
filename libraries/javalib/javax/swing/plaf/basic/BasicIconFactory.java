/* BasicIconFactory.java --
   Copyright (C) 2002, 2004  Free Software Foundation, Inc.

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


package javax.swing.plaf.basic;

import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.Polygon;
import java.io.Serializable;

import javax.swing.AbstractButton;
import javax.swing.Icon;
import javax.swing.UIDefaults;
import javax.swing.UIManager;

/**
 * STUBBED
 */
public class BasicIconFactory implements Serializable
{
  static final long serialVersionUID = 5605588811185324383L;

  private static class DummyIcon 
    implements Icon
  {    
    public int getIconHeight() { return 10; }
    public int getIconWidth() { return 10; }
    public void paintIcon(Component c, Graphics g, int x, int y)
    {
      Color save = g.getColor();
      g.setColor(c.getForeground());
      g.drawRect(x, y, 10, 10);
      g.setColor(save);
    }
  }

  /**
   * The icon used for CheckBoxes in the BasicLookAndFeel. This is an empty
   * icon with a size of 13x13 pixels.
   */
  static class CheckBoxIcon
    implements Icon
  {
    /**
     * Returns the height of the icon. The BasicLookAndFeel CheckBox icon
     * has a height of 13 pixels.
     *
     * @return the height of the icon
     */
    public int getIconHeight()
    {
      return 13;
    }

    /**
     * Returns the width of the icon. The BasicLookAndFeel CheckBox icon
     * has a width of 13 pixels.
     *
     * @return the height of the icon
     */
    public int getIconWidth()
    {
      return 13;
    }

    /**
     * Paints the icon. The BasicLookAndFeel CheckBox icon is empty and does
     * not need to be painted.
     *
     * @param c the component to be painted
     * @param g the Graphics context to be painted with
     * @param x the x position of the icon
     * @param y the y position of the icon
     */
    public void paintIcon(Component c, Graphics g, int x, int y)
    {
      // The icon is empty and needs no painting.
    }
  }

  /**
   * The icon used for RadioButtons in the BasicLookAndFeel. This is an empty
   * icon with a size of 13x13 pixels.
   */
  static class RadioButtonIcon
    implements Icon
  {
    /**
     * Returns the height of the icon. The BasicLookAndFeel RadioButton icon
     * has a height of 13 pixels.
     *
     * @return the height of the icon
     */
    public int getIconHeight()
    {
      return 13;
    }

    /**
     * Returns the width of the icon. The BasicLookAndFeel RadioButton icon
     * has a width of 13 pixels.
     *
     * @return the height of the icon
     */
    public int getIconWidth()
    {
      return 13;
    }

    /**
     * Paints the icon. The BasicLookAndFeel RadioButton icon is empty and does
     * not need to be painted.
     *
     * @param c the component to be painted
     * @param g the Graphics context to be painted with
     * @param x the x position of the icon
     * @param y the y position of the icon
     */
    public void paintIcon(Component c, Graphics g, int x, int y)
    {
      // The icon is empty and needs no painting.
    }
  }

  /** The cached CheckBoxIcon instance. */
  private static CheckBoxIcon checkBoxIcon;

  /** The cached CheckBoxIcon instance. */
  private static RadioButtonIcon radioButtonIcon;

  public static Icon getMenuItemCheckIcon()
  {
    return new DummyIcon();
  }
  public static Icon getMenuItemArrowIcon()
  {
    return new DummyIcon();
  }
  public static Icon getMenuArrowIcon()
  {
    return new Icon()
      {
	public int getIconHeight()
	{
	  return 12;
	}

	public int getIconWidth()
	{
	  return 12;
	}

	public void paintIcon(Component c, Graphics g, int x, int y)
	{
	  g.translate(x, y);

	  Color saved = g.getColor();

	  g.setColor(Color.BLACK);

	  g.fillPolygon(new Polygon(new int[] { 3, 9, 3 },
                                  new int[] { 2, 6, 10 },
                                  3));

	  g.setColor(saved);
	  g.translate(-x, -y);
	}
      };
  }

  /**
   * Returns an icon for CheckBoxes in the BasicLookAndFeel. CheckBox icons
   * in the Basic L&amp;F are empty and have a size of 13x13 pixels.
   * This method returns a shared single instance of this icon.
   *
   * @return an icon for CheckBoxes in the BasicLookAndFeel
   */
  public static Icon getCheckBoxIcon()
  {
    if (checkBoxIcon == null)
      checkBoxIcon = new CheckBoxIcon();
    return checkBoxIcon;
  }

  /**
   * Returns an icon for RadioButtons in the BasicLookAndFeel. RadioButton
   * icons in the Basic L&amp;F are empty and have a size of 13x13 pixels.
   * This method returns a shared single instance of this icon.
   *
   * @return an icon for RadioButtons in the BasicLookAndFeel
   */
  public static Icon getRadioButtonIcon()
  {
    if (radioButtonIcon == null)
      radioButtonIcon = new RadioButtonIcon();
    return radioButtonIcon;
  }

  public static Icon getCheckBoxMenuItemIcon()
  {
    return getCheckBoxIcon();
  }
  public static Icon getRadioButtonMenuItemIcon()
  {
    return getRadioButtonIcon();
  }
  public static Icon createEmptyFrameIcon()
  {
    return new DummyIcon();
  }
} // class BasicIconFactory
