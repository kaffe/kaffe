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

package java.security;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

/** @deprecated since 1.2 */
public interface Certificate {
    void decode(InputStream stream) throws IOException, KeyException;
    void encode(OutputStream stream) throws IOException, KeyException;
    String getFormat();
    Principal getGuarantor();
    Principal getPrincipal();
    PublicKey getPublicKey();
    String toString(boolean print_details);
}
