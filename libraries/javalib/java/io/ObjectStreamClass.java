/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.io.Serializable;
import java.io.Externalizable;
import java.util.Hashtable;

class ObjectStreamField {
	String	name;
	char	type;
	int	offset;
	String	typeString;
}

public class ObjectStreamClass
  implements Serializable
{

static private Hashtable streamClasses = new Hashtable();

String name;
Class clazz;
ObjectStreamClass superclazzStream;
int method;
long serialVersionUID;
ObjectStreamField[] fieldInfo;
int[] fieldRdWr;

public Class forClass()
{
	return (clazz);
}

public String getName()
{
	return (name);
}

public static ObjectStreamClass lookup(Class cl)
{
	// First check hash table for match.
	ObjectStreamClass osc = (ObjectStreamClass)streamClasses.get(cl);
	if (osc != null) {
		return (osc);
	}

	// Otherwise we build a new one.
	int method;
	if (cl == String.class) {
		method = ObjectStreamConstants.SC_STRING;
	}
	else if (Externalizable.class.isAssignableFrom(cl)) {
		method = ObjectStreamConstants.SC_EXTERNALIZABLE;
	}
	else if (Serializable.class.isAssignableFrom(cl)) {
		if (hasWriteObject(cl)) {
			method = ObjectStreamConstants.SC_WRRD_METHODS;
		}
		else {
			method = ObjectStreamConstants.SC_SERIALIZABLE;
		}
	}
	else {
		return (null);
	}

	osc = new ObjectStreamClass();
	osc.name = cl.getName();
	osc.clazz = cl;
	osc.method = method;

	// Get StreamClass for superclass
	osc.superclazzStream = lookup(cl.getSuperclass());

	if (!cl.isArray()) {
		osc.buildFieldsAndOffset();
	}
	else {
		osc.fieldInfo = new ObjectStreamField[0];
		osc.fieldRdWr = new int[0];
	}

	// Insert into hash table for later.
	streamClasses.put(cl, osc);

	return (osc);
}

void buildFieldsAndOffset()
{
	fieldInfo = getFields0(clazz);
	int len = fieldInfo.length;
	fieldRdWr = new int[len*2];
	for (int i = 0; i < len; i++) {
		fieldRdWr[i*2] = fieldInfo[i].type;
		fieldRdWr[i*2+1] = fieldInfo[i].offset;
	}
}

public String toString()
{
	return (getName() + ": static final long serialVersionUID = " + Long.toString(getSerialVersionUID(clazz)));
}

// -------------------------------------------------------------------

private void writeObject(ObjectOutputStream out) throws IOException
{
        out.writeUTF(name);
        out.writeLong(serialVersionUID);
	out.writeByte(method);
	int len = fieldInfo.length;
        out.writeShort(len);
        for (int i = 0; i < len; i++) {
                out.writeByte(fieldInfo[i].type);
                out.writeUTF(fieldInfo[i].name);
                if (fieldInfo[i].typeString != null) {
                        out.writeObject(fieldInfo[i].typeString);
                }
        }
	out.writeByte(ObjectStreamConstants.TC_ENDBLOCKDATA);

	// And now the superclass
	out.writeObject(superclazzStream);
}

private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException
{
	name = in.readUTF();
	serialVersionUID = in.readLong();
        method = in.readUnsignedByte();

        int len = in.readShort();
        fieldInfo = new ObjectStreamField[len];

        for (int i = 0; i < len; i++) {
                fieldInfo[i] = new ObjectStreamField();
                int type = in.readUnsignedByte();
                fieldInfo[i].type = (char)type;
                fieldInfo[i].name = in.readUTF();
                if (type == 'L' || type == '[') {
			try {
				fieldInfo[i].typeString = (String)in.readObject();
			}
			catch (ClassCastException _) {
				throw new StreamCorruptedException("expected string");
			}
                }
                else {
                        fieldInfo[i].typeString = null;
                }
                fieldInfo[i].offset = 0;
        }
	in.expectByte(ObjectStreamConstants.TC_ENDBLOCKDATA, "missing endblockdata");

	// And now the superclass
	try {
		superclazzStream = (ObjectStreamClass)in.readObject();
	}
	catch (ClassCastException _) {
		throw new StreamCorruptedException("expected class desc");
	}
}

// -------------------------------------------------------------------

private native static int getClassAccess(Class cls);
private native static String[] getMethodSignatures(Class cls);
private native static int getMethodAccess(Class cls, String str);
private native static String[] getFieldSignatures(Class cls);
private native static int getFieldAccess(Class cls, String str);
private native ObjectStreamField[] getFields0(Class cls);
public native static long getSerialVersionUID(Class cls);
private native static boolean hasWriteObject(Class cls);

}
