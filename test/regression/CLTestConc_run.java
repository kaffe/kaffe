class CLTestConc_bis {
    public String toString() {
        return Thread.currentThread().getName();
    }
}

public class CLTestConc_run {
    public String toString() {
        return new CLTestConc_bis().toString();
    }
}
