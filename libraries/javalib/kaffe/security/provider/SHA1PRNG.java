/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.security.provider;

import java.util.Random;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandomSpi;

public class SHA1PRNG
	extends SecureRandomSpi
{
	private static final int SEED_SIZE = 8;
	private static final int DATA_SIZE = 16;
	
	private MessageDigest md;
	private byte seed[] = new byte[SEED_SIZE];
	private int seedPos = 0;
	private byte data[] = new byte[DATA_SIZE];
	private int dataPos = 0;
	private long counter = 0;
	
	public SHA1PRNG()
	{
		try
		{
			byte digest[];
			
			this.md = MessageDigest.getInstance("SHA-1");

			new Random().nextBytes(this.seed);
			digest = this.md.digest(this.seed);
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
		this.counter += 1;
		if( bytes.length < (SEED_SIZE - this.dataPos) )
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
							 0,
							 SEED_SIZE);
					this.data[SEED_SIZE    ] =
						(byte)(this.counter);
					this.data[SEED_SIZE + 1] =
						(byte)(this.counter >>  8);
					this.data[SEED_SIZE + 2] =
						(byte)(this.counter >> 16);
					this.data[SEED_SIZE + 3] =
						(byte)(this.counter >> 24);
					this.data[SEED_SIZE + 4] =
						(byte)(this.counter >> 32);
					this.data[SEED_SIZE + 5] =
						(byte)(this.counter >> 40);
					this.data[SEED_SIZE + 6] =
						(byte)(this.counter >> 48);
					this.data[SEED_SIZE + 7] =
						(byte)(this.counter >> 56);
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
