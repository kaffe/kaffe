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

public final static short STREAM_MAGIC = (short)0xaced;
public final static short STREAM_VERSION = 5;

public final static byte TC_NULL = (byte)0x70;
public final static byte TC_REFERENCE = (byte)0x71;
public final static byte TC_CLASSDESC = (byte)0x72;
public final static byte TC_OBJECT = (byte)0x73;
public final static byte TC_STRING = (byte)0x74;
public final static byte TC_ARRAY = (byte)0x75;
public final static byte TC_CLASS = (byte)0x76;
public final static byte TC_BLOCKDATA = (byte)0x77;
public final static byte TC_ENDBLOCKDATA = (byte)0x78;
public final static byte TC_RESET = (byte)0x79;
public final static byte TC_BLOCKDATALONG = (byte)0x7A;
public final static byte TC_EXCEPTION = (byte)0x7B;

public final static byte SC_WRRD_METHODS = (byte)0x01;
public final static byte SC_SERIALIZABLE = (byte)0x02;
public final static byte SC_EXTERNALIZABLE = (byte)0x04;
public final static byte SC_STRING = (byte)0x08;

}
