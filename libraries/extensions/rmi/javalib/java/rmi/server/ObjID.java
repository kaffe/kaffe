/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.io.Serializable;
import java.io.ObjectOutput;
import java.io.ObjectInput;
import java.io.IOException;
import java.lang.Math;
import java.io.DataInput;
import java.io.DataOutput;
import java.util.Random;

public final class ObjID
	implements Serializable {

static final long serialVersionUID = -6386392263968365220L;

private static long next = 0x8000000000000000L;
private static final Object lock = ObjID.class;

public static final int REGISTRY_ID = 0;
public static final int ACTIVATOR_ID = 1;
public static final int DGC_ID = 2;

private long objNum;
private UID space;

public ObjID() {
	synchronized (lock) {
		objNum = next++;
	}
	space = new UID();
}

public ObjID(int num) {
	objNum = (long)num;
	space = new UID((short)0);
}

public void write(ObjectOutput out) throws IOException {
	DataOutput dout = (DataOutput)out;
	dout.writeLong(objNum);
	space.write(dout);
}

public static ObjID read(ObjectInput in) throws IOException {
	DataInput din = (DataInput)in;
	ObjID id = new ObjID();
	id.objNum = din.readLong();
	id.space = UID.read(din);
	return (id);
}

public int hashCode() {
	return ((int)objNum);
}

public boolean equals(Object obj) {
	if (obj instanceof ObjID && this.objNum == ((ObjID)obj).objNum) {
		return (true);
	}
	return (false);
}

public String toString() {
	return ("[objNum: " + objNum + ", " + space + "]");
}

}
