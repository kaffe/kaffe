package java.text;

public class ParseException extends Exception {

private int offset;

public ParseException(String mess, int off) {
	super(mess);
	offset = off;
}

public int getErrorOffset() {
	return (offset);
}

};
