/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import kaffe.lang.SystemClassLoader;

/**
 * This isn't really a class loader - but it enables us to get hold
 * of the current class loader.
 */
public class DummyClassLoader extends SecurityManager {

static DummyClassLoader singleton = new DummyClassLoader();

DummyClassLoader() {
}

static public ClassLoader getCurrentClassLoader() {
	ClassLoader loader = singleton.currentClassLoader();
	if (loader == null) {
		loader = SystemClassLoader.getClassLoader();
	}
	return (loader);
}

}
