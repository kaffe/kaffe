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

public class DataTruncation 
  extends SQLWarning {

private int index;
private boolean parameter;
private boolean read;
private int dataSize;
private int transferSize;

public DataTruncation(int index, boolean parameter, boolean read, int dataSize, int transferSize) {
	super("Data truncation", "01004");
	this.index = index;
	this.parameter = parameter;
	this.read = read;
	this.dataSize = dataSize;
	this.transferSize = transferSize;
}

public int getIndex() {
	return (index);
}

public boolean getParameter() {
	return (parameter);
}

public boolean getRead() {
	return (read);
}

public int getDataSize() {
	return (dataSize);
}

public int getTransferSize() {
	return (transferSize);
}

}
