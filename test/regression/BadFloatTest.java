import java.io.*;

class BadFloatTest {
    public static void main(String[] args)
           throws FileNotFoundException, IOException {
        float x = 1.2345f;
        double y = 9.8765;

        File inputFile = new File("example");
        
        System.out.println(x + " " + y);
    }   
}
