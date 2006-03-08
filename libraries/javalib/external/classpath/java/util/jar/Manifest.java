/* Manifest.java -- Reads, writes and manipulaties jar manifest files
   Copyright (C) 2000, 2004 Free Software Foundation, Inc.

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

package java.util.jar;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;

/**
 * Reads, writes and manipulaties jar manifest files.
 * XXX
 * 
 * @since 1.2
 * @author Mark Wielaard (mark@klomp.org)
 */
public class Manifest implements Cloneable
{
  // Fields

  /** Platform-independent line-ending. */
  private static final byte[] CRLF = new byte[] { 0x0D, 0x0A };

  /** The main attributes of the manifest (jar file). */
  private final Attributes mainAttr;

  /** A map of atrributes for all entries described in this Manifest. */
  private final Map entries;

  // Constructors

  /**
   * Creates a new empty Manifest.
   */
  public Manifest()
  {
    mainAttr = new Attributes();
    entries = new Hashtable();
  }

  /**
   * Creates a Manifest from the supplied input stream.
   *
   * @see #read(InputStream)
   * @see #write(OutputStream)
   *
   * @param in the input stream to read the manifest from
   * @exception IOException when an i/o exception occurs or the input stream
   * does not describe a valid manifest
   */
  public Manifest(InputStream in) throws IOException
  {
    this();
    read(in);
  }

  /**
   * Creates a Manifest from another Manifest.
   * Makes a deep copy of the main attributes, but a shallow copy of
   * the other entries. This means that you can freely add, change or remove
   * the main attributes or the entries of the new manifest without effecting
   * the original manifest, but adding, changing or removing attributes from
   * a particular entry also changes the attributes of that entry in the
   * original manifest.
   *
   * @see #clone()
   * @param man the Manifest to copy from
   */
  public Manifest(Manifest man)
  {
    mainAttr = new Attributes(man.getMainAttributes());
    entries = new Hashtable(man.getEntries());
  }

  // Methods

  /**
   * Gets the main attributes of this Manifest.
   */
  public Attributes getMainAttributes()
  {
    return mainAttr;
  }

  /**
   * Gets a map of entry Strings to Attributes for all the entries described
   * in this manifest. Adding, changing or removing from this entries map
   * changes the entries of this manifest.
   */
  public Map getEntries()
  {
    return entries;
  }

  /**
   * Returns the Attributes associated with the Entry.
   * <p>
   * Implemented as:
   * <code>return (Attributes)getEntries().get(entryName)</code>
   *
   * @param entryName the name of the entry to look up
   * @return the attributes associated with the entry or null when none
   */
  public Attributes getAttributes(String entryName)
  {
    return (Attributes) getEntries().get(entryName);
  }

  /**
   * Clears the main attributes and removes all the entries from the
   * manifest.
   */
  public void clear()
  {
    mainAttr.clear();
    entries.clear();
  }

  /**
   * Read and merge a <code>Mainfest</code> from the designated input stream.
   * 
   * @param in the input stream to read from.
   * @throws IOException if an I/O related exception occurs during the process.
   */
  public void read(InputStream in) throws IOException
  {
    BufferedReader br = new BufferedReader(new InputStreamReader(in, "UTF-8"));
    read_main_section(getMainAttributes(), br);
    read_individual_sections(getEntries(), br);
  }

  // Private Static methods for reading the Manifest file from BufferedReader

  private static void read_main_section(Attributes attr,
					BufferedReader br) throws IOException
  {
    // According to the spec we should actually call read_version_info() here.
    read_attributes(attr, br);
    // Explicitly set Manifest-Version attribute if not set in Main
    // attributes of Manifest.
    if (attr.getValue(Attributes.Name.MANIFEST_VERSION) == null)
	    attr.putValue(Attributes.Name.MANIFEST_VERSION, "0.0");
  }

  /**
   * Pedantic method that requires the next attribute in the Manifest to be
   * the "Manifest-Version". This follows the Manifest spec closely but
   * reject some jar Manifest files out in the wild.
   */
  private static void read_version_info(Attributes attr,
					BufferedReader br) throws IOException
  {
    String version_header = Attributes.Name.MANIFEST_VERSION.toString();
    try
      {
	String value = expect_header(version_header, br);
	attr.putValue(Attributes.Name.MANIFEST_VERSION, value);
      }
    catch (IOException ioe)
      {
	throw new JarException("Manifest should start with a " +
			       version_header + ": " + ioe.getMessage());
      }
  }

  private static String expect_header(String header, BufferedReader br)
    throws IOException
  {
    String s = br.readLine();
    if (s == null)
      {
	throw new JarException("unexpected end of file");
      }
    return expect_header(header, br, s);
  }

  private static String expect_header(String header, BufferedReader br,
				      String s) throws IOException
  {
    try
      {
	String name = s.substring(0, header.length() + 1);
	if (name.equalsIgnoreCase(header + ":"))
	  {
	    String value_start = s.substring(header.length() + 2);
	    return read_header_value(value_start, br);
	  }
      }
    catch (IndexOutOfBoundsException iobe)
      {
      }
    // If we arrive here, something went wrong
    throw new JarException("unexpected '" + s + "'");
  }

  private static String read_header_value(String s, BufferedReader br)
    throws IOException
  {
    boolean try_next = true;
    while (try_next)
      {
	// Lets see if there is something on the next line
	br.mark(1);
	if (br.read() == ' ')
	  {
	    s += br.readLine();
	  }
	else
	  {
	    br.reset();
	    try_next = false;
	  }
      }
    return s;
  }

  private static void read_attributes(Attributes attr,
				      BufferedReader br) throws IOException
  {
    String s = br.readLine();
    while (s != null && (!s.equals("")))
      {
	read_attribute(attr, s, br);
	s = br.readLine();
      }
  }

  private static void read_attribute(Attributes attr, String s,
				     BufferedReader br) throws IOException
  {
    try
      {
	int colon = s.indexOf(": ");
	String name = s.substring(0, colon);
	String value_start = s.substring(colon + 2);
	String value = read_header_value(value_start, br);
	attr.putValue(name, value);
      }
    catch (IndexOutOfBoundsException iobe)
      {
	throw new JarException("Manifest contains a bad header: " + s);
      }
  }

  private static void read_individual_sections(Map entries,
					       BufferedReader br) throws
    IOException
  {
    String s = br.readLine();
    while (s != null && (!s.equals("")))
      {
	Attributes attr = read_section_name(s, br, entries);
	read_attributes(attr, br);
	s = br.readLine();
      }
  }

  private static Attributes read_section_name(String s, BufferedReader br,
					      Map entries) throws JarException
  {
    try
      {
	String name = expect_header("Name", br, s);
	Attributes attr = new Attributes();
	entries.put(name, attr);
	return attr;
      }
    catch (IOException ioe)
      {
	throw new JarException
	  ("Section should start with a Name header: " + ioe.getMessage());
      }
  }

  /**
   * Writes the contents of this <code>Manifest</code> to the designated
   * output stream. Line-endings are platform-independent and consist of the
   * 2-codepoint sequence <code>0x0D</code> and <code>0x0A</code>.
   * 
   * @param out the output stream to write this <code>Manifest</code> to.
   * @throws IOException if an I/O related exception occurs during the process.
   */
  public void write(OutputStream out) throws IOException
  {
    BufferedOutputStream bos = out instanceof BufferedOutputStream
                               ? (BufferedOutputStream) out
                               : new BufferedOutputStream(out, 4096);
    write_main_section(getMainAttributes(), bos);
    bos.write(CRLF);
    write_individual_sections(getEntries(), bos);
    bos.flush();
  }

  // Private Static functions for writing the Manifest file to a PrintWriter

  private static void write_main_section(Attributes attr, OutputStream out)
    throws IOException
  {
    write_version_info(attr, out);
    write_main_attributes(attr, out);
  }

  private static void write_version_info(Attributes attr, OutputStream out)
    throws IOException
  {
    // First check if there is already a version attribute set
    String version = attr.getValue(Attributes.Name.MANIFEST_VERSION);
    if (version == null)
      {
	version = "1.0";
      }
    write_header(Attributes.Name.MANIFEST_VERSION.toString(), version, out);
  }

  /**
   * The basic method for writing <code>Mainfest</code> attributes. This
   * implementation respects the rule stated in the Jar Specification concerning
   * the maximum allowed line length; i.e.
   * 
   * <pre>
   * No line may be longer than 72 bytes (not characters), in its UTF8-encoded
   * form. If a value would make the initial line longer than this, it should
   * be continued on extra lines (each starting with a single SPACE).
   * </pre>
   * and
   * <pre>
   * Because header names cannot be continued, the maximum length of a header
   * name is 70 bytes (there must be a colon and a SPACE after the name).
   * </pre>
   * 
   * @param name the name of the attribute.
   * @param value the value of the attribute.
   * @param out the output stream to write the attribute's name/value pair to.
   * @throws IOException if an I/O related exception occurs during the process.
   */
  private static void write_header(String name, String value, OutputStream out)
    throws IOException
  {
    String target = name + ": ";
    byte[] b = target.getBytes("UTF-8");
    if (b.length > 72)
      throw new IOException("Attribute's name already longer than 70 bytes");

    if (b.length == 72)
      {
        out.write(b);
        out.write(CRLF);
        target = " " + value;
      }
    else
      target = target + value;

    int n;
    while (true)
      {
        b = target.getBytes("UTF-8");
        if (b.length < 73)
        {
          out.write(b);
          break;
        }

        // find an appropriate character position to break on
        n = 72;
        while (true)
          {
            b = target.substring(0, n).getBytes("UTF-8");
            if (b.length < 73)
              break;

            n--;
            if (n < 1)
              throw new IOException("Header is unbreakable and longer than 72 bytes");
          }

        out.write(b);
        out.write(CRLF);
        target = " " + target.substring(n);
      }

    out.write(CRLF);
  }

  private static void write_main_attributes(Attributes attr, OutputStream out) 
    throws IOException
  {
    Iterator it = attr.entrySet().iterator();
    while (it.hasNext())
      {
	Map.Entry entry = (Map.Entry) it.next();
	// Don't print the manifest version again
	if (!Attributes.Name.MANIFEST_VERSION.equals(entry.getKey()))
      write_attribute_entry(entry, out);
      }
  }

  private static void write_attribute_entry(Map.Entry entry, OutputStream out) 
    throws IOException
  {
    String name = entry.getKey().toString();
    String value = entry.getValue().toString();

    if (name.equalsIgnoreCase("Name"))
      {
	throw new JarException("Attributes cannot be called 'Name'");
      }
    if (name.startsWith("From"))
      {
	throw new
	  JarException("Header cannot start with the four letters 'From'" +
		       name);
      }
    write_header(name, value, out);
  }

  private static void write_individual_sections(Map entries, OutputStream out)
    throws IOException
  {

    Iterator it = entries.entrySet().iterator();
    while (it.hasNext())
      {
        Map.Entry entry = (Map.Entry) it.next();
        write_header("Name", entry.getKey().toString(), out);
        write_entry_attributes((Attributes) entry.getValue(), out);
        out.write(CRLF);
      }
  }

  private static void write_entry_attributes(Attributes attr, OutputStream out) 
    throws IOException
  {
    Iterator it = attr.entrySet().iterator();
    while (it.hasNext())
      {
	Map.Entry entry = (Map.Entry) it.next();
	write_attribute_entry(entry, out);
      }
  }

  /**
   * Makes a deep copy of the main attributes, but a shallow copy of
   * the other entries. This means that you can freely add, change or remove
   * the main attributes or the entries of the new manifest without effecting
   * the original manifest, but adding, changing or removing attributes from
   * a particular entry also changes the attributes of that entry in the
   * original manifest. Calls <CODE>new Manifest(this)</CODE>.
   */
  public Object clone()
  {
    return new Manifest(this);
  }

  /**
   * Checks if another object is equal to this Manifest object.
   * Another Object is equal to this Manifest object if it is an instance of
   * Manifest and the main attributes and the entries of the other manifest
   * are equal to this one.
   */
  public boolean equals(Object o)
  {
    return (o instanceof Manifest) &&
      (mainAttr.equals(((Manifest) o).mainAttr)) &&
      (entries.equals(((Manifest) o).entries));
  }

  /**
   * Calculates the hash code of the manifest. Implemented by a xor of the
   * hash code of the main attributes with the hash code of the entries map.
   */
  public int hashCode()
  {
    return mainAttr.hashCode() ^ entries.hashCode();
  }

}
