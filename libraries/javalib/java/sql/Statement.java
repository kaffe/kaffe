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

public interface Statement {

public abstract ResultSet executeQuery(String sql) throws SQLException;
public abstract int executeUpdate(String sql) throws SQLException;
public abstract void close() throws SQLException;
public abstract int getMaxFieldSize() throws SQLException;
public abstract void setMaxFieldSize(int max) throws SQLException;
public abstract int getMaxRows() throws SQLException;
public abstract void setMaxRows(int max) throws SQLException;
public abstract void setEscapeProcessing(boolean enable) throws SQLException;
public abstract int getQueryTimeout() throws SQLException;
public abstract void setQueryTimeout(int seconds) throws SQLException;
public abstract void cancel() throws SQLException;
public abstract SQLWarning getWarnings() throws SQLException;
public abstract void clearWarnings() throws SQLException;
public abstract void setCursorName(String name) throws SQLException;
public abstract boolean execute(String sql) throws SQLException;
public abstract ResultSet getResultSet() throws SQLException;
public abstract int getUpdateCount() throws SQLException;
public abstract boolean getMoreResults() throws SQLException;

}
