package java.rmi.activation;

import java.io.Serializable;
import java.util.Properties;
import java.rmi.MarshalledObject;

public final class ActivationGroupDesc
	implements Serializable {

public static class CommandEnvironment
	implements Serializable {

private String cmdpath;
private String[] argv;

public CommandEnvironment(String cmdpath, String[] argv) {
	this.cmdpath = cmdpath;
	this.argv = argv;
}

public String getCommandPath() {
	return (cmdpath);
}

public String[] getCommandOptions() {
	return (argv);
}

public boolean equals(Object obj) {
	if (!(obj instanceof CommandEnvironment)) {
		return (false);
	}
	CommandEnvironment that = (CommandEnvironment)obj;
	
	if (!this.cmdpath.equals(that.cmdpath)) {
		return (false);
	}

	int len = this.argv.length;
	if (len != that.argv.length) {
		return (false);
	}
	for (int i = 0; i < len; i++) {
		if (!this.argv[i].equals(that.argv[i])) {
			return (false);
		}
	}
	return (true);
}

public int hashCode() {
	return (cmdpath.hashCode()); // Not a very good hash code.
}

}

public ActivationGroupDesc(Properties overrides, ActivationGroupDesc.CommandEnvironment cmd) {
	throw new kaffe.util.NotImplemented();
}

public ActivationGroupDesc(String className, String location, MarshalledObject data, Properties overrides, ActivationGroupDesc.CommandEnvironment cmd) {
	throw new kaffe.util.NotImplemented();
}

public String getClassName() {
	throw new kaffe.util.NotImplemented();
}

public String getLocation() {
	throw new kaffe.util.NotImplemented();
}

public MarshalledObject getData() {
	throw new kaffe.util.NotImplemented();
}

public Properties getPropertyOverrides() {
	throw new kaffe.util.NotImplemented();
}

public ActivationGroupDesc.CommandEnvironment getCommandEnvironment() {
	throw new kaffe.util.NotImplemented();
}

public boolean equals(Object obj) {
	throw new kaffe.util.NotImplemented();
}

public int hashCode() {
	throw new kaffe.util.NotImplemented();
}

}
