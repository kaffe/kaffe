/**
 * Maxim Kizub's test for tableswitch bug in jit
 */
public class TableSwitch {
 
        public static void main(String[] args) {
                int i = 0x7fffffff;
                System.out.println("switch("+i+")");
                switch(i) {
                case -1: System.out.println("-1"); break;
                case 0: System.out.println("0"); break;
                case 1: System.out.println("1"); break;
                default: System.out.println("default"); break;
                }
        }
 
}
/* Expected Output:
switch(2147483647)
default
*/
