/*
 * Java core library component.
 *
 * Copyright (c) 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2001
 *      Dalibor Topic
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.io.InvalidObjectException;
import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public interface AttributedCharacterIterator extends CharacterIterator {

    Set getAllAttributeKeys();
    Object getAttribute(Attribute attribute);
    Map getAttributes();
    int getRunLimit();
    int getRunLimit(Attribute attribute);
    int getRunLimit(Set attributes);
    int getRunStart();
    int getRunStart(Attribute atribute);
    int getRunStart(Set attributes);

    public class Attribute implements Serializable {
	private static final Map ATTRIBUTES = new HashMap(3); 
	private String name;

	public static final Attribute INPUT_METHOD_SEGMENT = new Attribute("input_method_segment");
	public static final Attribute LANGUAGE = new Attribute("language");
	public static final Attribute READING = new Attribute("reading");

	protected Attribute(String attribute_name) {	    
	    name = attribute_name;
	    if (getClass() == Attribute.class) {
		ATTRIBUTES.put(attribute_name, this);
	    }
	}

	public final boolean equals(Object obj) {
	    return super.equals(obj);
	}

	protected String getName() {
	    return name;
	}

	public final int hashCode() {
	    return super.hashCode();
	}

	protected Object readResolve() throws InvalidObjectException {
	    if (ATTRIBUTES.containsKey(name)) {
		return ATTRIBUTES.get(name);
	    }
	    else {
		throw new InvalidObjectException("subclass didn't correctly implement readResolve");
	    }
	}

	public String toString() {
	    return getClass().getName() + "(" + getName() + ")";
	}
    }
}
