
/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import gnu.java.nio.channels.FileChannelImpl;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class UNIXProcess extends Process {
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
	FileOutputStream sync;
	Throwable throwable;		// saved to rethrow in correct thread

public UNIXProcess(final String argv[], final String arge[], File dir)
		throws Throwable {
	
	/*
	 * Use supplied directory, or current directory if null
	 */
	final String dirPath = (dir != null) ? dir.toString() : ".";

	/* We first create a thread to start the new process in.  This
	 * is because on some system we can only wait for the child from
	 * it's parent (UNIX for example).  So, we have to create a thread
	 * to do the waiting.
	 */
	Thread sitter = new Thread() {
		public void run() {
			int fae = 0;
			try {
				fae = forkAndExec(argv, arge, dirPath);
			}
			catch (Throwable t) {
				// save it to rethrow in correct thread
				throwable = t;
				synchronized(this) {
					this.notify();
				}
				return;
			}
			if (fae == 0) {
				synchronized(this) {
					isalive = true;
					this.notify();
				}
				exit_code = execWait();
			}
			synchronized(this) {
				isalive = false;
				this.notifyAll();
			}
			synchronized(UNIXProcess.this) {
				try_close(sync);
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
		if (throwable != null) {
			// rethrow in current thread
			try {
				/* Try to chain the exceptions.
				 * The exception we want to chain may already
				 * have its cause set, so we need a fresh instance
				 * to work on.
				 */
				Throwable throw_me = (Throwable) throwable.getClass().newInstance();
				throw_me.initCause(throwable);
				throw throw_me;
			}
			catch (InstantiationException e) {
				throw throwable.fillInStackTrace();
			}
			catch (IllegalAccessException e) {
				throw throwable.fillInStackTrace();
			}
		}

		// Create streams from the file descriptors
		stdin_stream = new FileOutputStream(stdin_fd);
		raw_stdout = new FileInputStream(stdout_fd);
		raw_stderr = new FileInputStream(stderr_fd);

		// now signal child to proceed
		sync = new FileOutputStream(sync_fd);
		byte[] sbuf = new byte[1];
		try {
			sync.write(sbuf);
		}
		catch (IOException _) {
		}
	}
}

public int exitValue() {
	if (isalive) {
		throw new IllegalThreadStateException();
	}
	return exit_code;
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

public int getPID() {
	return pid;
}

public int waitFor() throws InterruptedException {
	synchronized(this) {
		while (isalive) {
			wait();
		}
	}
	return (exit_code);
}

public void destroy() {
	sendSignal(getKillSignal());
	try {
		raw_stdout.close();
		raw_stderr.close();
		stdin_stream.close();
		sync.close();
	}
	catch (IOException e) {
		e.printStackTrace();
	}
}

public void sendSignal(int signum) {
	if (!isalive)
		return;
	sendSignal(pid, signum);
}

public static void sendSignal(int pid, int signum) {
	sendSignal0(pid, signum);
}

private native int forkAndExec(Object cmd[], Object env[], String dirPath);
private native int execWait();
private native static void sendSignal0(int pid, int signum);
private native static int getKillSignal();

protected void finalize() throws Throwable {
	super.finalize();
	try_close(raw_stdout);
	try_close(raw_stderr);
	try_close(stdin_stream);
}

private static void try_close(InputStream stream) {
	if (stream != null) {
		try {
			stream.close();
		}
		catch (IOException e) {}
	}
}

private static void try_close(OutputStream stream) {
	if (stream != null) {
		try {
			stream.close();
		}
		catch (IOException e) {}
	}
}

}

