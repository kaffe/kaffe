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

public interface ObjectStreamConstants {

short STREAM_MAGIC = (short)0xaced;
short STREAM_VERSION = 5;

int PROTOCOL_VERSION_1 = 0x1;
int PROTOCOL_VERSION_2 = 0x2;

byte TC_BASE = (byte)0x70;
byte TC_NULL = (byte)0x70;
byte TC_REFERENCE = (byte)0x71;
byte TC_CLASSDESC = (byte)0x72;
byte TC_OBJECT = (byte)0x73;
byte TC_STRING = (byte)0x74;
byte TC_ARRAY = (byte)0x75;
byte TC_CLASS = (byte)0x76;
byte TC_BLOCKDATA = (byte)0x77;
byte TC_ENDBLOCKDATA = (byte)0x78;
byte TC_RESET = (byte)0x79;
byte TC_BLOCKDATALONG = (byte)0x7A;
byte TC_EXCEPTION = (byte)0x7B;
byte TC_MAX = (byte)0x7B;

byte SC_WRITE_METHOD = (byte)0x01;
byte SC_SERIALIZABLE = (byte)0x02;
byte SC_EXTERNALIZABLE = (byte)0x04;
byte SC_BLOCK_DATA = (byte)0x08;

int baseWireHandle = 0x7E0000; 
//public final static SerializablePermission SUBCLASS_IMPLEMENTATION_PERMISSION = ???;
//public final static SerializablePermission SUBSTITUTION_PERMISSION = ???;

}
