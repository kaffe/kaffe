/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.dgc;

import java.io.Serializable;
import java.rmi.dgc.VMID;

public final class Lease
	implements Serializable {

static final long serialVersionUID = -5713411624328831948L;

private VMID vmid;
private long value;

public Lease(VMID id, long duration) {
	vmid = id;
	value = duration;
}

public VMID getVMID() {
	return (vmid);
}

public long getValue() {
	return (value);
}

public String toString() {
	return ("[" + vmid.toString() + ", " + Long.toString(value) + "]");
}

}
