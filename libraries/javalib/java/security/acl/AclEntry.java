/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *	Dalibor Topic.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Dalibor Topic <robilad@yahoo.com>
 */

package java.security.acl;

import java.security.Principal;
import java.util.Enumeration;

public interface AclEntry extends Cloneable {
    boolean addPermission(Permission to_be_added);
    boolean checkPermission(Permission to_be_checked);
    Object clone();
    Principal getPrincipal();
    boolean isNegative();
    Enumeration permissions();
    boolean removePermission(Permission to_be_removed);
    void setNegativePermissions();
    boolean setPrincipal(Principal to_be_set);
    String toString();
}
