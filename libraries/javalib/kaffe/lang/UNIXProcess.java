/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import java.lang.String;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class UNIXProcess
  extends Process
  implements Runnable
{
	boolean isalive;
	int exit_code;
	FileDescriptor stdin_fd;
	FileDescriptor stdout_fd;
	FileDescriptor stderr_fd;
	FileDescriptor sync_fd;
	int pid;
	OutputStream stdin_stream;
	InputStream raw_stdout;
	InputStream raw_stderr;
	int numReaders;		// what's that for?
	private static Thread tidy;

public UNIXProcess() {
}

public UNIXProcess(String argv[], String arge[]) {

	stdin_fd = new FileDescriptor();
	stdout_fd = new FileDescriptor();
	stderr_fd = new FileDescriptor();
	sync_fd = new FileDescriptor();

	/* We first create a thread to start the new process in.  This
	 * is because on some system we can only wait for the child from
	 * it's parent (UNIX for example).  So, we have to create a thread
	 * to do the waiting.
	 */
	final String _argv[] = argv;
	final String _arge[] = arge;
	Thread sitter = new Thread() {
		public void run() {
			if (forkAndExec(_argv, _arge) == 0) {
				synchronized(this) {
					isalive = true;
					notify();
				}
				exit_code = execWait();
			}
			synchronized(this) {
				isalive = false;
				notifyAll();
			}
			synchronized(UNIXProcess.this) {
				UNIXProcess.this.notifyAll();
			}
		}
	};

	/* Start the sitter then wait until it says it's child has started.
	 * We then retrieve the childs connection information.
	 */
	synchronized(sitter) {
		sitter.start();
		try {
			sitter.wait();
		}
		catch (InterruptedException _) {
		}

		// Create streams from the file descriptors
		stdin_stream = new FileOutputStream(stdin_fd);
		raw_stdout = new FileInputStream(stdout_fd);
		raw_stderr = new FileInputStream(stderr_fd);

		// now signal child to proceed
		FileOutputStream sync = new FileOutputStream(sync_fd);
		byte[] sbuf = new byte[1];
		try {
			sync.write(sbuf);
		}
		catch (IOException _) {
		}
	}
}

native public void destroy();

public int exitValue() {
	if (isalive) {
		throw new IllegalThreadStateException();
	}
	return exit_code;
}

native private int forkAndExec(Object cmd[], Object env[]);
native private int execWait();

public void run() {
}

public InputStream getErrorStream() {
	return raw_stderr;
}

public InputStream getInputStream() {
	return raw_stdout;
}

public OutputStream getOutputStream() {
	return stdin_stream;
}

public int waitFor() throws InterruptedException {
	synchronized(this) {
		while (isalive == true) {
			wait();
		}
	}
	return (exit_code);
}

/**
 * Native callback.
 */
private void processDied(int status) {
	synchronized(this) {
		isalive = false;
		exit_code = status;
		notifyAll();
	}
}

}
