/*
 * JIT.java
 * Interface to KVM JIT compiler.
 *
 * Copyright (c) 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
package kaffe.management;

/**
 * This class allow direct access to the KVM's translator
 * It allows to dump a description of all currently active methods
 * to a print stream, and it allows the restoration of that state
 * from a low-priority thread concurrently with a running program.
 * 
 * In certain application scenarios, this can be used to reduce the
 * perception of the startup latency introduced by loading and translating
 * classes and methods on demand.
 */

import java.io.*;

public class JIT implements Runnable {
    /**
     * dump a list of all methods loaded by a given class loader
     */
    public static native void dumpActiveMethods(PrintStream printstream,
						ClassLoader loader);

    /**
     * dump a list of all methods loaded by the system class loader
     */
    public static void dumpActiveMethods(PrintStream printstream) {
	dumpActiveMethods(printstream, null);
    }

    /**
     * load a method consisting of class, method, signature as dumped
     * by dumpActiveMethods using a given loader
     */
    public static native void translateMethod(String clazz, 
					String method, 
					String signature,
					ClassLoader loader);

    /**
     * load a method consisting of class, method, signature as dumped
     * by dumpActiveMethods using the system class loader
     */
    public static void translateMethod(String clazz, 
					String method, 
					String signature) 
    {
	translateMethod(clazz, method, signature, null);
    }

    /**
     * Do a full flush of the D cache to ensure all jitted code is out of
     * the cache.  Architecture-dependent.
     */
    public static native void flushCache();

    /**
     * Dump all active methods loaded by the system loader to a file
     */
    public static void dumpActiveMethods(String fname) 
	throws IOException 
    {
	PrintStream p = new PrintStream(new FileOutputStream(fname));
	dumpActiveMethods(p);
	p.close();
    }

    /**
     * load and translate from file using system classloader
     */
    public static void startPreloading(String fname) throws IOException 
    {
	startPreloading(fname, false);
    }

    /**
     * start loading classes through system loader
     */
    public static void startPreloading(String fname, boolean verbose) 
	throws IOException 
    {
	startPreloading(new FileInputStream(fname), null, verbose);
    }

    /**
     * load and translate from stream using given classloader and be verbose
     * or not.
     */
    public static void startPreloading(InputStream is, ClassLoader loader, boolean verbose) 
	throws IOException 
    {
	Thread t = new Thread(new JIT(is, loader, verbose));
	t.setPriority(Thread.MIN_PRIORITY);
	t.start();
    }

    public void run() {
	Thread.currentThread().setName("JIT-Translator");
	try {
	    String []l = new String[3];

	    for (;;) {
		for (int i = 0; i < l.length; i++) {
		    l[i] = reader.readLine();
		    if (l[i] == null) {
			throw new Exception("reached end of file");
		    }
		}
		if (verbose) {
		    System.err.println("translating " 
			+ l[0] + "." + l[1] + l[2]);
		}

		/* ignore linkage errors and exceptions */
		try {
		    translateMethod(l[0], l[1], l[2], loader);
		} catch (LinkageError le) {
		    if (verbose) {
			System.err.println("JIT-Translator: " + le);
		    }
		} catch (Exception e) {
		    if (verbose) {
			System.err.println("JIT-Translator: " + e);
		    }
		}
		Thread.yield();
	    }
	} catch (Exception _) {
	    if (verbose) {
		System.err.println("JIT-Translator: " + _);
	    }
	}

	flushCache();
	if (verbose) {
	    System.err.println("JIT-Translator done.");
	}
    }

    private JIT(InputStream is, ClassLoader loader, boolean verbose) throws IOException {
        this.verbose = verbose;
        this.loader = loader;
        this.reader = new LineNumberReader(new InputStreamReader(is));
    }

    private LineNumberReader    reader;
    private boolean             verbose;
    private ClassLoader         loader;

    public static void main(String av[]) throws IOException {
        PrintStream p = new PrintStream(new BufferedOutputStream(new FileOutputStream(av[0])));
        dumpActiveMethods(p);
        System.exit(0);
    }

    // load native library if not already loaded
    static {
        System.loadLibrary("management");
    }
}
