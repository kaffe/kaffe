package kaffe.awt;


public interface FlavorConverter
{
public byte[] exportObject ( Object data );

public Object importBytes ( byte[] data );
}
