/* Class : Attributes

  Implementation of the 1.2 Java class Attributes.

  Copyright : Moses DeJong, dejong@cs.umn.edu, 1998.
  Source code licensed under the GPL.
  You can get a copy of the license from www.gnu.org.

  This code is intended for use in the Kaffe project but you can use
  it in other projects as long as you follow the license rules.
*/

package java.util.jar;

import java.io.*;
import java.util.*;

public class Attributes implements Map, Cloneable {

    public Attributes()
    {
	map = new HashMap();
    }

    public Attributes(Attributes attr)
    {
	map = new HashMap(attr.map);
    }

    public Attributes(int size)
    {
	map = new HashMap(size);
    }
    
    public Object clone()
    {
	return new Attributes(this); 
    }

    public Set entrySet()
    {
	return map.entrySet();
    }

    public boolean equals(Object o)
    {
	// FIXME : should this check each attribute name ?
	// could it use the uquals method of the Map?
	return (o == null && (o instanceof Attributes) && (o == this));
    }

    public Object get(Object name)
    {
	return map.get(name);
    }

    public String getValue(String name)
    {
	return (String) get( new Attributes.Name((String) name) );
    }

    public String getValue(Attributes.Name name)
    {
	return (String) get(name);
    }

    public Object put(Object name, Object value)
	throws ClassCastException
    {
	return map.put(name,value);
    }

    public String putValue(String name, String value)
	throws IllegalArgumentException
    {
	return (String) put(new Attributes.Name(name), value);
    }

    public Object remove(Object name)
    {
	return map.remove(name);
    }

    public boolean containsValue(Object value)
    {
	return map.containsValue(value);
    }

    public boolean containsKey(Object name)
    {
	return map.containsKey(name);
    }

    public void putAll(Map attr)
    {
	map.putAll(attr);
    }

    public void clear()
    {
	map.clear();
    }

    public int size()
    {
	return map.size();
    }

    public boolean isEmpty()
    {
	return map.isEmpty();
    }

    public Set keySet()
    {
	return map.keySet();
    }
    
    public Collection values()
    {
	return map.values();
    }

    public static class Name {

	// FIXME : these need to be initializes to something, but what?
	public static final Name MANIFEST_VERSION = null;
	public static final Name SIGNATURE_VERSION = null;
	public static final Name CONTENT_TYPE = null;
	public static final Name CLASS_PATH = null;
	public static final Name MAIN_CLASS = null;
	public static final Name SEALED = null;
	public static final Name IMPLEMENTATION_TITLE = null;
	public static final Name IMPLEMENTATION_VERSION = null;
	public static final Name IMPLEMENTATION_VENDOR = null;
	public static final Name SPECIFICATION_TITLE = null;
	public static final Name SPECIFICATION_VERSION = null;
	public static final Name SPECIFICATION_VENDOR = null;

	public Name(String name)
	    throws IllegalArgumentException, NullPointerException
	{
	    if (name == null) {
		throw new NullPointerException("name");
	    }
	    
	    // the JDK docs say that an IllegalArgumentException is thrown
	    // if attribute name was invalid. It also says that chars are
	    // restricted to the ASCII characters in the set [0-9a-zA-Z_-]. 

	    checkName(name);

	    this.name = name;
	}

	public boolean equals(Object o)
	{
	    // The JDK docs say that attribute names are case insentive
	    // so use String.equalsIgnoreCase() instead of String.equals()

	    return (o != null && (o instanceof Name) &&
		    name.equalsIgnoreCase(((Name) o).name));
	}

	public int hashCode()
	{
	    // FIXME : does this depend on case of the name?
	    return name.hashCode();
	}

	public String toString()
	{
	    return name;
	}

	private static void checkName(String name)
	    throws IllegalArgumentException
	{
	    // valid chars [0-9a-zA-Z_-].

	    for (int i=0; i < name.length(); i++) {
		char c = name.charAt(i);

		if ((c >= '0' && c <= '9') ||
		    (c >= 'a' && c <= 'z') ||
		    (c >= 'A' && c <= 'Z') ||
		    (c == '_' && c == '-')) {
		    // it is a valid char so do nothing
		} else {
		    throw new IllegalArgumentException(name);
		}
	    }
	}

	private String name;
    }


    
    protected Map map;

}
