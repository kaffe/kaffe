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

public interface Group extends Principal {
    boolean addMember(Principal to_be_added);
    boolean isMember(Principal to_be_checked);
    Enumeration members();
    boolean removeMember(Principal to_be_removed);
}
