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

import java.util.Properties;

public interface Driver {

public abstract Connection connect(String url, Properties info) throws SQLException;
public abstract boolean acceptsURL(String url) throws SQLException;
public abstract DriverPropertyInfo[] getPropertyInfo(String url, Properties info) throws SQLException;
public abstract int getMajorVersion();
public abstract int getMinorVersion();
public abstract boolean jdbcCompliant();

}
