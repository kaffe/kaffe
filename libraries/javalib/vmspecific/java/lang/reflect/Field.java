/* java.lang.reflect.Field - reflection of Java fields
   Copyright (C) 1998, 2001, 2005 Free Software Foundation, Inc.

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


package java.lang.reflect;

import gnu.java.lang.ClassHelper;
import gnu.java.lang.CPStringBuilder;

import gnu.java.lang.reflect.FieldSignatureParser;

/**
 * The Field class represents a member variable of a class. It also allows
 * dynamic access to a member, via reflection. This works for both
 * static and instance fields. Operations on Field objects know how to
 * do widening conversions, but throw {@link IllegalArgumentException} if
 * a narrowing conversion would be necessary. You can query for information
 * on this Field regardless of location, but get and set access may be limited
 * by Java language access controls. If you can't do it in the compiler, you
 * can't normally do it here either.<p>
 *
 * <B>Note:</B> This class returns and accepts types as Classes, even
 * primitive types; there are Class types defined that represent each
 * different primitive type.  They are <code>java.lang.Boolean.TYPE,
 * java.lang.Byte.TYPE,</code>, also available as <code>boolean.class,
 * byte.class</code>, etc.  These are not to be confused with the
 * classes <code>java.lang.Boolean, java.lang.Byte</code>, etc., which are
 * real classes.<p>
 *
 * Also note that this is not a serializable class.  It is entirely feasible
 * to make it serializable using the Externalizable interface, but this is
 * on Sun, not me.
 *
 * @author John Keiser
 * @author Eric Blake <ebb9@email.byu.edu>
 * @see Member
 * @see Class
 * @see Class#getField(String)
 * @see Class#getDeclaredField(String)
 * @see Class#getFields()
 * @see Class#getDeclaredFields()
 * @since 1.1
 * @status updated to 1.4
 */
public final class Field
extends AccessibleObject implements Member
{
  private Class declaringClass;
  private String name;
  private int slot;
  private Class type;

  private static final int FIELD_MODIFIERS
    = Modifier.FINAL | Modifier.PRIVATE | Modifier.PROTECTED
      | Modifier.PUBLIC | Modifier.STATIC | Modifier.TRANSIENT
      | Modifier.VOLATILE;

  /**
   * This class is uninstantiable except natively.
   */
  private Field(Class declaringClass, String name, int slot)
  {
    this.declaringClass = declaringClass;
    this.name = name;
    this.slot = slot;
  }

  /**
   * Gets the class that declared this field, or the class where this field
   * is a non-inherited member.
   * @return the class that declared this member
   */
  public Class getDeclaringClass()
  {
    return declaringClass;
  }

  /**
   * Gets the name of this field.
   * @return the name of this field
   */
  public String getName()
  {
    return name;
  }

  /**
   * Return the raw modifiers for this field.
   * @return the field's modifiers
   */
  private native int getModifiersInternal();

  /**
   * Gets the modifiers this field uses.  Use the <code>Modifier</code>
   * class to interpret the values.  A field can only have a subset of the
   * following modifiers: public, private, protected, static, final,
   * transient, and volatile.
   *
   * @return an integer representing the modifiers to this Member
   * @see Modifier
   */
  public int getModifiers()
  {
    return getModifiersInternal() & FIELD_MODIFIERS;
  }

  /**
   * Return true if this field is synthetic, false otherwise.
   * @since 1.5
   */
  public boolean isSynthetic()
  {
    return (getModifiersInternal() & Modifier.SYNTHETIC) != 0;
  }

  /**
   * Return true if this field represents an enum constant,
   * false otherwise.
   * @since 1.5
   */
  public boolean isEnumConstant()
  {
    return (getModifiersInternal() & Modifier.ENUM) != 0;
  }

  /**
   * Gets the type of this field.
   * @return the type of this field
   */
  public Class getType()
  {
    return type;
  }

  /**
   * Compare two objects to see if they are semantically equivalent.
   * Two Fields are semantically equivalent if they have the same declaring
   * class, name, and type. Since you can't creat a Field except through
   * the VM, this is just the == relation.
   *
   * @param o the object to compare to
   * @return <code>true</code> if they are equal; <code>false</code> if not
   */
  public boolean equals(Object o)
  {
    if (!(o instanceof Field))
      return false;
    Field that = (Field)o; 
    if (this.getDeclaringClass() != that.getDeclaringClass())
      return false;
    if (!this.getName().equals(that.getName()))
      return false;
    if (this.getType() != that.getType())
      return false;
    return true;
  }

  /**
   * Get the hash code for the Field. The Field hash code is the hash code
   * of its name XOR'd with the hash code of its class name.
   *
   * @return the hash code for the object.
   */
  public int hashCode()
  {
    return getDeclaringClass().getName().hashCode() ^ getName().hashCode();
  }

  /**
   * Get a String representation of the Field. A Field's String
   * representation is "&lt;modifiers&gt; &lt;type&gt;
   * &lt;class&gt;.&lt;fieldname&gt;".<br> Example:
   * <code>public transient boolean gnu.parse.Parser.parseComplete</code>
   *
   * @return the String representation of the Field
   */
  public String toString()
  {
    // 64 is a reasonable buffer initial size for field
    CPStringBuilder sb = new CPStringBuilder(64);
    Modifier.toString(getModifiers(), sb).append(' ');
    sb.append(ClassHelper.getUserName(getType())).append(' ');
    sb.append(getDeclaringClass().getName()).append('.');
    sb.append(getName());
    return sb.toString();
  }
 
  public String toGenericString()
  {
    CPStringBuilder sb = new CPStringBuilder(64);
    Modifier.toString(getModifiers(), sb).append(' ');
    sb.append(getGenericType()).append(' ');
    sb.append(getDeclaringClass().getName()).append('.');
    sb.append(getName());
    return sb.toString();
  }

  /**
   * Get the value of this Field.  If it is primitive, it will be wrapped
   * in the appropriate wrapper type (boolean = java.lang.Boolean).<p>
   *
   * If the field is static, <code>o</code> will be ignored. Otherwise, if
   * <code>o</code> is null, you get a <code>NullPointerException</code>,
   * and if it is incompatible with the declaring class of the field, you
   * get an <code>IllegalArgumentException</code>.<p>
   *
   * Next, if this Field enforces access control, your runtime context is
   * evaluated, and you may have an <code>IllegalAccessException</code> if
   * you could not access this field in similar compiled code. If the field
   * is static, and its class is uninitialized, you trigger class
   * initialization, which may end in a
   * <code>ExceptionInInitializerError</code>.<p>
   *
   * Finally, the field is accessed, and primitives are wrapped (but not
   * necessarily in new objects). This method accesses the field of the
   * declaring class, even if the instance passed in belongs to a subclass
   * which declares another field to hide this one.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if <code>o</code> is not an instance of
   *         the class or interface declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #getBoolean(Object)
   * @see #getByte(Object)
   * @see #getChar(Object)
   * @see #getShort(Object)
   * @see #getInt(Object)
   * @see #getLong(Object)
   * @see #getFloat(Object)
   * @see #getDouble(Object)
   */
  public Object get(Object o)
    throws IllegalAccessException
  {
    if (type == Double.TYPE)
      return new Double(getDouble0(o));
    if (type == Float.TYPE)
      return new Float(getFloat0(o));
    if (type == Long.TYPE)
      return new Long(getLong0(o));
    if (type == Integer.TYPE)
      return new Integer(getInt0(o));
    if (type == Short.TYPE)
      return new Short(getShort0(o));
    if (type == Byte.TYPE)
      return new Byte(getByte0(o));
    if (type == Character.TYPE)
      return new Character(getChar0(o));
    if (type == Boolean.TYPE)
      return getBoolean0(o) ? Boolean.TRUE : Boolean.FALSE;
    return getObject0(o);
  }

  /**
   * Get the value of this boolean Field. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a boolean field of
   *         <code>o</code>, or if <code>o</code> is not an instance of the
   *         declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public boolean getBoolean(Object o)
    throws IllegalAccessException
  {
    if (type == Boolean.TYPE)
      return getBoolean0(o);
    throw new IllegalArgumentException();
  }

  /**
   * Get the value of this byte Field. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte field of
   *         <code>o</code>, or if <code>o</code> is not an instance of the
   *         declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public byte getByte(Object o)
    throws IllegalAccessException
  {
    if (type == Byte.TYPE)
      return getByte0(o);
    throw new IllegalArgumentException();
  }

  /**
   * Get the value of this Field as a char. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a char field of
   *         <code>o</code>, or if <code>o</code> is not an instance
   *         of the declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public char getChar(Object o)
    throws IllegalAccessException
  {
    if (type == Character.TYPE)
      return getChar0(o);
    throw new IllegalArgumentException();
  }

  /**
   * Get the value of this Field as a short. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte or short
   *         field of <code>o</code>, or if <code>o</code> is not an instance
   *         of the declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public short getShort(Object o)
    throws IllegalAccessException
  {
    if (type == Short.TYPE)
      return getShort0(o);
    return getByte(o);
  }

  /**
   * Get the value of this Field as an int. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte, short, char, or
   *         int field of <code>o</code>, or if <code>o</code> is not an
   *         instance of the declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public int getInt(Object o)
    throws IllegalAccessException
  {
    if (type == Integer.TYPE)
      return getInt0(o);
    if (type == Character.TYPE)
      return getChar0(o);
    return getShort(o);
  }

  /**
   * Get the value of this Field as a long. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte, short, char, int,
   *         or long field of <code>o</code>, or if <code>o</code> is not an
   *         instance of the declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public long getLong(Object o)
    throws IllegalAccessException
  {
    if (type == Long.TYPE)
      return getLong0(o);
    return getInt(o);
  }

  /**
   * Get the value of this Field as a float. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte, short, char, int,
   *         long, or float field of <code>o</code>, or if <code>o</code> is
   *         not an instance of the declaring class of this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public float getFloat(Object o)
    throws IllegalAccessException
  {
    if (type == Float.TYPE)
      return getFloat0(o);
    return getLong(o);
  }

  /**
   * Get the value of this Field as a double. If the field is static,
   * <code>o</code> will be ignored.
   *
   * @param o the object to get the value of this Field from
   * @return the value of the Field
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte, short, char, int,
   *         long, float, or double field of <code>o</code>, or if
   *         <code>o</code> is not an instance of the declaring class of this
   *         field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #get(Object)
   */
  public double getDouble(Object o)
    throws IllegalAccessException
  {
    if (type == Double.TYPE)
      return getDouble0(o);
    return getFloat(o);
  }

  /**
   * Set the value of this Field.  If it is a primitive field, the value
   * will be unwrapped from the passed object (boolean = java.lang.Boolean).<p>
   *
   * If the field is static, <code>o</code> will be ignored. Otherwise, if
   * <code>o</code> is null, you get a <code>NullPointerException</code>,
   * and if it is incompatible with the declaring class of the field, you
   * get an <code>IllegalArgumentException</code>.<p>
   *
   * Next, if this Field enforces access control, your runtime context is
   * evaluated, and you may have an <code>IllegalAccessException</code> if
   * you could not access this field in similar compiled code. This also
   * occurs whether or not there is access control if the field is final.
   * If the field is primitive, and unwrapping your argument fails, you will
   * get an <code>IllegalArgumentException</code>; likewise, this error
   * happens if <code>value</code> cannot be cast to the correct object type.
   * If the field is static, and its class is uninitialized, you trigger class
   * initialization, which may end in a
   * <code>ExceptionInInitializerError</code>.<p>
   *
   * Finally, the field is set with the widened value. This method accesses
   * the field of the declaring class, even if the instance passed in belongs
   * to a subclass which declares another field to hide this one.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if <code>value</code> cannot be
   *         converted by a widening conversion to the underlying type of
   *         the Field, or if <code>o</code> is not an instance of the class
   *         declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #setBoolean(Object, boolean)
   * @see #setByte(Object, byte)
   * @see #setChar(Object, char)
   * @see #setShort(Object, short)
   * @see #setInt(Object, int)
   * @see #setLong(Object, long)
   * @see #setFloat(Object, float)
   * @see #setDouble(Object, double)
   */
  public void set(Object o, Object value)
    throws IllegalAccessException
  {
    checkFinal();
    setInternal(o, value);
  }

  /**
   * Set this boolean Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a boolean field, or if
   *         <code>o</code> is not an instance of the class declaring this
   *         field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setBoolean(Object o, boolean value)
    throws IllegalAccessException
  {
    checkFinal();
    setBooleanInternal(o, value);
  }

  /**
   * Set this byte Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a byte, short, int, long,
   *         float, or double field, or if <code>o</code> is not an instance
   *         of the class declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setByte(Object o, byte value)
    throws IllegalAccessException
  {
    checkFinal();
    setByteInternal(o, value);
  }

  /**
   * Set this char Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a char, int, long,
   *         float, or double field, or if <code>o</code> is not an instance
   *         of the class declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setChar(Object o, char value)
    throws IllegalAccessException
  {
    checkFinal();
    setCharInternal(o, value);
  }

  /**
   * Set this short Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a short, int, long,
   *         float, or double field, or if <code>o</code> is not an instance
   *         of the class declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setShort(Object o, short value)
    throws IllegalAccessException
  {
    checkFinal();
    setShortInternal(o, value);
  }

  /**
   * Set this int Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not an int, long, float, or
   *         double field, or if <code>o</code> is not an instance of the
   *         class declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setInt(Object o, int value)
    throws IllegalAccessException
  {
    checkFinal();
    setIntInternal(o, value);
  }

  /**
   * Set this long Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a long, float, or double
   *         field, or if <code>o</code> is not an instance of the class
   *         declaring this field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setLong(Object o, long value)
    throws IllegalAccessException
  {
    checkFinal();
    setLongInternal(o, value);
  }

  /**
   * Set this float Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a float or long field, or
   *         if <code>o</code> is not an instance of the class declaring this
   *         field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setFloat(Object o, float value)
    throws IllegalAccessException
  {
    checkFinal();
    setFloatInternal(o, value);
  }

  /**
   * Set this double Field. If the field is static, <code>o</code> will be
   * ignored.
   *
   * @param o the object to set this Field on
   * @param value the value to set this Field to
   * @throws IllegalAccessException if you could not normally access this field
   *         (i.e. it is not public)
   * @throws IllegalArgumentException if this is not a double field, or if
   *         <code>o</code> is not an instance of the class declaring this
   *         field
   * @throws NullPointerException if <code>o</code> is null and this field
   *         requires an instance
   * @throws ExceptionInInitializerError if accessing a static field triggered
   *         class initialization, which then failed
   * @see #set(Object, Object)
   */
  public void setDouble(Object o, double value)
    throws IllegalAccessException
  {
    checkFinal();
    setDoubleInternal(o, value);
  }

  /**
   * Return the generic type of the field. If the field type is not a generic
   * type, the method returns the same as <code>getType()</code>.
   *
   * @throws GenericSignatureFormatError if the generic signature does
   *         not conform to the format specified in the Virtual Machine
   *         specification, version 3.
   * @since 1.5
   */
  public Type getGenericType()
  {
    String signature = getSignature();
    if (signature == null)
      return getType();
    FieldSignatureParser p = new FieldSignatureParser(getDeclaringClass(),
                                                      signature);
    return p.getFieldType();
  }

  /**
   * Return the String in the Signature attribute for this field. If there
   * is no Signature attribute, return null.
   */
  private native String getSignature();


  /* The following code has been merged in from Kaffe's Field.java implementation */

  /*
   * Java core library component.
   *
   * Copyright (c) 1997, 1998, 2001
   *      Transvirtual Technologies, Inc.  All rights reserved.
   *
   * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006
   *      Kaffe.org contributors. See ChangeLogs for details.
   *
   * See the file "license.terms" for information on usage and redistribution
   * of this file.
   */

  private void setInternal(Object obj, Object value) 
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type.isPrimitive()) {
      if (value instanceof Boolean) {
	setBooleanInternal(obj, ((Boolean)value).booleanValue());
      }
      else if (value instanceof Byte) {
	setByteInternal(obj, ((Byte)value).byteValue());
      }
      else if (value instanceof Short) {
	setShortInternal(obj, ((Short)value).shortValue());
      }
      else if (value instanceof Character) {
	setCharInternal(obj, ((Character)value).charValue());
      }
      else if (value instanceof Integer) {
	setIntInternal(obj, ((Integer)value).intValue());
      }
      else if (value instanceof Long) {
	setLongInternal(obj, ((Long)value).longValue());
      }
      else if (value instanceof Float) {
	setFloatInternal(obj, ((Float)value).floatValue());
      }
      else {
	setDoubleInternal(obj, ((Double)value).doubleValue());
      }
    }
    else {
      if (value!=null && !type.isInstance(value)) {
	throw new IllegalArgumentException("field type mismatch: Trying to assign a " 
					   + value.getClass().getName() 
					   + " to " + toString());
      }

      setObject0(obj, value);
    }
  }

  public void setBooleanInternal(Object obj, boolean z) 
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type == Boolean.TYPE)
      setBoolean0(obj, z);
    else
      throw new IllegalArgumentException();
  }

  public void setByteInternal(Object obj, byte b)
    throws IllegalArgumentException, IllegalAccessException
  {
    if (type == Byte.TYPE)
      setByte0(obj, b);
    else 
      setShortInternal(obj, b);
  }

  public void setCharInternal(Object obj, char c) 
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type == Character.TYPE)
      setChar0(obj, c);
    else
      setIntInternal(obj, c);
  }

  public void setDoubleInternal(Object obj, double d) 
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type == Double.TYPE)
      setDouble0(obj, d);
    else
      throw new IllegalArgumentException();
  }

  public void setFloatInternal(Object obj, float f)
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type == Float.TYPE)
      setFloat0(obj, f);
    else
      setDoubleInternal(obj, f);
  }

  public void setIntInternal(Object obj, int i)
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type == Integer.TYPE)
      setInt0(obj, i);
    else
      setLongInternal(obj, i);
  }

  public void setLongInternal(Object obj, long l)
    throws IllegalArgumentException, IllegalAccessException 
  {
    if (type == Long.TYPE)
      setLong0(obj, l);
    else
      setFloatInternal(obj, l);
  }

  public void setShortInternal(Object obj, short s)
    throws IllegalArgumentException, IllegalAccessException
  {
    if (type == Short.TYPE)
      setShort0(obj, s);
    else
      setIntInternal(obj, s);
  }

  private void checkFinal()
    throws IllegalAccessException
  {
    if (Modifier.isFinal(getModifiers()) && !flag) 
    {
	throw new IllegalAccessException("trying to set final field " 
					 + toString());
    }
  }

  private native boolean getBoolean0(Object obj);
  private native byte getByte0(Object obj);
  private native char getChar0(Object obj);
  private native short getShort0(Object obj);
  private native int getInt0(Object obj);
  private native long getLong0(Object obj);
  private native float getFloat0(Object obj);
  private native double getDouble0(Object obj);
  private native Object getObject0(Object obj);
  
  private native void setBoolean0(Object obj, boolean v);
  private native void setByte0(Object obj, byte v);
  private native void setChar0(Object obj, char v);
  private native void setShort0(Object obj, short v);
  private native void setInt0(Object obj, int v);
  private native void setLong0(Object obj, long v);
  private native void setFloat0(Object obj, float v);
  private native void setDouble0(Object obj, double v);
  private native void setObject0(Object obj, Object v);
}
