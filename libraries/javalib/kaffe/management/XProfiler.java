/*
 * XProfiler.java
 * Java interface to the cross language profiler
 *
 * Copyright (c) 2000 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */
package kaffe.management;

/**
 * This class provides access to the underlying cross language profiling
 * infrastructure.  Currently, its limited to turning accounting on or off
 * and starting a new profiling stage.
 */
public class XProfiler {

	static {
		System.loadLibrary("management");
	}
	
	/**
	 * Turn profile accounting on, the default
	 */
	public static native void on();
	/**
	 * Turn profile accounting off
	 */
	public static native void off();

	/**
	 * Transition to a new profiling stage, the given name is used to
	 * identify the previous stage.
	 */
	public static native void stage(String name);

}
