package kaffe.util.log;


public interface LogClient
{
public void disable();

public void enable();

public void newLogLine ( LogStream ls );
}
