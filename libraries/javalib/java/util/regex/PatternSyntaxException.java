package java.util.regex;

public class PatternSyntaxException extends IllegalArgumentException {
    protected String description;
    protected String pattern;
    protected int index;

    public PatternSyntaxException(String description, String pattern, int index) {
	this.description = description;
	this.pattern = pattern;
	this.index = index;
    }

    public String getDescription() {
	return description;
    }

    public String getPattern() {
	return pattern;
    }

    public int getIndex() {
	return index;
    }

    public String getMessage() {
	return description; // XXX
    }
}
