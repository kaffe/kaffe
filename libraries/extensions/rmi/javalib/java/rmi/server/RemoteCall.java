/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.io.IOException;
import java.io.ObjectOutput;
import java.io.ObjectInput;
import java.io.StreamCorruptedException;

public interface RemoteCall {

    ObjectOutput getOutputStream() throws IOException;
    void releaseOutputStream() throws IOException;
    ObjectInput getInputStream() throws IOException;
    void releaseInputStream() throws IOException;
    ObjectOutput getResultStream(boolean success) throws IOException, StreamCorruptedException;
    void executeCall() throws Exception;
    void done() throws IOException;

}
