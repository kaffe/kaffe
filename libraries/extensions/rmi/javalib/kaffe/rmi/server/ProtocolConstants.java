/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.server;

public interface ProtocolConstants {

    int PROTOCOL_HEADER = 0x4a524d49; // JRMI
    int PROTOCOL_VERSION = 2;

    int STREAM_PROTOCOL = 0x4b;
    int SINGLE_OP_PROTOCOL = 0x4c;
    int MULTIPLEX_PROTOCOL = 0x4d;

    int PROTOCOL_ACK = 0x4e;
    int PROTOCOL_NACK = 0x4f;

    int MESSAGE_CALL = 0x50;
    int MESSAGE_CALL_ACK = 0x51;
    int MESSAGE_PING = 0x52;
    int MESSAGE_PING_ACK = 0x53;
    int MESSAGE_DGCACK = 0x54;

    int RETURN_ACK = 0x01;
    int RETURN_NACK = 0x02;

    int DEFAULT_PROTOCOL = STREAM_PROTOCOL;

}
