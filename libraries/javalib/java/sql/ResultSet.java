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

public abstract boolean next() throws SQLException;
public abstract void close() throws SQLException;
public abstract boolean wasNull() throws SQLException;
public abstract String getString(int columnIndex) throws SQLException;
public abstract boolean getBoolean(int columnIndex) throws SQLException;
public abstract byte getByte(int columnIndex) throws SQLException;
public abstract short getShort(int columnIndex) throws SQLException;
public abstract int getInt(int columnIndex) throws SQLException;
public abstract long getLong(int columnIndex) throws SQLException;
public abstract float getFloat(int columnIndex) throws SQLException;
public abstract double getDouble(int columnIndex) throws SQLException;
public abstract BigDecimal getBigDecimal(int columnIndex, int scale) throws SQLException;
public abstract byte[] getBytes(int columnIndex) throws SQLException;
public abstract Date getDate(int columnIndex) throws SQLException;
public abstract Time getTime(int columnIndex) throws SQLException;
public abstract Timestamp getTimestamp(int columnIndex) throws SQLException;
public abstract InputStream getAsciiStream(int columnIndex) throws SQLException;
public abstract InputStream getUnicodeStream(int columnIndex) throws SQLException;
public abstract InputStream getBinaryStream(int columnIndex) throws SQLException;
public abstract String getString(String columnName) throws SQLException;
public abstract boolean getBoolean(String columnName) throws SQLException;
public abstract byte getByte(String columnName) throws SQLException;
public abstract short getShort(String columnName) throws SQLException;
public abstract int getInt(String columnName) throws SQLException;
public abstract long getLong(String columnName) throws SQLException;
public abstract float getFloat(String columnName) throws SQLException;
public abstract double getDouble(String columnName) throws SQLException;
public abstract BigDecimal getBigDecimal(String columnName, int scale) throws SQLException;
public abstract byte[] getBytes(String columnName) throws SQLException;
public abstract Date getDate(String columnName) throws SQLException;
public abstract Time getTime(String columnName) throws SQLException;
public abstract Timestamp getTimestamp(String columnName) throws SQLException;
public abstract InputStream getAsciiStream(String columnName) throws SQLException;
public abstract InputStream getUnicodeStream(String columnName) throws SQLException;
public abstract InputStream getBinaryStream(String columnName) throws SQLException;
public abstract SQLWarning getWarnings() throws SQLException;
public abstract void clearWarnings() throws SQLException;
public abstract String getCursorName() throws SQLException;
public abstract ResultSetMetaData getMetaData() throws SQLException;
public abstract Object getObject(int columnIndex) throws SQLException;
public abstract Object getObject(String columnName) throws SQLException;
public abstract int findColumn(String columnName) throws SQLException;

}
