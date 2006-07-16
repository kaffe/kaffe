/* MBeanInfo.java -- Information about a management bean.
   Copyright (C) 2006 Free Software Foundation, Inc.

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

package javax.management;

import java.io.Serializable;

/**
 * <p>
 * Describes the interface of a management bean.  This allows
 * the user to access the bean dynamically, without knowing
 * the details of any of its attributes, operations,
 * constructors or notifications beforehand.  The information
 * is immutable as standard.  Of course, subclasses may change
 * this, but this behaviour is not recommended.
 * </p>
 * <p>
 * The contents of this class, for standard management beans,
 * are dynamically compiled using reflection.
 * {@link #getClassName()} and {@link #getConstructors()}
 * return the name of the class and its constructors, respectively.
 * This is much the same as could be obtained by reflection on the
 * bean.  {@link #getAttributes()} and {@link #getOperations()},
 * however, do something more in splitting the methods of the
 * class into two sets.  Those of the form, <code>getXXX</code>,
 * <code>setXXX</code> and <code>isXXX</code> are taken to be
 * the accessors and mutators of a series of attributes, with
 * <code>XXX</code> being the attribute name.  These are returned
 * by {@link getAttributes()} and the {@link Attribute} class can
 * be used to manipulate them.  The remaining methods are classified
 * as operations and returned by {@link getOperations()}.
 * </p>
 * <p>
 * Beans can also broadcast notifications.  If the bean provides this
 * facility, by implementing the {@link NotificationBroadcaster}
 * interface, then an array of {@link MBeanNotificationInfo} objects
 * may be obtained from {@link #getNotifications()}, which describe
 * the notifications emitted.
 * </p>
 * <p>
 * Model management beans and open management beans also supply an
 * instance of this class, as part of implementing the
 * {@link DynamicMBean#getMBeanInfo()} method of {@link DynamicMBean}.
 * </p>
 * 
 * @author Andrew John Hughes (gnu_andrew@member.fsf.org)
 * @since 1.5
 */
public class MBeanInfo
  implements Cloneable, Serializable
{

  /**
   * Compatible with JDK 1.5
   */
  private static final long serialVersionUID = -6451021435135161911L;

  /**
   * A description of the bean.
   * 
   * @serial The bean's description.
   */
  private String description;

  /**
   * The class name of the management bean.
   *
   * @serial The bean's class name.
   */
  private String className;

  /**
   * Returns a shallow clone of the information.  This is
   * simply a new copy of each string and a clone
   * of each array, which still references the same objects,
   * as obtained by the {@link Object} implementation of
   * {@link Object#clone()}.  As the fields can not be
   * changed, this method is only really of interest to
   * subclasses which may add new mutable fields or make
   * the existing ones mutable.
   *
   * @return a shallow clone of this {@link MBeanInfo}.
   */
  public Object clone()
  {
    MBeanInfo clone = null;
    try
      {
	clone = (MBeanInfo) super.clone();
      }
    catch (CloneNotSupportedException e)
      {
	/* This won't happen as we implement Cloneable */
      }
    return clone;
  }

  /**
   * Returns the class name of the management bean.
   *
   * @return the bean's class name.
   */
  public String getClassName()
  {
    return className;
  }

  /**
   * Returns a description of the management bean.
   *
   * @return the bean's description.
   */
  public String getDescription()
  {
    return description;
  }

}
