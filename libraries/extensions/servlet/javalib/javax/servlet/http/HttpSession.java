/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet.http;

public interface HttpSession {

public abstract String getId();
public abstract HttpSessionContext getSessionContext();
public abstract long getCreationTime();
public abstract long getLastAccessedTime();
public abstract void invalidate();
public abstract void putValue(String name, Object value);
public abstract Object getValue(String name);
public abstract void removeValue(String name);
public abstract String[] getValueNames();
public abstract boolean isNew();

}
