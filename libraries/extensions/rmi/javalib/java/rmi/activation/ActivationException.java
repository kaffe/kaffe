package java.rmi.activation;

import java.io.PrintStream;
import java.io.PrintWriter;

public class ActivationException
	extends Exception {

public Throwable detail;

public ActivationException() {
	super();
}

public ActivationException(String s) {
	super(s);
}

public ActivationException(String s, Throwable ex) {
	super(s);
	detail = ex;
}

public String getMessage() {
	if (detail == null) {
		return (super.getMessage());
	}
	else {
		return (super.getMessage() + ":" + detail.getMessage());
	}
}

public void printStackTrace(PrintStream s) {
        if (detail != null) {
                detail.printStackTrace(s);
        }
        super.printStackTrace(s);
}

public void printStackTrace() {
	printStackTrace(System.err);
}

public void printStackTrace(PrintWriter s) {
        if (detail != null) {
                detail.printStackTrace(s);
        }
        super.printStackTrace(s);
}

}
