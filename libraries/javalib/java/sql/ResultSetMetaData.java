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

int columnNoNulls = 0;
int columnNullable = 1;
int columnNullableUnknown = 2;

int getColumnCount() throws SQLException;
boolean isAutoIncrement(int column) throws SQLException;
boolean isCaseSensitive(int column) throws SQLException;
boolean isSearchable(int column) throws SQLException;
boolean isCurrency(int column) throws SQLException;
int isNullable(int column) throws SQLException;
boolean isSigned(int column) throws SQLException;
int getColumnDisplaySize(int column) throws SQLException;
String getColumnLabel(int column) throws SQLException;
String getColumnName(int column) throws SQLException;
String getSchemaName(int column) throws SQLException;
int getPrecision(int column) throws SQLException;
int getScale(int column) throws SQLException;
String getTableName(int column) throws SQLException;
String getCatalogName(int column) throws SQLException;
int getColumnType(int column) throws SQLException;
String getColumnTypeName(int column) throws SQLException;
boolean isReadOnly(int column) throws SQLException;
boolean isWritable(int column) throws SQLException;
boolean isDefinitelyWritable(int column) throws SQLException;

}
