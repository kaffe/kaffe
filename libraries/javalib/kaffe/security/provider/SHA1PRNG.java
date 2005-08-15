/*
 * Java core library component.
 *
 * SHA1PRNG.java
 * SHA-1 based pseudo-random number generator.
 *
 * Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

package kaffe.security.provider;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandomSpi;

import kaffe.security.Randomness;

/**
 * SHA-1 based pseudo-random number generator.
 */
public class SHA1PRNG
	extends SecureRandomSpi
{
	/**
	 * The list of randomness implmentation classes, ordered from most to
	 * least desirable.
	 */
	private static final String RANDOMNESS_IMPLS[] = {
		"kaffe.security.UnixRandomness",
		"kaffe.security.LameRandomness"
	};

	/**
	 * The root source of randomness.
	 */
	private static final Randomness impl;
	
	static {
		Randomness rand = null;
		int lpc;

		/* Walk the list of implementations. */
		for( lpc = 0;
		     (lpc < RANDOMNESS_IMPLS.length) && (rand == null);
		     lpc++ )
		{
			try
			{
				ClassLoader cl;
				Class rclass;

				cl = ClassLoader.getSystemClassLoader();
				/* Try to load and */
				rclass = cl.loadClass(RANDOMNESS_IMPLS[lpc]);
				/* ... instantiate an object. */
				rand = (Randomness)rclass.newInstance();
				/*
				 * Success!
				 * Set a property to inform the user.
				 */
				System.setProperty("org.kaffe.randomness",
						   rclass.getName());
			}
			catch(IllegalAccessException e)
			{
				/* Really should not happen. */
				throw new InternalError(e.toString());
			}
			catch(ExceptionInInitializerError e)
			{
			}
			catch(InstantiationException e)
			{
			}
			catch(NoClassDefFoundError e)
			{
			}
			catch(ClassNotFoundException e)
			{
			}
			catch(NullPointerException e)
			{
				throw e;
			}
			catch(Exception e)
			{
			}
		}
		if( rand == null )
		{
			throw new UnsatisfiedLinkError(
				"Cannot find working Randomness");
		}
		impl = rand;
	}
	
	/**
	 * The "true" random seed size.
	 */
	private static final int SEED_SIZE = 8;

	/**
	 * The size of the data that we'll run the digest over.
	 */
	private static final int DATA_SIZE = 16;

	/**
	 * An SHA-1 digest object.
	 */
	private MessageDigest md;

	/**
	 * The "true" random seed.
	 */
	private byte seed[] = new byte[SEED_SIZE];
	
	/**
	 * Uh...
	 */
	private int seedPos = 0;

	/**
	 * The random data, we only use the first SEED_SIZE bytes.
	 */
	private byte data[] = new byte[DATA_SIZE];

	/**
	 * The position in data pointing to the first unused bytes.
	 */
	private int dataPos = 0;

	/**
	 * Counter that is added into the data to be digested.
	 */
	private long counter = 0;

	/**
	 * Construct an initialize an SHA1PRNG pseudo-random number generator.
	 */
	public SHA1PRNG()
	{
		/*
		 * Fill the seed using the implementation specific source of
		 * randomness.
		 */
		impl.fill(this.seed);
		
		try
		{
			byte digest[];
			
			this.md = MessageDigest.getInstance("SHA-1");
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
	
	protected synchronized void engineSetSeed(byte[] otherSeed)
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
	
	protected synchronized void engineNextBytes(byte[] bytes)
	{
		if( bytes.length < (SEED_SIZE - this.dataPos) )
		{
			/*
			 * We can satisfy the request without generating new
			 * random data.
			 */
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

			/*
			 * Its a request that will require new randomness be
			 * generated.
			 */
			while( bpos < blen )
			{
				subLen = blen - bpos;
				dataLen = SEED_SIZE - this.dataPos;
				if( dataLen < subLen )
				{
					subLen = dataLen;
				}
				/* Copy available data. */
				System.arraycopy(this.data,
						 this.dataPos,
						 bytes,
						 bpos,
						 subLen);
				bpos += subLen;
				this.dataPos += subLen;
				if( this.dataPos >= SEED_SIZE )
				{
					/*
					 * Generate new stuff, first copy the
					 * seed into the random data.
					 */
					System.arraycopy(this.seed,
							 0,
							 this.data,
							 0,
							 SEED_SIZE);
					this.counter += 1;
					/* Now copy the counter. */
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
					/* Rerun the digest. */
					digest = this.md.digest(this.data);
					/* Copy the first 64 bits over. */
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
