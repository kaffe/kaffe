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

public interface PreparedStatement 
  extends Statement {

public abstract ResultSet executeQuery() throws SQLException;
public abstract int executeUpdate() throws SQLException;
public abstract void setNull(int parameterIndex, int sqlType) throws SQLException;
public abstract void setBoolean(int parameterIndex, boolean x) throws SQLException;
public abstract void setByte(int parameterIndex, byte x) throws SQLException;
public abstract void setShort(int parameterIndex, short x) throws SQLException;
public abstract void setInt(int parameterIndex, int x) throws SQLException;
public abstract void setLong(int parameterIndex, long x) throws SQLException;
public abstract void setFloat(int parameterIndex, float x) throws SQLException;
public abstract void setDouble(int parameterIndex, double x) throws SQLException;
public abstract void setBigDecimal(int parameterIndex, BigDecimal x) throws SQLException;
public abstract void setString(int parameterIndex, String x) throws SQLException;
public abstract void setBytes(int parameterIndex, byte x[]) throws SQLException;
public abstract void setDate(int parameterIndex, Date x) throws SQLException;
public abstract void setTime(int parameterIndex, Time x) throws SQLException;
public abstract void setTimestamp(int parameterIndex, Timestamp x) throws SQLException;
public abstract void setAsciiStream(int parameterIndex, InputStream x, int length) throws SQLException;
public abstract void setUnicodeStream(int parameterIndex, InputStream x, int length) throws SQLException;
public abstract void setBinaryStream(int parameterIndex, InputStream x, int length) throws SQLException;
public abstract void clearParameters() throws SQLException;
public abstract void setObject(int parameterIndex, Object x, int targetSqlType, int scale) throws SQLException;
public abstract void setObject(int parameterIndex, Object x, int targetSqlType) throws SQLException;
public abstract void setObject(int parameterIndex, Object x) throws SQLException;
public abstract boolean execute() throws SQLException;

}
