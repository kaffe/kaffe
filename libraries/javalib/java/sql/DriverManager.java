/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.sql;

import java.io.PrintStream;
import java.util.Properties;
import java.util.Vector;
import java.util.Enumeration;
import java.util.StringTokenizer;

public class DriverManager {

static private Vector drivers;
static private int timeout;
static private PrintStream strm;

static {
	drivers = new Vector();
	timeout = 0;
	strm = null;

	// Load in default drivers ... 
	StringTokenizer toks = new StringTokenizer(System.getProperty("jdbc.drivers", ""), ":");
	while (toks.hasMoreTokens()) {
		String str = toks.nextToken();
		try {
			Class.forName(str);
		}
		catch (ClassNotFoundException _) {
			// Silently ignore.
		}
	}
}

private DriverManager() {}
    
public static synchronized Connection getConnection(String url, String user, String password) throws SQLException {
	Properties p = new Properties();
	if (user != null)
		p.put("user", user);
	if (password != null)
		p.put("password", password);
	return (getConnection(url, p));
}

public static synchronized Connection getConnection(String url) throws SQLException {
	return (getConnection(url, new Properties()));
}

public static synchronized Connection getConnection(String url, Properties info) throws SQLException {
	Enumeration e = getDrivers();
	while (e.hasMoreElements()) {
		Driver d = (Driver)e.nextElement();
		if (d.acceptsURL(url)) {
			Connection c = d.connect(url, info);
			if (c != null) {
				return (c);
			}
		}
	}
	throw new SQLException("driver not found: " + url);
}

public static Driver getDriver(String url) throws SQLException {
	Enumeration e = getDrivers();
	while (e.hasMoreElements()) {
		Driver d = (Driver)e.nextElement();
		if (d.acceptsURL(url)) {
			return (d);
		}
	}
	throw new SQLException("driver not found: " + url);
}

public static synchronized void registerDriver(Driver driver) throws SQLException {
	drivers.addElement(driver);
}

public static void deregisterDriver(Driver driver) throws SQLException {
	drivers.removeElement(driver);
}

public static Enumeration getDrivers() {
	return (drivers.elements());
}

public static void setLoginTimeout(int seconds) {
	timeout = seconds;
}

public static int getLoginTimeout() {
	return (timeout);
}

public static void setLogStream(PrintStream out) {
	strm = out;
}

public static PrintStream getLogStream() {
	return (strm);
}

public static void println(String message) {
	if (strm != null) {
		strm.println(message);
	}
}

}
