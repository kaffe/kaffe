package java.lang;

import java.util.Random;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class Math
{
	final public static double E = 2.7182818284590452354;
	final public static double PI = 3.14159265358979323846;
	public static Random randomGenerator = new Random();

native public static double IEEEremainder(double f1, double f2);

public static double abs(double a) {
	if (a < 0.0) {
		return (-a);
	}
	else {
		return (a);
	}
}

public static float abs(float a) {
	if (a < 0.0f) {
		return (-a);
	}
	else {
		return (a);
	}
}

public static int abs(int a) {
	if (a < 0) {
		return (-a);
	}
	else {
		return (a);
	}
}

public static long abs(long a) {
	if (a < 0L) {
		return (-a);
	}
	else {
		return (a);
	}
}

native public static double acos(double a);

native public static double asin(double a);

native public static double atan(double a);

native public static double atan2(double a, double b);

native public static double ceil(double a);

native public static double cos(double a);

native public static double exp(double a);

native public static double floor(double a);

native public static double log(double a);

public static double max(double a, double b) {
	if (b > a) {
		return (b);
	}
	else {
		return (a);
	}
}

public static float max(float a, float b) {
	if (b > a) {
		return (b);
	}
	else {
		return (a);
	}
}

public static int max(int a, int b) {
	if (b > a) {
		return (b);
	}
	else {
		return (a);
	}
}

public static long max(long a, long b) {
	if (b > a) {
		return (b);
	}
	else {
		return (a);
	}
}

public static double min(double a, double b) {
	if (a < b) {
		return (a);
	}
	else {
		return (b);
	}
}

public static float min(float a, float b) {
	if (a < b) {
		return (a);
	}
	else {
		return (b);
	}
}

public static int min(int a, int b) {
	if (a < b) {
		return (a);
	}
	else {
		return (b);
	}
}

public static long min(long a, long b) {
	if (a < b) {
		return (a);
	}
	else {
		return (b);
	}
}

native public static double pow(double a, double b);

public static synchronized double random() {
	double dbl=randomGenerator.nextDouble();
	return dbl;
}

native public static double rint(double a);

public static long round(double a) {
	if ((a<(double )Long.MIN_VALUE) || (a==Double.NEGATIVE_INFINITY)) {
		return Long.MIN_VALUE;
	}
	else if ((a>(double )Long.MAX_VALUE) || (a==Double.POSITIVE_INFINITY)) {
		return Long.MAX_VALUE;
	}
	else {
		return (long )rint(a);
	}
}

public static int round(float a) {
	if ((a<(float )Integer.MIN_VALUE) || (a==Float.NEGATIVE_INFINITY)) {
		return Integer.MIN_VALUE;
	}
	else if ((a>(float )Integer.MAX_VALUE) || (a==Float.POSITIVE_INFINITY)) {
		return Integer.MAX_VALUE;
	}
	else {
		return (int )rint((double )a);
	}
}

native public static double sin(double a);

native public static double sqrt(double a);

native public static double tan(double a);
}
