/* Class : Manifest

  Implementation of the 1.2 Java class Manifest.

  Copyright : Moses DeJong, dejong@cs.umn.edu, 1998.
  Source code licensed under the GPL.
  You can get a copy of the license from www.gnu.org.

  This code is intended for use in the Kaffe project but you can use
  it in other projects as long as you follow the license rules.
*/

package java.util.jar;

import java.io.*;
import java.util.*;

public class Manifest implements Cloneable
{
    // Create empty manifest object

    public Manifest() {
	//throw new RuntimeException("not implemented");
    }

    // Create manifest from data in a stream

    public Manifest(InputStream in) throws IOException
    {
	//throw new RuntimeException("not implemented");
	read(in);	
    }
    
    // Create manifest by copying data in manifest argument

    public Manifest(Manifest man)
    {
	throw new RuntimeException("not implemented");

	// FIXME : need to copy both the main attributes and the entires
    }
    
    public Attributes getMainAttributes()
    {
	//throw new RuntimeException("not implemented");
	return main;
    }

    public Map getEntries()
    {
	//throw new RuntimeException("not implemented");
	return entries;
    }
    
    public Attributes getAttributes(String name)
    {
	throw new RuntimeException("not implemented");

	// FIXME : how do these differ from the main attributes ?
    }

    public void clear()
    {
	//throw new RuntimeException("not implemented");

	// JDK docs says : clear out the main Attributes and the entries
	
	main.clear();
	entries.clear();
    }	

    public void write(OutputStream out) throws IOException
    {
	//throw new RuntimeException("not implemented");

	// FIXME: implementation needed
    }
    
    public void read(InputStream is) throws IOException
    {
	//throw new RuntimeException("not implemented");

	// FIXME: implementation needed
    }

    public boolean equals(Object o)
    {
	// FIXME : should this compare entries in the tables ?
	return (o != null && (o instanceof Manifest) && (o == this));
    }

    public Object clone()
    {
	return new Manifest(this);
    }

    public int hashCode()
    {
	// FIXME : what should this return ?
	return super.hashCode();
    }


    // the main attributes
    private Attributes main = new Attributes();

    // the entries
    private Map entries = new HashMap();
}
