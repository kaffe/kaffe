
import java.util.*;

class SortTest {

  public static void main (String[] args) throws Exception {
    long seed = -1;
    int num = 100;

    switch (args.length) {
      case 2:
	seed = Long.parseLong(args[1]);
	/* fall through */
      case 1:
	num = Integer.parseInt(args[0]);
	/* fall through */
      case 0:
	break;
      default:
	throw new Exception("Usage: SortTest [#elem [randseed]]");
    }
    final Random r = (seed == -1) ? new Random() : new Random(seed);

    // Create arrays
    double[] da = new double[num];
    float[] fa = new float[num];
    byte[] ba = new byte[num];
    char[] ca = new char[num];
    int[] ia = new int[num];
    long[] la = new long[num];
    short[] sa = new short[num];
    Object[] oa = new Object[num];

    // Fill arrays with random values
    r.nextBytes(ba);
    for (int i = 0; i < num; i++) {
      da[i] = r.nextDouble();
      fa[i] = r.nextFloat();
      ca[i] = (char) r.nextInt();
      ia[i] = r.nextInt();
      la[i] = r.nextLong();
      sa[i] = (short) r.nextInt();
      oa[i] = new Integer(r.nextInt());
    }

    // Sort arrays
    Arrays.sort(da);
    Arrays.sort(fa);
    Arrays.sort(ca);
    Arrays.sort(ia);
    Arrays.sort(la);
    Arrays.sort(sa);
    Arrays.sort(oa);

    // Verify sorting
    for (int i = 1; i < num; i++) {
      assert(da[i - 1] <= da[i]);
      assert(fa[i - 1] <= fa[i]);
      assert(ca[i - 1] <= ca[i]);
      assert(ia[i - 1] <= ia[i]);
      assert(la[i - 1] <= la[i]);
      assert(sa[i - 1] <= sa[i]);
      assert(((Comparable)oa[i - 1]).compareTo(oa[i]) <= 0);
    }

    // Get random keys
    double dkey = r.nextDouble();
    float fkey = r.nextFloat();
    char ckey = (char) r.nextInt();
    int ikey = r.nextInt();
    long lkey = r.nextLong();
    short skey = (short) r.nextInt();
    Integer okey = new Integer(r.nextInt());

    // Verify binary search for each key
    int index;
    index = Arrays.binarySearch(da, dkey);
    assert((index >= 0) ? da[index] == dkey :
      (index == -1 || da[-(index + 2)] < dkey)
	&& (index == -num - 1 || da[-(index + 1)] > dkey));
    index = Arrays.binarySearch(fa, fkey);
    assert((index >= 0) ? fa[index] == fkey :
      (index == -1 || fa[-(index + 2)] < fkey)
	&& (index == -num - 1 || fa[-(index + 1)] > fkey));
    index = Arrays.binarySearch(ca, ckey);
    assert((index >= 0) ? ca[index] == ckey :
      (index == -1 || ca[-(index + 2)] < ckey)
	&& (index == -num - 1 || ca[-(index + 1)] > ckey));
    index = Arrays.binarySearch(ia, ikey);
    assert((index >= 0) ? ia[index] == ikey :
      (index == -1 || ia[-(index + 2)] < ikey)
	&& (index == -num - 1 || ia[-(index + 1)] > ikey));
    index = Arrays.binarySearch(la, lkey);
    assert((index >= 0) ? la[index] == lkey :
      (index == -1 || la[-(index + 2)] < lkey)
	&& (index == -num - 1 || la[-(index + 1)] > lkey));
    index = Arrays.binarySearch(sa, skey);
    assert((index >= 0) ? sa[index] == skey :
      (index == -1 || sa[-(index + 2)] < skey)
	&& (index == -num - 1 || sa[-(index + 1)] > skey));
    index = Arrays.binarySearch(oa, okey);
    assert((index >= 0) ? oa[index].equals(okey) :
      (index == -1 || ((Comparable)oa[-(index + 2)]).compareTo(okey) < 0)
	&& (index == -num - 1 || ((Comparable)oa[-(index + 1)]).compareTo(okey) > 0));

    // Re-sort first half of array in reverse
    Arrays.sort(oa, 0, num / 2, new Comparator() {
	public int compare(Object o1, Object o2) {
	  final int val1 = ((Integer)o1).intValue();
	  final int val2 = ((Integer)o2).intValue();

	  return (val1 == val2) ? 0 : (val1 < val2) ? 1 : -1;
	}
      });

    for (int i = 1; i < num / 2; i++) {
      assert(((Integer)oa[i - 1]).compareTo((Integer)oa[i]) >= 0);
    }
    for (int i = num / 2 + 1; i < num; i++) {
      assert(((Integer)oa[i - 1]).compareTo((Integer)oa[i]) <= 0);
    }

    System.out.println("Success.");
  }

  public static void assert(boolean truth) {
    if (!truth) {
      throw new Error("assertion failure");
    }
  }
}

/* Expected Output:
Success.
*/


