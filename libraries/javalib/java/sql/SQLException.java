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

public class SQLException
  extends Exception {

// NB: these are part of the serial form;
private String SQLState;
private int vendorCode;
private SQLException next;

public SQLException(String reason, String SQLState, int vendorCode) {
	super(reason);
	this.SQLState = SQLState;
	this.vendorCode = vendorCode;
	next = null;
}

public SQLException(String reason, String SQLState) {
	this(reason, SQLState, 0);
}

public SQLException(String reason) {
	this(reason, null, 0);
}

public SQLException() {
	this(null, null, 0);
}

public String getSQLState() {
	return (SQLState);
}

public int getErrorCode() {
	return (vendorCode);
}

public SQLException getNextException() {
	return (next);
}

public synchronized void setNextException(SQLException ex) {
	if (next == null) {
		next = ex;
	}
	else {
		next.setNextException(ex);
	}
}

}
