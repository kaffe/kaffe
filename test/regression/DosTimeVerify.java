// Submitted by Moses DeJong <dejong@cs.umn.edu>
import java.io.*;
import java.util.*;
import java.util.zip.*;

public class DosTimeVerify {

    public static void main(String[] argv) throws Exception {

	TimeZone.setDefault(TimeZone.getTimeZone("GMT"));

	ByteArrayOutputStream baos = new ByteArrayOutputStream();

	ZipOutputStream zout = new ZipOutputStream(baos);

	boolean compressed = false;

	byte[] bytes = {1, 2};

	addEntry("data", bytes, zout, compressed);

	zout.close();

	// Get the bytes written to the stream as an array
	byte[] data = baos.toByteArray();

	// Get the dosTime from offset 10

	byte[] timedate = new byte[4];
	timedate[0] = data[10];
	timedate[1] = data[11];
	timedate[2] = data[12];
	timedate[3] = data[13];

	// u4 time / date encoded in dosTime
	// 1/1/1984 12:30 and 30 seconds ( this is 0x66ddd29670L converted to dos time)
	byte[] expected_timedate = {-49, 99, 33, 8};

	boolean ok = true;

	for (int i=0; i < timedate.length; i++) {
	    if (timedate[i] != expected_timedate[i]) {
		System.out.println("timedate[" + i + "] is " +
				   timedate[i] + " expected " +
				   expected_timedate[i]);

		ok = false;
	    }
	}

	if (ok) {
	    System.out.println("OK");
	} else {

	    int int_timedate = get32(timedate, 0);
	    int int_expected_timedate = get32(expected_timedate, 0);

	    // Double cleck our assumed value

	    if (0x82163cf != int_expected_timedate) {
		System.out.println("int_expected_timedate is 0x" +
				   Integer.toHexString(int_expected_timedate) +
				   " not the expected result of 0x82163cf");
	    }

	    if (int_timedate != int_expected_timedate) {
		System.out.println("int_timedate is 0x" + Integer.toHexString(int_timedate));
		System.out.println("expected     is 0x" + Integer.toHexString(int_expected_timedate));

		System.out.println("---BINARY-SPLIT-INTO-HEX-DIGITS----------------------");

		String binary;

		System.out.print("int_timedate  ");
	    
		binary = Integer.toBinaryString(int_timedate);

		for (int i=0; i < binary.length() ; i+=4) {
		    System.out.print(binary.substring(i, i+4));
		    System.out.print(' ');
		}
		System.out.println();
		
		System.out.print("expected      ");

		binary = Integer.toBinaryString(int_expected_timedate);

		for (int i=0; i < binary.length() ; i+=4) {
		    System.out.print(binary.substring(i, i+4));
		    System.out.print(' ');
		}
		System.out.println();
	    } else {
		System.out.println("int_timedate == int_expected_timedate");
	    }

	}
    }

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

	// 1/1/1984 12:30 and 30 seconds
	ze.setTime( 0x66ddd29670L );

	zout.putNextEntry(ze);

	zout.write(bytes);
	
	CRC32 crc = new CRC32();
	crc.update(bytes);	
	ze.setCrc( crc.getValue() );

	zout.closeEntry();
    }


    public static int get32(byte[] buf, int base) {
	int val = (int)buf[base] & 0xFF;
	val |= ((int)buf[base+1] & 0xFF) << 8;
	val |= ((int)buf[base+2] & 0xFF) << 16;
	val |= ((int)buf[base+3] & 0xFF) << 24;
	return (val);
    }

}

/* Expected Output:
OK
*/
