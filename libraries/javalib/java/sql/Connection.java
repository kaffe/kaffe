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

public interface Connection {

int TRANSACTION_NONE = 0;
int TRANSACTION_READ_UNCOMMITTED = 1;
int TRANSACTION_READ_COMMITTED = 2;
int TRANSACTION_REPEATABLE_READ = 4;
int TRANSACTION_SERIALIZABLE = 8;

Statement createStatement() throws SQLException;
PreparedStatement prepareStatement(String sql) throws SQLException;
CallableStatement prepareCall(String sql) throws SQLException;
String nativeSQL(String sql) throws SQLException;
void setAutoCommit(boolean autoCommit) throws SQLException;
boolean getAutoCommit() throws SQLException;
void commit() throws SQLException;
void rollback() throws SQLException;
void close() throws SQLException;
boolean isClosed() throws SQLException;
DatabaseMetaData getMetaData() throws SQLException;
void setReadOnly(boolean readOnly) throws SQLException;
boolean isReadOnly() throws SQLException;
void setCatalog(String catalog) throws SQLException;
String getCatalog() throws SQLException;
void setTransactionIsolation(int level) throws SQLException;
int getTransactionIsolation() throws SQLException;
SQLWarning getWarnings() throws SQLException;
void clearWarnings() throws SQLException;

}
