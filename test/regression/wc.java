import java.io.*;

// This works like the UNIX wc(1) program
public class wc {
    private static int tc, tl, tw;

    // a helper class to count characters passing through a stream
    class counter extends InputStream {
	int c;
	InputStream in;

	counter(InputStream in) {
	    this.in = in;
	}

	public int read() throws IOException {
	    int r = in.read();
	    if (r >= 0)
		c++;
	    return r;
	}

        public void close() throws IOException {
	    in.close();
	}

        public int available() throws IOException {
            return in.available();
        }

        public synchronized void mark(int readlimit) {
            in.mark(readlimit);
        }

        public int read(byte b[]) throws IOException {
            int r = in.read(b);
	    if (r > 0)
		c += r;
	    return r;
        }

        public int read(byte b[], int off, int len) throws IOException {
            int r = in.read(b, off, len);
	    if (r > 0)
		c += r;
	    return r;
        }

        public long skip(long n) throws IOException {       
            return in.skip(n);
        }

        public synchronized void reset() throws IOException {
            in.reset();
        }

        public boolean markSupported() {
            return in.markSupported();
        }

	private int getCount() {
	    return c;
	}
    }

    public wc(InputStream in, String name) throws Exception {
	int c, l, w;

	c = l = w = 0;

	counter ct = new counter(in);
	StreamTokenizer s = new StreamTokenizer(new InputStreamReader(ct));
	s.resetSyntax();
	s.wordChars(0, 255);
	s.whitespaceChars(' ', ' ');
	s.whitespaceChars('\n', '\n');
	s.whitespaceChars('\t', '\t');
	s.eolIsSignificant(true);

	for (;;) {
	    int t = s.nextToken();
	    switch (t) {
	    case StreamTokenizer.TT_EOF:
		c = ct.getCount();
		System.out.println(l + "\t" + w + "\t" + c + "\t" + name);
		tc += c;
		tl += l;
		tw += w;
		return;

	    case StreamTokenizer.TT_EOL:
		l++;
		break;

	    default:
		w++;
		break;
	    }
	}
    }

    public static void main(String av[]) throws Exception {
        if (av.length == 0)
            new wc(System.in, "");

        for (int i = 0; i < av.length; i++)
            new wc(new FileInputStream(new File(av[i])), av[i]);
	if (av.length > 1)
	    System.out.println(tl + "\t" + tw + "\t" + tc + "\ttotal");
    }
}



// java args: wc < $1
/* Expected Output:
118	321	2379	
*/
