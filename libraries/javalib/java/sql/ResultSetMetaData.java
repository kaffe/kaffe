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

public interface ResultSetMetaData {

public static final int columnNoNulls = 0;
public static final int columnNullable = 1;
public static final int columnNullableUnknown = 2;

public abstract int getColumnCount() throws SQLException;
public abstract boolean isAutoIncrement(int column) throws SQLException;
public abstract boolean isCaseSensitive(int column) throws SQLException;
public abstract boolean isSearchable(int column) throws SQLException;
public abstract boolean isCurrency(int column) throws SQLException;
public abstract int isNullable(int column) throws SQLException;
public abstract boolean isSigned(int column) throws SQLException;
public abstract int getColumnDisplaySize(int column) throws SQLException;
public abstract String getColumnLabel(int column) throws SQLException;
public abstract String getColumnName(int column) throws SQLException;
public abstract String getSchemaName(int column) throws SQLException;
public abstract int getPrecision(int column) throws SQLException;
public abstract int getScale(int column) throws SQLException;
public abstract String getTableName(int column) throws SQLException;
public abstract String getCatalogName(int column) throws SQLException;
public abstract int getColumnType(int column) throws SQLException;
public abstract String getColumnTypeName(int column) throws SQLException;
public abstract boolean isReadOnly(int column) throws SQLException;
public abstract boolean isWritable(int column) throws SQLException;
public abstract boolean isDefinitelyWritable(int column) throws SQLException;

}
