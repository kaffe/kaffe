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

public interface Owner {
    boolean addOwner(Principal invoker, Principal to_be_added) throws NotOwnerException;
    boolean deleteOwner(Principal invoker, Principal to_be_deleted) throws LastOwnerException, NotOwnerException;
    boolean isOwner(Principal to_be_checked);
}
