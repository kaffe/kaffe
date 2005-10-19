package org.kaffe.awt;


public interface FlavorConverter
{
byte[] exportObject ( Object data );

Object importBytes ( byte[] data );
}
