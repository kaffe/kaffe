/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.rmi.server;

public interface ProtocolConstants {

final public static int PROTOCOL_HEADER = 0x4a524d49; // JRMI
final public static int PROTOCOL_VERSION = 2;

final public static int STREAM_PROTOCOL = 0x4b;
final public static int SINGLE_OP_PROTOCOL = 0x4c;
final public static int MULTIPLEX_PROTOCOL = 0x4d;

final public static int PROTOCOL_ACK = 0x4e;
final public static int PROTOCOL_NACK = 0x4f;

final public static int MESSAGE_CALL = 0x50;
final public static int MESSAGE_CALL_ACK = 0x51;
final public static int MESSAGE_PING = 0x52;
final public static int MESSAGE_PING_ACK = 0x53;
final public static int MESSAGE_DGCACK = 0x54;

final public static int RETURN_ACK = 0x01;
final public static int RETURN_NACK = 0x02;

final public static int DEFAULT_PROTOCOL = STREAM_PROTOCOL;

};
