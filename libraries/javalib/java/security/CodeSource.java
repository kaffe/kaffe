
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.	 All rights reserved.
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

    private final URL location;
    private final java.security.cert.Certificate [] certificates;

    public CodeSource(final URL location, final java.security.cert.Certificate[] certificates) {
	this.location = location;
	if (certificates != null) {
	    this.certificates = (java.security.cert.Certificate[]) certificates.clone();
	} else {
	    this.certificates = null;
	}
    }

    public boolean equals(Object obj) {
	if (this == obj) {
	    return true;
	}
	else if (obj == null || getClass() != obj.getClass()) {
	    return false;
	}

	CodeSource that = (CodeSource) obj;

	if (location == null) {
	    if (that.location != null) {
		return false;
	    }
	} else if (!location.equals(that.location)) {
	    return false;
	}
	if (certificates == null) {
	    if (that.certificates != null) {
		return false;
	    }
	} else {
	    if (!getCertSet().equals(that.getCertSet())) {
		return false;
	    }
	}
	return true;
    }

    public final java.security.cert.Certificate[] getCertificates() {
	if (certificates == null) {
	    return null;
	}
	return (java.security.cert.Certificate[]) certificates.clone();
    }

    private Set getCertSet() {
	return new HashSet(Arrays.asList(getCertificates()));
    }

    public final URL getLocation() {
	return location;
    }

    public int hashCode() {
	int sum = 0;
	if (location != null) {
	    sum += location.hashCode();
	}
	if (certificates != null) {
	    for (int i = 0; i < certificates.length; i++) {
		sum += certificates[i].hashCode();
	    }
	}
	return sum;
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
	    if	(getLocation().getHost() != null && ! getLocation().getHost().equals("")) {
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
	    + ",certificates=" + (certificates != null ? getCertSet().toString() : "none")
	    + ']';
    }
}
