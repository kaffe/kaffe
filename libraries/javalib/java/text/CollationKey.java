package java.text;

public final class CollationKey {

private String str;
private Collator target;

public int compareTo(CollationKey other) {
	if (target != other.target) {
		return (-1);
	}
	else {
		return (target.compare(str, other.str));
	}
}

public boolean equals(Object obj) {
	try {
		if (compareTo((CollationKey)obj) == 0) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
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
