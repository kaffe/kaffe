/*
 * Deubg.java.
 * Interface to kaffe's debugging and status reporting flags.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * 
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
package kaffe.management;

/**
 * An interface to kaffe's debugging and informational message
 * systems.  This class allows one to change the sorts of messages
 * printed after by a running virtual machine.
 *
 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
 */
public class Debug {
	static { System.loadLibrary("management"); }

	/**
	 * Reset debug flags, equivalent to invoking kaffe with
	 * -vmdebug <i>list</i>.  enable("") clears all debug flags.
	 *
	 * @param list ',' seperated list of debug flags
	 *
	 * <!-- void -->
	 *
	 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
	 */
	public static native void enable(String list);

	/**
	 * Print list of valid debug flags to stderr.
	 *
	 * <!-- void -->
	 *
	 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
	 */
	public static void list() { enable("list"); }

	/** Don't report garbage collections */
	public static final int GC_QUIET= 0;
	/**
	 * Report two line summary after each collection (equivalent
	 * to -verbosegc).
	 */
	public static final int GC_REPORT = 1;
	/** Show allocated memory by type (equivalent to -verbosemem) */
	public static final int GC_STATISTICS = 2;

	/**
	 * Set gc reporing level
	 * @param level one of { GC_QUIET, GC_REPORT, GC_STATISTICS }
	 *
	 * <!-- void -->
	 *
	 * @see GC_QUIET, GC_REPORT, GC_STATISTICS 
	 *
	 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
	 */
	public static native void setVerboseGC(int level);

	/**
	 * Toggle printing of informational messages by jit.  
	 * 
	 * @param on true to print a message for each method compiled
	 *
	 * <!-- void -->
	 *
	 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
	 */
	public static native void setVerboseJIT(boolean on);

	/**
	 * Enable or disable statistics collection.  Statistics are
	 * printed when Kaffe exits.  Equivalent to invoking kaffe
	 * with -vmstat <i>list</i>
	 *
	 * @param list "all" seems to work
	 *
	 * <!-- void -->
	 *
	 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
	 */
	public static native void enableStats(String list);

	/**
	 * currently does nothing, it would be nice if at least three
	 * settings where available:<br>
	 * 0: don't even think about it<br>
	 * 1: generate soft calls but don't print<br>
	 * 2: print method call and returns<br>
	 * since watching a JVM start up is seldom interesting.
	 * 
	 * @param level 
	 *
	 * <!-- void -->
	 *
	 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
	 */
	public static native void setTracing(int level);
}
		
