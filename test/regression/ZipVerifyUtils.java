// Utils used by the ZipVerify class

import java.io.*;
import java.util.zip.*;
import java.util.*;

public class ZipVerifyUtils {
    public static boolean debug = true;
    public static boolean debug_stream = true;

    // A Util class used to pass entry data to generateAndVerifyZipData()
    public static class EntryRecord {
	public EntryRecord(byte[] expected_file_name,
			   byte[] uncompressed_file_data,
			   byte[] expected_extra_field,
			   long expected_unix_time) {
	    this.expected_file_name = expected_file_name;
	    this.uncompressed_file_data = uncompressed_file_data;
	    this.expected_extra_field = expected_extra_field;
	    this.expected_unix_time = expected_unix_time;
	}

	byte[] expected_file_name;
	byte[] uncompressed_file_data;
	byte[] expected_extra_field;
	long expected_unix_time;
    }

    public static long getTime(int year, int month, int date, int hour,
			       int minute, int second)
    {
	GregorianCalendar c;

	// This is broken, need to call constructor with times!
	//c = new GregorianCalendar();
	//c.set(year, month, date, hour, minute, second);

	c = new GregorianCalendar(year, month, date, hour, minute, second);

	return c.getTime().getTime();
    }
    
    public static int get16(byte[] buf, int base) {
	int val = (int)buf[base] & 0xFF;
	val |= ((int)buf[base+1] & 0xFF) << 8;
	return (val);
    }

    public static int get32(byte[] buf, int base) {
	int val = (int)buf[base] & 0xFF;
	val |= ((int)buf[base+1] & 0xFF) << 8;
	val |= ((int)buf[base+2] & 0xFF) << 16;
	val |= ((int)buf[base+3] & 0xFF) << 24;
	return (val);
    }

    public static void put16(byte[] array, int pos, int val) {
	array[pos]   = (byte)val;
	array[pos+1] = (byte)(val >>> 8);
    }

    public static void put32(byte[] array, int pos, int val) {
	array[pos]   = (byte) val;
	array[pos+1] = (byte)(val >>> 8);
	array[pos+2] = (byte)(val >>> 16);
	array[pos+3] = (byte)(val >>> 24);
    }

    // This is a quick little helper method to convert a String
    // into an array of bytes

    public static byte[] toBytes(String value) {
	final int len = value.length();
	byte[] bytes = new byte[len];

	for (int i=0; i < len; i++) {
	    bytes[i] = (byte) value.charAt(i);
	}
	return bytes;
    }

    // This method will do something to signal an error
    // but it should not actually stop execution

    public static void error(String err) throws Exception
    {
	// throw new RuntimeError(err);
	System.err.println(err);
    }

    // Just print the given message without an "error"

    public static void output(String msg) throws Exception
    {
	System.out.println(msg);
    }


    // Compare the bytes in data at the given offset to the expected array
    // if there is a mismatch, print those bytes that do not match
    
    static int expect(String field, byte[] data, int zip_offset, byte[] expected)
    throws Exception
    {
	if (expected == null)
	    return 0;

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
	if (expected == null)
	    return 0;

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

    static int expectL(String field, byte[] data, int zip_offset, long expected)
    throws Exception
    {
	long num = (long) get32(data, zip_offset);

	if (num != expected) {
	    error(field + " is " + num +
		  " expected " + expected
		  + " at field offset " + zip_offset);
	}

	return 4; // Return the number of bytes to advance in the stream
    }

    // This method is used to add an uncompressed entry to the given ZipOutputStream

    public static void addEntry(ZipOutputStream zout, boolean compressed,
				byte[] name, byte[] bytes, byte[] extra, long unix_time)
	throws Exception
    {
	// Convert byte[] to String
	StringBuffer name_buf = new StringBuffer();
	for (int i=0; i < name.length; i++) {
	    name_buf.append( (char) name[i] );
	}
	String name_str = name_buf.toString();
 
	ZipEntry ze = new ZipEntry(name_str);

	int bytes_length = (bytes == null ? 0 : bytes.length);

	if (compressed) {
	    ze.setMethod(ZipEntry.DEFLATED);
	} else {
	    ze.setMethod(ZipEntry.STORED);
	    ze.setSize(bytes_length);
	    ze.setCrc(0);
	}

	ze.setExtra(extra);

	ze.setTime(unix_time);

	zout.putNextEntry(ze);

	if (bytes_length > 0) {

	    zout.write(bytes);

	    if (! compressed) {
		CRC32 crc = new CRC32();
		crc.update(bytes);	
		ze.setCrc( crc.getValue() );
	    }

	}

	zout.closeEntry();

	if (debug) {
	    output("added entry \"" + name_str
			       + "\", data size = " + bytes_length);
	}
    }

    // This method will create a byte[] containing data from a zip stream
    // and do some simple checks on the size of the data generated

    public static byte[] generateAndVerifyZipData(boolean compressed,
						  EntryRecord[] entries,
						  int uncompressed_zipfile_length,
						  int compressed_zipfile_length)
	throws Exception
    {
	// Create the in-memory output stream the zip will be written to

	ByteArrayOutputStream baos = new ByteArrayOutputStream();

	// Write a zip stream containing the data we want to add

	ZipOutputStream zout = new ZipOutputStream(baos);

	for (int i=0; i < entries.length; i++) {
	    EntryRecord er = entries[i];
	    addEntry(zout, compressed, er.expected_file_name,
		     er.uncompressed_file_data, er.expected_extra_field,
		     er.expected_unix_time);
	}

	zout.close();

	// Get the bytes written to the stream as an array

	byte[] data = baos.toByteArray();

	// Display all the bytes that were written to the zip file!

	if (debug_stream) {
	    for (int i=0; i < data.length; i++) {
		output(i + "\t" + data[i] +
				   "\t (char) " + ((char) data[i]));
	    }
	}

	if (debug) {
	    output((compressed ? "compressed" : "uncompressed") + " zip file written");
	}

	// Display the total number of bytes written

	if (debug) {
	    output("zip bytes written = " + data.length);
	}

	int zipfile_length = uncompressed_zipfile_length;

	if (compressed) {
	    zipfile_length = compressed_zipfile_length;
	}

	if (data.length != zipfile_length){
	    error("length of zip file should be " + zipfile_length + " bytes it was " +
		  data.length + " bytes");
	}

	return data;
    }


    // This method will read a LOC header and verify that
    // the fields in the header match the expected results.
    // This method returns the new zip_offset.

    public static int verifyLOCHeader(boolean compressed,
				      byte[] expected_file_name,
				      byte[] uncompressed_file_data,
				      byte[] compressed_file_data,
				      byte[] expected_extra_field,
				      byte[] expected_dostime,
				      byte[] data,
				      int zip_offset)
	throws Exception
    {
	// u4 LOC signature {80, 75, 3, 4}

	byte[] expected_loc_signature = {80, 75, 3, 4};

	int old_zip_offset = zip_offset;

	int uncompressed_file_data_length = (uncompressed_file_data == null ? 0 :
					     uncompressed_file_data.length);

	int compressed_file_data_length = (compressed_file_data == null ? 0 :
					     compressed_file_data.length);

	int expected_extra_field_length = (expected_extra_field == null ? 0
					   : expected_extra_field.length);

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

	zip_offset += expectL("crc32", data, zip_offset, 0L);


	// u4 compressed_size

	zip_offset += expectL("compressed_size", data, zip_offset,
			     (compressed ? 0 : uncompressed_file_data_length));

	// u4 uncompressed_size

	zip_offset += expectL("uncompressed_size", data, zip_offset,
			     (compressed ? 0 : uncompressed_file_data_length));


	// u2 filename_length

	zip_offset += expect("filename_length", data, zip_offset,
			     expected_file_name.length);

	// u2 extra_field_length

	zip_offset += expect("extra_field_length", data, zip_offset,
			     expected_extra_field_length);

	if (debug) {
	    output("after reading LOC Header, zip_offset is " + zip_offset);
	}

	if ((zip_offset - old_zip_offset) != 30) {
	    error("LOC header size should be 30, it was " + (zip_offset - old_zip_offset));
	}

	return (zip_offset - old_zip_offset);
    }

    // verify the name, extra, and data fields that show up after
    // a LOC header. This method returns the new zip_offset.

    public static int verifyNameAndExtra(byte[] expected_file_name,
					 byte[] expected_extra_field,
					 byte[] data,
					 int zip_offset)
	throws Exception
    {
	int old_zip_offset = zip_offset;

	// The entry name, we called ZipEntry() with it

	zip_offset += expectC("file_name", data, zip_offset, expected_file_name);

	// Check for "extra" data segment, it can be zero so
	// there may not be any data here!

	zip_offset += expectC("extra_field", data, zip_offset, expected_extra_field);

	if (debug) {
	    output("after reading Name-Extra, zip_offset is " + zip_offset);
	}

	return (zip_offset - old_zip_offset);
    }

    // verify the actual file data from the entry

    public static int verifyFileData(boolean compressed,
				     byte[] uncompressed_file_data,
				     byte[] compressed_file_data,
				     byte[] data,
				     int zip_offset)
	throws Exception
    {
	int old_zip_offset = zip_offset;

	zip_offset += expect("file_data", data, zip_offset,
			     (compressed ? compressed_file_data :
			      uncompressed_file_data) );

	if (debug) {
	    output("after reading File-Data, zip_offset is " + zip_offset);
	}

	return (zip_offset - old_zip_offset);
    }

    // This method will read a DATA header and verify that
    // the fields in the header match the expected results.
    // This method returns the new zip_offset.

    public static int verifyDATAHeader(byte[] uncompressed_file_data,
				       byte[] compressed_file_data,
				       long expected_checksum,
				       byte[] data,
				       int zip_offset)
	throws Exception
    {
	// A DATA header should show up after a compressed entry

	byte[] compressed_post_data_header   = {80, 75, 7, 8}; // aka DATA

	int old_zip_offset = zip_offset;

	int uncompressed_file_data_length = (uncompressed_file_data == null ? 0 :
					     uncompressed_file_data.length);

	int compressed_file_data_length = (compressed_file_data == null ? 0 :
					     compressed_file_data.length);

	zip_offset += expect("data_header", data, zip_offset, compressed_post_data_header);

	// u4 CRC32 checksum

	zip_offset += expectL("crc32", data, zip_offset, expected_checksum);

	// u4 compressed_size

	zip_offset += expectL("compressed_size", data, zip_offset,
			      compressed_file_data_length);

	// u4 uncompressed_size

	zip_offset += expectL("uncompressed_size", data, zip_offset,
			      uncompressed_file_data_length);

	if (debug) {
	    output("after reading DATA header, zip_offset is " + zip_offset);
	}

	if ((zip_offset - old_zip_offset) != 16) {
	    error("DATA header size should be 16, it was " + (zip_offset - old_zip_offset));
	}

	return (zip_offset - old_zip_offset);
    }

    // This method will read a CEN header and verify that
    // the fields in the header match the expected results.
    // This method returns the new zip_offset.

    public static int verifyCENHeader(boolean compressed,
				      byte[] expected_file_name,
				      byte[] uncompressed_file_data,
				      byte[] compressed_file_data,
				      byte[] expected_extra_field,
				      byte[] expected_dostime,
				      long expected_checksum,
				      long uncompressed_rel_header_offset,
				      long compressed_rel_header_offset,
				      byte[] data,
				      int zip_offset)
	throws Exception
    {
	// A CEN header should show up after an uncompressed entry or
	// after the DATA header that follows a compressed entry

	byte[] uncompressed_post_data_header = {80, 75, 1, 2}; // aka CEN

	int old_zip_offset = zip_offset;

	int uncompressed_file_data_length = (uncompressed_file_data == null ? 0 :
					     uncompressed_file_data.length);

	int compressed_file_data_length = (compressed_file_data == null ? 0 :
					     compressed_file_data.length);

	int expected_extra_field_length = (expected_extra_field == null ? 0
					   : expected_extra_field.length);

	zip_offset += expect("cen_header", data, zip_offset, uncompressed_post_data_header);

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

	zip_offset += expectL("crc32", data, zip_offset, expected_checksum);

	// u4 compressed_size

	zip_offset += expectL("compressed_size", data, zip_offset,
			     (compressed ? compressed_file_data_length :
			      uncompressed_file_data_length));

	// u4 uncompressed_size

	zip_offset += expectL("uncompressed_size", data, zip_offset,
			      uncompressed_file_data_length);

	// u2 filename_length

	zip_offset += expect("filename_length", data, zip_offset,
			     expected_file_name.length);

	// u2 extra_field_length

	zip_offset += expect("extra_field_length", data, zip_offset,
			     expected_extra_field_length);
	
	// u2 comment_length

	zip_offset += expect("comment_length", data, zip_offset, 0);

	// u2 disknum

	zip_offset += expect("disknum", data, zip_offset, 0);

	// u2 internal_file_attributes

	zip_offset += expect("internal_file_attributes", data, zip_offset, 0);

	// u4 external_file_attributes

	zip_offset += expectL("external_file_attributes", data, zip_offset, 0);

	// u4 relative_offset_of_local_header

	zip_offset += expectL("relative_offset_of_local_header", data, zip_offset,
			      (compressed ? compressed_rel_header_offset :
			       uncompressed_rel_header_offset));

	if (debug) {
	    output("after reading CEN header, zip_offset is " + zip_offset);
	}

	if ((zip_offset - old_zip_offset) != 46) {
	    error("CEN header size should be 46, it was " + (zip_offset - old_zip_offset));
	}

	return (zip_offset - old_zip_offset);
    }


    // This method will read an END header and verify that
    // the fields in the header match the expected results.
    // This method returns the new zip_offset.

    public static int verifyENDHeader(boolean compressed,
				      int total_disknumber,
				      int total_central,
				      long expected_cen_size,
				      long uncompressed_cen_offset,
				      long compressed_cen_offset,
				      byte[] data,
				      int zip_offset)
	throws Exception
    {
	byte[] expected_end_signature = {80, 75, 5, 6}; // aka END

	int old_zip_offset = zip_offset;

	zip_offset += expect("end_signature", data, zip_offset, expected_end_signature);

	// u2 disknumber

	zip_offset += expect("disknumber", data, zip_offset, 0);

	// u2 central_disknumber

	zip_offset += expect("central_disknumber", data, zip_offset, 0);

	// u2 total_disknumber

	zip_offset += expect("total_disknumber", data, zip_offset, total_disknumber);

	// u2 total_central

	zip_offset += expect("total_central", data, zip_offset, total_central);

	// u4 cen_size

	zip_offset += expectL("cen_size", data, zip_offset, expected_cen_size);

	// u2 cen_offset (index into file where CEN header begins)

	zip_offset += expectL("cen_offset", data, zip_offset,
			     (compressed ? compressed_cen_offset : uncompressed_cen_offset));

	// u2 comment_length

	zip_offset += expect("comment_length", data, zip_offset, 0);

	if (debug) {
	    output("after reading END header, zip_offset is " + zip_offset);
	}

	if ((zip_offset - old_zip_offset) != 22) {
	    error("END header size should be 22, it was " + (zip_offset - old_zip_offset));
	}

	return (zip_offset - old_zip_offset);
    }

    // This method will check that all the data in the stream
    // has been verified by comparing the zip_offset to the
    // length of the data array.

    public static void verifyEOF(byte[] data,
				 int zip_offset)
	throws Exception
    {
	if (debug) {
	    output("verified " + zip_offset + " bytes");
	}

	if (zip_offset != data.length) {
	    error("zip_offset not at EOF");
	} else {
	    if (debug) {
		output("zip_offset is at EOF");
	    }
	}
    }

    // --------------------------------------------------------------------------------

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

    // ----------------------------------------------------------------------

    // If there are two entries in the zip file the headers appear like this

    // LOC (one)
    // ?DATA (one)?
    // LOC (two)
    // ?DATA (two)?
    // CEN (one)
    // CEN (two)
    // END

    // Three entries would look like this

    // LOC (one)
    // ?DATA (one)?
    // LOC (two)
    // ?DATA (two)?
    // LOC (three)
    // ?DATA (three)?
    // CEN (one)
    // CEN (two)
    // CEN (three)
    // END

}
