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

ResultSet executeQuery() throws SQLException;
int executeUpdate() throws SQLException;
void setNull(int parameterIndex, int sqlType) throws SQLException;
void setBoolean(int parameterIndex, boolean x) throws SQLException;
void setByte(int parameterIndex, byte x) throws SQLException;
void setShort(int parameterIndex, short x) throws SQLException;
void setInt(int parameterIndex, int x) throws SQLException;
void setLong(int parameterIndex, long x) throws SQLException;
void setFloat(int parameterIndex, float x) throws SQLException;
void setDouble(int parameterIndex, double x) throws SQLException;
void setBigDecimal(int parameterIndex, BigDecimal x) throws SQLException;
void setString(int parameterIndex, String x) throws SQLException;
void setBytes(int parameterIndex, byte x[]) throws SQLException;
void setDate(int parameterIndex, Date x) throws SQLException;
void setTime(int parameterIndex, Time x) throws SQLException;
void setTimestamp(int parameterIndex, Timestamp x) throws SQLException;
void setAsciiStream(int parameterIndex, InputStream x, int length) throws SQLException;
void setUnicodeStream(int parameterIndex, InputStream x, int length) throws SQLException;
void setBinaryStream(int parameterIndex, InputStream x, int length) throws SQLException;
void clearParameters() throws SQLException;
void setObject(int parameterIndex, Object x, int targetSqlType, int scale) throws SQLException;
void setObject(int parameterIndex, Object x, int targetSqlType) throws SQLException;
void setObject(int parameterIndex, Object x) throws SQLException;
boolean execute() throws SQLException;

}
