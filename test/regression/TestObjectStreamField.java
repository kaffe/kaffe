/*
 * Test the most basic features of the ObjectStreamField class.
 *
 * Copyright (c) 2002 Pat Tullmann
 * All Rights Reserved.
 *
 * This file is released into the public domain.
 *
 * @author Pat Tullmann <pat_kaffe@tullmann.org>
 */
import java.io.*;

public class TestObjectStreamField
{
	static int i = 0;

	private static void check(String n, Class c)
	{
		i++;
		System.out.print(i+ ": ");
		try
		{
			ObjectStreamField osf = new ObjectStreamField(n,c);
			
			System.out.println("\"" +osf.getName()+ "\": " + osf.getType()
					   + " / " +osf.getTypeCode() 
					   + " / " +osf.getTypeString());
		}
		catch (Exception e)
		{
			System.out.println(" <threw: " +e.getClass().getName()+ ">");
		}
	}

	public static void main(String[] args)
	{
		check("name", int.class);
		check("", int.class);
		check(null, int.class);
		
		check("x", boolean.class);
		check("x", byte.class);
		check("x", char.class);
		check("x", short.class);
		check("x", float.class);
		check("x", double.class);
		check("x", int.class);
		check("x", long.class);
		check("x", void.class);
		check("x", Integer.class);
		check("x", String.class);
		check("x", (new int[10]).getClass());
		check("x", (new String[10][11]).getClass());
	}
}

/* Expected Output:
1: "name": int / I / null
2: "": int / I / null
3:  <threw: java.lang.NullPointerException>
4: "x": boolean / Z / null
5: "x": byte / B / null
6: "x": char / C / null
7: "x": short / S / null
8: "x": float / F / null
9: "x": double / D / null
10: "x": int / I / null
11: "x": long / J / null
12: "x": void / V / null
13: "x": class java.lang.Integer / L / Ljava/lang/Integer;
14: "x": class java.lang.String / L / Ljava/lang/String;
15: "x": class [I / [ / [I
16: "x": class [[Ljava.lang.String; / [ / [[Ljava/lang/String;
*/
