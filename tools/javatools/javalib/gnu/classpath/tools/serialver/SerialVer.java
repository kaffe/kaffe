/* gnu.classpath.tools.SerialVer
 Copyright (C) 1998, 1999, 2000, 2001 Free Software Foundation, Inc.

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
 02110-1301 USA. */
package gnu.classpath.tools.serialver;

import java.io.ObjectStreamClass;

/**
 * This class is an implementation of the `serialver' program. Any number of
 * class names can be passed as arguments, and the serial version unique
 * identitfier for each class will be printed in a manner suitable for cuting
 * and pasting into a Java source file.
 */
public class SerialVer
{
  public static void main(String[] args)
  {
    if (args.length == 0)
      {
        System.out.println("Usage: serialver [CLASS]...");
        return;
      }
    Class clazz;
    ObjectStreamClass osc;
    for (int i = 0; i < args.length; i++)
      {
        try
          {
            clazz = Class.forName(args[i]);
            osc = ObjectStreamClass.lookup(clazz);
            if (osc != null)
              System.out.println(clazz.getName() + ": "
                                 + "static final long serialVersionUID = "
                                 + osc.getSerialVersionUID() + "L;");
            else
              System.err.println("Class " + args[i] + " is not serializable");
          }
        catch (ClassNotFoundException e)
          {
            System.err.println("Class for " + args[i] + " not found");
          }
      }
  }
}