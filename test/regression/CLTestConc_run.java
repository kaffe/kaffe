/*
 * This file is technically part of CLTestConc
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
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
