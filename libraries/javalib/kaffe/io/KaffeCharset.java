/*
 * Java core library component.
 *
 * Copyright (c) 2005
 *	Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;
import java.nio.charset.*;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;

public class KaffeCharset extends Charset
{

  float averageCharsPerByte;
  float maxCharsPerByte;
  float averageBytesPerChar;
  float maxBytesPerChar;
  String kaffeIOName;

  KaffeCharset(String canonicalName, String kaffeIOName,
      String[] aliases, float[] factors)
  {
    super (canonicalName, aliases);
    this.kaffeIOName = kaffeIOName;
    this.averageCharsPerByte = factors[0];
    this.maxCharsPerByte = factors[1];
    this.averageBytesPerChar = factors[2];
    this.maxBytesPerChar =  factors[3];
  }

  public CharsetDecoder newDecoder ()
  {
    return new Decoder (this, kaffeIOName, averageCharsPerByte, maxCharsPerByte);
  }

  public CharsetEncoder newEncoder ()
  {
    return new Encoder (this, kaffeIOName, averageBytesPerChar, maxBytesPerChar);
  }

  // I know nothing about any charset other than myself.
  public boolean contains (Charset cs)
  {
    return this.name().equalsIgnoreCase(cs.name());
  }

  private static final class Decoder extends CharsetDecoder
  {
    // Package-private to avoid a trampoline constructor.
    Decoder (Charset cs, String kaffeIOName,
        float averageCharsPerByte, float maxCharsPerByte)
    {
      super (cs, averageCharsPerByte, maxCharsPerByte);
      try
        {
          b2c = kaffe.io.ByteToCharConverter.getConverter(kaffeIOName);
        }
      catch (java.io.UnsupportedEncodingException e)
        {
          throw new UnsupportedOperationException(e.toString());
        }
    }

    private kaffe.io.ByteToCharConverter b2c;

    // Kaffe's BytetoCharConverter cannot report byte sequence being
    // malformed. Undecodable bytes are always converted to '?'.
    protected CoderResult decodeLoop (ByteBuffer in, CharBuffer out)
    {
      byte[] inbuf = null;
      int inPos = 0;
      int inLen = 0;
      char[] outbuf = null;
      int outPos = 0;
      int outLen = 0;

      if (in.hasArray())
        {
          inbuf = in.array();
          inPos = in.arrayOffset() + in.position();
          inLen = in.remaining();
          in.position(in.limit());
        }
      else {
          inbuf = new byte[in.remaining()];
          inPos = 0;
          inLen = inbuf.length;
          in.get(inbuf, inPos, inLen);
        }

      if (out.hasArray())
        {
          outbuf = out.array();
          outPos = out.arrayOffset() + out.position();
          outLen = out.remaining();
        }
      else {
          outbuf = new char[out.remaining()];
          outPos = 0;
          outLen = outbuf.length;
        }

      int l = b2c.convert(inbuf, inPos, inLen, outbuf, outPos, outLen);

      if (out.hasArray())
        {
          out.position(out.position() + l);
        }
      else
        {
          out.put(outbuf, outPos, l);
        }

      int n = 0;
      if (b2c.havePending())
        {
          n = b2c.pendingLength();
          b2c.reset();
        }
      in.position(in.position() - n);
      if (n > 0 && out.remaining() == 0)
        {
          return CoderResult.OVERFLOW;
        }
      else
        {
          return CoderResult.UNDERFLOW;
        }
    }

    protected void implReset() {
      b2c.reset();
    }
  }

  private static final class Encoder extends CharsetEncoder
  {
    // Package-private to avoid a trampoline constructor.
    Encoder (Charset cs, String kaffeIOName,
        float averageBytesPerChar, float maxBytesPerChar)
    {
      super (cs, averageBytesPerChar, maxBytesPerChar);
      try
        {
          c2b = kaffe.io.CharToByteConverter.getConverter(kaffeIOName);
        }
      catch (java.io.UnsupportedEncodingException e)
        {
          throw new UnsupportedOperationException(e.toString());
        }
    }

    private kaffe.io.CharToByteConverter c2b;

    // Kaffe's CharToByteConverter cannot report characters being unmappable.
    // Unmappable characters are always converted to '?'.
    protected CoderResult encodeLoop (CharBuffer in, ByteBuffer out)
    {
      char[] inbuf = null;
      int inPos = 0;
      int inLen = 0;
      byte[] outbuf = null;
      int outPos = 0;
      int outLen = 0;

      if (in.hasArray())
        {
          inbuf = in.array();
          inPos = in.arrayOffset() + in.position();
          inLen = in.remaining();
          in.position(in.limit());
        }
      else {
          inbuf = new char[in.remaining()];
          inPos = 0;
          inLen = inbuf.length;
          in.get(inbuf, inPos, inLen);
        }

      if (out.hasArray())
        {
          outbuf = out.array();
          outPos = out.arrayOffset() + out.position();
          outLen = out.remaining();
        }
      else {
          outbuf = new byte[out.remaining()];
          outPos = 0;
          outLen = outbuf.length;
        }

      int l = c2b.convert(inbuf, inPos, inLen, outbuf, outPos, outLen);

      if (out.hasArray())
        {
          out.position(out.position() + l);
        }
      else
        {
          out.put(outbuf, outPos, l);
        }

      int n = 0;
      if (c2b.havePending())
        {
          n = c2b.pendingLength();
          c2b.reset();
        }
      in.position(in.position() - n);
      if (n > 0 && out.remaining() == 0)
        {
          return CoderResult.OVERFLOW;
        }
      else
        {
          return CoderResult.UNDERFLOW;
        }
    }

    protected void implReset() {
      c2b.reset();
    }
  }

}
