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

ResultSet executeQuery(String sql) throws SQLException;
int executeUpdate(String sql) throws SQLException;
void close() throws SQLException;
int getMaxFieldSize() throws SQLException;
void setMaxFieldSize(int max) throws SQLException;
int getMaxRows() throws SQLException;
void setMaxRows(int max) throws SQLException;
void setEscapeProcessing(boolean enable) throws SQLException;
int getQueryTimeout() throws SQLException;
void setQueryTimeout(int seconds) throws SQLException;
void cancel() throws SQLException;
SQLWarning getWarnings() throws SQLException;
void clearWarnings() throws SQLException;
void setCursorName(String name) throws SQLException;
boolean execute(String sql) throws SQLException;
ResultSet getResultSet() throws SQLException;
int getUpdateCount() throws SQLException;
boolean getMoreResults() throws SQLException;

}
