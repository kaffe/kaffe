// This class will write a zip file into memory and verify that the
// contents being written matches the expected output.
// Submitted by Moses DeJong <dejong@cs.umn.edu>

import java.io.*;
import java.util.zip.*;

public class ZipVerify {
    public static boolean debug = true;


    public static void main(String[] argv) throws Exception
    {
	/*
	boolean compressed = false;

	if (argv.length > 0) {
	    compressed = true;
	}

	addEntry(compressed);
	*/


	addEntry(false);
	addEntry(true);
    }


    public static void error(String err) throws Exception
    {
	// throw new RuntimeError(err);
	System.err.println(err);
    }

    public static void addEntry(boolean compressed) throws Exception
    {
	// Create the in-memory output stream the zip will be written to

	ByteArrayOutputStream baos = new ByteArrayOutputStream();

	if (debug) {
	    System.out.println("writing " + (compressed ? "compressed" : "uncompressed") + " zip file");
	}

	// Write a uncompressed zip stream to it

	writeZipFile(baos, compressed);

	// Get the bytes written to the stream as an array

	byte[] data = baos.toByteArray();

	// Display all the bytes that were written to the zip file!

	if (false && debug) {
	    for (int i=0; i < data.length; i++) {
		System.out.println(i + "\t" + data[i] +
				   "\t (char) " + ((char) data[i]));
	    }
	}

	// Display the total number of bytes written

	if (debug) {
	    System.out.println("zip bytes written = " + data.length);
	}


	// Uncompressed Size = 120 bytes
	// Compressed Size   = 138 bytes

	int zipfile_length = 120;

	if (compressed) {
	    zipfile_length = 138;
	}

	if (data.length != zipfile_length) {
	    error("length of zip file should be " + zipfile_length + " bytes");
	}


	// The name we passed to ZipEntry(String)
	byte[] expected_file_name = {(byte)'d', (byte)'a', (byte)'t', (byte)'a'};


	// "extra" data segment that can be added to a ZipEntry
	byte[] expected_extra_field = {(byte)'h', (byte)'i'};


	// 1/1/1984 12:30 and 30 seconds ( this is 0x66ddd29670L converted to dostime)
	byte[] expected_dostime = {-49, 99, 33, 8};

	// Try this date later, kaffe seems to have some problems with it
	// 10 Mon Feb 07 22:17:38 GMT 2000

	byte[] compressed_file_data = {99, 96, 100, 98, 102, 97, 101, 99, -25, -32, 4, 0};
	byte[] uncompressed_file_data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};




	// file offset used to keep track of each field position in the stream
	int zip_offset = 0;

	// u4 LOC signature {80, 75, 3, 4}

	byte[] expected_loc_signature = {80, 75, 3, 4};

	zip_offset += expect("loc_signature", data, zip_offset, expected_loc_signature);


	// u2 version_needed_to_extract (10 or 20)

	zip_offset += expect("extract_version", data, zip_offset,
			     (compressed ? 20 : 10));


	// u2 zip entry flags (seems to be set to 0 or 8)
	// what is this for ??? is it like the compression method ???
	
	zip_offset += expect("flags", data, zip_offset,
			     (compressed ? 8 : 0));
	

	// u2 compression_method (0 or 8)
 
	zip_offset += expect("compression_method", data, zip_offset,
			     (compressed ? 8 : 0));

	// u4 dostime

	zip_offset += expect("dostime", data, zip_offset, expected_dostime);


	// u4 CRC32 checksum

	zip_offset += expect("crc32", data, zip_offset, 0L);
	

	// u4 compressed_size

	zip_offset += expect("compressed_size", data, zip_offset,
			     (compressed ? 0L : 10L));

	// u4 uncompressed_size

	zip_offset += expect("uncompressed_size", data, zip_offset,
			     (compressed ? 0L : 10L));


	// u2 filename_length

	zip_offset += expect("filename_length", data, zip_offset,
			     expected_file_name.length);

	// u2 extra_field_length

	zip_offset += expect("extra_field_length", data, zip_offset,
			     expected_extra_field.length);

	// Double check our offset before we start reading variable length data
	if (zip_offset != 30) {
	    error("zip_offset is " + zip_offset + " expected " + 30);
	}

	// The entry name, we called ZipEntry("data")
	// expected_file_name should be defined above

	zip_offset += expectC("file_name", data, zip_offset, expected_file_name);

	// Check for "extra" data segment, it can be zero so
	// there may not be any data here!

	if (expected_extra_field.length != 0) {
	    zip_offset += expectC("extra_field", data, zip_offset, expected_extra_field);
	}

	// Now the zip_offset should be set to the postion
	// in the file where the data actually begins.
	// the question is, how would we know how much data
	// we could read?

	zip_offset += expect("file_data", data, zip_offset,
			     (compressed ? compressed_file_data :
			      uncompressed_file_data) );


	// Check for the next header, the header that
	// shows up will differ if we used compression

	byte[] compressed_post_data_header   = {80, 75, 7, 8}; // aka DATA
	byte[] uncompressed_post_data_header = {80, 75, 1, 2}; // aka CEN

	zip_offset += expect("post_data_header", data, zip_offset,
			     (compressed ? compressed_post_data_header :
			      uncompressed_post_data_header) );


	if (compressed) {
	    // Read the rest of the DATA header

	    // u4 CRC32 checksum

	    zip_offset += expect("crc32", data, zip_offset,  1164760902L);

	    // u4 compressed_size

	    zip_offset += expect("compressed_size", data, zip_offset,  12L);

	    // u4 uncompressed_size

	    zip_offset += expect("uncompressed_size", data, zip_offset,  10L);

	    // Make sure the CEN header signature shows up next

	    zip_offset += expect("cen_header", data, zip_offset,
				 uncompressed_post_data_header);
	}


	// Finish reading the rest of the CEN header


	// u2 version_used_to_write (10 or 20)

	zip_offset += expect("write_version", data, zip_offset,
				 (compressed ? 20 : 10));
	

	// u2 version_needed_to_extract (10 or 20)

	zip_offset += expect("extract_version", data, zip_offset,
				 (compressed ? 20 : 10));
	

	// u2 zip entry flags (seems to be set to 0 or 8)
	// what is this for ??? is it like the compression method ???

	zip_offset += expect("flags", data, zip_offset,
			     (compressed ? 8 : 0));

	// u2 compression_method (0 or 8)

	zip_offset += expect("compression_method", data, zip_offset,
			     (compressed ? 8 : 0));

	// u4 dostime

	zip_offset += expect("dostime", data, zip_offset, expected_dostime);


	// u4 CRC32 checksum

	zip_offset += expect("crc32", data, zip_offset, 1164760902L);

	// u4 compressed_size

	zip_offset += expect("compressed_size", data, zip_offset,
			     (compressed ? 12L : 10L));

	// u4 uncompressed_size

	zip_offset += expect("uncompressed_size", data, zip_offset, 10L);

	// u2 filename_length

	zip_offset += expect("filename_length", data, zip_offset,
			     expected_file_name.length);

	// u2 extra_field_length

	zip_offset += expect("extra_field_length", data, zip_offset,
			     expected_extra_field.length);
	
	// u2 comment_length

	zip_offset += expect("comment_length", data, zip_offset, 0);

	// The next 12 bytes are used for disknum, file attrs, and offsets,
	// which we do not care about. They should all be zero anyway

	byte[] zero = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
       
	zip_offset += expect("zero", data, zip_offset, zero);

	// filename

	zip_offset += expectC("filename", data, zip_offset,
			     expected_file_name);

	// Check for "extra" data segment, it can be zero so
	// there may not be any data here!

	if (expected_extra_field.length != 0) {
	    zip_offset += expectC("extra_field", data, zip_offset, expected_extra_field);
	}


	// Now we should be at the END header

	byte[] expected_end_signature   = {80, 75, 5, 6};

	zip_offset += expect("end_signature", data, zip_offset, expected_end_signature);

	// u2 disknumber

	zip_offset += expect("disknumber", data, zip_offset, 0);


	// u2 central_disknumber

	zip_offset += expect("central_disknumber", data, zip_offset, 0);

	// u2 total_disknumber

	zip_offset += expect("total_disknumber", data, zip_offset, 1);


	// u2 total_central

	zip_offset += expect("total_central", data, zip_offset, 1);	

	// u4 cen_size

	zip_offset += expect("cen_size", data, zip_offset, 52L);

	// u2 cen_offset (index into file where CEN header begins)

	zip_offset += expect("cen_offset", data, zip_offset,
			     (compressed ? 64L : 46L));

	// u2 comment_length

	zip_offset += expect("comment_length", data, zip_offset, 0);

	


	// Double check that we examined all the bytes in the zip file

	System.out.println("verified " + zip_offset + " bytes");
	if (zip_offset != zipfile_length) {
	    error("zip_offset not at EOF");
	} else {
	    System.out.println("zip_offset is at EOF");
	}

    }




    // Compare the bytes in data at the given offset to the expected array
    // if there is a mismatch, print those bytes that do not match
    
    static int expect(String field, byte[] data, int zip_offset, byte[] expected)
    throws Exception
    {
	for (int i=0; i < expected.length; i++) {
	    if (expected[i] != data[i + zip_offset]) {
		error(field + "[" + i + "] is " +
		       data[i + zip_offset] + " expected " +
		       expected[i] + " at field offset " + zip_offset);
	    }
	}

	return expected.length;
    }

    // Same as expect() above except mismatches are printed as char types

    static int expectC(String field, byte[] data, int zip_offset, byte[] expected)
    throws Exception
    {
	for (int i=0; i < expected.length; i++) {
	    if (expected[i] != data[i + zip_offset]) {
		error(field + "[" + i + "] is '" +
		       (char) data[i + zip_offset] + "' expected '" +
		       (char) expected[i] + "' at field offset " + zip_offset);
	    }
	}

	return expected.length;
    }

    // Compare two 16 bit numbers and display and error if they do not match

    static int expect(String field, byte[] data, int zip_offset, int expected)
    throws Exception
    {
	int num = get16(data, zip_offset);

	if (num != expected) {
	    error(field + " is " + num +
		  " expected " + expected
		  + " at field offset " + zip_offset);
	}

	return 2; // Return the number of bytes to advance in the stream
    }

    // Compare two 32 bit numbers and display and error if they do not match

    static int expect(String field, byte[] data, int zip_offset, long expected)
    throws Exception
    {
	long num = get32(data, zip_offset);

	if (num != expected) {
	    error(field + " is " + num +
		  " expected " + expected
		  + " at field offset " + zip_offset);
	}

	return 4; // Return the number of bytes to advance in the stream
    }

   // This method writes a zip stream to the given OutputStream

    public static void writeZipFile(OutputStream os, boolean compressed)
    	throws Exception
    {
	ZipOutputStream zout = new ZipOutputStream(os);

	int total = 10;

	byte[] bytes = new byte[total];

	for (int i=0; i < total; i++) {
	    bytes[i] = (byte) i;
	}

	addEntry("data", bytes, zout, compressed);

	zout.close();
    }

    // This method is used to add an uncompressed entry to the given ZipOutputStream

    public static void addEntry(String name, byte[] bytes, ZipOutputStream zout, boolean compressed)
	throws Exception
    {
	ZipEntry ze = new ZipEntry(name);

	if (compressed) {
	    ze.setMethod(ZipEntry.DEFLATED);
	} else {
	    ze.setMethod(ZipEntry.STORED);
	}
	ze.setSize( bytes.length );
	ze.setCrc( 0 );

	// Set the "extra" field to something
	byte[] extra = { (byte)'h', (byte)'i'};
	ze.setExtra( extra );

	/*
	// Set to Noon on 11/27/75 (my b-day)
	ze.setTime( getTime(1975, 11, 27, 12, 0, 0) );

	// Set to 2:22 on 2/2/2000 (it just looks cool)
	ze.setTime( getTime(2000, 2, 2, 2, 2, 2) );

	// 10 Mon Feb 07 22:17:38 GMT 2000
	ze.setTime( getTime(2000, 2, 7, 22, 17, 38) );
	
	*/
	
	// 1/1/1984 12:30 and 30 seconds
	ze.setTime( 0x66ddd29670L );

	zout.putNextEntry(ze);

	zout.write(bytes);
	
	CRC32 crc = new CRC32();
	crc.update(bytes);	
	ze.setCrc( crc.getValue() );

	zout.closeEntry();

	if (debug) {
	    System.out.println("added entry \"" + name + "\", data size = " + bytes.length);
	}
    }

    /*
      // broken, need to call constructor with times!

    long getTime(int year, int month, int date, int hour, int minute, int second) {
	GregorianCalendar c = new GregorianCalendar();

	c.set(year, month, date, hour, minute, second);

	return c.getTime().getTime();
    }
    */
    
    public static int get16(byte[] buf, int base) {
	int val = (int)buf[base] & 0xFF;
	val |= ((int)buf[base+1] & 0xFF) << 8;
	return (val);
    }

    public static long get32(byte[] buf, int base) {
	long val = (long)buf[base] & 0xFF;
	val |= ((long)buf[base+1] & 0xFF) << 8;
	val |= ((long)buf[base+2] & 0xFF) << 16;
	val |= ((long)buf[base+3] & 0xFF) << 24;
	return (val);
    }

    public static void put16(byte[] array, int pos, int val) {
	array[pos]   = (byte)val;
	array[pos+1] = (byte)(val >>> 8);
    }

    public static void put32(byte[] array, int pos, long val) {
	array[pos]   = (byte) val;
	array[pos+1] = (byte)(val >>> 8);
	array[pos+2] = (byte)(val >>> 16);
	array[pos+3] = (byte)(val >>> 24);
    }







    // Zip file format
    // Byte offset -> Size DESCRIPTION

    // zo is the zip_offset, the integer index into the stream
    // z0 = 0

    // First the LOC header appears

    // zo + 0  -> u4 LOC signature {80, 75, 3, 4}
    // zo + 4  -> u2 version_needed_to_extract (10 or 20)
    // zo + 6  -> u2 zip entry flags
    // zo + 8  -> u2 compression_method (0 or 8)
    // zo + 10 -> u4 dostime
    // zo + 14 -> u4 crc32
    // zo + 18 -> u4 compressed_size
    // zo + 22 -> u4 uncompressed_size
    // zo + 26 -> u2 filename_length
    // zo + 28 -> u2 extra_field_length

    // zo = 30

    // zo - (zo + filename_length) filename
    // zo - (zo + extra_field_length) extra_field

    // then the actual data from the file appears in the zip file

    // zo = (zo + filename_length + extra_field_length + data_length)


    // If the entry was compressed, then the DATA header will appear

    // zo + 0  -> u4 DATA signature {80, 75, 7, 8}
    // zo + 4  -> u4 data_crc
    // zo + 8  -> u4 data_compressedsize
    // zo + 12 -> u4 data_uncompressedsize


    // Now the CEN header will appear

    // zo + 0  -> u4 CEN signature {80, 75, 1, 2}
    // zo + 4  -> u2 version used to write zipfile (10 or 20)
    // zo + 6  -> u2 version needed to extract (10 or 20)
    // zo + 8  -> u2 flags
    // zo + 10 -> u2 compression_method
    // zo + 12 -> u4 dostime
    // zo + 16 -> u4 crc
    // zo + 20 -> u4 compressed_size
    // zo + 24 -> u4 uncompressed_size
    // zo + 28 -> u2 filename_length
    // zo + 30 -> u2 extra_field_length
    // zo + 32 -> u2 comment_length
    // zo + 34 -> u2 disknum
    // zo + 36 -> u2 internal_file_attributes
    // zo + 38 -> u4 external_file_attributes
    // zo + 42 -> u4 relative_offset_of_local_header

    // zo = zo + 46

    // zo - (zo + filename_length) filename
    // zo - (zo + extra_field_length) extra_field

    // Now the END header will appear

    // zo + 0  -> u4 END signature {80, 75, 5, 6}
    // zo + 4  -> u2 disknumber
    // zo + 6  -> u2 central_disknumber
    // zo + 8  -> u2 total_disknumber
    // zo + 10 -> u2 total_central
    // zo + 12 -> u4 cen_size
    // zo + 16 -> u4 cen_offset (index into file where CEN header begins)
    // zo + 20 -> u2 comment_length

}

/* Expected Output:
writing uncompressed zip file
added entry "data", data size = 10
zip bytes written = 120
verified 120 bytes
zip_offset is at EOF
writing compressed zip file
added entry "data", data size = 10
zip bytes written = 138
verified 138 bytes
zip_offset is at EOF
*/
