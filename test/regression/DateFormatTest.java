
import java.util.Date;
import java.util.SimpleTimeZone;

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
	date = new Date();
	str = sdf.format(date);
	if( !date.equals(sdf.parse(str)) )
	{
	    System.out.println("Dates don't match? "
			       + date
			       + " != "
			       + sdf.parse(str));
	}

	SimpleTimeZone tz = new SimpleTimeZone(0, "Z");
	SimpleDateFormat sdf2;

	System.out.println("Different time zone");
	sdf.setTimeZone(tz);
	str = sdf.format(date);
	str += "GMT+00:00";
	sdf2 = new SimpleDateFormat("yyMMddHHmmssSSSz");
	if( !date.equals(sdf2.parse(str)) )
	{
	    System.out.println("Dates don't match? "
			       + date
			       + " != "
			       + sdf2.parse(str));
	}
    }
}

/* Expected Output:
Same time zone
Different time zone
*/
