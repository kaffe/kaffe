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

public abstract void registerOutParameter(int parameterIndex, int sqlType) throws SQLException;
public abstract void registerOutParameter(int parameterIndex, int sqlType, int scale) throws SQLException;
public abstract boolean wasNull() throws SQLException;
public abstract String getString(int parameterIndex) throws SQLException;
public abstract boolean getBoolean(int parameterIndex) throws SQLException;
public abstract byte getByte(int parameterIndex) throws SQLException;
public abstract short getShort(int parameterIndex) throws SQLException;
public abstract int getInt(int parameterIndex) throws SQLException;
public abstract long getLong(int parameterIndex) throws SQLException;
public abstract float getFloat(int parameterIndex) throws SQLException;
public abstract double getDouble(int parameterIndex) throws SQLException;
public abstract BigDecimal getBigDecimal(int parameterIndex, int scale) throws SQLException;
public abstract byte[] getBytes(int parameterIndex) throws SQLException;
public abstract Date getDate(int parameterIndex) throws SQLException;
public abstract Time getTime(int parameterIndex) throws SQLException;
public abstract Timestamp getTimestamp(int parameterIndex) throws SQLException;
public abstract Object getObject(int parameterIndex) throws SQLException;

}
