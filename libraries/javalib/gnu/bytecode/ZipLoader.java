// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/** Load classes from a Zip archive.
 * @author	Per Bothner
 */

public class ZipLoader extends ClassLoader
{
  /** The zip archive from which we will load the classes.
   * The format of the archive is the same as classes.zip. */
  java.util.zip.ZipFile zar;

  /** Number of classes managed by this loader. */
  int size;

  /** name of ZipFile */
  private String zipname;

  /* A list of pairs of (name, class) of already loaded classes. */
  private java.util.Vector loadedClasses;

  public ZipLoader (String name) throws java.io.IOException
  {
    this.zipname=name;
    this.zar = new java.util.zip.ZipFile(name);
    size = 0;
    java.util.Enumeration e = this.zar.entries();
    while (e.hasMoreElements())
      {
	java.util.zip.ZipEntry ent = (java.util.zip.ZipEntry) e.nextElement();
	if (! ent.isDirectory())
	  size++;
      }
    loadedClasses = new java.util.Vector(size);
  }

  public Class loadClass (String name, boolean resolve)
       throws ClassNotFoundException
  {
    Class clas;
    int index = loadedClasses.indexOf(name);
    if (index >= 0)
      clas = (Class) loadedClasses.elementAt(index+1);
    else if (zar == null && loadedClasses.size() == 2*size)
      clas = Class.forName(name);
    else
      {
	boolean reopened=false;
	String member_name = name.replace ('.', '/') + ".class";
	if (this.zar == null)
	  {
	    try {
	      this.zar=new java.util.zip.ZipFile(zipname);
	      reopened=true;
	    }
	    catch (java.io.IOException ex)
	      {
		throw new
		  ClassNotFoundException ("IOException while loading "
					  + member_name + " from ziparchive \""
					  + name + "\": " + ex.toString ());
	      }
	  }
	java.util.zip.ZipEntry member = zar.getEntry(member_name);
	if (member == null) {
	  if (reopened) {
	    try {
	      close();
	    } catch (java.io.IOException e) {
	      throw new RuntimeException("failed to close \""+zipname+"\"");
	    }
	  }
	  clas = Class.forName(name);
	}
	else
	  {
	    try
	      {
		int member_size = (int) member.getSize();
		java.io.InputStream strm = zar.getInputStream(member);
		byte[] bytes = new byte[member_size];
		new java.io.DataInputStream(strm).readFully(bytes);
		clas = defineClass (name, bytes, 0, member_size);
		loadedClasses.addElement(name);
		loadedClasses.addElement(clas);
		if (2 * size == loadedClasses.size())
		  close();
	      }
	    catch (java.io.IOException ex)
	      {
		throw new
		  ClassNotFoundException ("IOException while loading "
					  + member_name + " from ziparchive \""
					  + name + "\": " + ex.toString ());
	      }
	  }
      }

    if (resolve)
      resolveClass (clas);
    return clas;
  }

  /** Load all classes immediately from zip archive, close archive. */
  public void loadAllClasses ()
    throws java.io.IOException
  {
    java.util.Enumeration e = this.zar.entries();
    while (e.hasMoreElements())
      {
	java.util.zip.ZipEntry member =
	  (java.util.zip.ZipEntry) e.nextElement();
	String name=member.getName().replace('/','.');
	name=name.substring(0,name.length()-"/class".length());
	int member_size = (int) member.getSize();
	java.io.InputStream strm = zar.getInputStream(member);
	byte[] bytes = new byte[member_size];
	new java.io.DataInputStream(strm).readFully(bytes);
	Class clas = defineClass (name, bytes, 0, member_size);
	loadedClasses.addElement(name);
	loadedClasses.addElement(clas);
      }
    close();
  }

  /** Close the zip archive - loadClass will reopen if necessary. */
  public void close()
    throws java.io.IOException
  {
    if (zar != null)
      zar.close ();
    zar = null;
  }

}
