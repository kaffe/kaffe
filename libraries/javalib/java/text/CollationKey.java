/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.lang.String;

public final class CollationKey implements Comparable {

private String str;
private Collator target;

private CollationKey() {}
    
public int compareTo(Object obj) {
	return compareTo((CollationKey)obj);
}

public int compareTo(CollationKey other) {
	if (target != other.target) {
		return (-1);
	}
	else {
		return (target.compare(str, other.str));
	}
}

public boolean equals(Object obj) {
	return (obj instanceof CollationKey)
		&& compareTo((CollationKey)obj) == 0;
}

public String getSourceString() {
	return (str);
}

public int hashCode() {
	return (super.hashCode());
}

public byte[] toByteArray() {
	return (str.getBytes());
}

}
