/*
 * DNSJavaInetAddressImpl.java
 *
 * Copyright (c) 2003 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_BASED_LICENSE@
 */

package java.net;

import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

import java.io.IOException;

import org.xbill.DNS.Name;
import org.xbill.DNS.Type;
import org.xbill.DNS.Cache;
import org.xbill.DNS.RRset;
import org.xbill.DNS.Rcode;
import org.xbill.DNS.DClass;
import org.xbill.DNS.Record;
import org.xbill.DNS.ARecord;
import org.xbill.DNS.Message;
import org.xbill.DNS.A6Record;
import org.xbill.DNS.PTRRecord;
import org.xbill.DNS.AAAARecord;
import org.xbill.DNS.FindServer;
import org.xbill.DNS.CNAMERecord;
import org.xbill.DNS.Credibility;
import org.xbill.DNS.DNAMERecord;
import org.xbill.DNS.SetResponse;
import org.xbill.DNS.ExtendedResolver;
import org.xbill.DNS.TextParseException;
import org.xbill.DNS.NameTooLongException;

/**
 * InetAddressImpl that uses DNSJAVA (http://www.xbill.org/dnsjava).
 */
final class DNSJavaInetAddressImpl
    extends InetAddressImpl
{
    /**
     * Suffix used when doing reverse lookups in IPv4 space.
     */
    private static final Name IN_ADDR_ARPA;

    /**
     * Suffix used when doing reverse lookups in IPv6 space.
     */
    private static final Name IP6_INT;

    /**
     * Maximum number of CNAME records to traverse.
     */
    private static final int CNAME_MAX = 6;

    static
    {
	try
	{
	    IN_ADDR_ARPA = Name.fromString("IN-ADDR.ARPA.");
	    IP6_INT = Name.fromString("IP6.INT.");
	}
	catch(TextParseException e)
	{
	    throw new ExceptionInInitializerError(e);
	}
    }

    /**
     * Exception thrown when the name server is queried about a non-existent
     * domain.
     */
    static class NXDomainException
	extends Exception
    {
	/**
	 * Construct an NXDomainException with the given value.
	 *
	 * @param s A message describing the problem.
	 */
	public NXDomainException(String s)
	{
	    super(s);
	}
    }

    /*
     * The Resolver we'll be using.
     */
    private final ExtendedResolver er;

    /**
     * The default search path.
     */
    private final Name searchPath[];

    /**
     * Lookup cache.
     */
    private final Cache cache = new Cache();

    /**
     * @throws UnknownHostException if the super class could not find the DNS
     * server.
     */
    DNSJavaInetAddressImpl()
	throws UnknownHostException
    {
	boolean foundRootPath = false;
	Name paths[];
	int lpc;

	this.er = new ExtendedResolver();
	/*
	 * Initialize the search path.  First, make sure the root name space is
	 * part of the path.
	 */
	paths = FindServer.searchPath();
	for( lpc = 0; lpc < paths.length; lpc++ )
	{
	    if( paths[lpc].equals(Name.root) )
	    {
		foundRootPath = true;
	    }
	}
	if( !foundRootPath )
	{
	    /* The root name space is not in the path, add it. */
	    this.searchPath = new Name[paths.length + 1];
	    this.searchPath[this.searchPath.length - 1] = Name.root;
	}
	else
	{
	    this.searchPath = new Name[paths.length];
	}
	System.arraycopy(paths, 0,
			 this.searchPath, 0,
			 paths.length);
    }

    /**
     * Send and process the response to a query.
     *
     * @param question The question to ask the name server.
     * @param reverse True if this is a reverse lookup and the caller expects
     * PTRRecords or false if they want A*Records.
     * @param depth Query counter, used to detect loops in CNAME records.
     * @return An array of Records corresponding to the ones requested.
     * @throws UnknownHostException if a problem is encountered while handling
     * the query.
     * @throws NXDomainException if the query is for a non-existent domain.
     */
    private Record[] doQuery(Record question, boolean reverse, int depth)
	throws UnknownHostException,
	       NXDomainException
    {
	Record retval[] = null;
	Message query;

	if( depth >= CNAME_MAX )
	{
	    /* Break a CNAME loop. */
	    throw new UnknownHostException("CNAME loop for: "
					   + question.getName());
	}
	
	query = Message.newQuery(question);
	try
	{
	    Message response;
	    SetResponse sr;

	    response = this.er.send(query);
	    switch( response.getHeader().getRcode() )
	    {
	    case Rcode.NOERROR:
		sr = this.cache.addMessage(response);

		if( sr == null )
		{
		    sr = this.cache.lookupRecords(question.getName(),
						  Type.ANY,
						  Credibility.NORMAL);
		}

		if( sr.isSuccessful() )
		{
		    RRset rrsets[] = sr.answers();
		    List list = new ArrayList();
		    int lpc;

		    /* Presumably, we got what we wanted. */
		    for( lpc = 0; lpc < rrsets.length; lpc++ )
		    {
			Iterator it;

			it = rrsets[lpc].rrs();
			while( it.hasNext() )
			{
			    Object record;

			    record = it.next();
			    /*
			     * Add only the records the caller is interested
			     * in...
			     */
			    if( !reverse && record instanceof ARecord )
			    {
				/* ... IPv4 */
				list.add(record);
			    }
			    else if( !reverse && record instanceof A6Record )
			    {
				/* ... IPv6 */
				list.add(record);
			    }
			    else if( !reverse && record instanceof AAAARecord )
			    {
				/* ... IPv6 */
				list.add(record);
			    }
			    else if( reverse && record instanceof PTRRecord )
			    {
				/* ... reverse lookup */
				list.add(record);
			    }
			}
		    }
		    retval = new Record[list.size()];
		    list.toArray(retval);
		}
		else if( sr.isCNAME() )
		{
		    CNAMERecord cname = sr.getCNAME();

		    /* Given name is an alias, follow the canonical name. */
		    retval = this.doQuery(Record.newRecord(cname.getTarget(),
							   Type.ANY,
							   DClass.IN),
					  reverse,
					  depth + 1);
		}
		else if( sr.isNXRRSET() )
		{
		    /* The server has the name but no data. */
		    throw new UnknownHostException("No data for host: "
						   + question.getName());
		}
		else if( sr.isDNAME() )
		{
		    DNAMERecord dname = sr.getDNAME();
		    
		    /*
		     * Substitute part of the domain name with one from the
		     * server.
		     */
		    try
		    {
			retval = this.doQuery(
				Record.newRecord(question.getName().
						 fromDNAME(dname),
						 Type.ANY,
						 DClass.IN),
				reverse,
				depth + 1);
		    }
		    catch(NameTooLongException e)
		    {
			throw new UnknownHostException(
				"Substituting "
				+ dname.getTarget()
				+ " for "
				+ dname.getName()
				+ " in "
				+ question.getName()
				+ " created an illegally long name",
				e);
		    }
		}
		else
		{
		    /* Bad response (or bad handling by us). */
		    throw new UnknownHostException("Broken name server");
		}
		break;
	    case Rcode.NXDOMAIN:
		sr = this.cache.addMessage(response);
		throw new NXDomainException("Unknown host: "
					    + question.getName());
	    default:
		throw new UnknownHostException("Broken name server");
	    }
	}
	catch(IOException e)
	{
	    throw new UnknownHostException("Cannot reach name server", e);
	}
	return retval;
    }

    private Record[] doQuery(Record question, boolean reverse)
	throws NameTooLongException,
	       UnknownHostException,
	       NXDomainException
    {
	return this.doQuery(question, reverse, 0);
    }
    
    private Record[] doQuery(Name name, Name suffix, boolean reverse)
	throws NameTooLongException,
	       UnknownHostException,
	       NXDomainException
    {
	if( suffix != null )
	{
	    name = Name.concatenate(name, suffix);
	}
	return this.doQuery(Record.newRecord(name, Type.ANY, DClass.IN),
			    reverse);
    }
    
    private Record[] doQuery(Name name)
	throws UnknownHostException,
	       NXDomainException
    {
	try
	{
	    return this.doQuery(name, (Name)null, false);
	}
	catch(NameTooLongException e)
	{
	    throw new InternalError(e);
	}
    }

    private Record[] doQuery(byte addr[])
	throws UnknownHostException,
	       NXDomainException
    {
	try
	{
	    StringBuffer sb = new StringBuffer();
	    Name suffix;
	    int lpc;

	    /* Need to construct a name that is in IP address form. */
	    switch( addr.length )
	    {
	    case 4:
		for( lpc = addr.length - 1; lpc >= 0; lpc-- )
		{
		    sb.append(addr[lpc] & 0xFF);
		    if( lpc > 0 )
			sb.append(".");
		}
		suffix = IN_ADDR_ARPA;
		break;
	    case 16:
		for( lpc = addr.length - 1; lpc >= 0; lpc-- )
		{
		    sb.append(addr[lpc] & 0x0F);
		    sb.append(".");
		    sb.append(addr[lpc] & 0xF0);
		    if( lpc > 0 )
			sb.append(".");
		}
		suffix = IP6_INT;
		break;
	    default:
		throw new InternalError("Cannot handle address length: "
					+ addr.length);
	    }
	    return this.doQuery(Name.fromString(sb.toString()),
				suffix,
				true);
	}
	catch(TextParseException e)
	{
	    throw new InternalError(e);
	}
	catch(NameTooLongException e)
	{
	    throw new InternalError(e);
	}
    }

    byte[][] lookupAllHostAddr(String host)
	throws UnknownHostException
    {
	byte retval[][];

	try
	{
	    Name name = Name.fromString(host);
	    Record answer[] = null;
	    int lpc;
	    
	    if( name.isAbsolute() )
	    {
		try
		{
		    /*
		     * Absolute address, don't try it with the suffixes in the
		     * search path.
		     */
		    answer = this.doQuery(name);
		}
		catch(NXDomainException e)
		{
		    throw new UnknownHostException("Unknown host: "
						   + host,
						   e);
		}
	    }
	    else
	    {
		/* Relative address, scan the path. */
		for( lpc = 0;
		     (lpc < this.searchPath.length) && (answer == null);
		     lpc++ )
		{
		    try
		    {
			answer = this.doQuery(name,
					      this.searchPath[lpc],
					      false);
		    }
		    catch(NameTooLongException e)
		    {
			/* Ignore. */
		    }
		    catch(NXDomainException e)
		    {
			/* Ignore. */
		    }
		}
		if( answer == null )
		{
		    throw new UnknownHostException("Unknown host: "
						   + host);
		}
	    }

	    /* Convert the reply to a set of byte arrays. */
	    retval = new byte[answer.length][];
	    for( lpc = 0; lpc < answer.length; lpc++ )
	    {
		if( answer[lpc] instanceof ARecord )
		{
		    String str;
		    
		    str = answer[lpc].rdataToString();
		    retval[lpc] = InetAddress.getByName(str).getAddress();
		}
		else if( answer[lpc] instanceof A6Record )
		{
		    retval[lpc] = ((A6Record)answer[lpc]).getSuffix().
			toBytes();
		}
		else if( answer[lpc] instanceof AAAARecord )
		{
		    retval[lpc] = ((AAAARecord)answer[lpc]).getAddress().
			toBytes();
		}
		else
		{
		    throw new InternalError("Unhandled record type: "
					    + answer[lpc]);
		}
	    }
	}
	catch(TextParseException e)
	{
	    throw new UnknownHostException("Badly formatted host name: "
					   + host,
					   e);
	}
	return retval;
    }

    String getHostByAddr(byte addr[])
	throws UnknownHostException
    {
	try
	{
	    String retval;

	    retval = this.doQuery(addr)[0].rdataToString();
	    return retval.substring(0, retval.length() - 1);
	}
	catch(NXDomainException e)
	{
	    throw new UnknownHostException();
	}
    }

    public String toString()
    {
	return "DNSJavaInetAddressImpl["
	    + super.toString()
	    + "]";
    }
}
