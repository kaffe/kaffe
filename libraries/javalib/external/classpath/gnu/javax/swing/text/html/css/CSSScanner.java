/* CSSScanner.java -- A parser for CSS stylesheets
   Copyright (C) 2006 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package gnu.javax.swing.text.html.css;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;

/**
 * A parser for CSS stylesheets. This is based on the grammar from:
 *
 * http://www.w3.org/TR/CSS21/syndata.html
 *
 * @author Roman Kennke (kennke@aicas.com)
 */
class CSSScanner
{

  // The tokens. This list is taken from:
  // http://www.w3.org/TR/CSS21/syndata.html#tokenization
  private static final int IDENT = 1;
  private static final int ATKEYWORD = 2;
  private static final int STRING = 3;
  private static final int INVALID = 4;
  private static final int HASH = 5;
  private static final int NUMBER = 6;
  private static final int PERCENTAGE = 7;
  private static final int DIMENSION = 8;
  private static final int URI = 9;
  private static final int UNICODE_RANGE = 10;
  private static final int CDO = 11;
  private static final int CDC = 12;
  private static final int SEMICOLON = 13;
  private static final int CURLY_LEFT = 14;
  private static final int CURLY_RIGHT = 15;
  private static final int PAREN_LEFT = 16;
  private static final int PAREN_RIGHT = 17;
  private static final int BRACE_LEFT = 16;
  private static final int BRACE_RIGHT = 17;
  private static final int S = 18;
  private static final int COMMENT = 19;
  private static final int FUNCTION = 20;
  private static final int INCLUDES = 21;
  private static final int DASHMATCH = 22;
  private static final int DELIM = 23;

  /**
   * The input source.
   */
  private Reader in;

  /**
   * The parse buffer.
   */
  private char[] parseBuffer;

  /**
   * The end index in the parseBuffer of the current token.
   */
  private int tokenEnd;

  /**
   * The lookahead 'buffer'.
   */
  private int[] lookahead;

  CSSScanner(Reader r)
  {
    lookahead = new int[2];
    parseBuffer = new char[2048];
    in = r;
  }

  /**
   * Fetches the next token. The actual character data is in the parseBuffer
   * afterwards with the tokenStart at index 0 and the tokenEnd field
   * pointing to the end of the token.
   *
   * @return the next token
   */
  int nextToken()
    throws IOException
  {
    tokenEnd = 0;
    int token = -1;
    int next = read();
    if (next == 65535)
      System.err.println("BUG");
    if (next != -1 && next != 65535) // Workaround.
      {
        char ch = (char) next;
        switch (ch)
        {
          case ';':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = SEMICOLON;
            break;
          case '{':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = CURLY_LEFT;
            break;
          case '}':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = CURLY_RIGHT;
            break;
          case '(':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = PAREN_LEFT;
            break;
          case ')':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = PAREN_RIGHT;
            break;
          case '[':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = BRACE_LEFT;
            break;
          case ']':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            token = BRACE_RIGHT;
            break;
          case '@':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            readIdent();
            token = ATKEYWORD;
            break;
          case '#':
            parseBuffer[0] = ch;
            tokenEnd = 1;
            readName();
            token = HASH;
            break;
          case '\'':
          case '"':
            lookahead[0] = ch;
            readString();
            token = STRING;
            break;
          case ' ':
          case '\t':
          case '\r':
          case '\n':
          case '\f':
            lookahead[0] = ch;
            readWhitespace();
            token = S;
            break;
            // FIXME: Detecting an URI involves several characters lookahead.
//          case 'u':
//            lookahead[0] = ch;
//            readURI();
//            token = URI;
//            break;
          case '<':
            parseBuffer[0] = ch;
            parseBuffer[1] = (char) read();
            parseBuffer[2] = (char) read();
            parseBuffer[3] = (char) read();
            if (parseBuffer[1] == '!' && parseBuffer[2] == '-'
              && parseBuffer[3] == '-')
              {
                token = CDO;
                tokenEnd = 4;
              }
            else
              throw new CSSLexicalException("expected CDO token");
            break;
          case '/':
            lookahead[0] = ch;
            readComment();
            token = COMMENT;
            break;
          case '~':
            parseBuffer[0] = ch;
            parseBuffer[1] = (char) read();
            if (parseBuffer[1] == '=')
              token = INCLUDES;
            else
              throw new CSSLexicalException("expected INCLUDES token");
            break;
          case '|':
            parseBuffer[0] = ch;
            parseBuffer[1] = (char) read();
            if (parseBuffer[1] == '=')
              token = DASHMATCH;
            else
              throw new CSSLexicalException("expected DASHMATCH token");
            break;
          case '-':
            char ch2 = (char) read();
            if (ch2 == '-')
              {
                char ch3 = (char) read();
                if (ch3 == '>')
                  {
                    parseBuffer[0] = ch;
                    parseBuffer[1] = ch2;
                    parseBuffer[2] = ch3;
                    tokenEnd = 3;
                    token = CDC;
                  }
                else
                  throw new CSSLexicalException("expected CDC token");
              }
            else
              {
                lookahead[0] = ch;
                lookahead[1] = ch2;
                readIdent();
                int ch3 = read();
                if (ch3 == -1 || ((char) ch3) != '(')
                  {
                    lookahead[0] = ch3;
                    token = IDENT;
                  }
                else
                  {
                    parseBuffer[tokenEnd] = (char) ch3;
                    tokenEnd++;
                    token = FUNCTION;
                  }
              }
            break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            lookahead[0] = ch;
            readNum();
            int ch3 = read();
            char ch3c = (char) ch3;
            if (ch3c == '%')
              {
                parseBuffer[tokenEnd] = ch3c;
                tokenEnd++;
                token = PERCENTAGE;
              }
            else if (ch3 == -1 || (! (ch3c == '_'
                                      || (ch3c >= 'a' && ch3c <= 'z')
                                      || (ch3c >= 'A' && ch3c <= 'Z')
                                      || ch3c == '\\' || ch3c > 177)))
              {
                lookahead[0] = ch3;
                token = NUMBER;
              }
            else
              {
                lookahead[0] = ch3;
                readIdent();
                token = DIMENSION;
              }
            break;
          default:
            // Handle IDENT that don't begin with '-'.
            if (ch == '_' || (ch >= 'a' && ch <= 'z')
                || (ch >= 'A' && ch <= 'Z') || ch == '\\' || ch > 177)
              {
                lookahead[0] = ch;
                readIdent();
                int ch4 = read();
                if (ch4 == -1 || ((char) ch4) != '(')
                  {
                  token = IDENT;
                  }
                else
                  {
                    parseBuffer[tokenEnd] = (char) ch4;
                    tokenEnd++;
                    token = FUNCTION;
                  }
              }
            else
              {
                parseBuffer[0] = ch;
                tokenEnd = 1;
                token = DELIM;
              }
          break;
        }
      }
    return token;
  }

  String currentTokenString()
  {
    return new String(parseBuffer, 0, tokenEnd);
  }

  /**
   * Reads one character from the input stream or from the lookahead
   * buffer, if it contains one character.
   *
   * @return the next character
   *
   * @throws IOException if problems occur on the input source
   */
  private int read()
    throws IOException
  {
    int ret;
    if (lookahead[0] != -1)
      {
        ret = lookahead[0];
        lookahead[0] = -1;
      }
    else if (lookahead[1] != -1)
      {
        ret = lookahead[1];
        lookahead[1] = -1;
      }
    else
      {
        ret = in.read();
      }
    return ret;
  }

  /**
   * Reads and identifier.
   *
   * @throws IOException if something goes wrong in the input source or if
   *         the lexical analyser fails to read an identifier
   */
  private void readIdent()
    throws IOException
  {
    char ch1 = (char) read();
    // Read possibly leading '-'.
    if (ch1 == '-')
      {
        parseBuffer[tokenEnd] = ch1;
        tokenEnd++;
        ch1 = (char) read();
      }
    // What follows must be '_' or a-z or A-Z or nonascii (>177) or an
    // escape.
    if (ch1 == '_' || (ch1 >= 'a' && ch1 <= 'z')
        || (ch1 >= 'A' && ch1 <= 'Z') || ch1 > 177)
      {
        parseBuffer[tokenEnd] = ch1;
        tokenEnd++;
      }
    else if (ch1 == '\\')
      {
        // Try to read an escape.
        lookahead[0] = ch1;
        readEscape();
      }
    else
      throw new CSSLexicalException("First character of identifier incorrect");

    // Read any number of [_a-zA-Z0-9-] chars.
    int ch = read();
    char chc = (char) ch;
    while (ch != -1 && (chc == '_' || chc == '-' || (chc >= 'a' && chc <= 'z')
           || (chc >= 'A' && chc <= 'Z') || (chc >= '0' && chc <= '9')))
      {
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
        ch = read();
        chc = (char) ch;
      }

    // Push back last read character since it doesn't belong to the IDENT.
    lookahead[0] = ch;
  }

  /**
   * Reads an escape.
   *
   * @throws IOException if something goes wrong in the input source or if
   *         the lexical analyser fails to read an escape
   */
  private void readEscape()
    throws IOException
  {
    int ch = read();
    char chc = (char) ch;
    if (ch != -1 && chc == '\\')
      {
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
        ch = read();
        chc = (char) ch;
        if ((chc >= '0' && chc <= '9') || (chc >= 'a' && chc <= 'f'))
          {
            // Read unicode escape.
            // Zero to five 0-9a-f chars can follow.
            int hexcount = 0;
            ch = read();
            chc = (char) ch;
            while (((chc >= '0' && chc <= '9') || (chc >= 'a' && chc <= 'f'))
                   && hexcount < 5)
              {
                parseBuffer[tokenEnd] = chc;
                tokenEnd++;
                hexcount++;
                ch = read();
                chc = (char) ch;
              }
            // Now we can have a \r\n or any whitespace character following.
            if (ch == '\r')
              {
                parseBuffer[tokenEnd] = chc;
                tokenEnd++;
                ch = read();
                chc = (char) ch;
                if (chc == '\n')
                  {
                    parseBuffer[tokenEnd] = chc;
                    tokenEnd++;
                  }
                else
                  {
                    lookahead[0] = ch;
                  }
              }
            else if (ch == ' ' || ch == '\n' || ch == '\f' || ch == '\t')
              {
                parseBuffer[tokenEnd] = chc;
                tokenEnd++;
              }
            else
              {
                lookahead[0] = ch;
              }
          }
        else if (chc != '\n' && chc != '\r' && chc != '\f')
          {
            parseBuffer[tokenEnd] = chc;
            tokenEnd++;
          }
        else
          throw new CSSLexicalException("Can't read escape");
      }
    else
      throw new CSSLexicalException("Escape must start with '\\'");
    
  }

  private void readName()
    throws IOException
  {
    // Read first name character.
    int ch = read();
    char chc = (char) ch;
    if (ch != -1 && (chc == '_' || chc == '-' || (chc >= 'a' && chc <= 'z')
           || (chc >= 'A' && chc <= 'Z') || (chc >= '0' && chc <= '9')))
      {
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
      }
    else
      throw new CSSLexicalException("Invalid name");

    // Read any number (at least one) of [_a-zA-Z0-9-] chars.
    ch = read();
    chc = (char) ch;
    while (ch != -1 && (chc == '_' || chc == '-' || (chc >= 'a' && chc <= 'z')
           || (chc >= 'A' && chc <= 'Z') || (chc >= '0' && chc <= '9')))
      {
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
      }

    // Push back last read character since it doesn't belong to the IDENT.
    lookahead[0] = ch;
  }

  /**
   * Reads in a string.
   *
   * @throws IOException
   */
  private void readString()
    throws IOException
  {
    int ch1 = read();
    char chc1 = (char) ch1;
    if (ch1 != -1 && (chc1 == '\'' || chc1 == '\"'))
      {
        parseBuffer[tokenEnd] = chc1;
        tokenEnd++;

        // Read any number of chars until we hit another chc1 char.
        // Reject newlines, except if prefixed with \.
        int ch = read();
        char chc = (char) ch;
        while (ch != -1 && chc != chc1)
          {
            // Every non-newline and non-\ char should be ok.
            if (ch != '\n' && ch != '\r' && ch != '\f' && ch != '\\')
              {
                parseBuffer[tokenEnd] = chc;
                tokenEnd++;
              }
            // Ok when followed by newline or as part of escape.
            else if (ch == '\\')
              {
                int ch2 = read();
                char chc2 = (char) ch2;
                if (chc2 == '\n' || chc2 == '\r')
                  {
                    parseBuffer[tokenEnd] = chc;
                    parseBuffer[tokenEnd + 1] = chc2;
                    tokenEnd += 2;
                  }
                else
                  {
                    // Try to parse an escape.
                    lookahead[0] = chc;
                    lookahead[1] = chc2;
                    readEscape();
                  }
              }
            else
              throw new CSSLexicalException("Invalid string");

            ch = read();
            chc = (char) ch;
          }
        if (ch != -1)
          {
            // Push the final char on the buffer.
            parseBuffer[tokenEnd] = chc;
            tokenEnd++;
          }
        else
          throw new CSSLexicalException("Unterminated string");
      }
    else
      throw new CSSLexicalException("Invalid string");
  }

  /**
   * Reads a chunk of whitespace.
   *
   * @throws IOException
   */
  private void readWhitespace()
    throws IOException
  {
    int ch = read();
    char chc = (char) ch;
    while (ch != -1 && (chc == ' ' || chc == '\t' || chc == '\r' || chc == '\n'
           || chc == '\f'))
      {
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
        ch = read();
        chc = (char) ch;
      }
    // Push back last character read.
    lookahead[0] = chc;
    
  }

  private void readURI()
    throws IOException
  {
    // FIXME: Implement.
  }

  /**
   * Reads a comment block.
   *
   * @throws IOException
   */
  private void readComment()
    throws IOException
  {
    // First we need a / and a *
    int ch = read();
    char chc = (char) ch;
    if (ch != -1 && chc == '/')
      {
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
        ch = read();
        chc = (char) ch;
        if (ch != -1 && chc == '*')
          {
            parseBuffer[tokenEnd] = chc;
            tokenEnd++;
            ch = read();
            chc = (char) ch;
            parseBuffer[tokenEnd] = chc;
            tokenEnd++;
            boolean finished = false;
            char lastChar = chc;
            ch = read();
            chc = (char) ch;
            while (! finished && ch != -1)
              {
                if (lastChar == '*' && chc == '/')
                  finished = true;
                parseBuffer[tokenEnd] = chc;
                tokenEnd++;
                lastChar = chc;
                ch = read();
                chc = (char) ch;
              }
          }
      }
    if (ch == -1)
      throw new CSSLexicalException("Unterminated comment");
    
    // Push back last character read.
    lookahead[0] = chc;
  }

  /**
   * Reads a number.
   *
   * @throws IOException
   */
  private void readNum()
    throws IOException
  {
    boolean hadDot = false;
    // First char must be number or .
    int ch = read();
    char chc = (char) ch;
    if (ch != -1 && ((chc >= '0' && chc <= '9') || chc == '.'))
      {
        if (chc == '.')
          hadDot = true;
        parseBuffer[tokenEnd] = chc;
        tokenEnd++;
        // Now read in any number of digits afterwards, and maybe one dot,
        // if we hadn't one already.
        ch = read();
        chc = (char) ch;
        while (ch != -1 && ((chc >= '0' && chc <= '9')
                            || (chc == '.' && ! hadDot)))
          {
            if (chc == '.')
              hadDot = true;
            parseBuffer[tokenEnd] = chc;
            tokenEnd++;
            ch = read();
            chc = (char) ch;
          }                            
      }
    else
      throw new CSSLexicalException("Invalid number");

    // Check if we haven't accidentally finished with a dot.
    if (parseBuffer[tokenEnd - 1] == '.')
      throw new CSSLexicalException("Invalid number");

    // Push back last character read.
    lookahead[0] = chc;
  }

  /**
   * For testing, we read in the default.css in javax/swing/text/html
   *
   * @param args
   */
  public static void main(String[] args)
  {
    try
      {
        String name = "/javax/swing/text/html/default.css";
        InputStream in = CSSScanner.class.getResourceAsStream(name);
        InputStreamReader r = new InputStreamReader(in);
        CSSScanner s = new CSSScanner(r);
        int token;
        do
          {
            token = s.nextToken();
            System.err.println("token: " + token + ": "
                               + s.currentTokenString());
          } while (token != -1);
      }
    catch (IOException ex)
      {
        ex.printStackTrace();
      }
  }
}
