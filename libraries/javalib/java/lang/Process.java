package java.lang;

import java.io.InputStream;
import java.io.OutputStream;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class Process
  extends Object
{
public Process() {}

abstract public void destory();

abstract public int exitValue();

abstract public InputStream getErrorStream();

abstract public InputStream getInputStream();

abstract public OutputStream getOutputStream();

abstract public int waitFor() throws InterruptedException;
}
