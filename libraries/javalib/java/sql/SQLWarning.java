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

public class SQLWarning
  extends SQLException {

public SQLWarning(String reason, String SQLstate, int vendorCode) {
	super(reason, SQLstate, vendorCode);
}

public SQLWarning(String reason, String SQLstate) {
	super(reason, SQLstate);
}

public SQLWarning(String reason) {
	super(reason);
}

public SQLWarning() {
	super();
}

public SQLWarning getNextWarning() {
	return ((SQLWarning)getNextException());
}

public void setNextWarning(SQLWarning w) {
	setNextException(w);
}

}
