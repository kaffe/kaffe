// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** Manages a pool of constants, as used in .class files and Java interpreters.
 * @author Per Bothner
 */

public class ConstantPool
{
  static public final byte CLASS = 7;
  static public final byte FIELDREF = 9;
  static public final byte METHODREF = 10;
  static public final byte INTERFACE_METHODREF = 11;
  static public final byte STRING = 8;
  static public final byte INTEGER = 3;
  static public final byte FLOAT = 4;
  static public final byte LONG = 5;
  static public final byte DOUBLE = 6;
  static public final byte NAME_AND_TYPE = 12;
  static public final byte UTF8 = 1;

  /** The entries in the constant pool.
   * The first element (constant_pool[0]) is an unused dummy. */
  CpoolEntry[] pool;

  /** Number of elements in the constant pool, not counting
   * the initial dummy element (with index 0). */
  int count;

  public final int getCount()
  {
    return count;
  }

  /**
   * Get the index'th entry in pool.
   * Will throw ArrayIndexOutOfBoundsException on an invalid index
   */
  public final CpoolEntry getPoolEntry(int index)
  {
    return pool[index];
  }

  boolean locked;

  CpoolEntry[] hashTab;

  void rehash ()
  {
    if (hashTab == null && count > 0)
      {
	// Entries may not have been hashed before.  Make sure they are now.
	for (int i = pool.length;  --i >= 0; )
	  {
	    CpoolEntry entry = pool[i];
	    // Make sure entry.hash is not the default value 0.
	    if (entry != null)
	      entry.hashCode();
	  }
      }

    hashTab = new CpoolEntry[count < 5 ? 101 : 2 * count];
    if (pool != null)
      {
	for (int i = pool.length;  --i >= 0; )
	  {
	    CpoolEntry entry = pool[i];
	    if (entry != null)
	      entry.add_hashed (this);
	  }
      }
  }

  public CpoolUtf8 addUtf8 (String s)
  {
    s = s.intern();
    int h = s.hashCode();

    // Check if we already have a matching CONSTANT_Utf8.
    if (hashTab == null)
      rehash();

    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash && entry instanceof CpoolUtf8)
	  {
	    CpoolUtf8 utf = (CpoolUtf8) entry;
	    if (utf.string == s)
	      return utf;
	  }
      }
    if (locked)
      throw new Error("adding new Utf8 entry to locked contant pool: "+s);
    return new CpoolUtf8(this, h, s);
  }

  public CpoolClass addClass (ObjectType otype)
  {
    return addClass(addUtf8(otype.getInternalName()));
  }

  public CpoolClass addClass (String name)
  {
    return addClass(addUtf8(name.replace('.', '/')));
  }

  public CpoolClass addClass (CpoolUtf8 name)
  {
    int h = CpoolClass.hashCode(name);

    // Check if we already have a matching CONSTANT_Class.
    if (hashTab == null)
      rehash();
    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash && entry instanceof CpoolClass)
	  {
	    CpoolClass ent = (CpoolClass) entry;
	    if (ent.name == name)
	      return ent;
	  }
      }
    return new CpoolClass (this, h, name);
  }

  CpoolValue1 addValue1 (int tag, int val)
  {
    int h = CpoolValue1.hashCode(val);

    // Check if we already have a matching CONSTANT_Integer.
    if (hashTab == null)
      rehash();
    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash && entry instanceof CpoolValue1)
	  {
	    CpoolValue1 ent = (CpoolValue1) entry;
	    if (ent.tag == tag && ent.value == val)
	      return ent;
	  }
      }
    return new CpoolValue1 (this, tag, h, val);
  }

  CpoolValue2 addValue2 (int tag, long val)
  {
    int h = CpoolValue2.hashCode(val);

    // Check if we already have a matching CONSTANT_Integer.
    if (hashTab == null)
      rehash();
    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash && entry instanceof CpoolValue2)
	  {
	    CpoolValue2 ent = (CpoolValue2) entry;
	    if (ent.tag == tag && ent.value == val)
	      return ent;
	  }
      }
    return new CpoolValue2 (this, tag, h, val);
  }

  public CpoolValue1 addInt (int val)
  {
    return addValue1(INTEGER, val);
  }

  public CpoolValue2 addLong (long val)
  {
    return addValue2(LONG, val);
  }

  public CpoolValue1 addFloat (float val)
  {
    return addValue1(FLOAT, Float.floatToIntBits(val));
  }

  public CpoolValue2 addDouble (double val)
  {
    return addValue2(DOUBLE, Double.doubleToLongBits(val));
  }

  public final CpoolString addString (String string)
  {
    return addString(addUtf8(string));
  }

  public CpoolString addString (CpoolUtf8 str)
  {
    int h = CpoolString.hashCode (str);

    // Check if we already have a matching CONSTANT_String.
    if (hashTab == null)
      rehash();
    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash && entry instanceof CpoolString)
	  {
	    CpoolString ent = (CpoolString) entry;
	    if (ent.str == str)
	      return ent;
	  }
      }
    return new CpoolString (this, h, str);
  }

  public CpoolNameAndType addNameAndType (Method method)
  {
    CpoolUtf8 name = addUtf8(method.getName());
    CpoolUtf8 type = addUtf8(method.getSignature ());
    return addNameAndType(name, type);
  }

  public CpoolNameAndType addNameAndType (Field field)
  {
    CpoolUtf8 name = addUtf8(field.getName());
    CpoolUtf8 type = addUtf8(field.getSignature ());
    return addNameAndType(name, type);
  }

  public CpoolNameAndType
  addNameAndType (CpoolUtf8 name, CpoolUtf8 type)
  {
    int h = CpoolNameAndType.hashCode (name, type);

    // Check if we already have a matching CONSTANT_Integer.
    if (hashTab == null)
      rehash();
    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash
	    && entry instanceof CpoolNameAndType
	    && ((CpoolNameAndType)entry).name == name
	    && ((CpoolNameAndType)entry).type == type)
	  return (CpoolNameAndType)entry;
      }
    return new CpoolNameAndType(this, h, name, type);
  }

  public CpoolRef
  addRef (int tag, CpoolClass clas, CpoolNameAndType nameAndType)
  {
    int h = CpoolRef.hashCode (clas, nameAndType);

    // Check if we already have a matching CONSTANT_Integer.
    if (hashTab == null)
      rehash();
    int index = (h & 0x7FFFFFFF) % hashTab.length;
    for (CpoolEntry entry = hashTab[index]; entry != null; entry = entry.next)
      {
	if (h == entry.hash && entry instanceof CpoolRef)
	  {
	    CpoolRef ref = (CpoolRef) entry;
	    if (ref.tag == tag
		&& ref.clas == clas
		&& ref.nameAndType== nameAndType)
	      return ref;
	  }
      }
    return new CpoolRef (this, h, tag, clas, nameAndType);
  }

  public CpoolRef addMethodRef (Method method)
  {
    CpoolClass clas = addClass(method.classfile.this_name);
    int tag;
    if ((method.getDeclaringClass().getModifiers() & Access.INTERFACE) == 0)
      tag = 10; // CONSTANT_Methodref
    else
      tag = 11; // CONSTANT_InterfaceMethodref
    CpoolNameAndType nameType = addNameAndType(method);
    return addRef(tag, clas, nameType);
  }

  public CpoolRef addFieldRef (Field field)
  {
    CpoolClass clas = addClass(field.owner.this_name);
    int tag = 9;  // CONSTANT_Fieldref
    CpoolNameAndType nameType = addNameAndType(field);
    return addRef(tag, clas, nameType);
  }

  void write (java.io.DataOutputStream dstr) throws java.io.IOException
  {
    dstr.writeShort(count+1);
    for (int i = 1; i <= count; i++)
      {
	CpoolEntry entry = pool[i];
	if (entry != null)
	  entry.write (dstr);
      }
    locked = true;
  }

  /** Get or create a CpoolEntry at a given index.
   * If there is an existing entry, it must match the tag.
   * If not, a new one is create of the class appropriate for the tag.
   */
  CpoolEntry getForced(int index, int tag)
  {
    index = index & 0xffff;
    CpoolEntry entry = pool[index];
    if (entry == null)
      {
	if (locked)
	  throw new Error("adding new entry to locked contant pool");
	switch (tag)
	  {
	  case UTF8: entry = new CpoolUtf8(); break;
	  case INTEGER:
	  case FLOAT: entry = new CpoolValue1(tag);  break;
	  case LONG:
	  case DOUBLE: entry = new CpoolValue2(tag);  break;
	  case CLASS: entry = new CpoolClass(); break;
	  case STRING: entry = new CpoolString(); break;
	  case FIELDREF:
	  case METHODREF:
	  case INTERFACE_METHODREF: entry = new CpoolRef(tag); break;
	  case NAME_AND_TYPE: entry = new CpoolNameAndType();
	  }
	pool[index] = entry;
	entry.index = index;
      }
    else if (entry.getTag() != tag)
      throw new ClassFormatError("conflicting constant pool tags at "+index);
    return entry;
  }

  public ConstantPool () { }

  public ConstantPool (java.io.DataInputStream dstr)
       throws java.io.IOException
  {
    count = dstr.readUnsignedShort() - 1;
    pool = new CpoolEntry[count+1];
    for (int i = 1;  i <= count;  i++)
      {
	byte tag = dstr.readByte();
	CpoolEntry entry = getForced(i, tag);
	switch (tag)
	  {
	  case UTF8:
	    ((CpoolUtf8) entry).string = dstr.readUTF();
	    break;
	  case INTEGER:
	  case FLOAT:
	    ((CpoolValue1) entry).value = dstr.readInt();
	    break;
	  case LONG:
	  case DOUBLE:
	    ((CpoolValue2) entry).value = dstr.readLong();
	    i++;
	    break;
	  case CLASS:
	    ((CpoolClass) entry).name =
	      (CpoolUtf8) getForced(dstr.readUnsignedShort(), UTF8);
	    break;
	  case STRING:
	    ((CpoolString) entry).str =
	      (CpoolUtf8) getForced(dstr.readUnsignedShort(), UTF8);
	    break;
	  case FIELDREF:
	  case METHODREF:
	  case INTERFACE_METHODREF:
	    CpoolRef ref = (CpoolRef) entry;
	    ref.clas = (CpoolClass) getForced(dstr.readUnsignedShort(), CLASS);
	    ref.nameAndType = (CpoolNameAndType)
	      getForced(dstr.readUnsignedShort(), NAME_AND_TYPE);
	    break;
	  case NAME_AND_TYPE:
	    CpoolNameAndType ntyp = (CpoolNameAndType) entry;
	    ntyp.name = (CpoolUtf8) getForced(dstr.readUnsignedShort(), UTF8);
	    ntyp.type = (CpoolUtf8) getForced(dstr.readUnsignedShort(), UTF8);
	    break;
	  }
      }
  }
}
