
import java.util.Date;
import java.util.SimpleTimeZone;

import java.text.ParseException;
import java.text.SimpleDateFormat;

public class DateFormatTest
{
    public static void main(String args[])
	throws Exception
    {
	SimpleDateFormat sdf;
	String str;
	Date date;

	System.out.println("Same time zone");
	sdf = new SimpleDateFormat("yyMMddHHmmssSSS");
	date = new Date(1098968427000L); // 04-10-28 14:00:27 GMT+00:00
	str = sdf.format(date);

	try {
		if( !date.equals(sdf.parse(str)) )
		{
		    System.out.println("Dates don't match? "
				       + date
				       + " != "
				       + sdf.parse(str));
		}
        }
        catch (ParseException e) {
                System.err.println("Parse error at position " + e.getErrorOffset());
                e.printStackTrace();
        }

	SimpleTimeZone tz = new SimpleTimeZone(0, "Z");
	SimpleDateFormat sdf2;

	System.out.println("Different time zone");
	sdf.setTimeZone(tz);
	str = sdf.format(date);
	str += "GMT+00:00";
	sdf2 = new SimpleDateFormat("yyMMddHHmmssSSSz");

	try {
		if( !date.equals(sdf2.parse(str)) )
		{
		    System.out.println("Dates don't match? "
				       + date
				       + " != "
				       + sdf2.parse(str));
		}
	}
	catch (ParseException e) {
		System.err.println("Parse error at position " + e.getErrorOffset());
		e.printStackTrace();
	}
    }
}

/* Expected Output:
Same time zone
Different time zone
*/
