
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

import java.io.IOException;
import java.io.InputStream;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

public abstract class Provider extends Properties {
	private final String name;
	private final double version;
	private final String info;

	protected Provider(String name, double version, String info) {
		this.name = name;
		this.version = version;
		this.info = info;
	}

	public String getName() {
		return name;
	}

	public double getVersion() {
		return version;
	}

	public String getInfo() {
		return info;
	}

	public String toString() {
		return name + " version " + version;
	}

	public void clear() {
		System.getSecurityManager().checkSecurityAccess(
			"clearProviderProperties." + name);
		super.clear();
	}

	public void load(InputStream in) throws IOException {
		super.load(in);		// XXX?
	}

	public void putAll(Map t) {
		super.putAll(t);	// XXX?
	}

	public Set entrySet() {
		return Collections.unmodifiableSet(super.entrySet());
	}

	public Set keySet() {
		return Collections.unmodifiableSet(super.keySet());
	}

	public Collection values() {
		return Collections.unmodifiableCollection(super.values());
	}

	public Object put(Object key, Object value) {
		System.getSecurityManager().checkSecurityAccess(
			"putProviderProperty." + name);
		return super.put(key, value);
	}

	public Object remove(Object key) {
		System.getSecurityManager().checkSecurityAccess(
			"removeProviderProperty." + name);
		return super.remove(key);
	}
}

