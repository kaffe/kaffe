// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;

/**
 * An entry in the constant pool for a ClassType.
 * Each entry belong to the constant pool table of the "owning"
 * ClassType.  Hashing is used to make each entry unique (with a ClassType).
 * By convention, each sub-class has a static get_const method which is
 * used when a contant pool entry is need:  The get_const method will
 * return an existing matching entry if possible, or allocate a new
 * one if needed.
 * @author	Per Bothner
 */

abstract public class CpoolEntry
{
  /** A hashvalue so we do not get duplicate constant pool entries. */
  int hash;

  /** This entry's index in the constant pool. */
  public int index;

  public int getIndex() { return index; }

  /** The next entry in the same hash bucket
   * (of the owning ConstantPool's hashTab). */
  CpoolEntry next;

  public abstract int getTag();

  public int hashCode () { return hash; }

  abstract void write(DataOutputStream str)
       throws java.io.IOException;

  /**
   * Enter current element into cpool.hashTab.
   */
  void add_hashed (ConstantPool cpool)
  {
    CpoolEntry[] hashTab = cpool.hashTab;
    int index = (hash & 0x7FFFFFFF) % hashTab.length;
    next = hashTab[index];
    hashTab[index] = this;
  }

  protected CpoolEntry () { }

  public CpoolEntry (ConstantPool cpool, int h)
  {
     hash = h;
     if (cpool.locked)
       throw new Error("adding new entry to locked contant pool");
     index = ++cpool.count;

     // (Re-)allocate the cpool.pool array if need be.
     if (cpool.pool == null)
	cpool.pool = new CpoolEntry[60];
     else if (index >= cpool.pool.length)
       {
	 int old_size = cpool.pool.length;
	 int new_size = 2 * cpool.pool.length;
	 CpoolEntry[] new_pool = new CpoolEntry[new_size];
	 for (int i = 0; i < old_size; i++)
	   new_pool[i] = cpool.pool[i];
	 cpool.pool = new_pool;
       }

     // Re-hash cpool.hashTab hash_table if needed.
     if (cpool.hashTab == null || index >= 0.60 * cpool.hashTab.length)
       cpool.rehash();

     // Enter into cpool.constant_pool array.
     cpool.pool[index] = this;
     // Enter into cpool.hashTab hash table.
     add_hashed (cpool);
  }

  /** Print this constant pool entry.
   * If verbosity==0, print very tersely (no extraneous text).
   * If verbosity==1, prefix the type of the constant.
   * If verbosity==2, add more descriptive text. */

  public abstract void print (ClassTypeWriter dst, int verbosity);
};
