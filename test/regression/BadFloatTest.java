import java.io.*;

class BadFloatTest {
    public static void main(String[] args)
           throws FileNotFoundException, IOException {
        float x = 1.2545f;
        double y = 9.8544;

        File inputFile = new File("example");
        
        System.out.println(x + " " + y);
    }   
}

/* Expected Output:
1.2545 9.8544
*/
