/*
 * Java core library component.
 * 
 * Copyright (c) 2003 Ito Kazumitsu <kaz@maczuka.gcd.org>. All rights reserved.
 * 
 * See the file "license.terms" for information on usage and redistribution of
 * this file.
 */
package gnu.classpath.tools.native2ascii;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

public class Native2ASCII
{
  public static void main(String[] args)
  {
    String encoding = System.getProperty("file.encoding");
    String input = null;
    String output = null;
    for (int i = 0; i < args.length; i++)
      {
        if (args[i].equals("-encoding"))
          {
            i++;
            if (i >= args.length)
              {
                System.err.println("encoding is missing");
              }
            else
              {
                encoding = args[i];
              }
          }
        else if (args[i].equals("-reverse") || args[i].startsWith("-J"))
          {
            System.err.println(args[i] + ": not supported");
          }
        else
          {
            if (input == null)
              {
                input = args[i];
              }
            else if (output == null)
              {
                output = args[i];
              }
            else
              {
                System.err.println(args[i] + ": ignored");
              }
          }
      }
    try
      {
        InputStream is = (input == null ? System.in
            : new FileInputStream(input));
        OutputStream os = (output == null ? (OutputStream) System.out
            : new FileOutputStream(output));
        BufferedReader rdr = new BufferedReader(new InputStreamReader(is,
                                                                      encoding));
        PrintWriter wtr = new PrintWriter(
                                          new BufferedWriter(
                                                             new OutputStreamWriter(
                                                                                    os,
                                                                                    encoding)));
        while (true)
          {
            String s = rdr.readLine();
            if (s == null)
              break;
            StringBuffer sb = new StringBuffer(s.length() + 80);
            for (int i = 0; i < s.length(); i++)
              {
                char c = s.charAt(i);
                if (c <= 0xff)
                  {
                    sb.append(c);
                  }
                else
                  {
                    sb.append("\\u" + Integer.toHexString((int) c));
                  }
              }
            wtr.println(sb.toString());
          }
        rdr.close();
        wtr.flush();
        wtr.close();
      }
    catch (Exception e)
      {
        e.printStackTrace();
      }
  }
}