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

public ObjectOutput getOutputStream() throws IOException;
public void releaseOutputStream() throws IOException;
public ObjectInput getInputStream() throws IOException;
public void releaseInputStream() throws IOException;
public ObjectOutput getResultStream(boolean success) throws IOException, StreamCorruptedException;
public void executeCall() throws Exception;
public void done() throws IOException;

}
