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

private Math() { /* this class is not instantiable by the general public */ }

native public static double IEEEremainder(double f1, double f2);

public static double abs(double a) { return (a < 0.0) ? (-a) : (a); }

public static float abs(float a) { return (a < 0.0f) ? (-a) : (a); }

public static int abs(int a) { return (a < 0) ? (-a) : (a); }

public static long abs(long a) { return (a < 0L) ? (-a) : (a); }

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
	if (Double.isNaN(a) || Double.isNaN(b)) {
		return (Double.NaN);
	}
	else {
		return (b > a) ? (b) : (a);
	}
}

public static float max(float a, float b) {
	if (Float.isNaN(a) || Float.isNaN(b)) {
		return (Float.NaN);
	}
	else {
		return (b > a) ? (b) : (a);
	}
}

public static int max(int a, int b) { return (b > a) ? (b) : (a); }

public static long max(long a, long b) { return (b > a) ? (b) : (a); }


public static double min(double a, double b) {
	if (Double.isNaN(a) || Double.isNaN(b)) {
		return (Double.NaN);
	}
	else {
		return (a < b) ? (a) : (b);
	}
}

public static float min(float a, float b) {
	if (Float.isNaN(a) || Float.isNaN(b)) {
		return (Float.NaN);
	}
	else {
		return (a < b) ? (a) : (b);
	}
}

public static int min(int a, int b) { return (a < b) ? (a) : (b); }

public static long min(long a, long b) { return (a < b) ? (a) : (b); }

public static double pow(double a, double b) {
	if (abs(a) == 1.0 && Double.isInfinite(b)) {
		return Double.NaN;
	}
	return pow0(a, b);
}

native private static double pow0(double a, double b);

public static synchronized double random() {
	return MathRandom.random.nextDouble();
}

native public static double rint(double a);

public static long round(double a) {
	if ( a < (double)Long.MIN_VALUE || a == Double.NEGATIVE_INFINITY) {
		return (Long.MIN_VALUE);
	}
	else if (a > (double)Long.MAX_VALUE || a == Double.POSITIVE_INFINITY) {
		return (Long.MAX_VALUE);
	}
	else {
		return ((long)floor(a + 0.5d));
	}
}

public static int round(float a) {
	if (a < (float)Integer.MIN_VALUE || a == Float.NEGATIVE_INFINITY) {
		return (Integer.MIN_VALUE);
	}
	else if (a > (float)Integer.MAX_VALUE || a == Float.POSITIVE_INFINITY) {
		return (Integer.MAX_VALUE);
	}
	else {
		return ((int)floor((double)a + 0.5d));
	}
}

native public static double sin(double a);

native public static double sqrt(double a);

native public static double tan(double a);

public static double toDegrees(double angrad) {
	return(angrad * 180 / PI);
}

public static double toRadians(double angdeg) {
	return(angdeg * PI / 180);
}

    /**
     * This is here we we only init the random number generator when we
     * actually use it.
     */
    private static final class MathRandom {
	
	static Random random = new Random();
	
    }

}

