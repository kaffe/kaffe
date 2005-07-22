/* Program : Jar

   Use this class to create .zip and .jar files in Java.
   It should work as a drop in replacement for the JDK's jar program.

   Copyright : Moses DeJong, dejong@cs.umn.edu, 1998, 2000.
   Source code licensed under the GPL.
   You can get a copy of the license from www.gnu.org.

   This code is intended for use in the Kaffe project but you can use
   it in other projects as long as you follow the license rules.

*/


package kaffe.tools.jar;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Date;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Enumeration;
import java.util.jar.Attributes;
import java.util.jar.JarOutputStream;
import java.util.jar.Manifest;
import java.util.zip.CRC32;
import java.util.zip.CheckedInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

import kaffe.util.Base64;

public class Jar {

    public static void main(String[] argv) {
	if (debug) {
	    System.out.println("entered Jar.main()");
	    System.out.flush();
	}

	Jar j = new Jar(argv);

	j.setExitOnError(true);

	try {
	    j.start();
	} catch (IOException e) {
	    e.printStackTrace();
	    j.exit(2);
	}
    }

    public Jar(String[] argv) {
	this.argv = argv;
    }


    //  Begin processing the command line

    public void start() throws IOException {

	// First process the command line arguments so that we know
	// What mode we are going to be running in.

	processCommandLine();

	processJar();

    }


    public void setExitOnError(boolean exitOnError) {
	if (exitOnError) {
	    exitCode = 1;
	} else {
	    exitCode = 0;
	}
    }

    public int getCompletionCode() {
	return exitCode;
    }

    void exit(int code) {
	if (exitCode != 0) {

	    if (debug) {
		System.out.println("now exiting with code " + code);
		System.out.flush();

		try {
		    throw new Exception();
		} catch (Exception e) {
		    e.printStackTrace();
		    System.err.flush();
		    try {
			Thread.sleep(5000);
		    } catch (Exception e2) {}
		}
	    }

	    System.out.flush();
	    System.err.flush();
	    System.exit(code);
	} else {
	    exitCode = code;
	}
    }

    void processCommandLine() throws IOException
    {
	// Initial set of options tell us if we are extracting or
	// creating an archive, if zip compressing will be used,
	// if a manifest will be included and so on.

	if (debug) {
	    System.out.println("entered Jar.processCommandLine()");
	    System.out.flush();
	}

	if (argv.length == 0) {
	    printUsage();
	    exit(1);
	}


	// The first argument gives the options to the jar program

	String options = argv[0];

	// This will store where the "next" argv index

	int options_index = 1;

	int i = 0;

	// if the first char is a '-' just ignore it

	if (options.charAt(0) == '-') {
	    i++;
	}

	for (; i < options.length() ; i++) {
	    switch (options.charAt(i)) {
	    case 'c': // Create new archive
	    case 't': // List archive table
	    case 'x': // Extract from archive
	    case 'u': // Update archive

		// Fall through an process all possible modes
		if (mode != 0) {
		    printUsage();
		    exit(1);
		}

		mode = options.charAt(i);
		break;

	    case 'v': // Verbose listing
		verbose = true;
		break;

	    case 'f': // Archive file name

		if (archive != null || options_index >= argv.length) {
		    printUsage();
		    exit(1);
		}

		archive = argv[options_index++];
		break;

	    case 'm': // Manifest file name

		if (manifest != null || options_index >= argv.length ||
		    !create_manifest) {
		    printUsage();
		    exit(1);
		}

		manifest = argv[options_index++];
		break;

	    case '0': // Turn off ZIP compression
		compression = false;
		break;

	    case 'M': // Do not create a Manifest file in the archive

		if (manifest != null) {
		    printUsage();
		    exit(1);
		}

		create_manifest = false;
		break;

	    default:
		System.out.println("Illegal option: " + options.charAt(i));
		printUsage();
		exit(1);
	    }
	}


	// now get the remaining files that need to be processed
	// and keep track of any -C (dir change) arguments

	i = options_index;
	if ((argv.length - i) > 0) {

	    files = new String[argv.length - i];

	    for (int j = 0; i < argv.length ; i++, j++) {
		if (argv[i].equals("-C")) {
		    // Make sure we did not run out of arguments
		    // and that "-C -C" or "-C dir -C" are not given
		    if ((i + 2) >= argv.length ||
                        argv[i+1].equals("-C") ||
			argv[i+2].equals("-C")) {
			printUsage();
			exit(1);
		    }

		    i++;

		    if (dir_changes == null) {
			dir_changes = new String[files.length]; 
		    }

		    dir_changes[j] = argv[i];
		j--;
		} else {
		    files[j] = argv[i];
		}
	    }


	    // If there were dir changes we must trim the files
	    // so that only valid files are in the array

	    if (dir_changes != null) {
		for (i=0; i < files.length ; i++) {
		    if (files[i] == null) {
			break;
		    }
		}
		String[] tmp;

		tmp = files;
		files = new String[i];
		System.arraycopy(tmp,0,files,0,i);

		tmp = dir_changes;
		dir_changes = new String[i];
		System.arraycopy(tmp,0,dir_changes,0,i);	       
	    }
	}


	// Check to make sure the mode is compatible with the arguments

	switch (mode) {
	case 'c':
	    // When creating we must have at least on file argument

	    if (files == null) {
		System.out.println("'c' flag requires that input files be specified!");
		printUsage();
		exit(1);
	    }

	    break;

	case 't':
	    // Listing an archive can have 0 file arguments.

	    // Make sure that no -C options were given when -t is used

	    if (dir_changes != null) {
		System.out.println("'t' flag can not be used with the -C flag!");
		printUsage();
		exit(1);  
	    }

	    // If archive file name is given then make sure it exists

	    if (archive != null) {
		XPFile archive_file = new XPFile(archive);
		if (! archive_file.exists()) {
		    System.out.println("archive \"" +
				       archive_file.getPath() + "\" does not exist");
		    exit(1);
		}
	    }

	    break;

	case 'x':
	    // Extract from archive can have 0 file arguments.

	    // If archive file name is given make sure it exists

	    if (archive != null) {
		XPFile archive_file = new XPFile(archive);
		if (! archive_file.exists()) {
		    System.out.println("archive \"" +
				       archive_file.getPath() + "\" does not exist");
		    exit(1);
		}
	    }

	    break;

	case 'u':
	    // Update of archive requires files to update
	    // Update without an archive makes no sense.

	    if (files == null || archive == null) {
		System.out.println("'u' flag requires that manifest or archive or input files be specified!");
		printUsage();
		exit(1);
	    }

	    if (archive != null) {
		XPFile archive_file = new XPFile(archive);
		if (! archive_file.exists()) {
		    System.out.println("archive \"" +
				       archive_file.getPath() + "\" does not exist");
		    System.out.flush();
		    exit(1);
		}
	    }

	    break;

	default:
	    System.out.println("One of the options -{ctxu} must be specified!");
	    printUsage();
	    exit(1);
	}

	// lookup table must be created after the mode has been checked
	createFileLookupTable();

	if (debug) {
	    dump();
	}
    }


    void printUsage() {
	PrintStream o = vout;

        o.println("Usage: jar {ctxu}[vfm0M] [jar-file] [manifest-file] [-C dir] files ...");
	o.println("Options:");
	o.print('\t');
	o.println("-c  create new archive");
	o.print('\t');
	o.println("-t  list table of contents for archive");
	o.print('\t');
	o.println("-x  extract named (or all) files from archive");
	o.print('\t');
	o.println("-u  update existing archive");
	o.print('\t');
	o.println("-v  generate verbose output on standard output");
	o.print('\t');
	o.println("-f  specify archive file name");
	o.print('\t');
	o.println("-m  include manifest information from specified manifest file");
	o.print('\t');
	o.println("-0  store only; use no ZIP compression");
	o.print('\t');
	o.println("-M  Do not create a manifest file for the entries");
	o.print('\t');
	o.println("-C  change to the specified directory and include the following files");
	o.println("If any file is a directory then it is processed recursively.");
	o.println("The manifest file name and the archive file name needs to be specified");
	o.println("in the same order the 'm' and 'f' flags are specified.");
	o.println();
	o.println("Example 1: to archive two class files into an archive called classes.jar:");
	o.print('\t');
	o.println("jar cvf classes.jar Foo.class Bar.class");
	o.println("Example 2: use an existing manifest file 'mymanifest' and archive all the");
	o.print('\t');
	o.print('\t');
	o.println("files in the foo/ directory into 'classes.jar':");
	o.print('\t');
	o.println("jar cvfm classes.jar mymanifest -C foo/ .");
    }




    void processJar() throws IOException
    {
	if (debug) {
	    System.out.println("entered Jar.processCommandLine()");
	    System.out.println("current mode is " + mode);
	    System.out.flush();
	}

	// Get the current directory

	switch (mode) {
	case 'c':
	    createJar(files, absolute_files);
	    exit(0);
	    break;
	case 't':
	    listFilesInJar(files);
	    exit(0);
	    break;
	case 'x':
	    extractFilesInJar(files, absolute_files);
	    exit(0);
	    break;
	case 'u':
	    updateFilesInJar(files, absolute_files);
	    exit(0);
	    break;
	default:
	    System.out.println("Unexpected error, '" + mode + "' not matched");
	    exit(1);
	}

    }

    void addMapEntry(Map m, String shortname, XPFile entry)
    {
      // Make sure the entryname ends with a '/' if it is a directory
      
      boolean entryfile_isDirectory = entry.isDirectory();

      if (entryfile_isDirectory) {
	if (shortname.length() == 0) {
	  // Something is very wrong here.
	  throw new RuntimeException("entryname was empty");
	}
	
	if (shortname.charAt( shortname.length() - 1 ) != '/') {
	  shortname = shortname + '/';
	}
      }
      
      m.put(shortname, entry);
      
      if (!entryfile_isDirectory)
	return;
      
      String[] dir_files = entry.list();

      for (int i = 0; i < dir_files.length; i++) {
	XPFile abs_entry = new XPFile(entry, dir_files[i]);
	String rel_entry = new XPFile(shortname, dir_files[i]).getPath();

	addMapEntry(m, rel_entry, abs_entry);
      }
    }

    // Build a set containing all shortnames
    void buildUnicityMap()
    {
      unicityMap = new HashMap();

      for (int i = 0; i < files.length; i++)
	addMapEntry(unicityMap, new XPFile(files[i]).getPath(), new XPFile(absolute_files[i]));
    }
  

    // Create a table that will map argument file names to
    // fully qualified file names.

    void createFileLookupTable() throws IOException
    {

	//final boolean debug = true;

	if (debug) {
	    System.out.println("entered Jar.createFileLookupTable()");
	    System.out.flush();
	}

	if (files == null) {
	    if (debug) {
		System.out.println("no files for lookup table");
	    }	    
	    return;
	}


	File parent = null;
	File tmp;


	// File existence requirements for each mode
	// 'u': Files must exist.
	// 'x': None
	// 't': None
	// 'c': Files must exist.

	boolean requireExistence = (mode == 'u' || mode == 'c');

	boolean existenceError = false;

	absolute_files = new String[files.length];

	for (int i = 0; i < files.length ; i++) {

	    // At this point, make sure that any Windows
	    // Style \ chars in the relative file names are
	    // replaced with unix style / chars so that matching
	    // the file name to the jar entry works. This also
	    // requires that XPFile is used on the file names later.

	    files[i] = files[i].replace('\\','/');

	    if (dir_changes != null && dir_changes[i] != null) {
		parent = new XPFile(dir_changes[i]);
	    }

	    // if the file is already absolute then do not
	    // combine it with the -C argument

	    tmp = new XPFile(files[i]);

	    if (! tmp.isAbsolute()) {
		tmp = new XPFile(parent,files[i]);
	    } else {
		// Find and remove the first '/' in the short name
		String name = files[i];
		int index = name.indexOf('/');

		if (index == -1) {
		    // Something really strange happended, the file
		    // is absolute so something is really screwed up

		    throw new RuntimeException("absolute file " +
			          name + " had no '/' chars in it");
		}

		// gaurd against the case where / is at the end of the string
		if ((index + 1) < name.length()) {
		    files[i] = name.substring(index + 1);
		}
	    }


	    String absolute_name = tmp.getAbsolutePath();

	    if (debug) {
	        String canon = tmp.getCanonicalPath();

		System.out.println("absolute  was \"" + absolute_name + "\"");
		System.out.println("canon     was \"" + canon + "\"");

		System.out.println("file name is \"" + files[i]
				   + "\"");

		System.out.println("absolute_name is \"" +
				   absolute_name + "\"");

		System.out.println("exists() is \"" + tmp.exists() + "\"");

		System.out.println("isDirectory() is \"" +
				      tmp.isDirectory() + "\"");
	    }

	    if (requireExistence && !tmp.exists()) {
		existenceError = true;
		// Non existant file error message
		System.out.println(tmp.getPath() + 
				   ": no such file or directory");
	    }

	    absolute_files[i] = absolute_name;
	}

	if (existenceError) {
	    // The JDK's jar will print each bad file name before
	    // exiting so we do that too

	    if (debug) {
		System.out.println("calling exit() because of existence error");
	    }

	    exit(2); 
	}
	
	// If we are in update mode we also have to walk the full tree to be able to
	// remove the redundant entries in the jar.
	if (requireExistence) {
	  buildUnicityMap();
	}
    }

    // List the file arguments that exist in this jar file
    // if files argument is null then list them all

    void listFilesInJar(String[] shortnames)
	throws IOException {

	if (debug) {
	    System.out.println("entered Jar.listFilesInJar()");
	    System.out.flush();
	}

	ZipInputStream zin;
	ZipEntry       entry;

	//final boolean debug = true;

	if (debug) {
	    System.out.println("opening archive \"" + archive + "\"");
	}

	if (archive == null) {
	    zin = new ZipInputStream(System.in);
	} else {
	    zin = new ZipInputStream(new XPFileInputStream(archive));
	}

	try {
	    while ((entry = zin.getNextEntry()) != null) { 
		// close entry right after we open it so that
		// all the data is read in and we can read the entry.

		zin.closeEntry();


		// see if the current ZipEntry's name equals 
		// the file we want to extract or if it starts
		// with a path matching the dir we want to extract.

		String entry_name = entry.getName();
		boolean match = (shortnames == null);

		if (! match) {
		    if (debug) {
			System.out.println("looking for match for \"" +
					   entry_name + "\"");
		    }

		    for (int i = 0; i < shortnames.length ; i++) {
			if (entry_name.equals(shortnames[i])) {
			    match = true;
			    break;
			} else if (shortnames[i].endsWith("/")) {
			    if (entry_name.startsWith(shortnames[i])) {
				match = true;
				break;
			    }
			} else {
			    if (entry_name.startsWith(shortnames[i] + '/')) { 
				match = true;
				break;
			    }
			}
		    }
		}

		if (match) {
		    if (debug) {
			System.out.println("found match for \"" +
					   entry_name + "\"");
		    }

		    if (verbose) {
			// FORMAT for verbose jar output.
			// 10 Wed Feb 10 01:42:40 CST 1999 data.txt


			// print size in at least 6 char wide field
			// with right justification.

			int width = 6;

			String info = String.valueOf(entry.getSize());

			if (info.length() > width) {
			    vout.print(info);
			} else {
			    while (width > info.length()) {
				vout.print(' ');
				width--;
			    }
			    vout.print(info);
			}


			vout.print(' ');

			Date date = new Date(entry.getTime());

			vout.print(date);

			vout.print(' ');

			vout.println(entry_name);

		    } else {
			vout.println(entry_name);
		    }
		}
	    }

	} finally {
	    zin.close();
	}

    }



    void extractFilesInJar(String[] shortnames, String[] longnames)
	throws IOException
    {
	ZipInputStream zin;
	ZipEntry       entry;

	//final boolean debug = true;

	if (debug) {
	    System.out.println("opening archive \"" + archive + "\"");
	}

	if (archive == null) {
	    zin = new ZipInputStream(System.in);
	} else {
	    zin = new ZipInputStream(new XPFileInputStream(archive));
	}

	try {
	    while ((entry = zin.getNextEntry()) != null) { 
		// see if the current ZipEntry's name equals 
		// the file we want to extract. If equal then
		// print the name of the file to stdout.

		String entry_name = entry.getName();
		boolean match = (shortnames == null);
		String longname = entry_name;

		if (! match) {
		    if (debug) {
			System.out.println("looking for match for \"" +
					   entry_name + "\"");
		    }

		    for (int i = 0; i < shortnames.length ; i++) {
			if (entry_name.equals(shortnames[i])) {
			    match = true;
			    longname = longnames[i];
			    break;
			} else if (shortnames[i].endsWith("/")) {
			    if (entry_name.startsWith(shortnames[i])) {
				String shortname = shortnames[i].substring(0, shortnames[i].length() -1);
				match = true;
				longname = longnames[i].substring(0, longnames[i].lastIndexOf(shortname)) + entry_name;
				break;
			    }
			} else {
			    if (entry_name.startsWith(shortnames[i] + '/')) { 
				match = true;
				longname = longnames[i].substring(0, longnames[i].lastIndexOf(shortnames[i])) + entry_name;
				break;
			    }
			}
		    }
		}

		if (match) {
		    if (debug) {
			System.out.println("found match for \"" +
					   entry_name + "\"");
		    }

		    if (verbose) {
			// FORMAT for verbose jar output.
			// Actually, there is a bug in the JDK jar
			// implementation in that the output is
			// exactly the opposite of what is shown here.
			// The JDK output makes not sense so we do not
			// duplicate the bug in this implementation.

			//  for DEFLATED entries
			//   created: META-INF/
			//  inflated: META-INF/MANIFEST.MF

			//  for STORED entries
		        //   created: META-INF/
			// extracted: META-INF/MANIFEST.MF


			// print in at least 10 char wide field
			// with right justification.

			int width = 10;

			String info;

			if (entry.isDirectory()) {
			    info = "created";
			} else {
			    if (entry.getMethod() == ZipEntry.STORED) {
				info = "extracted";
			    } else {
				info = "inflated";
			    }
			}

			if (info.length() > width) {
			    vout.print(info);
			} else {
			    while (width > info.length()) {
				vout.print(' ');
				width--;
			    }
			    vout.print(info);
			}


			vout.print(':');
			vout.print(' ');

			vout.println(entry_name);

		    }

		    // Now Extract the entry.


		    if (entry.isDirectory()) {
			// If the entry is a directory then
			// we can just create that directory
			// and go on to the next entry

			XPFile dir = new XPFile(longname);
			if (! dir.exists()) {
			    dir.mkdirs();
			}

			continue;
		    }


		    // If it is in a directory that does
		    // not exist we will need to create it.

		    ensureParentsExist(longname);

		    if (debug) {
			System.out.println("opening output file \"" +
					   longname + "\"");
		    }

		    XPFileOutputStream fos = new XPFileOutputStream(longname);

		    try {
			readwriteStreams(zin, fos);
		    } finally {
			fos.close();
		    }

		}
	    }

	} finally {
	    zin.close();
	}

    }






    void updateFilesInJar(String[] shortnames, String[] longnames)
	throws IOException
    {
	    // TODO: Handle the Manifest argument.

	    // create the jar output stream in memory.
	    // rewrite not updated files into jar output stream.
	    // write updated and fresh files into jar output stream.
	    // write memory contents of jar outpur stream to previous
	    // jar file.

	    ZipFile zf = new ZipFile(archive);
	    ByteArrayOutputStream baos = new ByteArrayOutputStream((int) new File(archive).length());
	    ZipOutputStream zos = new ZipOutputStream(new BufferedOutputStream(baos));
	    Enumeration entries = zf.entries();
	    
	    //final boolean debug = true;

	    if (debug) {
		    System.out.println("opening archive \"" + archive + "\"");
	    }
	    
	    try {
		    while (entries.hasMoreElements()) { 
			    // The ZIP file spec allows the entry data to be specified after the
			    // compressed data for that entry. This means entry.getSize() can be 0.
			    // A simple hack-around this is to mark the input stream with a
			    // sufficient look ahead buffer (512 k in this case), get the next entry
			    // and reset. This of course only works, if the files on the zip input stream

			    ZipEntry entry = (ZipEntry) entries.nextElement();

			    long size = entry.getSize();

			    // see if the current ZipEntry's name equals 
			    // the file we want to extract. If equal then
			    // print the name of the file to stdout.
			    
			    String entry_name = entry.getName();
			    String longname = entry_name;
			    
			    if (debug) {
			      System.out.println("looking for match for \"" +
						 entry_name + "\"");
			    }
			    
			    if (unicityMap.get(entry_name) != null) {
			      if (debug) {
				System.out.println("found match for \"" +
						   entry_name + "\"");
			      }
			      
			      // skip matching entries, removing them from the
			      // output stream. So entries that need to be 
			      // updated don't appear on the stream twice.
			      continue;
			    }
			    
			    ZipEntry new_entry = new ZipEntry(entry_name);
			    
			    // Copy values of fields over from original entry
			    // except for compressedSize, which can change
			    // depending on the zip compression implementation.
			    
			    new_entry.setComment(entry.getComment());
			    new_entry.setExtra(entry.getExtra());
			    new_entry.setMethod(entry.getMethod());
			    new_entry.setTime(entry.getTime());
			    new_entry.setSize(entry.getSize());

			    // Directories have a CRC of 0
			    if (size == 0) {
				    new_entry.setCrc(0);
			    }
	  
			    // write the entry out into jar output stream
			    zos.putNextEntry(new_entry);

			    InputStream in = zf.getInputStream(entry);

			    try {
				    readwriteStreams(in, zos);
			    } finally {
				    in.close();
			    }

			    zos.closeEntry();
		    }

		    // add the updated entries
		    
		    Iterator filenames = unicityMap.keySet().iterator();

		    while (filenames.hasNext()) {
		            String name = (String)filenames.next();
			    XPFile infile = (XPFile)unicityMap.get(name);
  			    
			    // add a regular file to the archive
			    addEntry(zos, name, infile);
		    }

		    zos.close();

		    // get the output stream
		    OutputStream os;
		    if (archive == null) {
			    // use stdout
			    os = System.out;
		    }
		    else {
			    // update the archive file
			    os = new XPFileOutputStream(archive);
		    }
		    
		    // write the contents of the updated zip file
		    try {
			    os.write(baos.toByteArray(), 0, baos.size());
		    }
		    finally {
			    os.close();
		    }		    
	    }
	    finally {
		    //		    zos.close();
	    }
	    
    }
  
    void createJar(String[] shortnames, String[] longnames)
	throws IOException
    {
	//final boolean debug = true;

	OutputStream out;

	if (archive == null) {
	    out = System.out;
	    vout = System.err;
	} else {	    
	    // FIXME : test buffering (do we need it, would it help?)

	    //out = new BufferedOutputStream(new XPFileOutputStream(archive));

	    out = new XPFileOutputStream(archive);
	}

	JarOutputStream jos = null;


	try {

	// If there is a manifest file that needs to be added to the jar
	// then we need to create it

	if (create_manifest) {
	    // generic type so that it will work in JDK1.1 with -M option
	    Manifest jar_manifest;

	    if (manifest == null) {
		// Use default manifest

		jar_manifest = new Manifest();
	    } else {
		// Use manifest from file

		InputStream manifest_in = null;

		try {
		    manifest_in =
		        new BufferedInputStream(
			    new XPFileInputStream(manifest) );

		    jar_manifest = new Manifest(manifest_in);

		} finally {
		    if (manifest_in != null)
			manifest_in.close();
		}
	    }

	    // Set manifest version
	    Attributes mainAttr = jar_manifest.getMainAttributes();
	    mainAttr.put(Attributes.Name.MANIFEST_VERSION, "1.0");

	    Iterator filenames = unicityMap.keySet().iterator();

	    // Checksum all files
	    while (filenames.hasNext()) {
		String[] algs = new String[] { "MD5", "SHA" };
		String name = (String)filenames.next();
	    	XPFile infile = (XPFile)unicityMap.get(name);

		// Skip directories
		if (!infile.isFile()) {
		    continue;
		}

		// Set up attributes for this file
		Attributes attr = jar_manifest.getAttributes(name);
		if (attr == null) {
		    attr = new Attributes();
		    jar_manifest.getEntries().put(name, attr);
		}

		// Compute hash values
		StringBuffer alglist = new StringBuffer();
		for (int j = 0; j < algs.length; j++) {
		    DigestInputStream dis;
		    try {
			dis = new DigestInputStream(
			    new BufferedInputStream(
			      new XPFileInputStream(infile)),
			    MessageDigest.getInstance(algs[j]));
		    } catch (NoSuchAlgorithmException e) {
			continue;
		    }
		    byte[] buf = new byte[1024];
		    while (dis.read(buf, 0, buf.length) != -1);
		    byte[] digest = dis.getMessageDigest().digest();
		    attr.putValue(algs[j] + "-Digest",
			new String(Base64.encode(digest)));
		    if (alglist.length() != 0) {
			alglist.append(' ');
		    }
		    alglist.append(algs[j]);
		}
		if (alglist.length() > 0) {
		    attr.putValue("Digest-Algorithms", alglist.toString());
		}
	    }

	    if (debug) {
		System.out.println("------BEGIN Manifest contents");
		jar_manifest.write(System.out);
		System.out.println("------END Manifest contents");
	    }

	    jos = new JarOutputStream(out, jar_manifest);

	    if (verbose) {
		// added manifest		
		vout.println("added manifest");
	    }

	} else {
	    // Do not add a manifest when -M cmd line arg is given

	    jos = new JarOutputStream(out);
	}


	// now that the Jar stream is open we need to add
	// each of the file arguments to the stream
	
	Iterator filenames = unicityMap.keySet().iterator();

	while (filenames.hasNext()) {
	    String name = (String)filenames.next();
	    XPFile infile = (XPFile)unicityMap.get(name);

	    // add a new entry to the archive.
	    addEntry(jos, name, infile);
	}


	} finally {
	    if (jos != null)
		jos.close();
	    else
		out.close();	    
	}
    }

    // This is a helper method that adds a single entry to an archive
    // the entry can be a file or a directory.

    void addEntry(ZipOutputStream zos, String entryname, XPFile entryfile)
    	throws IOException
    {
	// Delete "./" from the top of entryname.  Sun's jar command seems to
	// do this.
	if (entryname.startsWith("./") && entryname.length() > 2) {
	    entryname = entryname.substring(2);
	}
	// YIKES!! Watch out for the really ugly special case where
	// we attempt to add the archive that we are writing to the
	// archive we are writing to the archive we are writing.
	// You get the idea. There is no way to avoid this case
	// when writing to stdout.

	if (archive != null) {
	    XPFile tmp1 = new XPFile(new XPFile(archive).getCanonicalPath());
	    XPFile tmp2 = new XPFile(entryfile.getCanonicalPath());

	    if (debug) {
		System.out.println("archive file is " + archive);
		System.out.println("entry file is " + entryfile);

		System.out.println("archiveC file is " + tmp1);
		System.out.println("entryC file is " + tmp2);
	    }

	    if (tmp1.equals(tmp2)) {
		if (debug) {
		    System.out.println("avoided adding the archive file");
		}
		return;
	    }
	}

	boolean entryfile_isDirectory = entryfile.isDirectory();
	ZipEntry ze = new ZipEntry(entryname);
	long entryfile_length = (entryfile_isDirectory ? 0 : entryfile.length());

	// Set some entry attributes, take care to make sure that
	// directory entries are always saved with no compression
	if (compression && !entryfile_isDirectory) {
	    ze.setMethod(ZipEntry.DEFLATED); // compressed entry
	} else {
	    ze.setMethod(ZipEntry.STORED);   // uncompressed entry
	    ze.setSize(entryfile_length);
	    ze.setCrc( 0 );                  // set after stream is read/written to zip
	}

	zos.putNextEntry(ze);

	if (verbose) {
	    // adding: file1 (in=16) (out=18) (deflated -12%)
	    vout.print("adding: " + entryname + " ");
	    vout.print("(in=" + entryfile_length + ") ");
	}

	if (!entryfile_isDirectory) {

	    // Write file into the archive, compressing if so requested
	    if (debug) {
		System.out.println("opening input file \"" + entryfile + "\"");
	    }

	    InputStream in = new XPFileInputStream(entryfile);

	    // This is part of the ugly workaround for a design flaw
	    // in the JDK zip API, for uncompressed entries we
	    // are forced to calculate a CRC for the input stream
	    CRC32 crc = null;
	    
	    if (! compression) {
		crc = new CRC32();
		in = new CheckedInputStream(in,crc);
	    }

	    try {
		readwriteStreams(in, zos);
	    } finally {
		in.close();
	    }

	    // ZIP design flaw workaround
	    
	    if (! compression) {
		ze.setCrc(crc.getValue());
	    }
	}

	// finish writing the entry to the zip stream
	// this lets us access the compressed size
	// attribute of the current ZipEntry

	zos.closeEntry();


	// TEMP CHECK!
	// If we are writing a directory entry then double
	// check that we did not actually write any bytes to
	// the stream. IF we did then something is wrong.

	if (entryfile_isDirectory) {
	    long tmp;

	    tmp = ze.getSize();
	    if (tmp != 0) {
		throw new RuntimeException(
		    "ZipEntry.getSize() for directory was " + tmp +
		    " it should have been 0");
	    }
	    
	    tmp = ze.getCompressedSize();
	    if (tmp != 0) {
		throw new RuntimeException(
		    "ZipEntry.getCompressedSize() for directory was " + tmp +
		    " it should have been 0");
	    }
	}

	if (verbose) {

	    // Find compressed size of entry after it has been added

	    long unsize = ze.getSize();
	    long csize = ze.getCompressedSize();

	    // FIXME : if unsize >= 16M then divide unsize and csize by 256 ???
	    // if (unsize  > 0xffffffL) 
	    // unsize += 0x80;  unsize >>= 8;
	    // csize += 0x80; csize >>= 8;

	    if (debug) {
		System.out.println("uncompressed size was " + unsize + " " +
				   "compressed size was " + csize);
	    }

	    vout.print("(out=" + csize + ") ");

	    int percent;

	    if (unsize == csize) {
		percent = 0;
	    } else if (unsize > csize) {
		// it got smaller
		percent = Math.round( ((float) (csize) /
				       (float) unsize) * 100.0F );

		// if the compressed size if really small call it 99%
		if (percent == 0)
		    percent = 99;
	    } else {
		// it got bigger
		percent = Math.round( ((float) (csize - unsize) /
				       (float) unsize) * -100.0F );
	    }

	    if (compression && !entryfile_isDirectory) {
		vout.print("(deflated " + percent + "%)");
	    } else {
		vout.print("(stored " + percent + "%)");
	    }

	    vout.println();
	}
    }

    // Recursively add directory entries to an archive. This includes
    // both directories and the files located in those directories.

    void addEntryDir(ZipOutputStream zos, String entrydirname, XPFile entryfile)
	throws IOException
    {
	if (! entryfile.isDirectory()) {
	    // Something went horribly wrong
	    throw new RuntimeException(entryfile.getPath() +
	        " is not a file and it is not a directory");
	}

	if (debug) {
	    System.out.println("finding files in the directory \"" +
			       entryfile.getPath() + "\"");

	    System.out.println("entrydirname is \"" + entrydirname + "\"");
	}

	// add the directory entry to the zip file
	addEntry(zos, entrydirname, entryfile);

	// list all the files in the directory
	String[] files = entryfile.list();

	for (int i=0; i < files.length ; i++) {
	    String fname = files[i];
	    XPFile absfile = new XPFile(entryfile,fname);

	    String relname = new XPFile(entrydirname,fname).getPath();

	    if (debug) {
		System.out.println("files[" + i + "] is \"" +
				   fname + "\"");

		System.out.println(relname + " is a " +
				   (absfile.isFile() ? "File" : "Directory"));

		System.out.println("relname is \"" + relname + "\"");
	    }

	    if (absfile.isFile()) {
		addEntry(zos, relname, absfile);
	    } else {
		addEntryDir(zos, relname, absfile);
	    }
	}
    }


    // This method is used to transfer the contents of input stream to
    // and output stream. The input stream will be read until EOF is
    // returned by the read() method.

    void readwriteStreams(InputStream in, OutputStream out)
	throws IOException
    {
	final boolean debug = false;

	int numRead;

	if (debug) {
	    System.out.println("read/write buffer size is " +
			       buffer.length + " bytes");
	}

	while ((numRead = in.read(buffer,0,buffer.length)) != -1) {
	    if (debug) {
		System.out.println("read " + numRead +
				   " bytes, writing ...");
	    }

	    out.write(buffer,0,numRead);
	}	
    }



    // This method will make sure that all parent directories
    // for a given file exist. If the parent directories do not
    // exist then they will be created.

    void ensureParentsExist(String name) {

	//final boolean debug = true;

	if (debug) {	      
	    System.out.println("making sure parent directory for \"" +
			       name + "\" exists");
	}

	File file = new XPFile(name);
	String parentString = file.getParent();

	if (parentString != null) {
	    File parent =  new File(parentString);
	    parent.mkdirs();
	}

    }



    // Dump the internal contents of the class to stdout

    void dump() {
	System.out.println("Called dump()");
	for (int i=0; i < argv.length; i++) {
	    System.out.println("argv[" + i + "] is \"" + argv[i] + "\"");
	}

	System.out.println("manifest is \"" + manifest + "\"");
	System.out.println("create_manifest is \"" + create_manifest + "\"");
	System.out.println("archive is \"" + archive + "\"");	
	System.out.println("compression is \"" + compression + "\"");
	System.out.println("verbose is \"" + verbose + "\"");
	System.out.println("mode is \"" + mode + "\"");


	if (files == null) {
	    System.out.println("no files");
	} else {
	    System.out.println(files.length + " files to process");
	    for (int i=0; i < files.length; i++) {
		System.out.println("files[" + i + "] is \"" +
				   files[i] + "\"");
	    }
	}


	if (dir_changes == null) {
	    System.out.println("no dir changes");
	} else {
	    for (int i=0; i < files.length; i++) {
		System.out.println("dir_changes[" + i + "] is \"" +
				   dir_changes[i] + "\"");
	    }
	}


	if (absolute_files == null) {
	    System.out.println("no absolute file names");
	} else {
	    for (int i=0; i < absolute_files.length; i++) {
		System.out.println("absolute_files[" + i + "] is \"" +
				   absolute_files[i] + "\"");
	    }
	}


	/*
	System.out.println(" is \"" + + "\"");
	System.out.println(" is \"" + + "\"");
	System.out.println(" is \"" + + "\"");
	*/

    }





    // command line arguments

    private String[] argv;

    // The file name of a manifest file

    private String manifest;

    // If no manifest should be added to an archive this is false

    private boolean create_manifest = true;


    // The file name of the archive, null if none was given with -f

    private String archive;

    // Files to be added, updated, or extracted from the archive
    // If no files were given these will be null

    private String[] files;
    private String[] absolute_files;

    // non null directory change name, cd is done at i'th index in files

    private String[] dir_changes;


    // True if we are in verbose output mode

    private boolean verbose = false;


    // True if created archive should be compressed

    private boolean compression = true;


    // One of 't' (list), 'c' (create), 'x' (extract), or 'u' (update)
    private char mode = (char) 0;


    // Non zero if System.exit() is allowed

    private int exitCode = 0;

    // Size of read write buffers we will use

    private static final int BUFFER_SIZE = 128 * 1024;

    // The actual read write buffer

    private final byte[] buffer = new byte[BUFFER_SIZE];


    // set to true to see extra debug output

    private final static boolean debug = false;


    // stream where verbose output will be going

    private PrintStream vout = System.out;

    // A map to hold all file names and their implementation.
    private HashMap unicityMap;

}
