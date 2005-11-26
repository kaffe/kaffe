/* MediaTray.java --
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

package javax.print.attribute.standard;

import javax.print.attribute.EnumSyntax;

/**
 * An enumeration of input tray parameters.
 *
 * @author Sven de Marothy
 */
public class MediaTray extends Media
{
  /**
   * Bottom tray
   */
  public static final MediaTray BOTTOM = new MediaTray(0);

  /**
   * Envelope tray
   */
  public static final MediaTray ENVELOPE = new MediaTray(1);

  /**
   * Large capacity tray
   */
  public static final MediaTray LARGE_CAPACITY = new MediaTray(2);

  /**
   * Main tray
   */
  public static final MediaTray MAIN = new MediaTray(3);

  /**
   * Manual-feed tray
   */ 
  public static final MediaTray MANUAL = new MediaTray(4);

  /**
   * Middle tray
   */
  public static final MediaTray MIDDLE = new MediaTray(5);

  /**
   * Side tray
   */
  public static final MediaTray SIDE = new MediaTray(6);

  /**
   * Top tray
   */
  public static final MediaTray TOP = new MediaTray(7);
  
  protected MediaTray(int i)
  {
    super( i );
  }
 
  protected EnumSyntax[] getEnumValueTable()
  {
    // FIXME
    return (EnumSyntax[])null;
  }

  protected String[] getStringTable()
  {
    // FIXME
    return (String[])null;
  }
}

