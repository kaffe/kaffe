/* Class : Attributes

  Implementation of the 1.2 Java class Attributes.

  Copyright : Moses DeJong, dejong@cs.umn.edu, 1998.
  Source code licensed under the GPL.
  You can get a copy of the license from www.gnu.org.

  This code is intended for use in the Kaffe project but you can use
  it in other projects as long as you follow the license rules.
*/

package java.util.jar;

import java.util.*;

public class Attributes implements Map, Cloneable {
    protected Map map;

    public Attributes() {
	map = new HashMap();
    }

    public Attributes(Attributes attr) {
	map = new HashMap(attr.map);
    }

    public Attributes(int size) {
	map = new HashMap(size);
    }
    
    public Object clone() {
	return new Attributes(this); 
    }

    public Set entrySet() {
	return map.entrySet();
    }

    public boolean equals(Object o) {
	return map.equals(o);
    }

    public Object get(Object name) {
	return map.get(name);
    }

    public String getValue(String name) {
	return getValue(new Attributes.Name(name));
    }

    public String getValue(Attributes.Name name) {
	return (String) get(name);
    }

    public Object put(Object name, Object value) {
	return map.put((Attributes.Name)name, (String)value);
    }

    public String putValue(String name, String value) {
	return (String)put(new Attributes.Name(name), value);
    }

    public Object remove(Object name) {
	return map.remove(name);
    }

    public boolean containsValue(Object value) {
	return map.containsValue(value);
    }

    public boolean containsKey(Object name) {
	return map.containsKey(name);
    }

    public void putAll(Map attr) {
	map.putAll((Attributes)attr);
    }

    public void clear() {
	map.clear();
    }

    public int size() {
	return map.size();
    }

    public boolean isEmpty() {
	return map.isEmpty();
    }

    public Set keySet() {
	return map.keySet();
    }
    
    public Collection values() {
	return map.values();
    }

    public static class Name {
	private final String name;

	public static final Name
		MANIFEST_VERSION	= new Name("Manifest-Version");
	public static final Name
		SIGNATURE_VERSION	= new Name("Signature-Version");
	public static final Name
		CONTENT_TYPE		= new Name("Content-Type");
	public static final Name
		CLASS_PATH		= new Name("Class-Path");
	public static final Name
		MAIN_CLASS		= new Name("Main-Class");
	public static final Name
		SEALED			= new Name("Sealed");
	public static final Name
		IMPLEMENTATION_TITLE	= new Name("Implementation-Title");
	public static final Name
		IMPLEMENTATION_VERSION	= new Name("Implementation-Version");
	public static final Name
		IMPLEMENTATION_VENDOR	= new Name("Implementation-Vendor");
	public static final Name
		IMPLEMENTATION_VENDOR_ID= new Name("Implementation-Vendor-Id");
	public static final Name
		IMPLEMENTATION_VENDOR_URL=new Name("Implementation-Vendor-URL");
	public static final Name
		SPECIFICATION_TITLE	= new Name("Specification-Title");
	public static final Name
		SPECIFICATION_VERSION	= new Name("Specification-Version");
	public static final Name
		SPECIFICATION_VENDOR	= new Name("Specification-Vendor");
	public static final Name
		EXTENSION_LIST		= new Name("Extension-List");
	public static final Name
		EXTENSION_INSTALLATION	= new Name("Extension-Installation");

	public Name(String name) {
	    if (name.length() == 0) {
	    	throw new IllegalArgumentException("empty attribute name");
	    }
	    for (int i = 0; i < name.length(); i++) {
		char c = name.charAt(i);

		if (!((c >= '0' && c <= '9') ||
		      (c >= 'A' && c <= 'Z') ||
		      (c >= 'a' && c <= 'z') ||
		       c == '_' || c == '-' )) {
		    throw new IllegalArgumentException("bogus attribute name: "
			+ name);
		}
	    }
	    this.name = name;
	}

	public boolean equals(Object o) {
	    return ((o instanceof Name) &&
		    name.equalsIgnoreCase(((Name)o).name));
	}

	public int hashCode() {
	    return name.toUpperCase().hashCode();
	}

	public String toString() {
	    return name;
	}
    }
}

