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

import java.io.InputStream;
import java.math.BigDecimal;

public interface ResultSet {

boolean next() throws SQLException;
void close() throws SQLException;
boolean wasNull() throws SQLException;
String getString(int columnIndex) throws SQLException;
boolean getBoolean(int columnIndex) throws SQLException;
byte getByte(int columnIndex) throws SQLException;
short getShort(int columnIndex) throws SQLException;
int getInt(int columnIndex) throws SQLException;
long getLong(int columnIndex) throws SQLException;
float getFloat(int columnIndex) throws SQLException;
double getDouble(int columnIndex) throws SQLException;
BigDecimal getBigDecimal(int columnIndex, int scale) throws SQLException;
byte[] getBytes(int columnIndex) throws SQLException;
Date getDate(int columnIndex) throws SQLException;
Time getTime(int columnIndex) throws SQLException;
Timestamp getTimestamp(int columnIndex) throws SQLException;
InputStream getAsciiStream(int columnIndex) throws SQLException;
InputStream getUnicodeStream(int columnIndex) throws SQLException;
InputStream getBinaryStream(int columnIndex) throws SQLException;
String getString(String columnName) throws SQLException;
boolean getBoolean(String columnName) throws SQLException;
byte getByte(String columnName) throws SQLException;
short getShort(String columnName) throws SQLException;
int getInt(String columnName) throws SQLException;
long getLong(String columnName) throws SQLException;
float getFloat(String columnName) throws SQLException;
double getDouble(String columnName) throws SQLException;
BigDecimal getBigDecimal(String columnName, int scale) throws SQLException;
byte[] getBytes(String columnName) throws SQLException;
Date getDate(String columnName) throws SQLException;
Time getTime(String columnName) throws SQLException;
Timestamp getTimestamp(String columnName) throws SQLException;
InputStream getAsciiStream(String columnName) throws SQLException;
InputStream getUnicodeStream(String columnName) throws SQLException;
InputStream getBinaryStream(String columnName) throws SQLException;
SQLWarning getWarnings() throws SQLException;
void clearWarnings() throws SQLException;
String getCursorName() throws SQLException;
ResultSetMetaData getMetaData() throws SQLException;
Object getObject(int columnIndex) throws SQLException;
Object getObject(String columnName) throws SQLException;
int findColumn(String columnName) throws SQLException;

}
