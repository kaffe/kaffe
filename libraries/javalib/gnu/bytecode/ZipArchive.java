// Copyright (c) 1997  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;
import java.io.*;
import java.util.zip.*;

/** A class to manipulate a .zip archive.
 * Does not handle compression/uncompression, though that could be added.
 * When used an an application. provides a simplified tar-like interface.
 * @author	Per Bothner <bothner@cygnus.com>
 */

public class ZipArchive
{
  private static void usage ()
  {
    System.err.println ("zipfile [ptxq] archive [file ...]");
    System.exit (-1);
  }

  public static long copy(InputStream in, OutputStream out, byte[] buffer)
    throws IOException
  {
    long total = 0;
    for (;;)
      {
	int count = in.read(buffer);
	if (count <= 0)
	  return total;
	out.write(buffer, 0, count); 
	total += count;
      }
  }

  public static void copy(InputStream in, String name, byte[] buffer)
    throws IOException
    {
    File f = new File(name);
    String dir_name = f.getParent();
    if (dir_name != null)
      {
	File dir = new File(dir_name);
	if (! dir.exists())
	  System.err.println("mkdirs:"+dir.mkdirs());
      }
    if (name.charAt (name.length () - 1) != '/')
      {
	OutputStream out = new BufferedOutputStream(new FileOutputStream(f));
	copy(in, out, buffer);
	out.close ();
      }
  }

  /**
   * Manipulate a .zip archive using a tar-like interface.
   * <p>
   * Usage:  <code>ZipArchive</code> <var>command archive</var> [<var>file</var> ...]
   * <dl>
   * <dt><code>ZipArchive t</code> <var>archive file</var> ...<dd>
   *   List information about the named members of the archive.
   * <dt><code>ZipArchive x</code> <var>archive file</var> ...<dd>
   *   Extract the named members from the archive.
   * <dt><code>ZipArchive p</code> <var>archive file</var> ...<dd>
   *   Print the named members from the archive on standard output.
   *   Prints just the raw contents, with no headers or conversion.
   * <dt><code>ZipArchive</code> [<code>ptx</code>] <var>archive</var><dd>
   *   With no arguments, does each command for every member in the archive.
   * <dt><code>ZipArchive q</code> <var>archive file</var> ...<dd>
   *   Add the named files to the end of archive.
   *   Does not check for duplicates.
   * </dl>
   */

  public static void main (String args[]) throws IOException
  {
    if (args.length < 2)
      usage ();
    String command = args[0];
    String archive_name = args[1];

    try
      {
	if (command.equals ("t")
	    || command.equals ("p")
	    || command.equals ("x"))
	  {
	    PrintStream out = System.out;
	    byte[] buf = new byte[1024];
	    if (args.length == 2)
	      {
		BufferedInputStream in
		  = new BufferedInputStream(new FileInputStream(archive_name));
		ZipInputStream zin = new ZipInputStream (in);
		ZipEntry zent;
		while ((zent = zin.getNextEntry()) != null)
		  {
		    String name = zent.getName();
		    if (command.equals("t"))
		      {
			out.print(name);
			out.print(" size: ");
			out.println(zent.getSize());
		      }
		    else if (command.equals("p"))
		      {
			copy(zin, out, buf);
		      }
		    else // commend.equals("x")
		      {
			copy(zin, name, buf);
		      }
		  }
	      }
	    else
	      {
		ZipFile zar = new ZipFile(archive_name);
		for (int i = 2;  i < args.length; i++)
		  {
		    String name = args[i];
		    ZipEntry zent = zar.getEntry(name);
		    if (zent == null)
		      {
			System.err.println ("zipfile " + archive_name + ":" +
					    args[i] + " - not found");
			System.exit (-1);
		      }
		    else if (command.equals("t"))
		      {
			out.print(name);
			out.print(" size: ");
			out.println(zent.getSize());
		      }
		    else if (command.equals("p"))
		      {
			copy(zar.getInputStream(zent), out, buf);
		      }
		    else // commend.equals("x")
		      {
			copy(zar.getInputStream(zent), name, buf);
		      }
		  }
	      }
	  }
	else if (command.equals ("q"))
	  {
	    ZipOutputStream zar
	      = new ZipOutputStream(new FileOutputStream(archive_name));
	    for  (int i = 2;  i < args.length; i++)
	      {
		File in = new File (args[i]);
		if (!in.exists ())
		  throw new IOException (args[i] + " - not found");
		if (!in.canRead ())
		  throw new IOException (args[i] + " - not readable");
		int size = (int) in.length ();
		FileInputStream fin = new FileInputStream (in);
		byte[] contents = new byte[size];
		if (fin.read (contents) != size)
		  throw new IOException (args[i] + " - read error");
		fin.close ();

		ZipEntry ze = new ZipEntry(args[i]);
		ze.setSize(size);
		ze.setTime(in.lastModified());
		zar.putNextEntry(ze);
		zar.write(contents, 0, size);
	      }
	    zar.close ();
	  }
	else
	  usage ();
      }
    catch (IOException ex)
      {
	System.err.println ("I/O Exception:  " + ex);
      }
  }
}
