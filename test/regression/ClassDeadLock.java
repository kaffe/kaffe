/** 
 * Test case for deadlocking in the Jitter. 
 * 
 * Sequence of events: 
 * 
 * A call to blockOnJitter locks the class lock 
 * 
 * A call to blockInJitter starts the jit and then calls 
 * classMethod.c:resolveString() attempting to resolve the string for 
 * `blockInJitter'. However, the blockOnJitter thread already has 
 * the class lock so it blocks. 
 * 
 * Next the blockOnJitter thread tries to call the loop() method which 
 * blocks on the jitter lock 
 * 
 * And we have deadlock... 
 *
 * Patrick Tullmann <tullmann@cs.utah.edu>
 */ 
public class ClassDeadLock 
{ 
        private static void loop() 
        { 
                for( int lpc = 0; lpc < 100; lpc++ ) 
                { 
                } 
        } 
         
        private static synchronized void blockInJitter() 
        { 
                ClassDeadLock.print("2.2 This is blockInJitter"); 
        } 
         
        private static synchronized void blockOnJitter() 
        { 
                ClassDeadLock.print("1.2 blockOnJitter thread has class lock"); 
                for( int lpc = 0; lpc < 100; lpc++ ) 
                { 
                } 
                ClassDeadLock.print("1.3 Yielding for blockInJitter thread"); 
                Thread.yield(); 
                ClassDeadLock.print("1.4 Calling loop()"); 
                ClassDeadLock.loop(); 
        } 
         
        public static void main(String args[]) throws Exception 
        { 
                Thread th1, th2; 

                th1 = new Thread() { 
                        public void run() 
                        { 
                                ClassDeadLock.print("1.1 Start blockOnJitter"); 
                                ClassDeadLock.blockOnJitter(); 
                                ClassDeadLock.print("1.5 End blockOnJitter"); 
                        } 
                }; 
                th2 = new Thread() { 
                        public void run() 
                        { 
                                ClassDeadLock.print("2.1 Start blockInJitter"); 
                                ClassDeadLock.blockInJitter(); 
                                ClassDeadLock.print("2.3 End blockInJitter"); 
                        } 
                }; 
                th1.start(); 
                th2.start(); 
                th1.join(); 
                th2.join(); 
        } 

	static synchronized void print(String s) {
		System.out.println(s);
	}
} 

// Sort output
/* Expected Output:
1.1 Start blockOnJitter
1.2 blockOnJitter thread has class lock
1.3 Yielding for blockInJitter thread
1.4 Calling loop()
1.5 End blockOnJitter
2.1 Start blockInJitter
2.2 This is blockInJitter
2.3 End blockInJitter
*/
