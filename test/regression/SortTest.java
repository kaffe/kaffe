
import java.util.*;

class SortTest {

  public static void main (String[] args) throws Exception {
    long seed = 12345;
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
    final Random r = new Random(seed);

    double[] da = new double[num];
    float[] fa = new float[num];
    byte[] ba = new byte[num];
    char[] ca = new char[num];
    int[] ia = new int[num];
    long[] la = new long[num];
    short[] sa = new short[num];
    Object[] oa = new Object[num];

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

    Arrays.sort(da);
    Arrays.sort(fa);
    Arrays.sort(ca);
    Arrays.sort(ia);
    Arrays.sort(la);
    Arrays.sort(sa);
    Arrays.sort(oa);

    for (int i = 1; i < num; i++) {
      if (da[i - 1] > da[i]) throw new Exception("index="+i);
      if (fa[i - 1] > fa[i]) throw new Exception("index="+i);
      if (ca[i - 1] > ca[i]) throw new Exception("index="+i);
      if (ia[i - 1] > ia[i]) throw new Exception("index="+i);
      if (la[i - 1] > la[i]) throw new Exception("index="+i);
      if (sa[i - 1] > sa[i]) throw new Exception("index="+i);
      if (((Comparable)oa[i - 1]).compareTo(oa[i]) > 0)
	throw new Exception("index="+i);
    }

    Arrays.sort(oa, 0, num / 2, new Comparator() {
	public int compare(Object o1, Object o2) {
	  final int val1 = ((Integer)o1).intValue();
	  final int val2 = ((Integer)o2).intValue();

	  return (val1 == val2) ? 0 : (val1 < val2) ? 1 : -1;
	}
      });

    for (int i = 1; i < num / 2; i++) {
      if (((Integer)oa[i - 1]).compareTo((Integer)oa[i]) < 0)
	throw new Exception("index="+i);
    }
    for (int i = num / 2 + 1; i < num; i++) {
      if (((Integer)oa[i - 1]).compareTo((Integer)oa[i]) > 0)
	throw new Exception("index="+i);
    }

    System.out.println("Success.");
  }
}

/* Expected Output:
Success.
*/


