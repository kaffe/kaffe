package java.util.regex;
import gnu.regexp.RE;
import gnu.regexp.REMatch;

public final class Matcher {
    private Pattern pattern;
    private CharSequence input;
    private int position;
    private int appendPosition;
    private REMatch match;

    public Pattern pattern() {
	return pattern;
    }

    Matcher(Pattern pattern, CharSequence input) {
	this.pattern = pattern;
	this.input = input;
    }

    public boolean matches() {
	return find(0);
    }

    public boolean lookingAt() {
	match = pattern.getRE().getMatch(input, 0);
	if (match != null) {
	    if (match.getStartIndex() == 0) {
		return true;
	    }
	    match = null;
	}
	return false;
    }

    public boolean find() {
	boolean first = (match == null);
	match = pattern.getRE().getMatch(input, position);
	if (match != null) {
	    int endIndex = match.getEndIndex();
	    // Are we stuck at the same position?
	    if (!first && endIndex == position) {
		match = null;
		// Not at the end of the input yet?
		if (position < input.length() - 1) {
		    position++;
		    return find(position);
		} else {
		    return false;
		}
	    }
	    position = endIndex;
	    return true;
	}
	return false;
    }

    public boolean find(int start) {
	match = pattern.getRE().getMatch(input, start);
	if (match != null) {
	    position = match.getEndIndex();
	    return true;
	}
	return false;
    }

    private void assertMatchOp() {
	if (match == null) throw new IllegalStateException();
    }

    public int start() {
	assertMatchOp();
	return match.getStartIndex();
    }

    public int start(int group) {
	assertMatchOp();
	return match.getStartIndex(group);
    }

    public int end() {
	assertMatchOp();
	return match.getEndIndex();
    }

    public int end(int group) {
	assertMatchOp();
	return match.getEndIndex(group);
    }
    
    public String group() {
	assertMatchOp();
	return match.toString();
    }

    public String group(int group) {
	assertMatchOp();
	return match.toString(group);
    }

    public int groupCount() {
	return pattern.getRE().getNumSubs();
    }

    public Matcher appendReplacement(StringBuffer sb, String replacement) {
	assertMatchOp();
	sb.append(input.subSequence(appendPosition, match.getStartIndex()).toString());
	sb.append(match.substituteInto(replacement));
	appendPosition = match.getEndIndex();

	return this;
    }

    public StringBuffer appendTail(StringBuffer sb) {
	sb.append(input.subSequence(appendPosition, input.length()).toString());
	return sb;
    }

    public String replaceAll(String replacement) {
	reset();
	return pattern.getRE().substituteAll(input, replacement, position);
    }

    public String replaceFirst(String replacement) {
	reset();
	// Semantics might not quite match
	return pattern.getRE().substitute(input, replacement, position);
    }

    public Matcher reset() {
	position = 0;
	match = null;
	return this;
    }

    public Matcher reset(CharSequence input) {
	this.input = input;
	reset();
	return this;
    }
}
