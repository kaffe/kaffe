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

Connection connect(String url, Properties info) throws SQLException;
boolean acceptsURL(String url) throws SQLException;
DriverPropertyInfo[] getPropertyInfo(String url, Properties info) throws SQLException;
int getMajorVersion();
int getMinorVersion();
boolean jdbcCompliant();

}
