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

import java.math.BigDecimal;

public interface CallableStatement
  extends PreparedStatement {

void registerOutParameter(int parameterIndex, int sqlType) throws SQLException;
void registerOutParameter(int parameterIndex, int sqlType, int scale) throws SQLException;
boolean wasNull() throws SQLException;
String getString(int parameterIndex) throws SQLException;
boolean getBoolean(int parameterIndex) throws SQLException;
byte getByte(int parameterIndex) throws SQLException;
short getShort(int parameterIndex) throws SQLException;
int getInt(int parameterIndex) throws SQLException;
long getLong(int parameterIndex) throws SQLException;
float getFloat(int parameterIndex) throws SQLException;
double getDouble(int parameterIndex) throws SQLException;
BigDecimal getBigDecimal(int parameterIndex, int scale) throws SQLException;
byte[] getBytes(int parameterIndex) throws SQLException;
Date getDate(int parameterIndex) throws SQLException;
Time getTime(int parameterIndex) throws SQLException;
Timestamp getTimestamp(int parameterIndex) throws SQLException;
Object getObject(int parameterIndex) throws SQLException;

}
