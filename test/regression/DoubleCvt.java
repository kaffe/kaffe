/**
 * Handle Double and Float conversion.
 *
 * @author Edouard G. Parmelan <egp@free.fr>
 */

public class DoubleCvt 
{
    static void print (String v, double d) {
	System.out.println (v + ": " + Long.toString (Double.doubleToLongBits (d), 16));
	System.out.println (v + " as float: " + Integer.toString (Float.floatToIntBits ((float)d), 16));
	System.out.println (v + " as int: " + (int)d + " " + Integer.toString ((int)d, 16));
	System.out.println (v + " as long: " + (long)d + " " + Long.toString ((long)d, 16));
    }
    
    static void print (String v, float d) {
	System.out.println (v + ": " + Integer.toString (Float.floatToIntBits (d), 16));
	System.out.println (v + " as double: " + Long.toString (Double.doubleToLongBits ((double)d), 16));
	System.out.println (v + " as int: " + (int)d + " " + Integer.toString ((int)d, 16));
	System.out.println (v + " as long: " + (long)d + " " + Long.toString ((long)d, 16));
    }

    static long lnp (int shift) 
    {
	return -1L >>> (64 - shift);
    }
    
    
    static int inp (int shift) 
    {
	return -1 >>> (32 - shift);
    }
    
    
    static void test_double() {
	print ("NaN", Double.NaN);
	print ("MIN_VALUE", Double.MIN_VALUE);
	print ("MAX_VALUE", Double.MAX_VALUE);
	print ("POSITIVE_INFINITY", Double.POSITIVE_INFINITY);
	print ("NEGATIVE_INFINITY", Double.NEGATIVE_INFINITY);

	for (int i = 1; i <= 64; i++) {
	    long l = lnp(i);

	    print ("-lnp(" + i + ")", (double)(-l));
	    print ("lnp(" + i + ")", (double)l);
	}
    }
    
    static void test_float() {
	print ("NaN", Float.NaN);
	print ("MIN_VALUE", Float.MIN_VALUE);
	print ("MAX_VALUE", Float.MAX_VALUE);
	print ("POSITIVE_INFINITY", Float.POSITIVE_INFINITY);
	print ("NEGATIVE_INFINITY", Float.NEGATIVE_INFINITY);

	for (int i = 1; i <= 32; i++) {
	    int l = inp(i);

	    print ("-inp(" + i + ")", (float)(-l));
	    print ("inp(" + i + ")", (float)l);
	}

    }
    
    public static void main (String[] args) 
    {
	test_double();
	test_float();
    }
    
}

/* Expected Output:
NaN: 7ff8000000000000
NaN as float: 7fc00000
NaN as int: 0 0
NaN as long: 0 0
MIN_VALUE: 1
MIN_VALUE as float: 0
MIN_VALUE as int: 0 0
MIN_VALUE as long: 0 0
MAX_VALUE: 7fefffffffffffff
MAX_VALUE as float: 7f800000
MAX_VALUE as int: 2147483647 7fffffff
MAX_VALUE as long: 9223372036854775807 7fffffffffffffff
POSITIVE_INFINITY: 7ff0000000000000
POSITIVE_INFINITY as float: 7f800000
POSITIVE_INFINITY as int: 2147483647 7fffffff
POSITIVE_INFINITY as long: 9223372036854775807 7fffffffffffffff
NEGATIVE_INFINITY: -10000000000000
NEGATIVE_INFINITY as float: -800000
NEGATIVE_INFINITY as int: -2147483648 -80000000
NEGATIVE_INFINITY as long: -9223372036854775808 -8000000000000000
-lnp(1): -4010000000000000
-lnp(1) as float: -40800000
-lnp(1) as int: -1 -1
-lnp(1) as long: -1 -1
lnp(1): 3ff0000000000000
lnp(1) as float: 3f800000
lnp(1) as int: 1 1
lnp(1) as long: 1 1
-lnp(2): -3ff8000000000000
-lnp(2) as float: -3fc00000
-lnp(2) as int: -3 -3
-lnp(2) as long: -3 -3
lnp(2): 4008000000000000
lnp(2) as float: 40400000
lnp(2) as int: 3 3
lnp(2) as long: 3 3
-lnp(3): -3fe4000000000000
-lnp(3) as float: -3f200000
-lnp(3) as int: -7 -7
-lnp(3) as long: -7 -7
lnp(3): 401c000000000000
lnp(3) as float: 40e00000
lnp(3) as int: 7 7
lnp(3) as long: 7 7
-lnp(4): -3fd2000000000000
-lnp(4) as float: -3e900000
-lnp(4) as int: -15 -f
-lnp(4) as long: -15 -f
lnp(4): 402e000000000000
lnp(4) as float: 41700000
lnp(4) as int: 15 f
lnp(4) as long: 15 f
-lnp(5): -3fc1000000000000
-lnp(5) as float: -3e080000
-lnp(5) as int: -31 -1f
-lnp(5) as long: -31 -1f
lnp(5): 403f000000000000
lnp(5) as float: 41f80000
lnp(5) as int: 31 1f
lnp(5) as long: 31 1f
-lnp(6): -3fb0800000000000
-lnp(6) as float: -3d840000
-lnp(6) as int: -63 -3f
-lnp(6) as long: -63 -3f
lnp(6): 404f800000000000
lnp(6) as float: 427c0000
lnp(6) as int: 63 3f
lnp(6) as long: 63 3f
-lnp(7): -3fa0400000000000
-lnp(7) as float: -3d020000
-lnp(7) as int: -127 -7f
-lnp(7) as long: -127 -7f
lnp(7): 405fc00000000000
lnp(7) as float: 42fe0000
lnp(7) as int: 127 7f
lnp(7) as long: 127 7f
-lnp(8): -3f90200000000000
-lnp(8) as float: -3c810000
-lnp(8) as int: -255 -ff
-lnp(8) as long: -255 -ff
lnp(8): 406fe00000000000
lnp(8) as float: 437f0000
lnp(8) as int: 255 ff
lnp(8) as long: 255 ff
-lnp(9): -3f80100000000000
-lnp(9) as float: -3c008000
-lnp(9) as int: -511 -1ff
-lnp(9) as long: -511 -1ff
lnp(9): 407ff00000000000
lnp(9) as float: 43ff8000
lnp(9) as int: 511 1ff
lnp(9) as long: 511 1ff
-lnp(10): -3f70080000000000
-lnp(10) as float: -3b804000
-lnp(10) as int: -1023 -3ff
-lnp(10) as long: -1023 -3ff
lnp(10): 408ff80000000000
lnp(10) as float: 447fc000
lnp(10) as int: 1023 3ff
lnp(10) as long: 1023 3ff
-lnp(11): -3f60040000000000
-lnp(11) as float: -3b002000
-lnp(11) as int: -2047 -7ff
-lnp(11) as long: -2047 -7ff
lnp(11): 409ffc0000000000
lnp(11) as float: 44ffe000
lnp(11) as int: 2047 7ff
lnp(11) as long: 2047 7ff
-lnp(12): -3f50020000000000
-lnp(12) as float: -3a801000
-lnp(12) as int: -4095 -fff
-lnp(12) as long: -4095 -fff
lnp(12): 40affe0000000000
lnp(12) as float: 457ff000
lnp(12) as int: 4095 fff
lnp(12) as long: 4095 fff
-lnp(13): -3f40010000000000
-lnp(13) as float: -3a000800
-lnp(13) as int: -8191 -1fff
-lnp(13) as long: -8191 -1fff
lnp(13): 40bfff0000000000
lnp(13) as float: 45fff800
lnp(13) as int: 8191 1fff
lnp(13) as long: 8191 1fff
-lnp(14): -3f30008000000000
-lnp(14) as float: -39800400
-lnp(14) as int: -16383 -3fff
-lnp(14) as long: -16383 -3fff
lnp(14): 40cfff8000000000
lnp(14) as float: 467ffc00
lnp(14) as int: 16383 3fff
lnp(14) as long: 16383 3fff
-lnp(15): -3f20004000000000
-lnp(15) as float: -39000200
-lnp(15) as int: -32767 -7fff
-lnp(15) as long: -32767 -7fff
lnp(15): 40dfffc000000000
lnp(15) as float: 46fffe00
lnp(15) as int: 32767 7fff
lnp(15) as long: 32767 7fff
-lnp(16): -3f10002000000000
-lnp(16) as float: -38800100
-lnp(16) as int: -65535 -ffff
-lnp(16) as long: -65535 -ffff
lnp(16): 40efffe000000000
lnp(16) as float: 477fff00
lnp(16) as int: 65535 ffff
lnp(16) as long: 65535 ffff
-lnp(17): -3f00001000000000
-lnp(17) as float: -38000080
-lnp(17) as int: -131071 -1ffff
-lnp(17) as long: -131071 -1ffff
lnp(17): 40fffff000000000
lnp(17) as float: 47ffff80
lnp(17) as int: 131071 1ffff
lnp(17) as long: 131071 1ffff
-lnp(18): -3ef0000800000000
-lnp(18) as float: -37800040
-lnp(18) as int: -262143 -3ffff
-lnp(18) as long: -262143 -3ffff
lnp(18): 410ffff800000000
lnp(18) as float: 487fffc0
lnp(18) as int: 262143 3ffff
lnp(18) as long: 262143 3ffff
-lnp(19): -3ee0000400000000
-lnp(19) as float: -37000020
-lnp(19) as int: -524287 -7ffff
-lnp(19) as long: -524287 -7ffff
lnp(19): 411ffffc00000000
lnp(19) as float: 48ffffe0
lnp(19) as int: 524287 7ffff
lnp(19) as long: 524287 7ffff
-lnp(20): -3ed0000200000000
-lnp(20) as float: -36800010
-lnp(20) as int: -1048575 -fffff
-lnp(20) as long: -1048575 -fffff
lnp(20): 412ffffe00000000
lnp(20) as float: 497ffff0
lnp(20) as int: 1048575 fffff
lnp(20) as long: 1048575 fffff
-lnp(21): -3ec0000100000000
-lnp(21) as float: -36000008
-lnp(21) as int: -2097151 -1fffff
-lnp(21) as long: -2097151 -1fffff
lnp(21): 413fffff00000000
lnp(21) as float: 49fffff8
lnp(21) as int: 2097151 1fffff
lnp(21) as long: 2097151 1fffff
-lnp(22): -3eb0000080000000
-lnp(22) as float: -35800004
-lnp(22) as int: -4194303 -3fffff
-lnp(22) as long: -4194303 -3fffff
lnp(22): 414fffff80000000
lnp(22) as float: 4a7ffffc
lnp(22) as int: 4194303 3fffff
lnp(22) as long: 4194303 3fffff
-lnp(23): -3ea0000040000000
-lnp(23) as float: -35000002
-lnp(23) as int: -8388607 -7fffff
-lnp(23) as long: -8388607 -7fffff
lnp(23): 415fffffc0000000
lnp(23) as float: 4afffffe
lnp(23) as int: 8388607 7fffff
lnp(23) as long: 8388607 7fffff
-lnp(24): -3e90000020000000
-lnp(24) as float: -34800001
-lnp(24) as int: -16777215 -ffffff
-lnp(24) as long: -16777215 -ffffff
lnp(24): 416fffffe0000000
lnp(24) as float: 4b7fffff
lnp(24) as int: 16777215 ffffff
lnp(24) as long: 16777215 ffffff
-lnp(25): -3e80000010000000
-lnp(25) as float: -34000000
-lnp(25) as int: -33554431 -1ffffff
-lnp(25) as long: -33554431 -1ffffff
lnp(25): 417ffffff0000000
lnp(25) as float: 4c000000
lnp(25) as int: 33554431 1ffffff
lnp(25) as long: 33554431 1ffffff
-lnp(26): -3e70000008000000
-lnp(26) as float: -33800000
-lnp(26) as int: -67108863 -3ffffff
-lnp(26) as long: -67108863 -3ffffff
lnp(26): 418ffffff8000000
lnp(26) as float: 4c800000
lnp(26) as int: 67108863 3ffffff
lnp(26) as long: 67108863 3ffffff
-lnp(27): -3e60000004000000
-lnp(27) as float: -33000000
-lnp(27) as int: -134217727 -7ffffff
-lnp(27) as long: -134217727 -7ffffff
lnp(27): 419ffffffc000000
lnp(27) as float: 4d000000
lnp(27) as int: 134217727 7ffffff
lnp(27) as long: 134217727 7ffffff
-lnp(28): -3e50000002000000
-lnp(28) as float: -32800000
-lnp(28) as int: -268435455 -fffffff
-lnp(28) as long: -268435455 -fffffff
lnp(28): 41affffffe000000
lnp(28) as float: 4d800000
lnp(28) as int: 268435455 fffffff
lnp(28) as long: 268435455 fffffff
-lnp(29): -3e40000001000000
-lnp(29) as float: -32000000
-lnp(29) as int: -536870911 -1fffffff
-lnp(29) as long: -536870911 -1fffffff
lnp(29): 41bfffffff000000
lnp(29) as float: 4e000000
lnp(29) as int: 536870911 1fffffff
lnp(29) as long: 536870911 1fffffff
-lnp(30): -3e30000000800000
-lnp(30) as float: -31800000
-lnp(30) as int: -1073741823 -3fffffff
-lnp(30) as long: -1073741823 -3fffffff
lnp(30): 41cfffffff800000
lnp(30) as float: 4e800000
lnp(30) as int: 1073741823 3fffffff
lnp(30) as long: 1073741823 3fffffff
-lnp(31): -3e20000000400000
-lnp(31) as float: -31000000
-lnp(31) as int: -2147483647 -7fffffff
-lnp(31) as long: -2147483647 -7fffffff
lnp(31): 41dfffffffc00000
lnp(31) as float: 4f000000
lnp(31) as int: 2147483647 7fffffff
lnp(31) as long: 2147483647 7fffffff
-lnp(32): -3e10000000200000
-lnp(32) as float: -30800000
-lnp(32) as int: -2147483648 -80000000
-lnp(32) as long: -4294967295 -ffffffff
lnp(32): 41efffffffe00000
lnp(32) as float: 4f800000
lnp(32) as int: 2147483647 7fffffff
lnp(32) as long: 4294967295 ffffffff
-lnp(33): -3e00000000100000
-lnp(33) as float: -30000000
-lnp(33) as int: -2147483648 -80000000
-lnp(33) as long: -8589934591 -1ffffffff
lnp(33): 41fffffffff00000
lnp(33) as float: 50000000
lnp(33) as int: 2147483647 7fffffff
lnp(33) as long: 8589934591 1ffffffff
-lnp(34): -3df0000000080000
-lnp(34) as float: -2f800000
-lnp(34) as int: -2147483648 -80000000
-lnp(34) as long: -17179869183 -3ffffffff
lnp(34): 420ffffffff80000
lnp(34) as float: 50800000
lnp(34) as int: 2147483647 7fffffff
lnp(34) as long: 17179869183 3ffffffff
-lnp(35): -3de0000000040000
-lnp(35) as float: -2f000000
-lnp(35) as int: -2147483648 -80000000
-lnp(35) as long: -34359738367 -7ffffffff
lnp(35): 421ffffffffc0000
lnp(35) as float: 51000000
lnp(35) as int: 2147483647 7fffffff
lnp(35) as long: 34359738367 7ffffffff
-lnp(36): -3dd0000000020000
-lnp(36) as float: -2e800000
-lnp(36) as int: -2147483648 -80000000
-lnp(36) as long: -68719476735 -fffffffff
lnp(36): 422ffffffffe0000
lnp(36) as float: 51800000
lnp(36) as int: 2147483647 7fffffff
lnp(36) as long: 68719476735 fffffffff
-lnp(37): -3dc0000000010000
-lnp(37) as float: -2e000000
-lnp(37) as int: -2147483648 -80000000
-lnp(37) as long: -137438953471 -1fffffffff
lnp(37): 423fffffffff0000
lnp(37) as float: 52000000
lnp(37) as int: 2147483647 7fffffff
lnp(37) as long: 137438953471 1fffffffff
-lnp(38): -3db0000000008000
-lnp(38) as float: -2d800000
-lnp(38) as int: -2147483648 -80000000
-lnp(38) as long: -274877906943 -3fffffffff
lnp(38): 424fffffffff8000
lnp(38) as float: 52800000
lnp(38) as int: 2147483647 7fffffff
lnp(38) as long: 274877906943 3fffffffff
-lnp(39): -3da0000000004000
-lnp(39) as float: -2d000000
-lnp(39) as int: -2147483648 -80000000
-lnp(39) as long: -549755813887 -7fffffffff
lnp(39): 425fffffffffc000
lnp(39) as float: 53000000
lnp(39) as int: 2147483647 7fffffff
lnp(39) as long: 549755813887 7fffffffff
-lnp(40): -3d90000000002000
-lnp(40) as float: -2c800000
-lnp(40) as int: -2147483648 -80000000
-lnp(40) as long: -1099511627775 -ffffffffff
lnp(40): 426fffffffffe000
lnp(40) as float: 53800000
lnp(40) as int: 2147483647 7fffffff
lnp(40) as long: 1099511627775 ffffffffff
-lnp(41): -3d80000000001000
-lnp(41) as float: -2c000000
-lnp(41) as int: -2147483648 -80000000
-lnp(41) as long: -2199023255551 -1ffffffffff
lnp(41): 427ffffffffff000
lnp(41) as float: 54000000
lnp(41) as int: 2147483647 7fffffff
lnp(41) as long: 2199023255551 1ffffffffff
-lnp(42): -3d70000000000800
-lnp(42) as float: -2b800000
-lnp(42) as int: -2147483648 -80000000
-lnp(42) as long: -4398046511103 -3ffffffffff
lnp(42): 428ffffffffff800
lnp(42) as float: 54800000
lnp(42) as int: 2147483647 7fffffff
lnp(42) as long: 4398046511103 3ffffffffff
-lnp(43): -3d60000000000400
-lnp(43) as float: -2b000000
-lnp(43) as int: -2147483648 -80000000
-lnp(43) as long: -8796093022207 -7ffffffffff
lnp(43): 429ffffffffffc00
lnp(43) as float: 55000000
lnp(43) as int: 2147483647 7fffffff
lnp(43) as long: 8796093022207 7ffffffffff
-lnp(44): -3d50000000000200
-lnp(44) as float: -2a800000
-lnp(44) as int: -2147483648 -80000000
-lnp(44) as long: -17592186044415 -fffffffffff
lnp(44): 42affffffffffe00
lnp(44) as float: 55800000
lnp(44) as int: 2147483647 7fffffff
lnp(44) as long: 17592186044415 fffffffffff
-lnp(45): -3d40000000000100
-lnp(45) as float: -2a000000
-lnp(45) as int: -2147483648 -80000000
-lnp(45) as long: -35184372088831 -1fffffffffff
lnp(45): 42bfffffffffff00
lnp(45) as float: 56000000
lnp(45) as int: 2147483647 7fffffff
lnp(45) as long: 35184372088831 1fffffffffff
-lnp(46): -3d30000000000080
-lnp(46) as float: -29800000
-lnp(46) as int: -2147483648 -80000000
-lnp(46) as long: -70368744177663 -3fffffffffff
lnp(46): 42cfffffffffff80
lnp(46) as float: 56800000
lnp(46) as int: 2147483647 7fffffff
lnp(46) as long: 70368744177663 3fffffffffff
-lnp(47): -3d20000000000040
-lnp(47) as float: -29000000
-lnp(47) as int: -2147483648 -80000000
-lnp(47) as long: -140737488355327 -7fffffffffff
lnp(47): 42dfffffffffffc0
lnp(47) as float: 57000000
lnp(47) as int: 2147483647 7fffffff
lnp(47) as long: 140737488355327 7fffffffffff
-lnp(48): -3d10000000000020
-lnp(48) as float: -28800000
-lnp(48) as int: -2147483648 -80000000
-lnp(48) as long: -281474976710655 -ffffffffffff
lnp(48): 42efffffffffffe0
lnp(48) as float: 57800000
lnp(48) as int: 2147483647 7fffffff
lnp(48) as long: 281474976710655 ffffffffffff
-lnp(49): -3d00000000000010
-lnp(49) as float: -28000000
-lnp(49) as int: -2147483648 -80000000
-lnp(49) as long: -562949953421311 -1ffffffffffff
lnp(49): 42fffffffffffff0
lnp(49) as float: 58000000
lnp(49) as int: 2147483647 7fffffff
lnp(49) as long: 562949953421311 1ffffffffffff
-lnp(50): -3cf0000000000008
-lnp(50) as float: -27800000
-lnp(50) as int: -2147483648 -80000000
-lnp(50) as long: -1125899906842623 -3ffffffffffff
lnp(50): 430ffffffffffff8
lnp(50) as float: 58800000
lnp(50) as int: 2147483647 7fffffff
lnp(50) as long: 1125899906842623 3ffffffffffff
-lnp(51): -3ce0000000000004
-lnp(51) as float: -27000000
-lnp(51) as int: -2147483648 -80000000
-lnp(51) as long: -2251799813685247 -7ffffffffffff
lnp(51): 431ffffffffffffc
lnp(51) as float: 59000000
lnp(51) as int: 2147483647 7fffffff
lnp(51) as long: 2251799813685247 7ffffffffffff
-lnp(52): -3cd0000000000002
-lnp(52) as float: -26800000
-lnp(52) as int: -2147483648 -80000000
-lnp(52) as long: -4503599627370495 -fffffffffffff
lnp(52): 432ffffffffffffe
lnp(52) as float: 59800000
lnp(52) as int: 2147483647 7fffffff
lnp(52) as long: 4503599627370495 fffffffffffff
-lnp(53): -3cc0000000000001
-lnp(53) as float: -26000000
-lnp(53) as int: -2147483648 -80000000
-lnp(53) as long: -9007199254740991 -1fffffffffffff
lnp(53): 433fffffffffffff
lnp(53) as float: 5a000000
lnp(53) as int: 2147483647 7fffffff
lnp(53) as long: 9007199254740991 1fffffffffffff
-lnp(54): -3cb0000000000000
-lnp(54) as float: -25800000
-lnp(54) as int: -2147483648 -80000000
-lnp(54) as long: -18014398509481984 -40000000000000
lnp(54): 4350000000000000
lnp(54) as float: 5a800000
lnp(54) as int: 2147483647 7fffffff
lnp(54) as long: 18014398509481984 40000000000000
-lnp(55): -3ca0000000000000
-lnp(55) as float: -25000000
-lnp(55) as int: -2147483648 -80000000
-lnp(55) as long: -36028797018963968 -80000000000000
lnp(55): 4360000000000000
lnp(55) as float: 5b000000
lnp(55) as int: 2147483647 7fffffff
lnp(55) as long: 36028797018963968 80000000000000
-lnp(56): -3c90000000000000
-lnp(56) as float: -24800000
-lnp(56) as int: -2147483648 -80000000
-lnp(56) as long: -72057594037927936 -100000000000000
lnp(56): 4370000000000000
lnp(56) as float: 5b800000
lnp(56) as int: 2147483647 7fffffff
lnp(56) as long: 72057594037927936 100000000000000
-lnp(57): -3c80000000000000
-lnp(57) as float: -24000000
-lnp(57) as int: -2147483648 -80000000
-lnp(57) as long: -144115188075855872 -200000000000000
lnp(57): 4380000000000000
lnp(57) as float: 5c000000
lnp(57) as int: 2147483647 7fffffff
lnp(57) as long: 144115188075855872 200000000000000
-lnp(58): -3c70000000000000
-lnp(58) as float: -23800000
-lnp(58) as int: -2147483648 -80000000
-lnp(58) as long: -288230376151711744 -400000000000000
lnp(58): 4390000000000000
lnp(58) as float: 5c800000
lnp(58) as int: 2147483647 7fffffff
lnp(58) as long: 288230376151711744 400000000000000
-lnp(59): -3c60000000000000
-lnp(59) as float: -23000000
-lnp(59) as int: -2147483648 -80000000
-lnp(59) as long: -576460752303423488 -800000000000000
lnp(59): 43a0000000000000
lnp(59) as float: 5d000000
lnp(59) as int: 2147483647 7fffffff
lnp(59) as long: 576460752303423488 800000000000000
-lnp(60): -3c50000000000000
-lnp(60) as float: -22800000
-lnp(60) as int: -2147483648 -80000000
-lnp(60) as long: -1152921504606846976 -1000000000000000
lnp(60): 43b0000000000000
lnp(60) as float: 5d800000
lnp(60) as int: 2147483647 7fffffff
lnp(60) as long: 1152921504606846976 1000000000000000
-lnp(61): -3c40000000000000
-lnp(61) as float: -22000000
-lnp(61) as int: -2147483648 -80000000
-lnp(61) as long: -2305843009213693952 -2000000000000000
lnp(61): 43c0000000000000
lnp(61) as float: 5e000000
lnp(61) as int: 2147483647 7fffffff
lnp(61) as long: 2305843009213693952 2000000000000000
-lnp(62): -3c30000000000000
-lnp(62) as float: -21800000
-lnp(62) as int: -2147483648 -80000000
-lnp(62) as long: -4611686018427387904 -4000000000000000
lnp(62): 43d0000000000000
lnp(62) as float: 5e800000
lnp(62) as int: 2147483647 7fffffff
lnp(62) as long: 4611686018427387904 4000000000000000
-lnp(63): -3c20000000000000
-lnp(63) as float: -21000000
-lnp(63) as int: -2147483648 -80000000
-lnp(63) as long: -9223372036854775808 -8000000000000000
lnp(63): 43e0000000000000
lnp(63) as float: 5f000000
lnp(63) as int: 2147483647 7fffffff
lnp(63) as long: 9223372036854775807 7fffffffffffffff
-lnp(64): 3ff0000000000000
-lnp(64) as float: 3f800000
-lnp(64) as int: 1 1
-lnp(64) as long: 1 1
lnp(64): -4010000000000000
lnp(64) as float: -40800000
lnp(64) as int: -1 -1
lnp(64) as long: -1 -1
NaN: 7fc00000
NaN as double: 7ff8000000000000
NaN as int: 0 0
NaN as long: 0 0
MIN_VALUE: 1
MIN_VALUE as double: 36a0000000000000
MIN_VALUE as int: 0 0
MIN_VALUE as long: 0 0
MAX_VALUE: 7f7fffff
MAX_VALUE as double: 47efffffe0000000
MAX_VALUE as int: 2147483647 7fffffff
MAX_VALUE as long: 9223372036854775807 7fffffffffffffff
POSITIVE_INFINITY: 7f800000
POSITIVE_INFINITY as double: 7ff0000000000000
POSITIVE_INFINITY as int: 2147483647 7fffffff
POSITIVE_INFINITY as long: 9223372036854775807 7fffffffffffffff
NEGATIVE_INFINITY: -800000
NEGATIVE_INFINITY as double: -10000000000000
NEGATIVE_INFINITY as int: -2147483648 -80000000
NEGATIVE_INFINITY as long: -9223372036854775808 -8000000000000000
-inp(1): -40800000
-inp(1) as double: -4010000000000000
-inp(1) as int: -1 -1
-inp(1) as long: -1 -1
inp(1): 3f800000
inp(1) as double: 3ff0000000000000
inp(1) as int: 1 1
inp(1) as long: 1 1
-inp(2): -3fc00000
-inp(2) as double: -3ff8000000000000
-inp(2) as int: -3 -3
-inp(2) as long: -3 -3
inp(2): 40400000
inp(2) as double: 4008000000000000
inp(2) as int: 3 3
inp(2) as long: 3 3
-inp(3): -3f200000
-inp(3) as double: -3fe4000000000000
-inp(3) as int: -7 -7
-inp(3) as long: -7 -7
inp(3): 40e00000
inp(3) as double: 401c000000000000
inp(3) as int: 7 7
inp(3) as long: 7 7
-inp(4): -3e900000
-inp(4) as double: -3fd2000000000000
-inp(4) as int: -15 -f
-inp(4) as long: -15 -f
inp(4): 41700000
inp(4) as double: 402e000000000000
inp(4) as int: 15 f
inp(4) as long: 15 f
-inp(5): -3e080000
-inp(5) as double: -3fc1000000000000
-inp(5) as int: -31 -1f
-inp(5) as long: -31 -1f
inp(5): 41f80000
inp(5) as double: 403f000000000000
inp(5) as int: 31 1f
inp(5) as long: 31 1f
-inp(6): -3d840000
-inp(6) as double: -3fb0800000000000
-inp(6) as int: -63 -3f
-inp(6) as long: -63 -3f
inp(6): 427c0000
inp(6) as double: 404f800000000000
inp(6) as int: 63 3f
inp(6) as long: 63 3f
-inp(7): -3d020000
-inp(7) as double: -3fa0400000000000
-inp(7) as int: -127 -7f
-inp(7) as long: -127 -7f
inp(7): 42fe0000
inp(7) as double: 405fc00000000000
inp(7) as int: 127 7f
inp(7) as long: 127 7f
-inp(8): -3c810000
-inp(8) as double: -3f90200000000000
-inp(8) as int: -255 -ff
-inp(8) as long: -255 -ff
inp(8): 437f0000
inp(8) as double: 406fe00000000000
inp(8) as int: 255 ff
inp(8) as long: 255 ff
-inp(9): -3c008000
-inp(9) as double: -3f80100000000000
-inp(9) as int: -511 -1ff
-inp(9) as long: -511 -1ff
inp(9): 43ff8000
inp(9) as double: 407ff00000000000
inp(9) as int: 511 1ff
inp(9) as long: 511 1ff
-inp(10): -3b804000
-inp(10) as double: -3f70080000000000
-inp(10) as int: -1023 -3ff
-inp(10) as long: -1023 -3ff
inp(10): 447fc000
inp(10) as double: 408ff80000000000
inp(10) as int: 1023 3ff
inp(10) as long: 1023 3ff
-inp(11): -3b002000
-inp(11) as double: -3f60040000000000
-inp(11) as int: -2047 -7ff
-inp(11) as long: -2047 -7ff
inp(11): 44ffe000
inp(11) as double: 409ffc0000000000
inp(11) as int: 2047 7ff
inp(11) as long: 2047 7ff
-inp(12): -3a801000
-inp(12) as double: -3f50020000000000
-inp(12) as int: -4095 -fff
-inp(12) as long: -4095 -fff
inp(12): 457ff000
inp(12) as double: 40affe0000000000
inp(12) as int: 4095 fff
inp(12) as long: 4095 fff
-inp(13): -3a000800
-inp(13) as double: -3f40010000000000
-inp(13) as int: -8191 -1fff
-inp(13) as long: -8191 -1fff
inp(13): 45fff800
inp(13) as double: 40bfff0000000000
inp(13) as int: 8191 1fff
inp(13) as long: 8191 1fff
-inp(14): -39800400
-inp(14) as double: -3f30008000000000
-inp(14) as int: -16383 -3fff
-inp(14) as long: -16383 -3fff
inp(14): 467ffc00
inp(14) as double: 40cfff8000000000
inp(14) as int: 16383 3fff
inp(14) as long: 16383 3fff
-inp(15): -39000200
-inp(15) as double: -3f20004000000000
-inp(15) as int: -32767 -7fff
-inp(15) as long: -32767 -7fff
inp(15): 46fffe00
inp(15) as double: 40dfffc000000000
inp(15) as int: 32767 7fff
inp(15) as long: 32767 7fff
-inp(16): -38800100
-inp(16) as double: -3f10002000000000
-inp(16) as int: -65535 -ffff
-inp(16) as long: -65535 -ffff
inp(16): 477fff00
inp(16) as double: 40efffe000000000
inp(16) as int: 65535 ffff
inp(16) as long: 65535 ffff
-inp(17): -38000080
-inp(17) as double: -3f00001000000000
-inp(17) as int: -131071 -1ffff
-inp(17) as long: -131071 -1ffff
inp(17): 47ffff80
inp(17) as double: 40fffff000000000
inp(17) as int: 131071 1ffff
inp(17) as long: 131071 1ffff
-inp(18): -37800040
-inp(18) as double: -3ef0000800000000
-inp(18) as int: -262143 -3ffff
-inp(18) as long: -262143 -3ffff
inp(18): 487fffc0
inp(18) as double: 410ffff800000000
inp(18) as int: 262143 3ffff
inp(18) as long: 262143 3ffff
-inp(19): -37000020
-inp(19) as double: -3ee0000400000000
-inp(19) as int: -524287 -7ffff
-inp(19) as long: -524287 -7ffff
inp(19): 48ffffe0
inp(19) as double: 411ffffc00000000
inp(19) as int: 524287 7ffff
inp(19) as long: 524287 7ffff
-inp(20): -36800010
-inp(20) as double: -3ed0000200000000
-inp(20) as int: -1048575 -fffff
-inp(20) as long: -1048575 -fffff
inp(20): 497ffff0
inp(20) as double: 412ffffe00000000
inp(20) as int: 1048575 fffff
inp(20) as long: 1048575 fffff
-inp(21): -36000008
-inp(21) as double: -3ec0000100000000
-inp(21) as int: -2097151 -1fffff
-inp(21) as long: -2097151 -1fffff
inp(21): 49fffff8
inp(21) as double: 413fffff00000000
inp(21) as int: 2097151 1fffff
inp(21) as long: 2097151 1fffff
-inp(22): -35800004
-inp(22) as double: -3eb0000080000000
-inp(22) as int: -4194303 -3fffff
-inp(22) as long: -4194303 -3fffff
inp(22): 4a7ffffc
inp(22) as double: 414fffff80000000
inp(22) as int: 4194303 3fffff
inp(22) as long: 4194303 3fffff
-inp(23): -35000002
-inp(23) as double: -3ea0000040000000
-inp(23) as int: -8388607 -7fffff
-inp(23) as long: -8388607 -7fffff
inp(23): 4afffffe
inp(23) as double: 415fffffc0000000
inp(23) as int: 8388607 7fffff
inp(23) as long: 8388607 7fffff
-inp(24): -34800001
-inp(24) as double: -3e90000020000000
-inp(24) as int: -16777215 -ffffff
-inp(24) as long: -16777215 -ffffff
inp(24): 4b7fffff
inp(24) as double: 416fffffe0000000
inp(24) as int: 16777215 ffffff
inp(24) as long: 16777215 ffffff
-inp(25): -34000000
-inp(25) as double: -3e80000000000000
-inp(25) as int: -33554432 -2000000
-inp(25) as long: -33554432 -2000000
inp(25): 4c000000
inp(25) as double: 4180000000000000
inp(25) as int: 33554432 2000000
inp(25) as long: 33554432 2000000
-inp(26): -33800000
-inp(26) as double: -3e70000000000000
-inp(26) as int: -67108864 -4000000
-inp(26) as long: -67108864 -4000000
inp(26): 4c800000
inp(26) as double: 4190000000000000
inp(26) as int: 67108864 4000000
inp(26) as long: 67108864 4000000
-inp(27): -33000000
-inp(27) as double: -3e60000000000000
-inp(27) as int: -134217728 -8000000
-inp(27) as long: -134217728 -8000000
inp(27): 4d000000
inp(27) as double: 41a0000000000000
inp(27) as int: 134217728 8000000
inp(27) as long: 134217728 8000000
-inp(28): -32800000
-inp(28) as double: -3e50000000000000
-inp(28) as int: -268435456 -10000000
-inp(28) as long: -268435456 -10000000
inp(28): 4d800000
inp(28) as double: 41b0000000000000
inp(28) as int: 268435456 10000000
inp(28) as long: 268435456 10000000
-inp(29): -32000000
-inp(29) as double: -3e40000000000000
-inp(29) as int: -536870912 -20000000
-inp(29) as long: -536870912 -20000000
inp(29): 4e000000
inp(29) as double: 41c0000000000000
inp(29) as int: 536870912 20000000
inp(29) as long: 536870912 20000000
-inp(30): -31800000
-inp(30) as double: -3e30000000000000
-inp(30) as int: -1073741824 -40000000
-inp(30) as long: -1073741824 -40000000
inp(30): 4e800000
inp(30) as double: 41d0000000000000
inp(30) as int: 1073741824 40000000
inp(30) as long: 1073741824 40000000
-inp(31): -31000000
-inp(31) as double: -3e20000000000000
-inp(31) as int: -2147483648 -80000000
-inp(31) as long: -2147483648 -80000000
inp(31): 4f000000
inp(31) as double: 41e0000000000000
inp(31) as int: 2147483647 7fffffff
inp(31) as long: 2147483648 80000000
-inp(32): 3f800000
-inp(32) as double: 3ff0000000000000
-inp(32) as int: 1 1
-inp(32) as long: 1 1
inp(32): -40800000
inp(32) as double: -4010000000000000
inp(32) as int: -1 -1
inp(32) as long: -1 -1
*/
