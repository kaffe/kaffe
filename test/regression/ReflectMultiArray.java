import java.lang.reflect.Array;
public class ReflectMultiArray {
    public static void main(String[] argv) {
	int[] count= new int[1] ; // 1 seems to be the problem
	count[0]= 1 ;
	Object array= Array.newInstance(ReflectMultiArray.class, count) ;
    }
}
