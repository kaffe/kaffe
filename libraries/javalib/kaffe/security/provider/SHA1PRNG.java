/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * NB!!!! THIS DOES NOT ACTUALLY IMPLEMENT SHA1PRNG - it uses random and
 *        is a place holder.
 *
 */

package kaffe.security.provider;

import java.security.SecureRandomSpi;
import java.util.Random;

import java.security.SecureRandom;
import java.security.MessageDigest;
import java.security.SecureRandomSpi;
import java.security.NoSuchAlgorithmException;

public class SHA1PRNG
	extends SecureRandomSpi
{
	private static final int SEED_SIZE = 20;
	private static final int DATA_SIZE = 40;
	
	private MessageDigest md;
	private byte seed[] = new byte[SEED_SIZE];
	private int seedPos = 0;
	private byte data[] = new byte[DATA_SIZE];
	private int dataPos = 0;
	
	public SHA1PRNG()
	{
		try
		{
			byte digest[];
			
			this.md = MessageDigest.getInstance("SHA-1");

			new Random().nextBytes(this.seed);
			digest = this.md.digest(this.data);
			System.arraycopy(digest, 0, this.data, 0, SEED_SIZE);
		}
		catch(NoSuchAlgorithmException e)
		{
			e.printStackTrace();
		}
		catch(Throwable th)
		{
			th.printStackTrace();
		}
	}
	
	protected void engineSetSeed(byte[] otherSeed)
	{
		try
		{
			int lpc;
			
			for( lpc = 0; lpc < otherSeed.length; lpc++ )
			{
				this.seed[this.seedPos++ % SEED_SIZE] ^=
					otherSeed[lpc];
			}
			this.seedPos %= SEED_SIZE;
		}
		catch(Throwable th)
		{
			th.printStackTrace();
		}
	}
	
	protected void engineNextBytes(byte[] bytes)
	{
		if( bytes.length < (20 - this.dataPos) )
		{
			System.arraycopy(this.data, this.dataPos,
					 bytes, 0,
					 bytes.length);
			this.dataPos += bytes.length;
		}
		else
		{
			int blen = bytes.length, bpos = 0;
			int subLen, dataLen;
			byte digest[];

			while( bpos < blen )
			{
				subLen = blen - bpos;
				dataLen = SEED_SIZE - this.dataPos;
				if( dataLen < subLen )
				{
					subLen = dataLen;
				}
				System.arraycopy(this.data,
						 this.dataPos,
						 bytes,
						 bpos,
						 subLen);
				bpos += subLen;
				this.dataPos += subLen;
				if( this.dataPos >= SEED_SIZE )
				{
					System.arraycopy(this.seed,
							 0,
							 this.data,
							 SEED_SIZE,
							 SEED_SIZE);
					digest = this.md.digest(this.data);
					System.arraycopy(digest,
							 0,
							 this.data,
							 0,
							 SEED_SIZE);
					this.dataPos = 0;
				}
			}
		}
	}
	
	protected byte[] engineGenerateSeed(int numBytes)
	{
		byte retval[] = new byte[numBytes];

		this.engineNextBytes(retval);
		return retval;
	}
	
}
