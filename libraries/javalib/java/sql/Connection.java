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

public static final int TRANSACTION_NONE = 0;
public static final int TRANSACTION_READ_UNCOMMITTED = 1;
public static final int TRANSACTION_READ_COMMITTED = 2;
public static final int TRANSACTION_REPEATABLE_READ = 4;
public static final int TRANSACTION_SERIALIZABLE = 8;

public abstract Statement createStatement() throws SQLException;
public abstract PreparedStatement prepareStatement(String sql) throws SQLException;
public abstract CallableStatement prepareCall(String sql) throws SQLException;
public abstract String nativeSQL(String sql) throws SQLException;
public abstract void setAutoCommit(boolean autoCommit) throws SQLException;
public abstract boolean getAutoCommit() throws SQLException;
public abstract void commit() throws SQLException;
public abstract void rollback() throws SQLException;
public abstract void close() throws SQLException;
public abstract boolean isClosed() throws SQLException;
public abstract DatabaseMetaData getMetaData() throws SQLException;
public abstract void setReadOnly(boolean readOnly) throws SQLException;
public abstract boolean isReadOnly() throws SQLException;
public abstract void setCatalog(String catalog) throws SQLException;
public abstract String getCatalog() throws SQLException;
public abstract void setTransactionIsolation(int level) throws SQLException;
public abstract int getTransactionIsolation() throws SQLException;
public abstract SQLWarning getWarnings() throws SQLException;
public abstract void clearWarnings() throws SQLException;

}
