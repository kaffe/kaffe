
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

import java.io.Serializable;
import java.net.SocketPermission;
import java.net.URL;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class CodeSource implements Serializable {

    private URL location;
    private java.security.cert.Certificate [] certificates;

    public CodeSource(URL location, java.security.cert.Certificate[] certificates) {
	this.location = location;
	this.certificates = certificates;
    }

    public boolean equals(Object obj) {
	if (this == obj) {
	    return true;
	}
	else if (obj == null || getClass() != obj.getClass()) {
	    return false;
	}

	CodeSource that = (CodeSource) obj;

	return getLocation().equals(that.getLocation())
	    && getCertSet().equals(that.getCertSet());
    }

    public final java.security.cert.Certificate[] getCertificates() {
	return certificates;
    }

    private Set getCertSet() {
	return new HashSet(Arrays.asList(getCertificates()));
    }

    public final URL getLocation() {
	return location;
    }

    public int hashCode() {
	return getLocation().hashCode()
	    ^ getCertSet().hashCode();
    }

    public boolean implies(CodeSource other) {
	/* Check 1 */
	if (other == null) {
	    return false;
	}

	/* Check 2 */
	if (getCertificates() != null) {
	    if (other.getCertificates() == null) {
		return false;
	    }

	    if (!other.getCertSet().contains(getCertSet())) {
		return false;
	    }
	}

	/* Check 3 */
	if (getLocation() != null) {

	    /* Check 3.1 */
	    if (other.getLocation() == null) {
		return false;
	    }

	    /* Check 3.2 */
	    if (getLocation().equals(other.getLocation())) {
		return true;
	    }

	    /* Check 3.3 */
	    if (!getLocation().getProtocol().equals(other.getLocation().getProtocol())) {
		return false;
	    }

	    /* Check 3.4 */
	    if  (getLocation().getHost() != null) {
		if (! new SocketPermission(getLocation().getHost(), "")
		    .implies(new SocketPermission(other.getLocation().getHost(), ""))) {
		    return false;
		}
	    }

	    /* Check 3.5 */
	    if (getLocation().getPort() != -1) {
		if (getLocation().getPort() != other.getLocation().getPort()) {
		    return false;
		}
	    }

	    /* Check 3.6 */
	    if (! getLocation().getFile().equals(other.getLocation().getFile())) {
		String this_file = getLocation().getFile();
		String other_file = other.getLocation().getFile();

		if (this_file.endsWith("/-")) {
		    if (! other_file.startsWith(this_file.substring(0, this_file.length() - 1))) {
			return false;
		    }
		}
		else if (this_file.endsWith("/*")) {
		    if (! (other_file.startsWith(this_file)
			   && other_file.lastIndexOf('/') < this_file.length())) {
			return false;
		    }
		}
		else if (! this_file.endsWith("/")) {
		    if (! other_file.equals(this_file + '/')) {
			return false;
		    }
		}
	    }

	    /* Check 3.7 */
	    if (getLocation().getRef() != null) {
		if ( ! getLocation().getRef().equals(other.getLocation().getRef())) {
		    return false;
		}
	    }
	}


	return true;
    }

    public String toString() {
	return getClass().getName()
	    + "[location=" + getLocation()
	    + ",certificates=" + getCertSet()
	    + ']';
    }
}
