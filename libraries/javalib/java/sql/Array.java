/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic <robilad@yahoo.com>.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.sql;

import java.util.Map;

public interface Array {
    Object getArray() throws SQLException;
    Object getArray(long index, int count) throws SQLException;
    Object getArray(long index, int count, Map map) throws SQLException;
    Object getArray(Map map) throws SQLException;
    int getBaseType() throws SQLException;
    String getBaseTypeName() throws SQLException;
    ResultSet getResultSet() throws SQLException;
    ResultSet getResultSet(long index, int count) throws SQLException;
    ResultSet getResultSet(long index, int count, Map map) throws SQLException;
    ResultSet getResultSet(Map map) throws SQLException;
}
