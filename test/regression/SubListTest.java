import java.util.*;

public class SubListTest
{
  public static void main(String[] args)
  {
    testSubList(new LinkedList());
  }

  static void testSubList(List list)
  {
    list.clear();
    list.add("0");
    list.add("1");
    list.add("2");
    list.add("3");
    
    final int start = 1, end = 3;

    List sub = list.subList(start,end);
    System.out.println(sub);
    if (! (sub.get(0).equals(list.get(1))))
      throw new Error("Bug in get");

    Iterator it = sub.iterator();
    int i = 1;
    while (it.hasNext())
      {
        if (! (it.next().equals(list.get(i))))
          throw new Error("Bug in interator");
        i++;
      }

    if (i != end)
      throw new Error("Bug in interator");

    System.out.println("Success");
  }
}

/* Expected Output:
[1, 2]
Success
*/
