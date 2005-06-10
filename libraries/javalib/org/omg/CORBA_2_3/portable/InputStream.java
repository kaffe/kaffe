/* InputStream.java --
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


package org.omg.CORBA_2_3.portable;

import org.omg.CORBA.MARSHAL;
import org.omg.CORBA.ValueBaseHelper;
import org.omg.CORBA.portable.BoxedValueHelper;

import java.io.Serializable;

/**
 * This class defines a new CDR input stream methods, added since
 * CORBA 2.3.
 *
 * This class is abstract; no direct instances can be instantiated.
 * Also, up till v 1.4 inclusive there are no methods that would
 * return it, and only one unimplemented interface,
 * {@link org.omg.CORBA.portable.ValueFactory }, needs it as a parameter.
 *
 * However since 1.3 all methods, declared as returning an
 * org.omg.CORBA.portable.InputStream actually return the instance of this
 * derived class and the new methods are accessible after the casting
 * operation.
 *
 * OMG specification states the writing format of the value types
 * is outside the scope of GIOP definition. This implementation uses
 * java serialization mechanism, calling {@link ObjectInputStream#readObject}.
 *
 * @author Audrius Meskauskas (AudriusA@Bioinformatics.org)
 */
public abstract class InputStream
  extends org.omg.CORBA.portable.InputStream
{
  /**
   * Read the abstract interface. An abstract interface can be either
   * CORBA value type or CORBA object and is returned as an abstract
   * java.lang.Object.
   *
   * As specified in OMG specification, this reads a single
   * boolean and then delegates either to {@link #read_Object()} (for false)
   * or to {@link #read_Value()} (for true).
   *
   * @return an abstract interface, unmarshaled from the stream
   */
  public Object read_abstract_interface()
  {
    boolean isValue = read_boolean();

    if (isValue)
      return read_value();
    else
      return read_Object();
  }

  /**
   * Read the abstract interface, corresponding to the passed type.
   * An abstract interface can be either CORBA value type or CORBA
   * object and is returned as an abstract java.lang.Object.
   *
   * As specified in OMG specification, this reads a single
   * boolean and then delegates either to {@link #read_Object(Class)} (for false)
   * or to {@link #read_Value(Class)} (for true).
   *
   * @param clz a base class for the abstract interface.
   *
   * @return an abstract interface, unmarshaled from the stream
   */
  public Object read_abstract_interface(Class clz)
  {
    boolean isValue = read_boolean();

    if (isValue)
      return read_value(clz);
    else
      return read_Object(clz);
  }

  /**
   * Read a value type structure from the stream.
   *
   * OMG specification states the writing format is outside the scope
   * of GIOP definition. This implementation uses java serialization
   * mechanism, calling {@link ObjectInputStream#readObject}
   *
   * @return an value type structure, unmarshaled from the stream
   */
  public Serializable read_value()
  {
    return read_value((Class) null);
  }

  /**
   * Read a value type structure, corresponing to the passed type.
   *
   * OMG specification states the writing format is outside the scope
   * of GIOP definition. This implementation uses java serialization
   * mechanism, calling {@link ObjectInputStream#readObject}
   *
   * @param clz a base class for a value type. The class information
   * is currently used for security check only.
   *
   * @return an value type structure, unmarshaled from the stream
   */
  public Serializable read_value(Class clz)
  {
    Serializable rt = (Serializable) ValueBaseHelper.read(this);

    if (rt != null && clz != null)
      {
        if (!(clz.isAssignableFrom(rt.getClass())))
          {
            throw new MARSHAL(rt.getClass().getName() +
                              " is not an instance of " + clz.getName()
                             );
          }
      }
    return rt;
  }

  /**
   * Read a value type structure content, when the unitialised
   * instance is passed as a parameter.
   *
   * OMG specification states the writing format is outside the scope
   * of GIOP definition. This implementation uses java serialization
   * mechanism, calling {@link ObjectInputStream#readObject}
   * and then compares the loaded instance with the given class
   * of the passed instance for equality.
   *
   * @param unitialised_value, used for class check only.
   *
   * @return same value, filled in by the stream content.
   */
  public Serializable read_value(Serializable unitialised_value)
  {
    return read_value(unitialised_value.getClass());
  }

  /**
   * Read a value type structure, having the given repository id.
   * The casts the streams ORB into a CORBA 2.3 ORB and then
   * searched for a suitable value factory, where it delegates
   * the functionality.
   *
   * @param repository_id a repository id of the value type.
   *
   * @return an value type structure, unmarshaled from the stream
   */
  public Serializable read_value(String repository_id)
  {
    return ((org.omg.CORBA_2_3.ORB) orb()).lookup_value_factory(repository_id)
            .read_value(this);
  }

  public Serializable read_value(BoxedValueHelper helper)
  {
    return helper.read_value(this);
  }
}