// This class will write a zip file into memory and verify that the
// contents being written matches the expected output.

import java.io.*;
import java.util.*;
import java.util.zip.*;

public class ZipVerify extends ZipVerifyUtils {

    public static void main(String[] argv) throws Exception
    {
	TimeZone.setDefault(TimeZone.getTimeZone("GMT"));

	// Set debug to false and no "output" messages will be displayed
	// Error messages will still be displayed
	debug = false;

	// This will dump the entire stream unless set to false
	debug_stream = false;

	checkSingleEntry(false);
	checkSingleEntry(true);

	checkSingleEntry2(false);
	checkSingleEntry2(true);

	checkDirectoryEntry();

	checkTwoEntries(false);
	checkTwoEntries(true);

    }
    
    public static void checkSingleEntry(boolean compressed) throws Exception
    {
	// 1/1/1984 12:30 and 30 seconds
	long expected_unix_time = 0x66ddd29670L;

	// 1/1/1984 12:30 and 30 seconds ( this is 0x66ddd29670L converted to dostime)
	byte[] expected_dostime = {-49, 99, 33, 8};

	// The entry name we pass to ZipEntry(String)
	byte[] expected_file_name = toBytes("data");

	// "extra" data segment that is added to a ZipEntry
	byte[] expected_extra_field = toBytes("hi");

	byte[] uncompressed_file_data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	byte[] compressed_file_data = {99, 96, 100, 98, 102, 97, 101, 99, -25, -32, 4, 0};


	// The total size of the zip file
	int uncompressed_zipfile_length = 120;
	int compressed_zipfile_length = 138;

	// The CRC checksum we expect to find
	long expected_checksum = 1164760902L;

	// Zip internal data
	long uncompressed_rel_header_offset = 0;
	long compressed_rel_header_offset = 0;
	int total_disknumber = 1;
	int total_central = 1;
	long expected_cen_size = 52;
	long uncompressed_cen_offset = 46;
	long compressed_cen_offset = 64;

	// ---------------------------------------------

	EntryRecord[] entries = new EntryRecord[1];
	entries[0] = new EntryRecord(expected_file_name,
				     uncompressed_file_data,
				     expected_extra_field,
				     expected_unix_time);

	byte[] data = generateAndVerifyZipData(compressed,
					       entries,
					       uncompressed_zipfile_length,
					       compressed_zipfile_length);


	// file offset used to keep track of each field position in the stream
	int zip_offset = 0;

	zip_offset += verifyLOCHeader(compressed,
				      expected_file_name,
				      uncompressed_file_data,
				      compressed_file_data,
				      expected_extra_field,
				      expected_dostime,
				      data,
				      zip_offset);

	zip_offset += verifyNameAndExtra(expected_file_name,
					 expected_extra_field,
					 data,
					 zip_offset);

	zip_offset += verifyFileData(compressed,
				     uncompressed_file_data,
				     compressed_file_data,
				     data,
				     zip_offset);

	// If we used compression, a DATA header should show up next

	if (compressed) {
	    zip_offset += verifyDATAHeader(uncompressed_file_data,
					   compressed_file_data,
					   expected_checksum,
					   data,
					   zip_offset);
	}

	zip_offset += verifyCENHeader(compressed,
				      expected_file_name,
				      uncompressed_file_data,
				      compressed_file_data,
				      expected_extra_field,
				      expected_dostime,
				      expected_checksum,
				      uncompressed_rel_header_offset,
				      compressed_rel_header_offset,
				      data,
				      zip_offset);


	zip_offset += verifyNameAndExtra(expected_file_name,
					 expected_extra_field,
					 data,
					 zip_offset);

	zip_offset += verifyENDHeader(compressed,
				      total_disknumber,
				      total_central,
				      expected_cen_size,
				      uncompressed_cen_offset,
				      compressed_cen_offset,
				      data,
				      zip_offset);

	verifyEOF(data, zip_offset);
    }

    public static void checkSingleEntry2(boolean compressed) throws Exception
    {
	// 1/1/1984 12:30 and 30 seconds
	long expected_unix_time = 0x66ddd29670L;

	// 1/1/1984 12:30 and 30 seconds ( this is 0x66ddd29670L converted to dostime)
	byte[] expected_dostime = {-49, 99, 33, 8};

	// The entry name we pass to ZipEntry(String)
	byte[] expected_file_name = toBytes("file1");

	// "extra" data segment that is added to a ZipEntry
	byte[] expected_extra_field = null;

	byte[] uncompressed_file_data = {5, 4, 3, 2, 1, 0};
	byte[] compressed_file_data = {99, 101, 97, 102, 98, 100, 0, 0};

	// The total size of the zip file
	int uncompressed_zipfile_length = 114;
	int compressed_zipfile_length = 132;

	// The CRC checksum we expect to find
	long expected_checksum = 480631825L;

	// Zip internal data
	long uncompressed_rel_header_offset = 0;
	long compressed_rel_header_offset = 0;
	int total_disknumber = 1;
	int total_central = 1;
	long expected_cen_size = 51;
	long uncompressed_cen_offset = 41;
	long compressed_cen_offset = 59;

	// ---------------------------------------------


	EntryRecord[] entries = new EntryRecord[1];
	entries[0] = new EntryRecord(expected_file_name,
				     uncompressed_file_data,
				     expected_extra_field,
				     expected_unix_time);

	byte[] data = generateAndVerifyZipData(compressed,
					       entries,
					       uncompressed_zipfile_length,
					       compressed_zipfile_length);


	// file offset used to keep track of each field position in the stream
	int zip_offset = 0;

	zip_offset += verifyLOCHeader(compressed,
				      expected_file_name,
				      uncompressed_file_data,
				      compressed_file_data,
				      expected_extra_field,
				      expected_dostime,
				      data,
				      zip_offset);

	zip_offset += verifyNameAndExtra(expected_file_name,
					 expected_extra_field,
					 data,
					 zip_offset);

	zip_offset += verifyFileData(compressed,
				     uncompressed_file_data,
				     compressed_file_data,
				     data,
				     zip_offset);

	// If we used compression, a DATA header should show up next

	if (compressed) {
	    zip_offset += verifyDATAHeader(uncompressed_file_data,
					   compressed_file_data,
					   expected_checksum,
					   data,
					   zip_offset);
	}

	zip_offset += verifyCENHeader(compressed,
				      expected_file_name,
				      uncompressed_file_data,
				      compressed_file_data,
				      expected_extra_field,
				      expected_dostime,
				      expected_checksum,
				      uncompressed_rel_header_offset,
				      compressed_rel_header_offset,
				      data,
				      zip_offset);


	zip_offset += verifyNameAndExtra(expected_file_name,
					 expected_extra_field,
					 data,
					 zip_offset);

	zip_offset += verifyENDHeader(compressed,
				      total_disknumber,
				      total_central,
				      expected_cen_size,
				      uncompressed_cen_offset,
				      compressed_cen_offset,
				      data,
				      zip_offset);

	verifyEOF(data, zip_offset);
    }

    // The trick about directory entries is that they are never compressed

    public static void checkDirectoryEntry() throws Exception
    {
	// 1/1/1984 12:30 and 30 seconds
	long expected_unix_time = 0x66ddd29670L;

	// 1/1/1984 12:30 and 30 seconds ( this is 0x66ddd29670L converted to dostime)
	byte[] expected_dostime = {-49, 99, 33, 8};

	// The entry name we pass to ZipEntry(String)
	byte[] expected_file_name = toBytes("foo/");

	// "extra" data segment that is added to a ZipEntry
	byte[] expected_extra_field = null;

	byte[] uncompressed_file_data = null;
	byte[] compressed_file_data = null;

	// The total size of the zip file
	int uncompressed_zipfile_length = 106;
	int compressed_zipfile_length = 106;

	// The CRC checksum we expect to find
	long expected_checksum = 0;

	// Zip internal data
	long uncompressed_rel_header_offset = 0;
	long compressed_rel_header_offset = 0;
	int total_disknumber = 1;
	int total_central = 1;
	long expected_cen_size = 50;
	long uncompressed_cen_offset = 34;
	long compressed_cen_offset = 34;

	boolean compressed = false;

	// ---------------------------------------------


	EntryRecord[] entries = new EntryRecord[1];
	entries[0] = new EntryRecord(expected_file_name,
				     uncompressed_file_data,
				     expected_extra_field,
				     expected_unix_time);

	byte[] data = generateAndVerifyZipData(compressed,
					       entries,
					       uncompressed_zipfile_length,
					       compressed_zipfile_length);


	// file offset used to keep track of each field position in the stream
	int zip_offset = 0;

	zip_offset += verifyLOCHeader(compressed,
				      expected_file_name,
				      uncompressed_file_data,
				      compressed_file_data,
				      expected_extra_field,
				      expected_dostime,
				      data,
				      zip_offset);

	zip_offset += verifyNameAndExtra(expected_file_name,
					 expected_extra_field,
					 data,
					 zip_offset);

	// No file_data or DATA header for a directory entry

	zip_offset += verifyCENHeader(compressed,
				      expected_file_name,
				      uncompressed_file_data,
				      compressed_file_data,
				      expected_extra_field,
				      expected_dostime,
				      expected_checksum,
				      uncompressed_rel_header_offset,
				      compressed_rel_header_offset,
				      data,
				      zip_offset);


	zip_offset += verifyNameAndExtra(expected_file_name,
					 expected_extra_field,
					 data,
					 zip_offset);

	zip_offset += verifyENDHeader(compressed,
				      total_disknumber,
				      total_central,
				      expected_cen_size,
				      uncompressed_cen_offset,
				      compressed_cen_offset,
				      data,
				      zip_offset);

	verifyEOF(data, zip_offset);
    }

    // Write two entries to a zip file and verify the output

    public static void checkTwoEntries(boolean compressed) throws Exception
    {
	// 1/1/1984 12:30 and 30 seconds
	long expected_unix_time = 0x66ddd29670L;

	// 1/1/1984 12:30 and 30 seconds ( this is 0x66ddd29670L converted to dostime)
	byte[] expected_dostime = {-49, 99, 33, 8};

	// The entry name we pass to ZipEntry(String)
	byte[] expected_file_name_one = toBytes("data");

	// "extra" data segment that is added to a ZipEntry
	byte[] expected_extra_field_one = toBytes("hi");

	byte[] uncompressed_file_data_one = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	byte[] compressed_file_data_one = {99, 96, 100, 98, 102, 97, 101, 99, -25, -32, 4, 0};

	// The entry name we pass to ZipEntry(String)
	byte[] expected_file_name_two = toBytes("file1");

	// "extra" data segment that is added to a ZipEntry
	byte[] expected_extra_field_two = null;

	byte[] uncompressed_file_data_two = {5, 4, 3, 2, 1, 0};
	byte[] compressed_file_data_two = {99, 101, 97, 102, 98, 100, 0, 0};


	// The total size of the zip file
	int uncompressed_zipfile_length = 212;
	int compressed_zipfile_length = 248;

	// The CRC checksums we expect to find
	long expected_checksum_one = 1164760902L;
	long expected_checksum_two = 480631825L;

	// Zip internal data
	long uncompressed_rel_header_offset_one = 0;
	long compressed_rel_header_offset_one = 0;
	long uncompressed_rel_header_offset_two = 46;
	long compressed_rel_header_offset_two = 64;

	int total_disknumber = 2;
	int total_central = 2;

	long expected_cen_size = 103;
	long uncompressed_cen_offset = 87;
	long compressed_cen_offset = 123;

	// ---------------------------------------------


	EntryRecord[] entries = new EntryRecord[2];
	entries[0] = new EntryRecord(expected_file_name_one,
				     uncompressed_file_data_one,
				     expected_extra_field_one,
				     expected_unix_time);

	entries[1] = new EntryRecord(expected_file_name_two,
				     uncompressed_file_data_two,
				     expected_extra_field_two,
				     expected_unix_time);

	byte[] data = generateAndVerifyZipData(compressed,
					       entries,
					       uncompressed_zipfile_length,
					       compressed_zipfile_length);


	// file offset used to keep track of each field position in the stream
	int zip_offset = 0;

	zip_offset += verifyLOCHeader(compressed,
				      expected_file_name_one,
				      uncompressed_file_data_one,
				      compressed_file_data_one,
				      expected_extra_field_one,
				      expected_dostime,
				      data,
				      zip_offset);

	zip_offset += verifyNameAndExtra(expected_file_name_one,
					 expected_extra_field_one,
					 data,
					 zip_offset);

	zip_offset += verifyFileData(compressed,
				     uncompressed_file_data_one,
				     compressed_file_data_one,
				     data,
				     zip_offset);

	// If we used compression, a DATA header should show up next

	if (compressed) {
	    zip_offset += verifyDATAHeader(uncompressed_file_data_one,
					   compressed_file_data_one,
					   expected_checksum_one,
					   data,
					   zip_offset);
	}

	zip_offset += verifyLOCHeader(compressed,
				      expected_file_name_two,
				      uncompressed_file_data_two,
				      compressed_file_data_two,
				      expected_extra_field_two,
				      expected_dostime,
				      data,
				      zip_offset);

	zip_offset += verifyNameAndExtra(expected_file_name_two,
					 expected_extra_field_two,
					 data,
					 zip_offset);

	zip_offset += verifyFileData(compressed,
				     uncompressed_file_data_two,
				     compressed_file_data_two,
				     data,
				     zip_offset);

	// If we used compression, a DATA header should show up next

	if (compressed) {
	    zip_offset += verifyDATAHeader(uncompressed_file_data_two,
					   compressed_file_data_two,
					   expected_checksum_two,
					   data,
					   zip_offset);
	}

	// CEN for entry one

	zip_offset += verifyCENHeader(compressed,
				      expected_file_name_one,
				      uncompressed_file_data_one,
				      compressed_file_data_one,
				      expected_extra_field_one,
				      expected_dostime,
				      expected_checksum_one,
				      uncompressed_rel_header_offset_one,
				      compressed_rel_header_offset_one,
				      data,
				      zip_offset);


	zip_offset += verifyNameAndExtra(expected_file_name_one,
					 expected_extra_field_one,
					 data,
					 zip_offset);

	// CEN for entry two

	zip_offset += verifyCENHeader(compressed,
				      expected_file_name_two,
				      uncompressed_file_data_two,
				      compressed_file_data_two,
				      expected_extra_field_two,
				      expected_dostime,
				      expected_checksum_two,
				      uncompressed_rel_header_offset_two,
				      compressed_rel_header_offset_two,
				      data,
				      zip_offset);

	zip_offset += verifyNameAndExtra(expected_file_name_two,
					 expected_extra_field_two,
					 data,
					 zip_offset);

	zip_offset += verifyENDHeader(compressed,
				      total_disknumber,
				      total_central,
				      expected_cen_size,
				      uncompressed_cen_offset,
				      compressed_cen_offset,
				      data,
				      zip_offset);

	verifyEOF(data, zip_offset);
    }

}

// Sources: ZipVerifyUtils.java
