package kaffe.lang;

/**
 * This class implements the ResourceReader interface
 * based on a "CLASSPATH" which can be given as a parameter.
 *
 * @author Godmar Back (gback@cs.utah.edu)
 */
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public
class ClassPathReader implements ResourceReader
{
    public static boolean debug;
    void log(String msg)
    {
	if (debug) {
	    System.out.println("<ClassPathReader>: " + msg);
	}
    }

    /**
     * Components of this path
     */
    private Vector pathcomps;		// strings or zipfiles
    private Vector pathcompsStr;	// as strings
    private boolean cache;		// should I cache bytecode?

    /**
     * Hashtable of already read classes
     */
    private Hashtable	classcache;

    /**
     * Build a class path from the specified path string
     */
    public ClassPathReader(boolean cache, String pathstr) 
    {
	this.cache = cache;
	if (cache) {
	    classcache = new Hashtable();
	}
	pathcomps = new Vector(5,2);
	pathcompsStr = new Vector(5,2);
	Vector temp = new Vector(5,2);

	String s = pathstr;
	int i;
	while ((i = s.indexOf(File.pathSeparatorChar)) != -1) {
	    if (i == 0)
		temp.addElement(".");
	    else
		temp.addElement(s.substring(0, i));
	    s = s.substring(i+1);
	}
	temp.addElement(s);

	/* now look for zip files and replace entries with ZipFiles */
	for (i = 0; i < temp.size(); i++) {
	    s = (String)temp.elementAt(i);
	    File f = new File(s);
	    if ((s.endsWith(".jar") || s.endsWith(".zip")) && f.isFile()) {
		try {
		    ZipFile  zip   = new ZipFile(s);
		    pathcomps.addElement(zip);
		    pathcompsStr.addElement(s);
		} catch (Exception ze) {
		}
	    } else {
		if (f.isDirectory()) {
		    pathcomps.addElement(s);
		    pathcompsStr.addElement(s);
		}
	    }
	}
	log("pathcompsStr = " + pathcompsStr);
	log("pathcomps = " + pathcomps);
	log("cache = " + cache);
    }

    public InputStream getResourceAsStream(String cname) {
	InputStream	stream = null;

	// iterate through all components and look for file
	for (int i = 0; i < pathcomps.size(); i++) {

	    Object comp = pathcomps.elementAt(i);
	    if (comp instanceof String) {
		try {
		    String fn  = ((String)comp).concat("/").concat(cname);

		    log("opening " + fn);
		    // a regular file
		    stream = new FileInputStream(fn);
		} catch (IOException ie1) {
		    continue;
		} 
	    } else
	    if (comp instanceof ZipFile) {
		try {
		    ZipFile zip = (ZipFile)comp;
		    ZipEntry entry = zip.getEntry(cname);

		    log("entry for " + cname + " is " + entry);

		    // avoid throwing and catching a null pointer exception
		    if (entry == null)
			throw new Exception(cname + " not found in zip file");

		    stream = zip.getInputStream(entry);

		} catch (Exception ie0) {
		    continue;
		}
	    }

	    return (stream);
	}
	return (null);
    }

    /**
     * resolve a class name of the form foo.bar.baz
     *
     * @param classname fully qualified class name
     * @return byte[] array of bytes containing .class file
     */
    public byte[] getByteCode(String classname) throws Exception
    {
	byte [] p = null;

	// check in cache first if caching
	if (cache) {
	    p = (byte [])classcache.get(classname);
	}

	if (p != null) {
	    return p;
	}

	String cname = classname.replace('.', '/').concat(".class");
	BufferedInputStream file;
	file = new BufferedInputStream(getResourceAsStream(cname));

	if (file == null) {
	    throw new Exception("Couldn't resolve class `" 
		+ classname + "'\n" 
	    	+ " in CLASSPATH " + this);
	}

	// now try to read the file
	p = new byte[file.available()];
	int bread = file.read(p);
	if (bread != p.length)
	    throw new Exception("Short read for " + classname 
		    + " read " + bread + ", expected " + p.length);

	if (cache)
	    classcache.put(cname, p);
	return (p);
    }

    /**
     * convert in printable form
     * 
     * @return String describing this class path
     */
    public String toString() {
	return pathcompsStr.toString();
    }

    /** a test */
    public static void main(String a[]) throws Exception
    {
        ClassPathReader p = 
	    new ClassPathReader(true, System.getProperty("java.class.path"));
        System.out.println(p.getByteCode("java.lang.Object"));
        System.out.println(p.getByteCode("java.io.File"));
        System.out.println(p.getByteCode("kaffe.lang.ClassPathReader"));
        System.out.println(p.getByteCode("lalala"));
    }
}
