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

public interface Acl extends Owner {
    boolean addEntry(Principal invoker, AclEntry to_be_added) throws NotOwnerException;
    boolean checkPermission(Principal valid_authenticated_principal, Permission to_be_checked);
    Enumeration entries();
    String getName();
    Enumeration getPermissions(Principal principal);
    boolean removeEntry(Principal invoker, AclEntry to_be_removed) throws NotOwnerException;
    void setName(Principal invoker, String name) throws NotOwnerException;
    String toString();
}
