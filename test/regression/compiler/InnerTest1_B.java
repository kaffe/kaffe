public class InnerTest1_B extends InnerTest1_A {

    public class X extends InnerTest1_A.X {
        public InnerTest1_B.X x = new X();
    }

}
