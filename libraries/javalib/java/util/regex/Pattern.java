package java.util.regex;
import java.io.Serializable;
import gnu.regexp.RE;
import gnu.regexp.RESyntax;
import gnu.regexp.REException;
import java.util.ArrayList;

public final class Pattern implements Serializable {

    public static final int UNIX_LINES = 1;
    public static final int CASE_INSENSITIVE = 2;
    public static final int COMMENTS = 4;
    public static final int MULTILINE = 8;
    public static final int DOTALL = 32;
    public static final int UNICODE_CASE = 64;
    public static final int CANON_EQ = 128;

    private String pattern;
    private int flags;
    
    private RE re;
    RE getRE() { return re; }

    private Pattern(String pattern, int flags) throws PatternSyntaxException {
	this.pattern = pattern;
	this.flags = flags;

	int gnuFlags = 0;
	if ((flags & CASE_INSENSITIVE) != 0) gnuFlags |= RE.REG_ICASE;
	if ((flags & MULTILINE) != 0) gnuFlags |= RE.REG_MULTILINE;
	if ((flags & DOTALL) != 0) gnuFlags |= RE.REG_DOT_NEWLINE;
	// not yet supported:
	// if ((flags & UNICODE_CASE) != 0) gnuFlags =
	// if ((flags & CANON_EQ) != 0) gnuFlags =

	// Eventually there will be such a thing as JDK 1_4 syntax
	RESyntax syntax = RESyntax.RE_SYNTAX_PERL5;
	if ((flags & UNIX_LINES) != 0) {
	    // Use a syntax set with \n for linefeeds?
	    syntax = new RESyntax(syntax);
	    syntax.setLineSeparator("\n");
	}

	if ((flags & COMMENTS) != 0) {
	    // Use a syntax with support for comments?
	}

	try {
	    this.re = new RE(pattern, gnuFlags, syntax);
	} catch (REException e) {
	    throw new PatternSyntaxException(e.getMessage(),
					     pattern, e.getPosition());
	}
    }
    
    public static Pattern compile(String regex) throws PatternSyntaxException {
	return compile(regex, 0);
    }

    public static Pattern compile(String regex, int flags) throws PatternSyntaxException {
	return new Pattern(regex, flags);
    }

    public static boolean matches(String regex, CharSequence input) throws PatternSyntaxException {
	return compile(regex).matcher(input).matches();
    }

    public String pattern() {
	return pattern;
    }

    public int flags() {
	return flags;
    }

    public Matcher matcher(CharSequence input) {
	return new Matcher(this, input);
    }

    public String[] split(CharSequence input) {
	return split(input, 0);
    }

    private static final String[] modelArray = new String [0];

    public String[] split(CharSequence input, int limit) {
	Matcher matcher = new Matcher(this, input);
	ArrayList list = new ArrayList();
	int empties = 0;
	int count = 0;
	int start = 0;
	int end;
	boolean matched;
	while (matched = matcher.find() && (limit <= 0 || count < limit - 1)) {
	    ++count;
	    end = matcher.start();
	    if (start == end) {
		empties++;
	    } else {
		while (empties-- > 0) {
		    list.add("");
		}
		String text = input.subSequence(start, end).toString();
		list.add(text);
	    }
	    start = matcher.end();
	}

	// We matched nothing.
	if (!matched && count == 0) {
		return new String[] { input.toString() };
	}

	// Is the last token empty?
	boolean emptyLast = (start == input.length());

	// Can/Must we add empties or an extra last token at the end?
	if (list.size() < limit || limit < 0 || (limit == 0 && !emptyLast)) {
	    if (limit > list.size()) {
		int max = limit - list.size();
		empties = (empties > max) ? max : empties;
	    }
	    while (empties-- > 0) {
		list.add("");
	    }
	}

	// last token at end
	if (limit != 0 || (limit == 0 && !emptyLast)) {
	    String t = input.subSequence(start, input.length()).toString();
	    if ("".equals(t) && limit == 0) {
		// Don't add.
	    } else {
		list.add(t);
	    }
	}

	String[] output = new String [list.size()];
	list.toArray(output);
	return output;
    }
}
