package kaffe.util;


public class DoubleLinkedObject
{
	public Object data;
	public DoubleLinkedObject prev;
	public DoubleLinkedObject next;

public DoubleLinkedObject ( Object data, DoubleLinkedObject prev, DoubleLinkedObject next ){
	this.data = data;
	this.prev = prev;
	this.next = next;
	
	if ( prev != null )
		prev.next = this;
		
	if ( next != null )
		next.prev = this;
}
}
