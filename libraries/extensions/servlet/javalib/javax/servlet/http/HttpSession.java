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

    String getId();
    HttpSessionContext getSessionContext();
    long getCreationTime();
    long getLastAccessedTime();
    void invalidate();
    void putValue(String name, Object value);
    Object getValue(String name);
    void removeValue(String name);
    String[] getValueNames();
    boolean isNew();

}
